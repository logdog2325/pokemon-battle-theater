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
#include "save.h"
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

// v2.0.9 — Animated title screen. The commissioned art is a 5-frame loop
// (the battle scene with flickering flames + Sceptile poses; the logo is
// composited onto every frame so it stays rock-steady). Each frame is a
// full 240x160 Mode-4 8bpp bitmap (38400 bytes). All frames share ONE
// 256-color palette, so animation is pure pixel-index swapping — the palette
// loads once and never changes.
//
// Frames play ping-pong (0,1,2,3,4,3,2,1) to match the artist's GIF, ~220ms
// per step. Tearing is avoided with Mode-4 double-buffering: each step DMAs
// the next frame into the OFF-SCREEN page during the main loop, then the
// VBlank handler flips the DISPCNT display-frame bit so the just-written page
// becomes visible atomically.
//
// Declared u32[] for 4-byte alignment (DmaCopy32 requirement). Frame 0 keeps
// the legacy filename so a single-frame revert stays trivial.
static const u32 sTitleFrame0[] = INCBIN_U32("graphics/title_screen/battle_theater_full.bitmap");
static const u32 sTitleFrame1[] = INCBIN_U32("graphics/title_screen/battle_theater_f1.bitmap");
static const u32 sTitleFrame2[] = INCBIN_U32("graphics/title_screen/battle_theater_f2.bitmap");
static const u32 sTitleFrame3[] = INCBIN_U32("graphics/title_screen/battle_theater_f3.bitmap");
static const u32 sTitleFrame4[] = INCBIN_U32("graphics/title_screen/battle_theater_f4.bitmap");
static const u32 *const sTitleFrames[] = {
    sTitleFrame0, sTitleFrame1, sTitleFrame2, sTitleFrame3, sTitleFrame4,
};
// 256-color GBA-format palette (BGR555, 512 bytes), shared by all frames.
static const u16 sBattleTheaterPalette[256] = INCBIN_U16("graphics/title_screen/battle_theater_full.bgpal");

#define TITLE_FRAME_BYTES       0x9600  // 38400 = 240*160 visible bytes
#define MODE4_PAGE_STRIDE       0xA000  // page 1 begins at VRAM + 0xA000
#define DISPCNT_PAGE_SELECT     0x0010  // DISPCNT bit 4: Mode 4/5 frame select
#define TITLE_FRAME_DELAY       13      // ~220ms at 59.7fps (matches the GIF)
#define TITLE_PINGPONG_STEPS    8       // 0,1,2,3,4,3,2,1

// Base DISPCNT for the title (page-select bit added/cleared on flip).
static u16 sTitleDispcntBase;
static u8  sTitleAnimStep;      // 0..7 ping-pong step
static u8  sTitleAnimTimer;     // vblanks since last advance
static u8  sTitleDisplayPage;   // 0 or 1: Mode 4 page currently shown
static bool8 sTitleFlipPending; // VBlank should flip the page this frame

static void Task_TitleAnimate(u8);

static void VBlankCB(void)
{
    // Atomic page flip: the off-screen page already holds the next frame
    // (written during the main loop), so toggling the display-frame bit here
    // swaps it in with zero tearing.
    if (sTitleFlipPending)
    {
        sTitleDisplayPage ^= 1;
        SetGpuReg(REG_OFFSET_DISPCNT,
                  sTitleDisplayPage ? (sTitleDispcntBase | DISPCNT_PAGE_SELECT)
                                    : sTitleDispcntBase);
        sTitleFlipPending = FALSE;
    }
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

// Every TITLE_FRAME_DELAY vblanks, advance the ping-pong step, DMA that frame
// into the off-screen page, and arm the VBlank flip.
static void Task_TitleAnimate(u8 taskId)
{
    void *dst;
    u8 backPage, frame;

    if (++sTitleAnimTimer < TITLE_FRAME_DELAY)
        return;
    sTitleAnimTimer = 0;

    sTitleAnimStep = (sTitleAnimStep + 1) % TITLE_PINGPONG_STEPS;
    // Map the 8-step ping-pong onto the 5 unique frames: 0,1,2,3,4,3,2,1.
    frame = (sTitleAnimStep <= 4) ? sTitleAnimStep : (TITLE_PINGPONG_STEPS - sTitleAnimStep);

    // Write to whichever page ISN'T currently displayed, then flip at VBlank.
    backPage = sTitleDisplayPage ^ 1;
    dst = (void *)(VRAM + (backPage ? MODE4_PAGE_STRIDE : 0));
    DmaCopy32(3, sTitleFrames[frame], dst, TITLE_FRAME_BYTES);
    sTitleFlipPending = TRUE;
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
        // Copy frame 0 into BG2 framebuffer page 0 (Mode 4, VRAM 0x06000000).
        // DmaCopy32 takes size in bytes; 38400/4 = 9600 units is well under the
        // GBA DMA count limit. Subsequent frames stream into the off-screen
        // page from Task_TitleAnimate.
        DmaCopy32(3, sTitleFrame0, (void *)VRAM, TITLE_FRAME_BYTES);
        LoadPalette(sBattleTheaterPalette, BG_PLTT_ID(0), sizeof(sBattleTheaterPalette));
        // Reset animation state: frame 0 shown on page 0, timer primed.
        sTitleAnimStep = 0;
        sTitleAnimTimer = 0;
        sTitleDisplayPage = 0;
        sTitleFlipPending = FALSE;
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
        // fully composited at design time. Cache the base DISPCNT so the
        // VBlank page-flip can OR in the frame-select bit without disturbing
        // the other display flags.
        SetGpuReg(REG_OFFSET_BG2CNT, 0);
        sTitleDispcntBase = DISPCNT_MODE_4
                          | DISPCNT_OBJ_1D_MAP
                          | DISPCNT_BG2_ON
                          | DISPCNT_OBJ_ON;
        SetGpuReg(REG_OFFSET_DISPCNT, sTitleDispcntBase);
        // Start the frame-cycling task only now that Mode 4 is live, so an
        // early page-flip can't toggle DISPCNT before the display is set up.
        CreateTask(Task_TitleAnimate, 0);
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

// v1.4.1 — wraps CB2_ContinueSavedGame for the sim's "Continue" boot path.
// Skips the standard map-popup field callback so we land at the Battle Tower
// lobby instantly, then the gSimAutoOpenPending flag re-opens the trainer
// picker the next frame.
static void CB2_SimContinueSkipMapPopup(void)
{
    CB2_ContinueSavedGame();
    gFieldCallback = NULL;
}

static void CB2_GoToMainMenu(void)
{
    if (!UpdatePaletteFade())
    {
        // v1.4.1 — the trainer picker still auto-opens on either path.
        gSimAutoOpenPending = TRUE;

        // v1.4.1 — if a save exists, use the Continue path. The previous code
        // unconditionally called CB2_NewGame on every boot, which runs
        // NewGameInitData → ClearSav3 → wipes gSaveBlock3Ptr->simCustomTrainers
        // every time the user launched the ROM. That's the real reason custom
        // trainers didn't persist across sessions in v1.4: the save flushed
        // fine, but the boot path zeroed the in-memory copy before anything
        // could read it.
        if (gSaveFileStatus == SAVE_STATUS_OK)
        {
            SetMainCallback2(CB2_SimContinueSkipMapPopup);
        }
        else
        {
            // First boot (no save): skip main menu + Birch and jump straight
            // into a new game. Customize defaults on the FRESH save so existing
            // user prefs (e.g. v1.1 player-name override) aren't clobbered on
            // every subsequent boot.
            gSaveBlock2Ptr->playerGender = MALE;
            StringCopy(gSaveBlock2Ptr->playerName, COMPOUND_STRING("PLAYER"));
            // Battle style SET = auto-send next mon without "will you switch?" prompt.
            gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;
            gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_FAST;
            SetMainCallback2(CB2_SimNewGameSkipTruck);
        }
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
