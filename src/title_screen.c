#include "global.h"
#include "battle.h"
#include "title_screen.h"
#include "sprite.h"
#include "gba/m4a_internal.h"
#include "clear_save_data_menu.h"
#include "debug.h"
#include "decompress.h"
#include "event_data.h"
#include "intro.h"
#include "m4a.h"
#include "main.h"
#include "main_menu.h"
#include "overworld.h"
#include "constants/map_groups.h"
#include "palette.h"
#include "reset_rtc_screen.h"
#include "string_util.h"
#include "berry_fix_program.h"
#include "sound.h"
#include "task.h"
#include "scanline_effect.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "malloc.h"
#include "constants/rgb.h"
#include "constants/songs.h"

// Battle Simulator: replace the original Emerald title screen with a full-frame
// 240x160 bitmap render of the "Battle Theater" art. The image already contains
// the BATTLE THEATER logo and the "AI vs AI Simulator" subtitle baked in, so we
// drop everything from the original title (Rayquaza affine BG, clouds, the
// Pokemon logo, the procedural BATTLE THEATER text sprite, the version banner,
// the logo-shine effect) and render the entire scene as a single Mode 4
// 8bpp bitmap on BG2, with the OBJ layer reserved for any future overlay.
//
// Mode 4 layout:
//   - BG2 displays a 240x160 framebuffer at VRAM 0x06000000 (page 0).
//   - Each pixel is 1 byte = palette index into BG palette RAM.
//   - BG palette holds 256 colors as BGR555 entries.
//   - No tiles, no tilemap -- pure linear pixel data.

#define A_B_START_SELECT (A_BUTTON | B_BUTTON | START_BUTTON | SELECT_BUTTON)
#define CLEAR_SAVE_BUTTON_COMBO (B_BUTTON | SELECT_BUTTON | DPAD_UP)
#define RESET_RTC_BUTTON_COMBO  (B_BUTTON | SELECT_BUTTON | DPAD_LEFT)
#define BERRY_UPDATE_BUTTON_COMBO (B_BUTTON | SELECT_BUTTON)

static void MainCB2(void);
static void Task_TitleScreenIdle(u8);
static void CB2_GoToMainMenu(void);
static void CB2_GoToClearSaveDataScreen(void);
static void CB2_GoToResetRtcScreen(void);
static void CB2_GoToBerryFixScreen(void);
static void CB2_GoToCopyrightScreen(void);

// Used by intro.c during the Game Freak logo fade. Kept as part of the public
// title_screen.c symbol surface even though the new bitmap title screen doesn't
// use BLDALPHA itself.
const u16 gTitleScreenAlphaBlend[64] =
{
    BLDALPHA_BLEND(16, 0),  BLDALPHA_BLEND(16, 1),  BLDALPHA_BLEND(16, 2),  BLDALPHA_BLEND(16, 3),
    BLDALPHA_BLEND(16, 4),  BLDALPHA_BLEND(16, 5),  BLDALPHA_BLEND(16, 6),  BLDALPHA_BLEND(16, 7),
    BLDALPHA_BLEND(16, 8),  BLDALPHA_BLEND(16, 9),  BLDALPHA_BLEND(16, 10), BLDALPHA_BLEND(16, 11),
    BLDALPHA_BLEND(16, 12), BLDALPHA_BLEND(16, 13), BLDALPHA_BLEND(16, 14), BLDALPHA_BLEND(16, 15),
    BLDALPHA_BLEND(15, 16), BLDALPHA_BLEND(14, 16), BLDALPHA_BLEND(13, 16), BLDALPHA_BLEND(12, 16),
    BLDALPHA_BLEND(11, 16), BLDALPHA_BLEND(10, 16), BLDALPHA_BLEND(9, 16),  BLDALPHA_BLEND(8, 16),
    BLDALPHA_BLEND(7, 16),  BLDALPHA_BLEND(6, 16),  BLDALPHA_BLEND(5, 16),  BLDALPHA_BLEND(4, 16),
    BLDALPHA_BLEND(3, 16),  BLDALPHA_BLEND(2, 16),  BLDALPHA_BLEND(1, 16),  BLDALPHA_BLEND(0, 16),
    [32 ... 63] = BLDALPHA_BLEND(0, 16),
};

// 240x160 Mode-4 framebuffer pixel data (row-major, 8bpp, 38400 bytes).
// Declared as u32[] for natural 4-byte alignment so DmaCopy32 is legal.
static const u32 sBattleTheaterBitmap[] = INCBIN_U32("graphics/title_screen/battle_theater_full.bitmap");
// 256-color GBA-format palette (BGR555, 512 bytes).
static const u16 sBattleTheaterPalette[256] = INCBIN_U16("graphics/title_screen/battle_theater_full.bgpal");

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void CB2_InitTitleScreen(void)
{
    if (IS_FRLG)
    {
        CB2_InitTitleScreenFrlg();
        return;
    }
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankCallback(NULL);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        *((u16 *)PLTT) = RGB_WHITE;
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        SetGpuReg(REG_OFFSET_BG2CNT, 0);
        SetGpuReg(REG_OFFSET_BG1CNT, 0);
        SetGpuReg(REG_OFFSET_BG0CNT, 0);
        SetGpuReg(REG_OFFSET_BG2HOFS, 0);
        SetGpuReg(REG_OFFSET_BG2VOFS, 0);
        SetGpuReg(REG_OFFSET_BG1HOFS, 0);
        SetGpuReg(REG_OFFSET_BG1VOFS, 0);
        SetGpuReg(REG_OFFSET_BG0HOFS, 0);
        SetGpuReg(REG_OFFSET_BG0VOFS, 0);
        DmaFill16(3, 0, (void *)VRAM, VRAM_SIZE);
        DmaFill32(3, 0, (void *)OAM, OAM_SIZE);
        DmaFill16(3, 0, (void *)(PLTT + 2), PLTT_SIZE - 2);
        ResetPaletteFade();
        gMain.state = 1;
        break;
    case 1:
        // Copy 38,400 bytes of bitmap into BG2 framebuffer (Mode 4 page 0 at
        // VRAM 0x06000000). DmaCopy32 takes size in bytes; 38400/4 = 9600 units
        // is well under the GBA DMA count limit.
        DmaCopy32(3, sBattleTheaterBitmap, (void *)VRAM, sizeof(sBattleTheaterBitmap));
        LoadPalette(sBattleTheaterPalette, BG_PLTT_ID(0), sizeof(sBattleTheaterPalette));
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        FreeAllSpritePalettes();
        gMain.state = 2;
        break;
    case 2:
        CreateTask(Task_TitleScreenIdle, 0);
        gMain.state = 3;
        break;
    case 3:
        BeginNormalPaletteFade(PALETTES_ALL, 1, 16, 0, RGB_WHITEALPHA);
        SetVBlankCallback(VBlankCB);
        gMain.state = 4;
        break;
    case 4:
        // Mode 4 + BG2 (bitmap) + OBJ. No window, no blending -- the image is
        // fully composited at design time.
        SetGpuReg(REG_OFFSET_BG2CNT, 0);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_4
                                    | DISPCNT_OBJ_1D_MAP
                                    | DISPCNT_BG2_ON
                                    | DISPCNT_OBJ_ON);
        EnableInterrupts(INTR_FLAG_VBLANK);
        m4aSongNumStart(MUS_B_FRONTIER);
        gMain.state = 5;
        break;
    case 5:
        if (!UpdatePaletteFade())
            SetMainCallback2(MainCB2);
        break;
    }
}

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

// Wait for input; on A/Start, fade out and jump straight into the sim picker.
static void Task_TitleScreenIdle(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(START_BUTTON))
    {
        FadeOutBGM(4);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_WHITEALPHA);
        SetMainCallback2(CB2_GoToMainMenu);
    }
    else if (JOY_HELD(CLEAR_SAVE_BUTTON_COMBO) == CLEAR_SAVE_BUTTON_COMBO)
    {
        SetMainCallback2(CB2_GoToClearSaveDataScreen);
    }
    else if (JOY_HELD(RESET_RTC_BUTTON_COMBO) == RESET_RTC_BUTTON_COMBO
          && CanResetRTC() == TRUE)
    {
        FadeOutBGM(4);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        SetMainCallback2(CB2_GoToResetRtcScreen);
    }
    else if (JOY_HELD(BERRY_UPDATE_BUTTON_COMBO) == BERRY_UPDATE_BUTTON_COMBO)
    {
        FadeOutBGM(4);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        SetMainCallback2(CB2_GoToBerryFixScreen);
    }
    else
    {
        // Auto-timeout to copyright screen when the title music ends.
        if ((gMPlayInfo_BGM.status & 0xFFFF) == 0)
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_WHITEALPHA);
            SetMainCallback2(CB2_GoToCopyrightScreen);
        }
    }
}

// Battle Simulator: wraps CB2_NewGame to skip the truck-arrival cutscene.
// (Map redirect to Battle Tower happens inside WarpToTruck via gSimAutoOpenPending.)
static void CB2_SimNewGameSkipTruck(void)
{
    CB2_NewGame();
    gFieldCallback = NULL;
}

static void CB2_GoToMainMenu(void)
{
    if (!UpdatePaletteFade())
    {
        // Battle Simulator: skip main menu + Birch entirely, jump straight to a
        // new game and request the trainer picker to auto-open once the
        // overworld is interactive.
        gSaveBlock2Ptr->playerGender = MALE;
        StringCopy(gSaveBlock2Ptr->playerName, COMPOUND_STRING("PLAYER"));
        // Battle style SET = auto-send next mon without "will you switch?" prompt.
        gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;
        gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_FAST;
        gSimAutoOpenPending = TRUE;
        SetMainCallback2(CB2_SimNewGameSkipTruck);
    }
}

static void CB2_GoToCopyrightScreen(void)
{
    if (!UpdatePaletteFade())
        SetMainCallback2(CB2_InitCopyrightScreenAfterTitleScreen);
}

static void CB2_GoToClearSaveDataScreen(void)
{
    if (!UpdatePaletteFade())
        SetMainCallback2(CB2_InitClearSaveDataScreen);
}

static void CB2_GoToResetRtcScreen(void)
{
    if (!UpdatePaletteFade())
        SetMainCallback2(CB2_InitResetRtcScreen);
}

static void CB2_GoToBerryFixScreen(void)
{
    if (!UpdatePaletteFade())
    {
        m4aMPlayAllStop();
        SetMainCallback2(CB2_InitBerryFixProgram);
    }
}
