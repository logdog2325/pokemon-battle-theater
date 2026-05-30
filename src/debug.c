// v0.54 — RELEASE_BUILD strips personal trainers (Logan, Taylor) from the
// simulator picker. Uncomment for distributable patches; commented for normal
// dev builds. Only affects the picker — trainer entries stay in trainers.party
// so trainer IDs don't shift and dev builds preserve everything.
// #define RELEASE_BUILD 1

#include "global.h"
#include "battle.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "battle_util.h"
// v1.2 — Showdown team-code import (decoder + keyboard widget).
#include "sim_team_code.h"
#include "bg.h"
#include "fpmath.h"
#include "gpu_regs.h"
#include "text_window.h"
#include "menu.h"
#include "berry.h"
#include "clock.h"
#include "coins.h"
#include "credits.h"
#include "credits_frlg.h"
#include "data.h"
#include "daycare.h"
#include "debug.h"
#include "decoration.h"
#include "decoration_inventory.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "field_message_box.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "follower_npc.h"
#include "international_string_util.h"
#include "item.h"
#include "item_icon.h"
#include "item_use.h"
#include "list_menu.h"
#include "m4a.h"
#include "main.h"
#include "main_menu.h"
#include "match_call.h"
#include "malloc.h"
#include "map_name_popup.h"
#include "menu.h"
#include "money.h"
#include "naming_screen.h"
#include "new_game.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "region_map.h"
#include "rtc.h"
#include "save.h"
#include "script.h"
#include "script_pokemon_util.h"
#include "sound.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"
#include "tv.h"
#include "pokemon_summary_screen.h"
#include "wild_encounter.h"
#include "constants/abilities.h"
#include "constants/battle_ai.h"
#include "constants/battle_frontier.h"
#include "constants/coins.h"
#include "constants/decorations.h"
#include "constants/event_objects.h"
#include "constants/expansion.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/map_groups.h"
#include "constants/rgb.h"
#include "constants/script_commands.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/weather.h"
#include "siirtc.h"
#include "rtc.h"
#include "fake_rtc.h"
#include "save.h"
#include "vs_seeker.h"
#include "load_save.h"
#include "battle_partner.h"

enum FollowerNPCCreateDebugMenu
{
    DEBUG_FNPC_BRENDAN,
    DEBUG_FNPC_MAY,
    DEBUG_FNPC_STEVEN,
    DEBUG_FNPC_WALLY,
    DEBUG_FNPC_RED,
    DEBUG_FNPC_LEAF,
    DEBUG_FNPC_COUNT,
};

enum FlagsVarsDebugMenu
{
    DEBUG_FLAGVAR_MENU_ITEM_FLAGS,
    DEBUG_FLAGVAR_MENU_ITEM_VARS,
    DEBUG_FLAGVAR_MENU_ITEM_DEXFLAGS_ALL,
    DEBUG_FLAGVAR_MENU_ITEM_DEXFLAGS_RESET,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_POKEDEX,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_NATDEX,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_POKENAV,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_MATCH_CALL,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_RUN_SHOES,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_LOCATIONS,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_BADGES_ALL,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_GAME_CLEAR,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_FRONTIER_PASS,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_COLLISION,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_ENCOUNTER,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_TRAINER_SEE,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_CATCHING,
    DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_BAG_USE,
};

enum DebugBattleType
{
    DEBUG_BATTLE_0_MENU_ITEM_WILD,
    DEBUG_BATTLE_0_MENU_ITEM_WILD_DOUBLE,
    DEBUG_BATTLE_0_MENU_ITEM_SINGLE,
    DEBUG_BATTLE_0_MENU_ITEM_DOUBLE,
    DEBUG_BATTLE_0_MENU_ITEM_MULTI,
};

enum DebugBattleAIFlags
{
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_00,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_01,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_02,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_03,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_04,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_05,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_06,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_07,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_08,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_09,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_10,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_11,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_12,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_13,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_14,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_15,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_16,
    DEBUG_BATTLE_1_MENU_ITEM_AI_FLAG_17,
    DEBUG_BATTLE_1_MENU_ITEM_CONTINUE,
};

enum DebugBattleEnvironment
{
    DEBUG_BATTLE_2_MENU_ITEM_ENVIRONMENT_0,
    DEBUG_BATTLE_2_MENU_ITEM_ENVIRONMENT_1,
    DEBUG_BATTLE_2_MENU_ITEM_ENVIRONMENT_2,
    DEBUG_BATTLE_2_MENU_ITEM_ENVIRONMENT_3,
    DEBUG_BATTLE_2_MENU_ITEM_ENVIRONMENT_4,
    DEBUG_BATTLE_2_MENU_ITEM_ENVIRONMENT_5,
    DEBUG_BATTLE_2_MENU_ITEM_ENVIRONMENT_6,
    DEBUG_BATTLE_2_MENU_ITEM_ENVIRONMENT_7,
    DEBUG_BATTLE_2_MENU_ITEM_ENVIRONMENT_8,
    DEBUG_BATTLE_2_MENU_ITEM_ENVIRONMENT_9,
};

enum DebugTrainerSelection
{
    TRAINERS_DEBUG_SELECTION_TRAINER1,
    TRAINERS_DEBUG_SELECTION_TRAINER2,
    TRAINERS_DEBUG_SELECTION_PARTNER,
    TRAINERS_DEBUG_SELECTION_PLAYER,
    // v1.1 — picker is being used as the "copy preset team into active custom
    // slot" source instead of one of the sim battle slots. Confirm copies the
    // selected trainer's full party into gSaveBlock3Ptr->simCustomTrainers
    // [sBuildTrainerActiveSlot]; cancel returns to the slot menu unchanged.
    TRAINERS_DEBUG_SELECTION_COPY_TO_CUSTOM,
    // v1.7 — picker is being used as the Frontier Challenge team-picker.
    // Confirm loads the selected trainer's full party into gPlayerParty and
    // warps to MAP_BATTLE_FRONTIER_OUTSIDE_WEST so the user can walk into
    // any facility. Cancel returns to the wrapper menu.
    TRAINERS_DEBUG_SELECTION_FRONTIER,
};

// Battle Simulator: tier color prefix for picker display.
// LIGHT_RED = champion (gold-ish), BLUE = E4 (purple-ish), GREEN = gym leader, default = white.
static const u8 sSimTierColorChampion[] = _("{COLOR LIGHT_RED}");
static const u8 sSimTierColorE4[]       = _("{COLOR BLUE}");
static const u8 sSimTierColorGym[]      = _("{COLOR GREEN}");
static const u8 sSimTierColorNone[]     = _("");

// Battle Simulator: opponent tier classification — used to color the picker
// and to pick battle music. Keep these checks in sync with GetSimTierColorPrefix.
enum SimTier
{
    SIM_TIER_NONE,
    SIM_TIER_GYM,
    SIM_TIER_E4,
    SIM_TIER_CHAMPION,
};

static enum SimTier Sim_GetTier(u16 trainerId)
{
    if (trainerId == 335 || trainerId == 804 || trainerId == 851
     || trainerId == 859 || trainerId == 860 || trainerId == 861
     || trainerId == 886 || trainerId == 888 || trainerId == 889 || trainerId == 890
     || trainerId == 891 || trainerId == 892 || trainerId == 893 || trainerId == 894
     || trainerId == 902 || trainerId == 903 || trainerId == 905 || trainerId == 906)
        return SIM_TIER_CHAMPION;
    if ((trainerId >= 261 && trainerId <= 264)
     || (trainerId >= 855 && trainerId <= 858)
     || trainerId == 887
     || (trainerId >= 895 && trainerId <= 898)
     || (trainerId >= 899 && trainerId <= 901)
     || trainerId == 904)
        return SIM_TIER_E4;
    if (trainerId == 773 || trainerId == 777 || trainerId == 781 || trainerId == 785
     || trainerId == 789 || trainerId == 793 || trainerId == 797 || trainerId == 801
     || (trainerId >= 862 && trainerId <= 868)
     || (trainerId >= 869 && trainerId <= 885))
        return SIM_TIER_GYM;
    return SIM_TIER_NONE;
}

// Battle Simulator: pick the dramatic track for a sim battle. The engine's
// default GetBattleBGM picks by trainer class which gives us Steven the gym-
// leader theme — fine, but not "Battle Revolution showdown" energy. We pick
// by tier of the highest-tier trainer in the matchup so every fight feels
// appropriately epic.
// v1.17 — Hoenn rivals (Wally / May / Brendan ORAS) should play the RSE
// Rival theme instead of the FRLG Champion theme they currently get
// because they're flagged as Champion-tier in the picker color logic.
// Fan request (Itchy). 891=Wally, 892=Steven Delta, 893=May, 894=Brendan
// per the Sim_GetTierColorPrefix comment block above. Steven Delta stays
// on Champion music (he's a Champion), only the actual rivals re-route.
static bool32 IsHoennRivalForMusic(u16 trainerId)
{
    return trainerId == 891 || trainerId == 893 || trainerId == 894;
}

u16 Sim_GetBattleMusic(void)
{
    // Rival music override takes priority over tier-based selection. If any
    // opponent or the player AI slot is a Hoenn rival, play the RSE Rival
    // theme — feels right for May/Brendan/Wally even though they're tagged
    // Champion-tier in the picker.
    if (IsHoennRivalForMusic(TRAINER_BATTLE_PARAM.opponentA)
     || ((gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS) && IsHoennRivalForMusic(TRAINER_BATTLE_PARAM.opponentB))
     || (gPartnerTrainerId != 0 && gPartnerTrainerId < TRAINERS_COUNT && IsHoennRivalForMusic(gPartnerTrainerId)))
        return MUS_VS_RIVAL;

    enum SimTier best = SIM_TIER_NONE;
    // Check both opponent slots and the player AI's roster — pick the highest tier.
    enum SimTier t;
    t = Sim_GetTier(TRAINER_BATTLE_PARAM.opponentA);
    if (t > best) best = t;
    if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
    {
        t = Sim_GetTier(TRAINER_BATTLE_PARAM.opponentB);
        if (t > best) best = t;
    }
    // Player AI side (sim-only — gPartnerTrainerId holds it during AI-vs-AI builds).
    if (gPartnerTrainerId != 0 && gPartnerTrainerId < TRAINERS_COUNT)
    {
        t = Sim_GetTier(gPartnerTrainerId);
        if (t > best) best = t;
    }
    switch (best)
    {
    case SIM_TIER_CHAMPION:
        // FRLG Lance/Blue final theme — the most "showdown" track in the bank.
        return MUS_RG_VS_CHAMPION;
    case SIM_TIER_E4:
        // Frontier Brain — high tension, fits Indigo / Hoenn E4 rematches.
        return MUS_VS_FRONTIER_BRAIN;
    case SIM_TIER_GYM:
        // FRLG gym leader — bouncier, varied, fits multi-typed leader rosters.
        return MUS_RG_VS_GYM_LEADER;
    case SIM_TIER_NONE:
    default:
        // Hoenn champion theme as the fallback so it still sounds like a
        // big fight even when no curated trainer is in the matchup.
        return MUS_VS_CHAMPION;
    }
}

static const u8 *GetSimTierColorPrefix(u16 trainerId)
{
    // Champions (Hoenn + Kanto + PWT champs/rivals + Wally/Steven/May/Brendan ORAS + LGPE Red/Blue/Green + Logan)
    if (trainerId == 335 || trainerId == 804 || trainerId == 851
     || trainerId == 859 || trainerId == 860 || trainerId == 861
     || trainerId == 886 || trainerId == 888 || trainerId == 889 || trainerId == 890
     || trainerId == 891 || trainerId == 892 || trainerId == 893 || trainerId == 894
     || trainerId == 902 || trainerId == 903 || trainerId == 905 || trainerId == 906)
        return sSimTierColorChampion;
    // v0.9: Rainbow Rocket bosses (top tier — legendary aces) + Alola champions
    // Kukui (930) + Hau (931) + their variants (932-935) + Gladion variants (936-938).
    // v0.10: PWT champion-tier extras Cynthia (963) / Iris (964) / Alder (965).
    // v0.10.2: Blue HGSS (966) — champion-tier Kanto rival.
    // v0.10.3: Lance HGSS (981) — Indigo Plateau Champion.
    // v0.10.4: Trace LGPE Pikachu/Eevee (982-983) — Pokemon League Champion.
    // v0.11: Cynthia Pt rematch (1005) — Pokemon League Champion.
    // v0.12: Cynthia BDSP (1021) — BDSP Champion.
    // v0.14: Leon variants (1029-1031) — SwSh Champion. Hop+Mustard rivals.
    if ((trainerId >= 907 && trainerId <= 916) || (trainerId >= 930 && trainerId <= 938)
     || (trainerId >= 963 && trainerId <= 966) || (trainerId >= 981 && trainerId <= 983)
     || trainerId == 1005 || trainerId == 1021
     || (trainerId >= 1029 && trainerId <= 1031))
        return sSimTierColorChampion;
    // Hoenn E4 + Indigo E4 + PWT Lance + Hoenn ORAS E4 + Indigo LGPE E4 (incl. Bruno at 904)
    if ((trainerId >= 261 && trainerId <= 264)
     || (trainerId >= 855 && trainerId <= 858)
     || trainerId == 887
     || (trainerId >= 895 && trainerId <= 898)
     || (trainerId >= 899 && trainerId <= 901)
     || trainerId == 904)
        return sSimTierColorE4;
    // v0.9: Alola kahunas + Alola E4 (Hala, Olivia A., Nanu, Hapu, Molayne, Kahili)
    if (trainerId >= 924 && trainerId <= 929)
        return sSimTierColorE4;
    // v0.10.3: Johto Elite Four HGSS (Will/Koga/Bruno/Karen, IDs 977-980).
    if (trainerId >= 977 && trainerId <= 980)
        return sSimTierColorE4;
    // v0.11: Sinnoh Elite Four Platinum rematch (Aaron/Bertha/Flint/Lucian, 1001-1004).
    if (trainerId >= 1001 && trainerId <= 1004)
        return sSimTierColorE4;
    // v0.12: Sinnoh Elite Four BDSP (Aaron/Bertha/Flint/Lucian, 1017-1020).
    if (trainerId >= 1017 && trainerId <= 1020)
        return sSimTierColorE4;
    // Gym leaders (Hoenn _5 rematches, Kanto HGSS, all PWT leaders incl. v0.10 regions)
    if (trainerId == 773 || trainerId == 777 || trainerId == 781 || trainerId == 785
     || trainerId == 789 || trainerId == 793 || trainerId == 797 || trainerId == 801
     || (trainerId >= 862 && trainerId <= 868)
     || (trainerId >= 869 && trainerId <= 885)
     || (trainerId >= 939 && trainerId <= 962)    // v0.10 Johto/Sinnoh/Unova PWT
     || (trainerId >= 969 && trainerId <= 976)    // v0.10.3 Johto gym HGSS rematches
     || (trainerId >= 988 && trainerId <= 995)    // v0.11 Platinum Battleground gyms
     || (trainerId >= 1009 && trainerId <= 1016)  // v0.12 BDSP gym rematches
     || (trainerId >= 1042 && trainerId <= 1050)) // v0.14 SwSh Galar gym leaders
        return sSimTierColorGym;
    // v0.9: Alola trial captains (Ilima, Lana, Kiawe, Mallow, Sophocles, Acerola, Mina)
    if (trainerId >= 917 && trainerId <= 923)
        return sSimTierColorGym;
    return sSimTierColorNone;
}

// Battle Simulator: short source-tag suffix shown after the trainer name so you can
// see at a glance whether the team is the HGSS rematch import, the BW2 Pokemon
// World Tournament version, or the ORAS late-game team. Trainers without a tag are
// the vanilla Emerald rosters.
static const u8 sSimSourceSuffixHGSS[] = _(" (HGSS)");
static const u8 sSimSourceSuffixFRLG[] = _(" (FRLG)");    // v0.10.2 split off from HGSS
static const u8 sSimSourceSuffixEMR[]  = _(" (EMR)");     // v0.10.2 Emerald base game
static const u8 sSimSourceSuffixPWT[]  = _(" (PWT)");
static const u8 sSimSourceSuffixORAS[] = _(" (ORAS)");
static const u8 sSimSourceSuffixLGPE[] = _(" (LGPE)");
static const u8 sSimSourceSuffixRR[]   = _(" (RR)");      // v0.9 Rainbow Rocket
static const u8 sSimSourceSuffixSM[]   = _(" (SM)");      // v0.9 Sun/Moon Alola
static const u8 sSimSourceSuffixPT[]   = _(" (Pt)");      // v0.11 Platinum (DPPt)
static const u8 sSimSourceSuffixBDSP[] = _(" (BDSP)");    // v0.12 Brilliant Diamond/Shining Pearl
static const u8 sSimSourceSuffixSWSH[] = _(" (SwSh)");    // v0.14 Sword/Shield
static const u8 sSimSourceSuffixBW[]   = _(" (BW)");      // v0.40 Black/White + B2W2
static const u8 sSimSourceSuffixXY[]   = _(" (XY)");      // v0.49 X/Y (Kalos)
static const u8 sSimSourceSuffixAnime[] = _(" (Anime)");  // v0.50 Anime team builds
static const u8 sSimSourceSuffixCustom[] = _(" (Custom)"); // v0.51 user-built trainer slots
static const u8 sSimSourceSuffixVGC[]    = _(" (VGC)");    // v1.5 VGC 2012 finals (Wolfe / Ray)
static const u8 sSimSourceSuffixRGBY[]   = _(" (RGBY)");   // v1.6 Prof. Oak Glitch boss
// v0.41.2: per-variant tags for N / Cheren / Bianca / Hugh so the picker can
// disambiguate the three starter variants at a glance. N's pair distinguishes
// by signature legendary; rivals' three each tag the rival's ace starter
// (the one weak to the player's starter in canon).
static const u8 sSimSourceSuffixBW_NZek[]     = _(" (BW Zekrom)");
static const u8 sSimSourceSuffixBW_NResh[]    = _(" (BW Reshiram)");
static const u8 sSimSourceSuffixBW_Embo[]     = _(" (BW Emboar)");
static const u8 sSimSourceSuffixBW_Samu[]     = _(" (BW Samurott)");
static const u8 sSimSourceSuffixBW_Serp[]     = _(" (BW Serperior)");
static const u8 sSimSourceSuffixBW2_Embo[]    = _(" (BW2 Emboar)");
static const u8 sSimSourceSuffixBW2_Samu[]    = _(" (BW2 Samurott)");
static const u8 sSimSourceSuffixBW2_Serp[]    = _(" (BW2 Serperior)");
static const u8 sSimSourceSuffixNone[] = _("");

static const u8 *GetSimSourceSuffix(u16 trainerId)
{
    // Emerald base-game origin: Hoenn E4 (261-264), Champion Steven (335),
    // Champion Wallace (804), Hoenn gym leader _5 rematches (773-801 odd).
    if ((trainerId >= 261 && trainerId <= 264) || trainerId == 335 || trainerId == 804
     || trainerId == 773 || trainerId == 777 || trainerId == 781 || trainerId == 785
     || trainerId == 789 || trainerId == 793 || trainerId == 797 || trainerId == 801)
        return sSimSourceSuffixEMR;
    // FRLG-ported teams: Indigo E4 rematch (855-858, the Lv 63-72 imports) +
    // Blue's three starter-variant teams (859-861, Lv 72-75). Previously
    // mis-tagged HGSS — the actual HGSS Blue team is at ID 966 now.
    if (trainerId >= 855 && trainerId <= 861)
        return sSimSourceSuffixFRLG;
    // HGSS rematch imports: Red @ Mt. Silver (851), Kanto gym leader HGSS
    // rematches (862-868), Blue HGSS (966), and the full v0.10.3/4 HGSS
    // Indigo Plateau dump — Silver Cyndaquil/Totodile/Chikorita (967-968, 984),
    // Johto gym HGSS rematches (969-976), Johto E4 + Lance HGSS (977-981).
    if (trainerId == 851 || (trainerId >= 862 && trainerId <= 868)
     || (trainerId >= 966 && trainerId <= 981) || trainerId == 984)
        return sSimSourceSuffixHGSS;
    // BW2 Pokemon World Tournament imports.
    if (trainerId >= 869 && trainerId <= 890)
        return sSimSourceSuffixPWT;
    // ORAS post-game teams (Wally, Steven Delta Episode, May, Brendan, Hoenn E4).
    if (trainerId >= 891 && trainerId <= 898)
        return sSimSourceSuffixORAS;
    // Let's Go Pikachu/Eevee rematch teams (Indigo E4, Red, Blue, Bruno, Green,
    // and v0.10.4 Trace LGPE Champion Pikachu/Eevee variants 982-983).
    if ((trainerId >= 899 && trainerId <= 904) || trainerId == 906
     || trainerId == 982 || trainerId == 983)
        return sSimSourceSuffixLGPE;
    // v0.9 Rainbow Rocket bosses (Archie/Maxie/Cyrus/Lysandre/Ghetsis/Giovanni).
    if (trainerId >= 907 && trainerId <= 916)
        return sSimSourceSuffixRR;
    // v0.9 Alola trial captains + kahunas + E4 + champions (+ Kukui/Hau/Gladion variants).
    // v0.48 extends with Faba/Dexio/Plumeria/Ryuki/Guzma/Lusamine/Tristan +
    // Blue/Red USUM Battle Tree + Anabel USUM at 1073-1082.
    if ((trainerId >= 917 && trainerId <= 938)
     || (trainerId >= 1073 && trainerId <= 1082))
        return sSimSourceSuffixSM;
    // v0.10 PWT regional gym leaders + champions (Johto/Sinnoh/Unova).
    if (trainerId >= 939 && trainerId <= 965)
        return sSimSourceSuffixPWT;
    // v0.11 Pokemon Platinum dump (Barry, Battleground gym + stat trainers,
    // Sinnoh E4 rematch, Cynthia rematch). IDs 985-1005.
    if (trainerId >= 985 && trainerId <= 1005)
        return sSimSourceSuffixPT;
    // v0.12 BDSP (Brilliant Diamond/Shining Pearl) — 3 Barry BDSP, 8 gym
    // rematches, 4 Sinnoh E4, Cynthia, 6 Lucas/Dawn rivals (Battleground
    // rematch tier). IDs 1006-1028 with ID 1024 skipped (bad compiler slot).
    if (trainerId >= 1006 && trainerId <= 1028)
        return sSimSourceSuffixBDSP;
    // v0.14 SwSh (Sword/Shield) — Champion Leon variants, Hop variants,
    // Mustard, Marnie, Bede, Galar gym leaders, Klara/Avery/Peony.
    if (trainerId >= 1029 && trainerId <= 1053)
        return sSimSourceSuffixSWSH;
    // v0.40 BW (Black/White + B2W2) — N (Zekrom/Reshiram), Alder, Cheren x3,
    // Bianca x3, Hugh x3 (B2W2), Unova E4 (Shauntal/Marshal/Grimsley/Caitlin),
    // Ghetsis. v0.41 adds Colress (B2W2 Plasma scientist). IDs 1055-1072.
    //
    // v0.41.2: N/Cheren/Bianca/Hugh have multiple variants per character (one
    // per BW starter path) so the generic " (BW)" suffix isn't enough to tell
    // them apart in the picker. Return a per-variant tag for those IDs and
    // keep generic " (BW)" for Alder/E4/Ghetsis/Colress.
    switch (trainerId)
    {
        case 1055: return sSimSourceSuffixBW_NZek;   // N (Zekrom)
        case 1056: return sSimSourceSuffixBW_NResh;  // N (Reshiram)
        case 1058: return sSimSourceSuffixBW_Embo;   // Cheren — Emboar ace
        case 1059: return sSimSourceSuffixBW_Samu;   // Cheren — Samurott ace
        case 1060: return sSimSourceSuffixBW_Serp;   // Cheren — Serperior ace
        case 1061: return sSimSourceSuffixBW_Samu;   // Bianca — Samurott ace
        case 1062: return sSimSourceSuffixBW_Serp;   // Bianca — Serperior ace
        case 1063: return sSimSourceSuffixBW_Embo;   // Bianca — Emboar ace
        case 1064: return sSimSourceSuffixBW2_Embo;  // Hugh — Emboar ace (B2W2)
        case 1065: return sSimSourceSuffixBW2_Samu;  // Hugh — Samurott ace (B2W2)
        case 1066: return sSimSourceSuffixBW2_Serp;  // Hugh — Serperior ace (B2W2)
    }
    if (trainerId >= 1055 && trainerId <= 1072)
        return sSimSourceSuffixBW;
    // v0.49 XY (Kalos) — Diantha, Serena, Calem.
    if (trainerId >= 1083 && trainerId <= 1085)
        return sSimSourceSuffixXY;
    // v0.50 Anime — Ash's World Champion team.
    if (trainerId == 1086)
        return sSimSourceSuffixAnime;
    // v0.51 Custom — user-built trainer slots. v1.1: slots 4-6 (1099-1101)
    // sit AFTER the LA block, so the range is discontiguous.
    if ((trainerId >= TRAINER_SIM_CUSTOM_1 && trainerId <= TRAINER_SIM_CUSTOM_3)
     || (trainerId >= TRAINER_SIM_CUSTOM_4 && trainerId <= TRAINER_SIM_CUSTOM_6))
        return sSimSourceSuffixCustom;
    // v1.5 VGC 2012 World Championships finals (Wolfe Glick + Ray Rizo).
    if (trainerId == TRAINER_WOLFE_VGC2012 || trainerId == TRAINER_RAY_VGC2012)
        return sSimSourceSuffixVGC;
    // v1.6 Prof. Oak Glitch boss — the 3 starter variants.
    if (trainerId >= TRAINER_OAK_GLITCH_VENUSAUR && trainerId <= TRAINER_OAK_GLITCH_BLASTOISE)
        return sSimSourceSuffixRGBY;
    return sSimSourceSuffixNone;
}

// Battle Simulator: curated roster (Emerald-build trainers only).
// Kanto E4 rematch and Blue variants are ported from FRLG data into Emerald
// trainer slots 855-861 (v0.3). Kanto gym leaders skipped — too weak vs Red/Steven.
//
// ORDER (v0.11 reorg) — grouped by GAME ORIGIN so L/R section-jump walks a
// coherent timeline of trainers from the same source instead of bouncing
// between eras. From DPAD-UP starting at Logan:
//   Logan -> LGPE (Kanto remake) -> FRLG (Kanto original) -> HGSS (Johto) ->
//   Emerald (Hoenn base) -> ORAS (Hoenn remake) -> Platinum (Sinnoh) ->
//   Alola (USUM) -> Rainbow Rocket (USUM Episode RR) -> PWT chunk
//   (Kanto / Hoenn / Johto / Sinnoh / Unova / Champs) all consecutively.
static const u16 sSimulatorRoster[] = {
#ifndef RELEASE_BUILD
    // ---- Custom challengers (Logan, Taylor) — dev-only ----
    905,                                                 // Logan (custom Gen-9 challenger)
    1054,                                                // Taylor (rain team, Drake sprite)
#endif
    // ---- LGPE section (Kanto, Let's Go remakes) ----
    906,                                                 // Green LGPE (Mega Blastoise)
    899, 900, 901, 902, 903, 904,                        // Lorelei/Agatha/Lance/Red/Blue/Bruno LGPE
    982, 983,                                            // Trace LGPE Pikachu/Eevee variants
    // ---- FRLG section (Kanto, original, Lv 63-75 ports) ----
    855, 856, 857, 858,                                  // Indigo E4 FRLG rematch
    859, 860, 861,                                       // Blue starter variants
    // ---- HGSS section (Johto/Kanto remake, unified) ----
    851,                                                 // Red HGSS @ Mt. Silver
    966,                                                 // Blue HGSS
    967, 968, 984,                                       // Silver (Cyndaquil/Totodile/Chikorita paths)
    969, 970, 971, 972, 973, 974, 975, 976,              // Johto gym HGSS rematches
    862, 863, 864, 865, 866, 867, 868,                   // Kanto gym HGSS rematches
    977, 978, 979, 980,                                  // Johto Elite Four
    981,                                                 // Lance HGSS Champion
    // ---- Emerald section (Hoenn base game) ----
    773, 777, 781, 785, 789, 793, 797, 801,              // Hoenn gym _5 rematches
    261, 262, 263, 264,                                  // Hoenn E4
    335, 804,                                            // Steven + Wallace champion
    // ---- ORAS section (Hoenn remake post-game) ----
    895, 896, 897, 898,                                  // Hoenn ORAS Elite Four
    894, 893, 892, 891,                                  // Brendan/May/Steven Delta/Wally
    // ---- Platinum section (Sinnoh) ----
    985, 986, 987,                                       // Barry rival variants
    988, 989, 990, 991, 992, 993, 994, 995,              // Battleground gym leaders
    996, 997, 998, 999, 1000,                            // Stat trainers (Cheryl..Buck)
    1001, 1002, 1003, 1004,                              // Sinnoh Elite Four (high lvl)
    1005,                                                // Cynthia Pt rematch
    // ---- BDSP section (Sinnoh remake — adjacent to Platinum) ----
    1006, 1007, 1008,                                    // Barry BDSP (3 starter variants)
    1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016,      // BDSP gym leader rematches
    1017, 1018, 1019, 1020,                              // BDSP Sinnoh Elite Four
    1021,                                                // Cynthia BDSP Champion
    1022, 1023, 1025,                                    // Lucas BDSP (BT/BI/BE — BE skips bad ID 1024)
    1026, 1027, 1028,                                    // Dawn BDSP (BT/BI/BE — shifted +1 around bad slot)
    // ---- SwSh section (Sword/Shield + Isle of Armor + Crown Tundra) ----
    1029, 1030, 1031,                                    // Leon variants (Cinderace/Inteleon/Rillaboom counters)
    1032, 1033, 1034, 1035, 1036, 1037,                  // Hop x6 (Sword/Shield x Grookey/Scorbunny/Sobble)
    1038, 1039,                                          // Mustard Single/Rapid Strike
    1040, 1041,                                          // Marnie, Bede
    1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 1050, // 9 Galar gym leaders
    1051, 1052,                                          // Klara, Avery (Isle of Armor)
    1053,                                                // Peony (Crown Tundra)
    // ---- Alola section (Sun/Moon USUM) ----
    917, 918, 919, 920, 921, 922, 923,                   // Trial captains
    924, 925, 926, 927, 928, 929, 930, 931,              // Kahunas/E4/Champion
    932, 933, 934, 935, 936, 937, 938,                   // Hau/Kukui/Gladion variants
    1073, 1074, 1075, 1076, 1077, 1078, 1079,            // v0.48: Faba/Dexio/Plumeria/Ryuki/Guzma/Lusamine/Tristan
    1080, 1081, 1082,                                    // v0.48: Blue USUM / Red USUM / Anabel USUM
    // ---- Rainbow Rocket section (USUM Episode RR) ----
    907, 908, 909, 910, 911, 912, 913, 914, 915, 916,    // RR bosses
    // ---- PWT chunk (BW2 World Tournament rosters, all together) ----
    869, 870, 871, 872, 873, 874, 875, 876,              // PWT Kanto
    877, 878, 879, 880, 881, 882, 883, 884, 885,         // PWT Hoenn
    939, 940, 941, 942, 943, 944, 945, 946,              // PWT Johto
    947, 948, 949, 950, 951, 952, 953, 954,              // PWT Sinnoh
    955, 956, 957, 958, 959, 960, 961, 962,              // PWT Unova
    1102, 1103, 1104, 1105, 1106,                        // v1.5: Cilan/Chili/Cress/Cheren/Roxie PWT
    890, 886, 887, 888, 889, 963, 964, 965, 1107,        // PWT Champs (+ v1.5 Bianca World Leaders)
    // ---- BW section (v0.40 Black/White + B2W2) ----
    1055, 1056,                                          // N (Zekrom, Reshiram versions)
    1057,                                                // Alder BW (Unova champion)
    1058, 1059, 1060,                                    // Cheren x3 (Emboar/Samurott/Serperior aces)
    1061, 1062, 1063,                                    // Bianca x3 (Samurott/Serperior/Emboar aces)
    1064, 1065, 1066,                                    // Hugh x3 BW2 (Emboar/Samurott/Serperior aces)
    1067, 1068, 1069, 1070,                              // Unova E4 (Shauntal/Marshal/Grimsley/Caitlin)
    1071,                                                // Ghetsis (Team Plasma boss)
    1072,                                                // Colress (B2W2 Plasma scientist, Steel/Electric)
    // ---- XY section (Kalos) ----
    1083, 1084, 1085,                                    // v0.49: Diantha / Serena / Calem
    // ---- Anime section ----
    1086,                                                // v0.50: Ash World Champion (Z + Mega + Gmax)
    // ---- VGC section (2012 World Championships finals exhibition) ----
    1108, 1109,                                          // v1.5: Wolfe Glick (runner-up), Ray Rizo (champion)
    // ---- Custom section (user-built trainers, v0.51 + v1.1) ----
    1087, 1088, 1089,                                    // Custom 1/2/3 (v0.51)
    1099, 1100, 1101,                                    // Custom 4/5/6 (v1.1)
    // ---- Legends Arceus section (v0.53) — placed AFTER custom slots so
    //      adding them didn't shift TRAINER_SIM_CUSTOM_* IDs and break
    //      existing user saveblocks. Volo is the climax sweeper (Giratina-
    //      Origin ace); Adaman + Irida bring Dialga/Palkia-Origin trios;
    //      Ingo runs Hisuian forms; Akari is the PC rival.
    1090, 1091, 1092, 1093, 1094,                        // Volo / Adaman / Irida / Ingo / Akari
    1095, 1096, 1097, 1098,                              // v0.53.2: Kamado / Zisu / Beni / Rei
    // ---- RGBY section (Gen 1 unused content — Prof. Oak Glitch boss) ----
    1110, 1111, 1112,                                    // v1.6: Oak Venusaur/Charizard/Blastoise variants
};
#define SIMULATOR_ROSTER_COUNT (sizeof(sSimulatorRoster) / sizeof(sSimulatorRoster[0]))

// Battle Simulator: tournament cup definitions. Each cup is a curated list of
// trainers the Player AI will fight in single-elimination gauntlet style.
// Names are short for the picker display (≤10 chars to match the in-game
// nickname width budget). The "Off" entry at index 0 means tournament disabled.
struct SimCup
{
    const u8 *name;
    const u16 *trainers;
    u8 size;
};

static const u16 sCupHoenn[]      = { 773, 777, 781, 785, 789, 793, 797, 801 };           // Hoenn gym leader _5 rematches
static const u16 sCupKantoHGSS[]  = { 862, 863, 864, 865, 866, 867, 868 };                // 7 Kanto HGSS gym leaders
static const u16 sCupPwtKanto[]   = { 869, 870, 871, 872, 873, 874, 875, 876 };           // PWT Kanto leaders
static const u16 sCupPwtHoenn[]   = { 877, 878, 879, 880, 881, 882, 883, 884 };           // PWT Hoenn leaders (8 of 9)
static const u16 sCupOras[]       = { 891, 892, 893, 894, 895, 896, 897, 898 };           // Wally, Steven, May, Brendan, Sidney, Phoebe, Glacia, Drake
static const u16 sCupLgpe[]       = { 899, 900, 901, 902, 903, 904, 906 };                // Lorelei, Agatha, Lance, Red, Blue, Bruno, Green
static const u16 sCupChampions[]  = { 335, 804, 851, 886, 888, 889, 890, 902 };           // Steven, Wallace, Red, Blue PWT, Steven PWT, Wallace PWT, Red PWT, Red LGPE
static const u16 sCupIndigoE4[]   = { 855, 856, 857, 858, 859, 860, 861, 851 };           // Indigo E4 + Blue variants + Red
// v0.9 NEW CUPS
static const u16 sCupRRocket[]    = { 907, 908, 909, 910, 911, 913, 915, 916 };           // RR bosses: Archie, Maxie, Cyrus US/UM, Lysandre US, Ghetsis US, Giovanni US/UM
static const u16 sCupAlolaTrial[] = { 917, 918, 919, 920, 921, 922, 923 };                // Alola Trial Captains (Ilima, Lana, Kiawe, Mallow, Sophocles, Acerola, Mina)
static const u16 sCupAlolaLeague[] = { 924, 925, 926, 927, 928, 929, 930, 931 };          // Alola Kahunas+E4+Champion (Hala, Olivia A., Nanu, Hapu, Molayne, Kahili, Kukui, Hau)
// v0.10 PWT CHAMPIONS — the eight PWT-format champion teams in one bracket.
// Red (890), Blue (886), Lance (887), Steven (888), Wallace (889) from the
// original PWT specials, plus v0.10 additions Cynthia (963), Iris (964),
// Alder (965). Builder shuffles for random pairings each run.
static const u16 sCupPwtChamps[]  = { 890, 886, 887, 888, 889, 963, 964, 965 };

// v0.11 PLATINUM cups — Sinnoh Battleground gym leaders (8) and a Platinum
// "League" cup mixing the high-level Sinnoh E4 + Cynthia rematch + Barry.
static const u16 sCupPlatGym[]    = { 988, 989, 990, 991, 992, 993, 994, 995 };  // Roark..Volkner
static const u16 sCupPlatLeague[] = { 1001, 1002, 1003, 1004, 1005,              // Aaron, Bertha, Flint, Lucian, Cynthia
                                      986, 998, 1000 };                          // Barry-C, Riley, Buck

// v0.12 BDSP cups — BDSP Battleground gym leaders (8) + BDSP League (4 E4 +
// Cynthia + Barry-BC + Lucas-BC + Dawn-BC).
static const u16 sCupBdspGym[]    = { 1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016 };
static const u16 sCupBdspLeague[] = { 1017, 1018, 1019, 1020, 1021,             // Aaron, Bertha, Flint, Lucian, Cynthia BDSP
                                      1007, 1023, 1026 };                       // Barry-BC, Lucas-BC, Dawn-BC

// v0.14 SwSh cups — Galar gym leaders (8 of 9, Milo + Nessa + Kabu + Bea +
// Allister + Gordie + Piers + Raihan; Melony swapped in for Allister/Gordie
// in alt cup) and SwSh Champions (3 Leon variants + Mustard ×2 + Hop ×3).
static const u16 sCupSwshGym[]    = { 1042, 1043, 1044, 1045, 1046, 1047, 1049, 1050 };  // Milo, Nessa, Kabu, Bea, Allister, Gordie, Piers, Raihan
static const u16 sCupSwshChamps[] = { 1029, 1030, 1031,                                  // Leon-C / Leon-I / Leon-R
                                      1038, 1039,                                        // Mustard-S / Mustard-R
                                      1032, 1034, 1036 };                                // Hop-SWR / Hop-SWC / Hop-SWI

// v0.11 HGSS pool cup — 25-trainer pool covering Red, Blue HGSS, all three
// Silvers, Johto gym rematches, Kanto gym HGSS, Johto E4, and Lance HGSS.
// Builder picks 7 random per run so each tournament has a different roster.
static const u16 sCupHgssPool[]   = { 851, 966, 967, 968, 984,                   // Red, Blue HGSS, Silvers
                                      969, 970, 971, 972, 973, 974, 975, 976,    // Johto gym (Falkner..Clair)
                                      862, 863, 864, 865, 866, 867, 868,         // Kanto HGSS gym
                                      977, 978, 979, 980,                        // Johto E4
                                      981 };                                     // Lance HGSS

// v0.10 PWT REGIONAL CUPS (each region has its own 8-leader tournament).
// Johto PWT (8 leaders): Falkner, Bugsy, Whitney, Morty, Chuck, Jasmine, Pryce, Clair
static const u16 sCupPwtJohto[]   = { 946, 945, 944, 943, 942, 941, 940, 939 };
// Sinnoh PWT (8 leaders): Roark, Gardenia, Maylene, Wake, Fantina, Byron, Candice, Volkner
static const u16 sCupPwtSinnoh[]  = { 948, 949, 954, 951, 953, 950, 952, 947 };
// Unova PWT (8 leaders): Lenora, Burgh, Elesa, Clay, Skyla, Brycen, Drayden, Marlon
// v1.5 — PWT Unova converted to pool format (13 trainers, picks 7 random per
// run). Adds Cilan/Chili/Cress/Cheren/Roxie to the previously-curated 8-slot
// roster so the Striaton trio's first-gym presence finally shows up in PWT
// brackets, and players see different mixes of the 11 canonical BW2 Unova
// leaders each tournament.
static const u16 sCupPwtUnova[]   = {
    955, 956, 957, 958, 959, 960, 961, 962,         // Lenora/Burgh/Elesa/Clay/Skyla/Brycen/Drayden/Marlon
    1102, 1103, 1104, 1105, 1106,                   // Cilan/Chili/Cress/Cheren/Roxie (v1.5)
};
// PWT World Leaders: pool of ALL PWT gym leaders + champions across every
// region (Kanto + Hoenn + Johto + Sinnoh + Unova). Sim_BuildTournamentBracket
// Fisher-Yates shuffles the pool and picks 7 random candidates per run, so
// every World Leaders tournament has a different roster.
// v1.5 — pool grew 43 → 49 with the Striaton trio + Cheren + Roxie + Bianca's
// World Leaders Tournament entry. Bianca slots in with the Unova block.
static const u16 sCupPwtWorld[]   = {
    869, 870, 871, 872, 873, 874, 875, 876,         // Kanto PWT (8)
    877, 878, 879, 880, 881, 882, 883, 884,         // Hoenn PWT (8)
    939, 940, 941, 942, 943, 944, 945, 946,         // Johto PWT (8)
    947, 948, 949, 950, 951, 952, 953, 954,         // Sinnoh PWT (8)
    955, 956, 957, 958, 959, 960, 961, 962,         // Unova PWT (8)
    1102, 1103, 1104, 1105, 1106, 1107,             // v1.5: Cilan/Chili/Cress/Cheren/Roxie/Bianca (6)
    963, 964, 965,                                  // Champions (Cynthia/Iris/Alder)
};

static const u8 sCupName_Off[]      = _("Off");
static const u8 sCupName_Hoenn[]    = _("Hoenn");
static const u8 sCupName_KantoHGSS[] = _("Kanto HGSS");
static const u8 sCupName_PwtKanto[] = _("PWT Kanto");
static const u8 sCupName_PwtHoenn[] = _("PWT Hoenn");
static const u8 sCupName_Oras[]     = _("ORAS");
static const u8 sCupName_Lgpe[]     = _("LGPE");
static const u8 sCupName_Champions[] = _("Champions");
static const u8 sCupName_Indigo[]   = _("Indigo E4");
// v0.9 NEW CUP NAMES (≤10 chars each for picker display)
static const u8 sCupName_RRocket[]      = _("R Rocket");
static const u8 sCupName_AlolaTrial[]   = _("Alola Trial");
static const u8 sCupName_AlolaLeague[]  = _("Alola E4");
static const u8 sCupName_PwtWorld[]     = _("PWT World");
static const u8 sCupName_PwtJohto[]     = _("PWT Johto");
static const u8 sCupName_PwtSinnoh[]    = _("PWT Sinnoh");
static const u8 sCupName_PwtUnova[]     = _("PWT Unova");
static const u8 sCupName_PwtChamps[]    = _("PWT Champs");
static const u8 sCupName_PlatGym[]      = _("Pt Gym");
static const u8 sCupName_PlatLeague[]   = _("Pt League");
static const u8 sCupName_HgssPool[]     = _("HGSS Pool");
static const u8 sCupName_BdspGym[]      = _("BDSP Gym");
static const u8 sCupName_BdspLeague[]   = _("BDSP Leage");
static const u8 sCupName_SwshGym[]      = _("SwSh Gym");
static const u8 sCupName_SwshChamps[]   = _("SwSh Champs");

static const struct SimCup sSimCups[] =
{
    { sCupName_Off,       NULL,            0 },   // 0: Off
    { sCupName_Hoenn,     sCupHoenn,       8 },
    { sCupName_KantoHGSS, sCupKantoHGSS,   7 },
    { sCupName_PwtKanto,  sCupPwtKanto,    8 },
    { sCupName_PwtHoenn,  sCupPwtHoenn,    8 },
    { sCupName_Oras,      sCupOras,        8 },
    { sCupName_Lgpe,      sCupLgpe,        7 },
    { sCupName_Champions, sCupChampions,   8 },
    { sCupName_Indigo,      sCupIndigoE4,      8 },
    // ---- v0.9 NEW CUPS ----
    { sCupName_RRocket,     sCupRRocket,       8 },
    { sCupName_AlolaTrial,  sCupAlolaTrial,    7 },
    { sCupName_AlolaLeague, sCupAlolaLeague,   8 },
    // ---- v0.10 PWT regional cups + expanded World pool ----
    { sCupName_PwtJohto,    sCupPwtJohto,      8 },
    { sCupName_PwtSinnoh,   sCupPwtSinnoh,     8 },
    // v1.5 — PWT Unova bumped 8 → 13 (added Striaton trio + Cheren + Roxie).
    // Count >7 triggers Sim_BuildTournamentBracket's pool-shuffle path, so
    // each tournament run picks 7 random opponents from the 13-trainer pool.
    { sCupName_PwtUnova,    sCupPwtUnova,     13 },
    { sCupName_PwtChamps,   sCupPwtChamps,     8 },
    // PWT World Leaders — 49-entry pool (all PWT leaders + Cynthia/Iris/Alder
    // + v1.5 Striaton trio / Cheren / Roxie / Bianca), builder picks 7 random
    // per run.
    { sCupName_PwtWorld,    sCupPwtWorld,     49 },
    // v0.11 Platinum + HGSS cups
    { sCupName_PlatGym,     sCupPlatGym,       8 },     // Sinnoh Battleground gym leaders
    { sCupName_PlatLeague,  sCupPlatLeague,    8 },     // Sinnoh E4 + Cynthia + Barry-C + Riley/Buck
    { sCupName_HgssPool,    sCupHgssPool,     25 },     // 25-trainer HGSS pool, picks 7 random
    // v0.12 BDSP cups
    { sCupName_BdspGym,     sCupBdspGym,       8 },     // BDSP Battleground gym leaders
    { sCupName_BdspLeague,  sCupBdspLeague,    8 },     // BDSP E4 + Cynthia + Barry/Lucas/Dawn rivals
    // v0.14 SwSh cups
    { sCupName_SwshGym,     sCupSwshGym,       8 },     // 8 Galar gym leaders
    { sCupName_SwshChamps,  sCupSwshChamps,    8 },     // 3 Leon + 2 Mustard + 3 Hop
    // ---- v0.10+ DEFERRED CUPS ----
    // PWT Type Experts cup — user dumped ~324 Pokemon (54 unlabeled teams).
    // Needs disambiguation before adding. Targeting v0.10.
    //     { sCupName_PwtTypes,    sCupPwtTypes,     8 },
};
#define SIM_CUP_COUNT (sizeof(sSimCups) / sizeof(sSimCups[0]))

// ============================================================================
// v0.9 PREP: Flag-Space Expansion Plan (added in v0.8 polish pass)
// ============================================================================
// Three big trainer dumps are queued for v0.9 (~75-80 new trainer entries):
//   - PWT Type Experts (~35-40 trainers, one set of 8 per type tournament)
//   - Alola League (~30 trainers — Kahunas, Trial Captains, E4, Champion)
//   - Rainbow Rocket boss roster (~10 trainers — 6 bosses + lieutenants)
//
// The blocker is that each trainer eats one FLAG_TRAINER_DEFEATED slot.
// As of v0.8 the budget is:
//   TRAINERS_COUNT_EMERALD     = 907    (next free ID)
//   MAX_TRAINERS_COUNT_EMERALD = 916    (hard ceiling before flag overflow)
//   slack                      = 9      (NOT enough for 80 new trainers)
//
// Expansion plan for v0.9 (do this FIRST before adding any trainers):
//   1) Bump MAX_TRAINERS_COUNT_EMERALD in include/constants/opponents.h
//        916 -> 1024 (or 1100 for headroom). This implicitly grows
//        TRAINER_FLAGS_END from 0x893 to 0x8FF (or higher).
//   2) Stale comment cleanup in include/constants/flags.h:
//        - Line 1340 ("0x500 - 0x85F, the last 9 of which are unused") is
//          already wrong; the macro is correct, but update the doc string
//          to reflect the new range and new slack count.
//        - The "FLAG_UNUSED_0x9XX" symbolic names in the SYSTEM region are
//          based on stale absolute addresses; the actual addresses shift by
//          (new_MAX - 916) when we bump MAX_TRAINERS_COUNT_EMERALD. The
//          names are aliases only — no code references the hex part.
//   3) SaveBlock1.flags[] grows by ROUND_BITS_TO_BYTES(delta) bytes.
//        For 916 -> 1024 that is ROUND_BITS_TO_BYTES(108) = 14 bytes.
//        Verify total SaveBlock1 size still fits 0x10000 (it does — we
//        have ~5KB headroom in v0.8).
//   4) Sprite gap: most Alola/PWT/Rainbow Rocket characters lack Emerald
//      OW + frontpic sprites. Two viable options:
//        a) Reuse closest base-game class sprite (Brendan for Hau, Lass
//           for Mallow, Beauty for Olivia, etc.). Simpler, no asset work.
//        b) Port sprites from pokefirered + DPPt resource dumps. More work
//           but more visual variety. Defer to v1.0 polish pass.
//      Default choice for v0.9: option (a).
//   5) After expansion, add trainers in this order so the IDs cluster
//      cleanly and the cup arrays stay contiguous:
//        907..946  PWT Type Experts (40 slots, one set of 8 per cup
//                  iteration, 5 of the 17 types fit; rotate the cup roster)
//        947..976  Alola League (30 slots — Kahunas 4, Captains 7,
//                  E4 4, Champion 1, plus Hau/Lillie/Kukui rematches)
//        977..986  Rainbow Rocket bosses (10 slots — Archie, Maxie,
//                  Cyrus, Lysandre, Ghetsis, Giovanni + 2 lieutenants
//                  each at lower tier)
//        987..1023 reserved for v1.0+ (Galar, Paldea, Unova, etc.)
//
// Risks / verifications before merging the expansion:
//   - grep for hardcoded trainer flag values (none expected; all flags use
//     macros). Sanity check: rg "0x8[0-9A-F]{2}" include/ src/
//   - Verify Battle Dome data path isn't hit at runtime (user already
//     confirmed we can break it).
//   - Test save migration: v0.8 saves WILL be invalidated by the
//     SaveBlock1.flags[] shift. Document this in the changelog and
//     instruct testers to clear save data on first v0.9 boot.
//
// ============================================================================

// *******************************
// Constants
#define DEBUG_MENU_FONT FONT_NORMAL

#define DEBUG_MENU_WIDTH_MAIN 17
#define DEBUG_MENU_HEIGHT_MAIN 9

#define DEBUG_MENU_WIDTH_EXTRA 10
#define DEBUG_MENU_HEIGHT_EXTRA 4

#define DEBUG_MENU_WIDTH_WEATHER 15
#define DEBUG_MENU_HEIGHT_WEATHER 3

#define DEBUG_MENU_WIDTH_SOUND 20
#define DEBUG_MENU_HEIGHT_SOUND 6

#define DEBUG_MENU_WIDTH_FLAGVAR 4
#define DEBUG_MENU_HEIGHT_FLAGVAR 2

#define DEBUG_NUMBER_DIGITS_FLAGS 4
#define DEBUG_NUMBER_DIGITS_VARIABLES 5
#define DEBUG_NUMBER_DIGITS_VARIABLE_VALUE 5
#define DEBUG_NUMBER_DIGITS_ITEMS 4
#define DEBUG_NUMBER_DIGITS_ITEM_QUANTITY 3
#define DEBUG_NUMBER_DIGITS_LOCALID 2
#define DEBUG_NUMBER_DIGITS_TRAINERS MAX_DIGITS(TRAINERS_COUNT)

#define DEBUG_NUMBER_ICON_X 210
#define DEBUG_NUMBER_ICON_Y 50

#define DEBUG_MAX_MENU_ITEMS 20
// v0.52.6 — Bumped from 4 to 8 because the Build Trainer hierarchy now nests
// 5 levels deep (Wrapper → BuildTrainer → Slot → Mon → EVs/IVs), and
// Debug_SaveCallbackMenu was silently failing at depth 5, leaving the
// dispatcher reading actions from the wrong menu's items array. Symptom was
// that opening EVs/IVs would display the right labels but the A button
// would fire whichever action lived at the same row index in the parent
// menu (e.g. "HP" would open the Species picker).
//
// Each level costs sizeof(const struct DebugMenuOption *) = 4 bytes, so
// 8 levels = 32 bytes of sDebugMenuListData. Negligible.
#define DEBUG_MAX_SUB_MENU_LEVELS 8

// *******************************
struct DebugMenuOption;

typedef void (*DebugFunc)(u8 taskId);
typedef void (*DebugSubmenuFunc)(u8 taskId, const struct DebugMenuOption *items);

struct DebugMenuOption
{
    const u8 *text;
    const void *action;
    const void *actionParams;
};

struct DebugMonData
{
    u16 species;
    u8 level;
    bool8 isShiny:1;
    u8 nature:5;
    u8 abilityNum:2;
    u8 monIVs[NUM_STATS];
    u16 monMoves[MAX_MON_MOVES];
    u8 monEVs[NUM_STATS];
    u8 teraType;
    u8 dynamaxLevel:7;
    u8 gmaxFactor:1;
};

struct DebugMenuListData
{
    const struct DebugMenuOption *subMenuItems[DEBUG_MAX_SUB_MENU_LEVELS];
    struct ListMenuItem listItems[DEBUG_MAX_MENU_ITEMS + 1];
    // v1.1: bumped 26 → 40. Player AI line "Player AI: {COLOR YELLOW}WALLACE (EMRQ)"
    // is ~28 bytes (3-byte color escape + 7-char name + 7-char source suffix),
    // overflowed the 26-byte slot and corrupted the next entry. 40 bytes covers
    // even worst-case lines like "Co-op Partner: {COLOR YELLOW}HUGH SCHOOLMATE (B2W2)".
    u8 itemNames[DEBUG_MAX_MENU_ITEMS + 1][40];
    u8 listId;
    s16 data[8];
};

// EWRAM
static EWRAM_DATA struct DebugMonData *sDebugMonData = NULL;
static EWRAM_DATA struct DebugMenuListData *sDebugMenuListData = NULL;
EWRAM_DATA bool8 gIsDebugBattle = FALSE;
EWRAM_DATA u64 gDebugAIFlags = 0;
EWRAM_DATA bool8 gSimAutoOpenPending = FALSE;
// v1.7 — Frontier Challenge post-warp fixup (see include/debug.h).
EWRAM_DATA bool8 gSimFrontierChallengePending = FALSE;
// v1.19 — STICKY flag: stays TRUE for the entire Frontier Challenge
// session (unlike gSimFrontierChallengePending which is one-shot and
// clears after the first field tick). Used by start_menu.c to show
// a "Sim Menu" entry in START menu while the player's in Frontier
// Challenge, letting them re-open the wrapper without soft resetting.
// Cleared at boot via EWRAM zero-init or when the user explicitly
// exits back to the wrapper menu.
EWRAM_DATA bool8 gSimFrontierChallengeActive = FALSE;
// v1.8 — buffer for the borrowed trainer name. The Frontier Challenge
// flow writes the trainer's name into gSaveBlock2Ptr->playerName, but
// the map-load process (MoveSaveBlocks_ResetHeap + map-script
// initialization) appears to either reset playerName from a snapshot
// taken pre-write, or shadow it via a saveblock-rebase race. The field
// tick after the warp lands re-applies the borrowed name from this
// buffer to guarantee it sticks. Cleared at boot via EWRAM zero-init.
EWRAM_DATA u8 gSimFrontierBorrowedName[PLAYER_NAME_LENGTH + 1] = {0};
// v0.52.5 — pending re-open of the Build Trainer slot menu after returning
// from DoNamingScreen. sBuildTrainerActiveSlot already persists in EWRAM so
// the slot index round-trips automatically.
EWRAM_DATA bool8 gSimBuildTrainerReopenSlot = FALSE;
// v1.3 — set TRUE before DoNamingScreen(NAMING_SCREEN_TEAMCODE, ...) launches.
// field_control_avatar's input poll sees this flag, reads sSimTeamCodeBuffer,
// runs Sim_DecodeTeamCode, copies the result into sBuildTrainerWorkMon, and
// re-opens the per-mon editor. Separate flag from gSimBuildTrainerReopenSlot
// because the destination menu (Mon editor vs Slot menu) differs.
EWRAM_DATA bool8 gSimImportCodePending = FALSE;
// Buffer the naming screen writes into. Up to 30 chars + EOS + slack.
EWRAM_DATA u8 gSimTeamCodeBuffer[32] = {0};
// v0.52.15 — TRUE while a sim battle is running in pilot mode. battle_main.c
// reads this to apply the level cap, since the AI-vs-AI flag (the cap's
// existing gate) is intentionally skipped in pilot mode.
EWRAM_DATA bool8 gSimPilotMode = FALSE;
EWRAM_DATA s16 gSimLevelCap = 0; // First-run init to 50 happens in Debug_ShowTrainersSubMenu. .sbss only allows zero initializers.

// Battle Simulator: best-of-N match state. gSimBestOf is the configured length
// (0/1 = single battle, 3 = best of 3, 5 = best of 5). EWRAM globals on GBA can
// only zero-initialize in .sbss, so 0 doubles as the default "Off" value and the
// display/toggle treat anything <= 1 as a single battle. The sSimMatch* values
// are captured at the start of round 1 so CB2_EndDebugBattle's rematch dispatch
// can re-launch the same matchup for round 2/3 without bouncing back to the picker.
EWRAM_DATA u8 gSimBestOf = 0;
EWRAM_DATA u8 gSimT1Wins = 0;
EWRAM_DATA u8 gSimT2Wins = 0;
static EWRAM_DATA u16 sSimMatchOpponent1 = 0;
static EWRAM_DATA u16 sSimMatchOpponent2 = 0;
static EWRAM_DATA u16 sSimMatchPartner = 0;
static EWRAM_DATA u16 sSimMatchPlayerAI = 0;
static EWRAM_DATA bool8 sSimMatchActive = FALSE;
// v0.52.13 — cache forceDouble + pilotMode at round 1 setup so best-of-N
// rounds 2+ have stable config. sDebugMenuListData is Free'd by
// Debug_DestroyMenu_Full after round 1, leaving a dangling pointer; reading
// data[5]/data[7] off that pointer gives whatever garbage now sits at that
// heap address, randomly flipping doubles + pilot mode on every rematch.
static EWRAM_DATA bool8 sSimMatchForceDouble = FALSE;
static EWRAM_DATA bool8 sSimMatchPilotMode = FALSE;

// Battle Simulator: VGC mode. When enabled the picker forces doubles, applies a
// Lv 50 cap to all sim mons, and limits each side to 4 mons-in-battle (bring 6
// pick 4 — the standard VGC format).
EWRAM_DATA bool8 gSimVGCMode = FALSE;

// Battle Simulator: tournament mode — 8-trainer single-elimination bracket.
// Player AI plays 3 matches max (QF → SF → Finals). The other 4 first-round
// matches and the simulated SF/F outcomes happen "off-screen" — winners are
// picked at tournament start with a small skill weighting using the trainer
// tier from Sim_GetTier (Champion > E4 > Gym > default), plus randomness so
// upsets can still happen. Lose any of your 3 matches → eliminated.
//
// Bracket slot layout (typical single-elim, 8 slots, 7 matches):
//   Slot 0 ─┐
//           ├ M0 (QF) ─┐
//   Slot 1 ─┘          ├ M4 (SF) ─┐
//   Slot 2 ─┐          │           │
//           ├ M1 (QF) ─┘           ├ M6 (FINAL)
//   Slot 3 ─┘                      │
//                                  │
//   Slot 4 ─┐                      │
//           ├ M2 (QF) ─┐           │
//   Slot 5 ─┘          ├ M5 (SF) ─┘
//   Slot 6 ─┐          │
//           ├ M3 (QF) ─┘
//   Slot 7 ─┘
//
// gSimTournamentCup: 0 = off, 1..N = selected cup index into sSimCups
// gSimTournamentRound: 0 = not started, 1 = QF, 2 = SF, 3 = Final (player's round)
// gSimTournamentMaxRounds: always 3 for 8-person single-elim
// gSimTournamentBracket[8]: the 8 trainers, indexed by bracket slot
// gSimTournamentResults[7]: which SLOT won each of the 7 bracket matches
//                         (set as the tournament progresses; 0xFF = TBD)
// gSimTournamentPlayerSlot: the bracket slot the Player AI occupies (0-7)
// gSimTournamentDone: TRUE once the tournament resolves (champion or eliminated)
// gSimTournamentEliminated: TRUE if the player AI lost a round
#define SIM_TOURNAMENT_BRACKET_SIZE 8
#define SIM_TOURNAMENT_MATCH_COUNT  7
#define SIM_TOURNAMENT_SLOT_TBD     0xFF
EWRAM_DATA u8 gSimTournamentCup = 0;
EWRAM_DATA u8 gSimTournamentRound = 0;
EWRAM_DATA u8 gSimTournamentMaxRounds = 0; // 0 = treated as default 3 when armed; init must be 0 (.sbss rule)
EWRAM_DATA u16 gSimTournamentBracket[SIM_TOURNAMENT_BRACKET_SIZE] = {0};
EWRAM_DATA u8 gSimTournamentResults[SIM_TOURNAMENT_MATCH_COUNT] = {0};
EWRAM_DATA u8 gSimTournamentPlayerSlot = 0;
EWRAM_DATA bool8 gSimTournamentDone = FALSE;
EWRAM_DATA bool8 gSimTournamentEliminated = FALSE;

// Battle Simulator: team-preview pick queue. Sim_PrepareTeamPicks runs once per
// match-round setup, computing which N mons each AI brings (out of their 6)
// against the opposing teamsheet. Each call to CreateNPCTrainerPartyFromTrainer
// consumes the next row. The "active" flag gates the whole feature off in
// non-AI-vs-AI battles so we don't accidentally rewrite vanilla trainer parties.
#define SIM_PICK_QUEUE_MAX 4
#define SIM_PICK_INDICES_MAX 6
EWRAM_DATA u8 gSimPickIndices[SIM_PICK_QUEUE_MAX][SIM_PICK_INDICES_MAX] = {0};
EWRAM_DATA u8 gSimPickCounts[SIM_PICK_QUEUE_MAX] = {0};
EWRAM_DATA u8 gSimPickQueueHead = 0;   // next row to consume
EWRAM_DATA u8 gSimPickQueueLen = 0;    // total rows populated
EWRAM_DATA bool8 gSimPickQueueActive = FALSE;

// Battle Simulator: adaptive picking memory. Each row remembers a trainer ID
// and the indices that trainer brought in their last battle. When a trainer
// is about to fight again (Best-Of-N round 2+, or any subsequent matchup in
// a session), the scoring engine looks them up and scores against the
// SUBSET they actually brought — not their full 6-mon teamsheet. So a BO3
// Game 2 becomes a "read-and-counter" mind game: each AI brings the 4 mons
// that best counter what their opponent showed in Game 1.
static EWRAM_DATA u16 sSimPrevPickTrainerIds[SIM_PICK_QUEUE_MAX] = {0};
static EWRAM_DATA u8 sSimPrevPickIndices[SIM_PICK_QUEUE_MAX][SIM_PICK_INDICES_MAX] = {0};
static EWRAM_DATA u8 sSimPrevPickCounts[SIM_PICK_QUEUE_MAX] = {0};
static EWRAM_DATA u8 sSimPrevPickRowCount = 0;
static EWRAM_DATA bool8 sSimPrevPicksValid = FALSE;
static EWRAM_DATA u16 sSimCurrentPickTrainerIds[SIM_PICK_QUEUE_MAX] = {0};

// *******************************
// Define functions
static void Debug_ShowMenu(DebugFunc HandleInput, const struct DebugMenuOption *items);
static u8  Debug_GenerateListTrainerMenu(void);
static u8 Debug_GenerateListMenuNames(void);
static void Debug_DestroyMenu(u8 taskId);
static void DebugAction_Cancel(u8 taskId);
static void DebugAction_DestroyExtraWindow(u8 taskId);
static void Debug_RefreshListMenu(u8 taskId);
static u8 DebugNativeStep_CreateDebugWindow(void);
static void DebugNativeStep_CloseDebugWindow(u8 taskId);

static void DebugAction_OpenSubMenu(u8 taskId, const struct DebugMenuOption *items);
static void DebugAction_OpenSubMenuTrainers(u8 taskId, const struct DebugMenuOption *items);
// v0.52 Phase 2 — Build Trainer menu skeleton
static void DebugAction_OpenSubMenuBuildTrainer(u8 taskId, const struct DebugMenuOption *items);
static void DebugAction_BuildTrainer_OpenSlot1(u8 taskId);
static void DebugAction_BuildTrainer_OpenSlot2(u8 taskId);
static void DebugAction_BuildTrainer_OpenSlot3(u8 taskId);
// v1.1 — 3 additional custom slots (community ask for more save room).
static void DebugAction_BuildTrainer_OpenSlot4(u8 taskId);
static void DebugAction_BuildTrainer_OpenSlot5(u8 taskId);
static void DebugAction_BuildTrainer_OpenSlot6(u8 taskId);
static void DebugAction_BuildTrainer_ResetSlot(u8 taskId);
static void DebugAction_BuildTrainer_SetAllLvl50(u8 taskId);  // v1.15
static void DebugAction_BuildTrainer_BackToWrapper(u8 taskId);
// v0.52.5 — slot-level identity (trainer name + sprite class). Name uses
// DoNamingScreen and returns via a field auto-reopen flag; sprite uses the
// scrollable-picker pattern (mirror of the species picker).
static void DebugAction_BuildTrainer_EditName(u8 taskId);
static void DebugAction_BuildTrainer_EditTrainerPic(u8 taskId);
// v0.52.2 Phase 2b — Per-mon editor
static void DebugAction_BuildTrainer_OpenMon1(u8 taskId);
static void DebugAction_BuildTrainer_OpenMon2(u8 taskId);
static void DebugAction_BuildTrainer_OpenMon3(u8 taskId);
static void DebugAction_BuildTrainer_OpenMon4(u8 taskId);
static void DebugAction_BuildTrainer_OpenMon5(u8 taskId);
static void DebugAction_BuildTrainer_OpenMon6(u8 taskId);
static void DebugAction_BuildTrainer_SaveSlot(u8 taskId);
static void DebugAction_BuildTrainer_MonSaveBack(u8 taskId);
static void DebugAction_BuildTrainer_MonCancel(u8 taskId);
// v1.3 — Showdown team-code import. Pushes the user into the naming screen
// (NAMING_SCREEN_TEAMCODE template) with a 30-char buffer; on confirm,
// field_control_avatar runs Sim_DecodeTeamCode and re-opens the per-mon
// editor with imported values.
static void DebugAction_BuildTrainer_OpenImportCode(u8 taskId);
static void DebugAction_BuildTrainer_EditLevel(u8 taskId);
static void DebugAction_BuildTrainer_EditNature(u8 taskId);
static void DebugAction_BuildTrainer_EditGender(u8 taskId);
static void DebugAction_BuildTrainer_EditAbility(u8 taskId);
static void DebugAction_BuildTrainer_OpenEVsMenu(u8 taskId);
static void DebugAction_BuildTrainer_OpenIVsMenu(u8 taskId);
static void DebugAction_BuildTrainer_EditShiny(u8 taskId);
// v0.52.3 Phase 2c — Scrollable pickers (PxHex-style hierarchical menus)
static void DebugAction_BuildTrainer_EditSpecies(u8 taskId);
static void DebugAction_BuildTrainer_EditHeldItem(u8 taskId);
static void DebugAction_BuildTrainer_EditMove1(u8 taskId);
static void DebugAction_BuildTrainer_EditMove2(u8 taskId);
static void DebugAction_BuildTrainer_EditMove3(u8 taskId);
static void DebugAction_BuildTrainer_EditMove4(u8 taskId);
// v0.52.4 — EV/IV per-stat editors. Each stat row has its own cycle handler
// (+4 EV, +1 IV with wrap to 0 when overflowing the cap). Reset / Max All /
// Back close-out actions handle bulk edits.
static void DebugAction_BuildTrainer_EditEV_HP(u8 taskId);
static void DebugAction_BuildTrainer_EditEV_Atk(u8 taskId);
static void DebugAction_BuildTrainer_EditEV_Def(u8 taskId);
static void DebugAction_BuildTrainer_EditEV_SpA(u8 taskId);
static void DebugAction_BuildTrainer_EditEV_SpD(u8 taskId);
static void DebugAction_BuildTrainer_EditEV_Spe(u8 taskId);
static void DebugAction_BuildTrainer_EVs_Reset(u8 taskId);
static void DebugAction_BuildTrainer_EVs_Back(u8 taskId);
static void DebugAction_BuildTrainer_EditIV_HP(u8 taskId);
static void DebugAction_BuildTrainer_EditIV_Atk(u8 taskId);
static void DebugAction_BuildTrainer_EditIV_Def(u8 taskId);
static void DebugAction_BuildTrainer_EditIV_SpA(u8 taskId);
static void DebugAction_BuildTrainer_EditIV_SpD(u8 taskId);
static void DebugAction_BuildTrainer_EditIV_Spe(u8 taskId);
static void DebugAction_BuildTrainer_IVs_MaxAll(u8 taskId);
static void DebugAction_BuildTrainer_IVs_ZeroAll(u8 taskId);
static void DebugAction_BuildTrainer_IVs_Back(u8 taskId);
static u8 Debug_GenerateListBuildTrainerSlotMenu(void);
static u8 Debug_GenerateListBuildTrainerMonMenu(void);
static u8 Debug_GenerateListBuildTrainerEVsMenu(void);
static u8 Debug_GenerateListBuildTrainerIVsMenu(void);

// v0.52.3 — Per-slot listIds for dynamic value display in the Build Trainer
// menus (PxHex-style "Species: Pikachu / Level: 50 / Nature: Adamant").
// Dispatched from Debug_ShowMenu's listId switch so each redraw reflects the
// current sBuildTrainerWorkMon state.
#define DEBUG_LISTID_BUILD_TRAINER_SLOT 3
#define DEBUG_LISTID_BUILD_TRAINER_MON  4
// v0.52.4 — Sub-menus for per-stat EV and IV editing.
#define DEBUG_LISTID_BUILD_TRAINER_EVS  5
#define DEBUG_LISTID_BUILD_TRAINER_IVS  6

static EWRAM_DATA u8 sBuildTrainerActiveSlot = 0;  // 0-5 = which slot is being edited (v1.1 bumped 0-2 → 0-5)
static EWRAM_DATA u8 sBuildTrainerActiveMon = 0;   // 0-5 = which Pokémon in slot
static EWRAM_DATA struct SimCustomTrainerMon sBuildTrainerWorkMon = {0};  // working buffer; committed on Save
// v0.52.3 — Picker state. Set when a picker opens so Cancel (B) can restore the
// pre-edit value. Move pickers also stash which move slot (0-3) is being edited.
static EWRAM_DATA u16 sBuildTrainerPickerOriginal = 0;
static EWRAM_DATA u8  sBuildTrainerActiveMoveSlot = 0;
static void DebugAction_OpenSubMenuFlagsVars(u8 taskId, const struct DebugMenuOption *items);
static void DebugAction_OpenSubMenuFakeRTC(u8 taskId, const struct DebugMenuOption *items);
static void DebugAction_OpenSubMenuCreateFollowerNPC(u8 taskId, const struct DebugMenuOption *items);
static void DebugAction_ExecuteScript(u8 taskId, const u8 *script);
static void DebugAction_ToggleFlag(u8 taskId);

static void DebugTask_HandleMenuInput_General(u8 taskId);

static void DebugAction_Util_Fly(u8 taskId);
static void DebugAction_Util_Warp_Warp(u8 taskId);
static void DebugAction_Util_Warp_SelectMapGroup(u8 taskId);
static void DebugAction_Util_Warp_SelectMap(u8 taskId);
static void DebugAction_Util_Warp_SelectWarp(u8 taskId);
static void DebugAction_Util_Weather(u8 taskId);
static void DebugAction_Util_Weather_SelectId(u8 taskId);
static void DebugAction_Util_WatchCredits(u8 taskId);
static void DebugAction_Util_CheatStart(u8 taskId);

static void DebugAction_TimeMenu_ChangeTimeOfDay(u8 taskId);
static void DebugAction_TimeMenu_ChangeWeekdays(u8 taskId);

static void DebugAction_CreateFollowerNPC(u8 taskId);
static void DebugAction_DestroyFollowerNPC(u8 taskId);

static void DebugAction_PCBag_Fill_PCBoxes_Fast(u8 taskId);
static void DebugAction_PCBag_Fill_PCBoxes_Slow(u8 taskId);
static void DebugAction_PCBag_Fill_PCItemStorage(u8 taskId);
static void DebugAction_PCBag_Fill_PocketItems(u8 taskId);
static void DebugAction_PCBag_Fill_PocketPokeBalls(u8 taskId);
static void DebugAction_PCBag_Fill_PocketTMHM(u8 taskId);
static void DebugAction_PCBag_Fill_PocketBerries(u8 taskId);
static void DebugAction_PCBag_Fill_PocketKeyItems(u8 taskId);
static void DebugAction_PCBag_ClearBag(u8 taskId);
static void DebugAction_PCBag_ClearBoxes(u8 taskId);

static void DebugAction_Party_HealParty(u8 taskId);
static void DebugAction_Party_ClearPokerus(u8 taskId);
static void DebugAction_Party_ClearParty(u8 taskId);
static void DebugAction_Party_SetParty(u8 taskId);
static void DebugAction_Party_BattleSingle(u8 taskId);

static void DebugAction_Trainers_ChooseFromMap(u8 taskId);
static void DebugAction_Trainers_ChooseTrainer(u8 taskId, u32 selection);
static void DebugAction_Trainers_SwitchDoublesFlag(u8 taskId);
static void DebugAction_Trainers_TogglePilotMode(u8 taskId);  // v0.51.1
static void DebugAction_Trainers_ToggleLevelCap(u8 taskId);
static void DebugAction_Trainers_ToggleBestOf(u8 taskId);
static void DebugAction_Trainers_ToggleVGC(u8 taskId);
static void DebugAction_Trainers_ToggleTournament(u8 taskId);
static bool32 Sim_IsBestOfActive(void);
static bool32 Sim_IsMatchDecided(void);
static bool32 Sim_IsTournamentActive(void);
static bool32 Sim_IsTournamentDone(void);
static void Sim_StartTournament(s32 playerSideId);
static u8 Sim_GetPlayerOpponentSlot(void);
static u8 Sim_GetPlayerMatchIdx(void);
void Sim_TriggerNextMatchRound(void);
static void DebugAction_Trainers_SetRematch(u8 taskId);
static void DebugAction_Trainers_SetRematchReadiness(u8 taskId);
static void DebugAction_Trainers_TryBattle(u8 taskId);
static void DebugAction_Trainers_RechargeVsSeeker(u8 taskId);
static void Sim_StartFrontierChallenge(s32 trainerId);  // v1.7

static void DebugAction_FlagsVars_Flags(u8 taskId);
static void DebugAction_FlagsVars_FlagsSelect(u8 taskId);
static void DebugAction_FlagsVars_Vars(u8 taskId);
static void DebugAction_FlagsVars_Select(u8 taskId);
static void DebugAction_FlagsVars_SetValue(u8 taskId);
static void DebugAction_FlagsVars_PokedexFlags_All(u8 taskId);
static void DebugAction_FlagsVars_PokedexFlags_Reset(u8 taskId);
static void DebugAction_FlagsVars_SwitchDex(u8 taskId);
static void DebugAction_FlagsVars_SwitchNatDex(u8 taskId);
static void DebugAction_FlagsVars_SwitchPokeNav(u8 taskId);
static void DebugAction_FlagsVars_SwitchMatchCall(u8 taskId);
static void DebugAction_FlagsVars_ToggleFlyFlags(u8 taskId);
static void DebugAction_FlagsVars_ToggleBadgeFlags(u8 taskId);
static void DebugAction_FlagsVars_ToggleGameClear(u8 taskId);
static void DebugAction_FlagsVars_ToggleFrontierPass(u8 taskId);
static void DebugAction_FlagsVars_CollisionOnOff(u8 taskId);
static void DebugAction_FlagsVars_EncounterOnOff(u8 taskId);
static void DebugAction_FlagsVars_TrainerSeeOnOff(u8 taskId);
static void DebugAction_FlagsVars_BagUseOnOff(u8 taskId);
static void DebugAction_FlagsVars_CatchingOnOff(u8 taskId);
static void DebugAction_FlagsVars_RunningShoes(u8 taskId);

static void DebugAction_Give_Item(u8 taskId);
static void DebugAction_Give_Item_SelectId(u8 taskId);
static void DebugAction_Give_Item_SelectQuantity(u8 taskId);
static void DebugAction_Give_PokemonSimple(u8 taskId);
static void DebugAction_Give_PokemonComplex(u8 taskId);
static void DebugAction_Give_NewEgg(u8 taskId);
static void DebugAction_Give_Pokemon_SelectId(u8 taskId);
static void DebugAction_Give_Pokemon_SelectLevel(u8 taskId);
static void DebugAction_Give_Pokemon_SelectShiny(u8 taskId);
static void DebugAction_Give_Pokemon_SelectNature(u8 taskId);
static void DebugAction_Give_Pokemon_SelectAbility(u8 taskId);
static void DebugAction_Give_Pokemon_SelectTeraType(u8 taskId);
static void DebugAction_Give_Pokemon_SelectDynamaxLevel(u8 taskId);
static void DebugAction_Give_Pokemon_SelectGigantamaxFactor(u8 taskId);
static void DebugAction_Give_Pokemon_SelectIVs(u8 taskId);
static void DebugAction_Give_Pokemon_SelectEVs(u8 taskId);
static void DebugAction_Give_Pokemon_ComplexCreateMon(u8 taskId);
static void DebugAction_Give_Pokemon_Move(u8 taskId);
static void DebugAction_Give_Decoration(u8 taskId);
static void DebugAction_Give_Decoration_SelectId(u8 taskId);
static void DebugAction_Give_MaxMoney(u8 taskId);
static void DebugAction_Give_MaxCoins(u8 taskId);
static void DebugAction_Give_MaxBattlePoints(u8 taskId);
static void DebugAction_Give_DayCareEgg(u8 taskId);

static void DebugAction_Sound_SE(u8 taskId);
static void DebugAction_Sound_SE_SelectId(u8 taskId);
static void DebugAction_Sound_MUS(u8 taskId);
static void DebugAction_Sound_MUS_SelectId(u8 taskId);

static void DebugAction_BerryFunctions_ClearAll(u8 taskId);
static void DebugAction_BerryFunctions_Ready(u8 taskId);
static void DebugAction_BerryFunctions_NextStage(u8 taskId);
static void DebugAction_BerryFunctions_Pests(u8 taskId);
static void DebugAction_BerryFunctions_Weeds(u8 taskId);

static void DebugAction_Player_Name(u8 taskId);
static void DebugAction_Player_Gender(u8 taskId);
static void DebugAction_Player_Id(u8 taskId);

extern const u8 Debug_FlagsNotSetOverworldConfigMessage[];
extern const u8 Debug_FlagsNotSetBattleConfigMessage[];
extern const u8 Debug_VarsNotSetBattleConfigMessage[];
extern const u8 Debug_FlagsAndVarNotSetBattleConfigMessage[];
extern const u8 Debug_EventScript_FontTest[];
extern const u8 Debug_EventScript_CheckEVs[];
extern const u8 Debug_EventScript_CheckIVs[];
extern const u8 Debug_EventScript_GivePokerus[];
extern const u8 Debug_EventScript_InflictStatus1[];
extern const u8 Debug_EventScript_KoPokemon[];
extern const u8 Debug_EventScript_SetHiddenNature[];
extern const u8 Debug_EventScript_SetAbility[];
extern const u8 Debug_EventScript_SetFriendship[];
extern const u8 Debug_EventScript_Script_1[];
extern const u8 Debug_EventScript_Script_2[];
extern const u8 Debug_EventScript_Script_3[];
extern const u8 Debug_EventScript_Script_4[];
extern const u8 Debug_EventScript_Script_5[];
extern const u8 Debug_EventScript_Script_6[];
extern const u8 Debug_EventScript_Script_7[];
extern const u8 Debug_EventScript_Script_8[];
extern const u8 DebugScript_DaycareMonsNotCompatible[];
extern const u8 DebugScript_OneDaycareMons[];
extern const u8 DebugScript_ZeroDaycareMons[];

extern const u8 Debug_ShowFieldMessageStringVar4[];
extern const u8 Debug_CheatStart[];
extern const u8 Debug_CheatStartFrlg[];
extern const u8 Debug_HatchAnEgg[];
extern const u8 PlayersHouse_2F_EventScript_SetWallClock[];
extern const u8 PlayersHouse_2F_EventScript_CheckWallClock[];
extern const u8 Debug_CheckSaveBlock[];
extern const u8 Debug_CheckROMSpace[];
extern const u8 Debug_BoxFilledMessage[];
extern const u8 Debug_ShowExpansionVersion[];
extern const u8 Debug_EventScript_EWRAMCounters[];
extern const u8 Debug_Follower_NPC_Event_Script[];
extern const u8 Debug_Follower_NPC_Not_Enabled[];
extern const u8 Debug_EventScript_Steven_Multi[];
extern const u8 Debug_EventScript_WallyTutorial[];
extern const u8 Debug_EventScript_PrintTimeOfDay[];
extern const u8 Debug_EventScript_TellTheTime[];
extern const u8 Debug_EventScript_FakeRTCNotEnabled[];

extern const u8 Debug_BerryPestsDisabled[];
extern const u8 Debug_BerryWeedsDisabled[];

extern const u8 Common_EventScript_MoveRelearner[];

#include "data/map_group_count.h"

// Text
// General
static const u8 sDebugText_Arrow[] =          _("{CLEAR_TO 110}{RIGHT_ARROW}");
static const u8 sDebugText_True[] =          _("TRUE");
static const u8 sDebugText_False[] =         _("FALSE");
static const u8 sDebugText_Colored_True[] =  _("{COLOR GREEN}TRUE");
static const u8 sDebugText_Colored_False[] = _("{COLOR RED}FALSE");
static const u8 sDebugText_Dashes[] =        _("---");
static const u8 sDebugText_Empty[] =         _("");
static const u8 sDebugText_Continue[] =      _("Continue…");
// Util Menu
static const u8 sDebugText_Util_WarpToMap_SelectMapGroup[] = _("Group: {STR_VAR_1}{CLEAR_TO 90}\n{CLEAR_TO 90}\n\n{STR_VAR_3}{CLEAR_TO 90}");
static const u8 sDebugText_Util_WarpToMap_SelectMap[] =      _("Map: {STR_VAR_1}{CLEAR_TO 90}\nMapSec:{CLEAR_TO 90}\n{STR_VAR_2}{CLEAR_TO 90}\n{STR_VAR_3}{CLEAR_TO 90}");
static const u8 sDebugText_Util_WarpToMap_SelectWarp[] =     _("Warp:{CLEAR_TO 90}\n{STR_VAR_1}{CLEAR_TO 90}\n{CLEAR_TO 90}\n{STR_VAR_3}{CLEAR_TO 90}");
static const u8 sDebugText_Util_WarpToMap_SelMax[] =         _("{STR_VAR_1} / {STR_VAR_2}");
static const u8 sDebugText_Util_Weather_ID[] =               _("Weather ID: {STR_VAR_3}\n{STR_VAR_1}\n{STR_VAR_2}");

//Time Menu

static const u8 *const gDayNameStringsTable[WEEKDAY_COUNT] = {
    COMPOUND_STRING("Sunday"),
    COMPOUND_STRING("Monday"),
    COMPOUND_STRING("Tuesday"),
    COMPOUND_STRING("Wednesday"),
    COMPOUND_STRING("Thursday"),
    COMPOUND_STRING("Friday"),
    COMPOUND_STRING("Saturday"),
};

static const u8 *const gTimeOfDayStringsTable[TIMES_OF_DAY_COUNT] = {
    COMPOUND_STRING("Morning"),
    COMPOUND_STRING("Day"),
    COMPOUND_STRING("Evening"),
    COMPOUND_STRING("Night"),
};

// Follower NPC

static const u8 *const gFollowerNPCStringsTable[DEBUG_FNPC_COUNT] = {
    COMPOUND_STRING("Brendan"),
    COMPOUND_STRING("May"),
    COMPOUND_STRING("Steven"),
    COMPOUND_STRING("Wally"),
    COMPOUND_STRING("Red"),
    COMPOUND_STRING("Leaf"),
};

// Flags/Vars Menu
static const u8 sDebugText_FlagsVars_VariableHex[] =         _("{STR_VAR_1}{CLEAR_TO 90}\n0x{STR_VAR_2}{CLEAR_TO 90}");
static const u8 sDebugText_FlagsVars_Variable[] =            _("Var: {STR_VAR_1}{CLEAR_TO 90}\nVal: {STR_VAR_3}{CLEAR_TO 90}\n{STR_VAR_2}");
static const u8 sDebugText_FlagsVars_VariableValueSet[] =    _("Var: {STR_VAR_1}{CLEAR_TO 90}\nVal: {STR_VAR_3}{CLEAR_TO 90}\n{STR_VAR_2}");
// Give Menu
static const u8 sDebugText_PokemonShiny[] =             _("Shiny:{CLEAR_TO 90}\n   {STR_VAR_2}{CLEAR_TO 90}\n{CLEAR_TO 90}\n{CLEAR_TO 90}");
static const u8 sDebugText_IVs[] =                      _("IV {STR_VAR_1}:{CLEAR_TO 90}\n    {STR_VAR_3}{CLEAR_TO 90}\n{CLEAR_TO 90}\n{STR_VAR_2}{CLEAR_TO 90}");
static const u8 sDebugText_EVs[] =                      _("EV {STR_VAR_1}:{CLEAR_TO 90}\n    {STR_VAR_3}{CLEAR_TO 90}\n{CLEAR_TO 90}\n{STR_VAR_2}{CLEAR_TO 90}");
// Sound Menu
static const u8 sDebugText_Sound_SFX_ID[] =             _("SFX ID: {STR_VAR_3}   {START_BUTTON} Stop\n{STR_VAR_1}    \n{STR_VAR_2}");
static const u8 sDebugText_Sound_Music_ID[] =           _("Music ID: {STR_VAR_3}   {START_BUTTON} Stop\n{STR_VAR_1}    \n{STR_VAR_2}");

const u8 *const gText_DigitIndicator[] =
{
    COMPOUND_STRING("{LEFT_ARROW}+1{RIGHT_ARROW}        "),
    COMPOUND_STRING("{LEFT_ARROW}+10{RIGHT_ARROW}       "),
    COMPOUND_STRING("{LEFT_ARROW}+100{RIGHT_ARROW}      "),
    COMPOUND_STRING("{LEFT_ARROW}+1000{RIGHT_ARROW}     "),
    COMPOUND_STRING("{LEFT_ARROW}+10000{RIGHT_ARROW}    "),
    COMPOUND_STRING("{LEFT_ARROW}+100000{RIGHT_ARROW}   "),
    COMPOUND_STRING("{LEFT_ARROW}+1000000{RIGHT_ARROW}  "),
    COMPOUND_STRING("{LEFT_ARROW}+10000000{RIGHT_ARROW} "),
};

static const s32 sPowersOfTen[] =
{
             1,
            10,
           100,
          1000,
         10000,
        100000,
       1000000,
      10000000,
     100000000,
    1000000000,
};

// *******************************
// Menu Actions. Make sure that submenus are defined before the menus that call them.
static const struct DebugMenuOption sDebugMenu_Actions_TimeMenu_TimesOfDay[] =
{
    [TIME_MORNING] = { gTimeOfDayStringsTable[TIME_MORNING], DebugAction_TimeMenu_ChangeTimeOfDay },
    [TIME_DAY]     = { gTimeOfDayStringsTable[TIME_DAY],     DebugAction_TimeMenu_ChangeTimeOfDay },
    [TIME_EVENING] = { gTimeOfDayStringsTable[TIME_EVENING], DebugAction_TimeMenu_ChangeTimeOfDay },
    [TIME_NIGHT]   = { gTimeOfDayStringsTable[TIME_NIGHT],   DebugAction_TimeMenu_ChangeTimeOfDay },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_TimeMenu_Weekdays[] =
{
    [WEEKDAY_SUN] = { gDayNameStringsTable[WEEKDAY_SUN], DebugAction_TimeMenu_ChangeWeekdays },
    [WEEKDAY_MON] = { gDayNameStringsTable[WEEKDAY_MON], DebugAction_TimeMenu_ChangeWeekdays },
    [WEEKDAY_TUE] = { gDayNameStringsTable[WEEKDAY_TUE], DebugAction_TimeMenu_ChangeWeekdays },
    [WEEKDAY_WED] = { gDayNameStringsTable[WEEKDAY_WED], DebugAction_TimeMenu_ChangeWeekdays },
    [WEEKDAY_THU] = { gDayNameStringsTable[WEEKDAY_THU], DebugAction_TimeMenu_ChangeWeekdays },
    [WEEKDAY_FRI] = { gDayNameStringsTable[WEEKDAY_FRI], DebugAction_TimeMenu_ChangeWeekdays },
    [WEEKDAY_SAT] = { gDayNameStringsTable[WEEKDAY_SAT], DebugAction_TimeMenu_ChangeWeekdays },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_FollowerNPCMenu_Create[] =
{
    [DEBUG_FNPC_BRENDAN] = { gFollowerNPCStringsTable[DEBUG_FNPC_BRENDAN], DebugAction_CreateFollowerNPC },
    [DEBUG_FNPC_MAY] =     { gFollowerNPCStringsTable[DEBUG_FNPC_MAY],     DebugAction_CreateFollowerNPC },
    [DEBUG_FNPC_STEVEN] =  { gFollowerNPCStringsTable[DEBUG_FNPC_STEVEN],  DebugAction_CreateFollowerNPC },
    [DEBUG_FNPC_WALLY] =   { gFollowerNPCStringsTable[DEBUG_FNPC_WALLY],   DebugAction_CreateFollowerNPC },
    [DEBUG_FNPC_RED] =     { gFollowerNPCStringsTable[DEBUG_FNPC_RED],     DebugAction_CreateFollowerNPC },
    [DEBUG_FNPC_LEAF] =    { gFollowerNPCStringsTable[DEBUG_FNPC_LEAF],    DebugAction_CreateFollowerNPC },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_TimeMenu[] =
{
    { COMPOUND_STRING("Get time…"),         DebugAction_ExecuteScript, Debug_EventScript_TellTheTime },
    { COMPOUND_STRING("Get time of day…"),  DebugAction_ExecuteScript, Debug_EventScript_PrintTimeOfDay },
    { COMPOUND_STRING("Set time of day…"),  DebugAction_OpenSubMenuFakeRTC, sDebugMenu_Actions_TimeMenu_TimesOfDay },
    { COMPOUND_STRING("Set weekday…"),      DebugAction_OpenSubMenuFakeRTC, sDebugMenu_Actions_TimeMenu_Weekdays },
    { COMPOUND_STRING("Check wall clock…"), DebugAction_ExecuteScript, PlayersHouse_2F_EventScript_CheckWallClock },
    { COMPOUND_STRING("Set wall clock…"),   DebugAction_ExecuteScript, PlayersHouse_2F_EventScript_SetWallClock },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_BerryFunctions[] =
{
    { COMPOUND_STRING("Clear map trees"),      DebugAction_BerryFunctions_ClearAll },
    { COMPOUND_STRING("Ready map trees"),      DebugAction_BerryFunctions_Ready },
    { COMPOUND_STRING("Grow map trees"),       DebugAction_BerryFunctions_NextStage },
    { COMPOUND_STRING("Give map trees pests"), DebugAction_BerryFunctions_Pests },
    { COMPOUND_STRING("Give map trees weeds"), DebugAction_BerryFunctions_Weeds },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_FollowerNPCMenu[] =
{
    { COMPOUND_STRING("Create Follower"),  DebugAction_OpenSubMenuCreateFollowerNPC, sDebugMenu_Actions_FollowerNPCMenu_Create },
    { COMPOUND_STRING("Destroy Follower"), DebugAction_DestroyFollowerNPC },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_Utilities[] =
{
    { COMPOUND_STRING("Fly to map…"),       DebugAction_Util_Fly },
    { COMPOUND_STRING("Warp to map warp…"), DebugAction_Util_Warp_Warp },
    { COMPOUND_STRING("Set weather…"),      DebugAction_Util_Weather },
    { COMPOUND_STRING("Font Test…"),        DebugAction_ExecuteScript, Debug_EventScript_FontTest },
    { COMPOUND_STRING("Time Functions…"),   DebugAction_OpenSubMenu, sDebugMenu_Actions_TimeMenu, },
    { COMPOUND_STRING("Watch credits…"),    DebugAction_Util_WatchCredits },
    { COMPOUND_STRING("Cheat start"),       DebugAction_Util_CheatStart },
    { COMPOUND_STRING("Berry Functions…"),  DebugAction_OpenSubMenu, sDebugMenu_Actions_BerryFunctions },
    { COMPOUND_STRING("EWRAM Counters…"),   DebugAction_ExecuteScript, Debug_EventScript_EWRAMCounters },
    { COMPOUND_STRING("Follower NPC…"),     DebugAction_OpenSubMenu, sDebugMenu_Actions_FollowerNPCMenu },
    { COMPOUND_STRING("Wally Tutorial"),    DebugAction_ExecuteScript, Debug_EventScript_WallyTutorial },
    { COMPOUND_STRING("Steven Multi"),      DebugAction_ExecuteScript, Debug_EventScript_Steven_Multi },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_PCBag_Fill[] =
{
    { COMPOUND_STRING("Fill PC Boxes Fast"),        DebugAction_PCBag_Fill_PCBoxes_Fast },
    { COMPOUND_STRING("Fill PC Boxes Slow (LAG!)"), DebugAction_PCBag_Fill_PCBoxes_Slow },
    { COMPOUND_STRING("Fill PC Items") ,            DebugAction_PCBag_Fill_PCItemStorage },
    { COMPOUND_STRING("Fill Pocket Items"),         DebugAction_PCBag_Fill_PocketItems },
    { COMPOUND_STRING("Fill Pocket Poké Balls"),    DebugAction_PCBag_Fill_PocketPokeBalls },
    { COMPOUND_STRING("Fill Pocket TMHM"),          DebugAction_PCBag_Fill_PocketTMHM },
    { COMPOUND_STRING("Fill Pocket Berries"),       DebugAction_PCBag_Fill_PocketBerries },
    { COMPOUND_STRING("Fill Pocket Key Items"),     DebugAction_PCBag_Fill_PocketKeyItems },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_PCBag[] =
{
    { COMPOUND_STRING("Access PC"),           DebugAction_ExecuteScript, EventScript_PC },
    { COMPOUND_STRING("Fill…"),               DebugAction_OpenSubMenu, sDebugMenu_Actions_PCBag_Fill },
    { COMPOUND_STRING("Clear Bag"),           DebugAction_PCBag_ClearBag },
    { COMPOUND_STRING("Clear Storage Boxes"), DebugAction_PCBag_ClearBoxes },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_EditPokemon[] =
{
    { COMPOUND_STRING("Inflict Status1"),    DebugAction_ExecuteScript, Debug_EventScript_InflictStatus1 },
    { COMPOUND_STRING("Faint Pokemon"),      DebugAction_ExecuteScript, Debug_EventScript_KoPokemon },
    { COMPOUND_STRING("Set Hidden Nature"),  DebugAction_ExecuteScript, Debug_EventScript_SetHiddenNature },
    { COMPOUND_STRING("Set Friendship"),     DebugAction_ExecuteScript, Debug_EventScript_SetFriendship },
    { COMPOUND_STRING("Set Ability"),        DebugAction_ExecuteScript, Debug_EventScript_SetAbility },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_Party[] =
{
    { COMPOUND_STRING("Move Relearner"),     DebugAction_ExecuteScript, Common_EventScript_MoveRelearner },
    { COMPOUND_STRING("Hatch an Egg"),       DebugAction_ExecuteScript, Debug_HatchAnEgg },
    { COMPOUND_STRING("Heal party"),         DebugAction_Party_HealParty },
    { COMPOUND_STRING("Edit Pokemon"),       DebugAction_OpenSubMenu, sDebugMenu_Actions_EditPokemon },
    { COMPOUND_STRING("Check EVs"),          DebugAction_ExecuteScript, Debug_EventScript_CheckEVs },
    { COMPOUND_STRING("Check IVs"),          DebugAction_ExecuteScript, Debug_EventScript_CheckIVs },
    { COMPOUND_STRING("Give Pokerus"),       DebugAction_ExecuteScript, Debug_EventScript_GivePokerus },
    { COMPOUND_STRING("Clear Pokerus"),      DebugAction_Party_ClearPokerus},
    { COMPOUND_STRING("Clear Party"),        DebugAction_Party_ClearParty },
    { COMPOUND_STRING("Set Party"),          DebugAction_Party_SetParty },
    { COMPOUND_STRING("Start Debug Battle"), DebugAction_Party_BattleSingle },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_Give[] =
{
    { COMPOUND_STRING("Give item XYZ…"),    DebugAction_Give_Item },
    { COMPOUND_STRING("Pokémon (Basic)"),   DebugAction_Give_PokemonSimple },
    { COMPOUND_STRING("Pokémon (Complex)"), DebugAction_Give_PokemonComplex },
    { COMPOUND_STRING("Give Egg"),          DebugAction_Give_NewEgg },
    { COMPOUND_STRING("Give Decoration…"),  DebugAction_Give_Decoration },
    { COMPOUND_STRING("Max Money"),         DebugAction_Give_MaxMoney },
    { COMPOUND_STRING("Max Coins"),         DebugAction_Give_MaxCoins },
    { COMPOUND_STRING("Max Battle Points"), DebugAction_Give_MaxBattlePoints },
    { COMPOUND_STRING("Daycare Egg"),       DebugAction_Give_DayCareEgg },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_Player[] =
{
    { COMPOUND_STRING("Player name"),    DebugAction_Player_Name },
    { COMPOUND_STRING("Toggle gender"),  DebugAction_Player_Gender },
    { COMPOUND_STRING("New Trainer ID"), DebugAction_Player_Id },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_Scripts[] =
{
    { COMPOUND_STRING("Script 1"), DebugAction_ExecuteScript, Debug_EventScript_Script_1 },
    { COMPOUND_STRING("Script 2"), DebugAction_ExecuteScript, Debug_EventScript_Script_2 },
    { COMPOUND_STRING("Script 3"), DebugAction_ExecuteScript, Debug_EventScript_Script_3 },
    { COMPOUND_STRING("Script 4"), DebugAction_ExecuteScript, Debug_EventScript_Script_4 },
    { COMPOUND_STRING("Script 5"), DebugAction_ExecuteScript, Debug_EventScript_Script_5 },
    { COMPOUND_STRING("Script 6"), DebugAction_ExecuteScript, Debug_EventScript_Script_6 },
    { COMPOUND_STRING("Script 7"), DebugAction_ExecuteScript, Debug_EventScript_Script_7 },
    { COMPOUND_STRING("Script 8"), DebugAction_ExecuteScript, Debug_EventScript_Script_8 },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_Trainers[] =
{
    { COMPOUND_STRING("{COLOR LIGHT_RED}BATTLE THEATER"), DebugAction_Trainers_ChooseFromMap },
    { COMPOUND_STRING("Player AI: {STR_VAR_1}"), DebugAction_Trainers_ChooseTrainer, (void *)TRAINERS_DEBUG_SELECTION_PLAYER},
    { COMPOUND_STRING("Opponent: {STR_VAR_1}"), DebugAction_Trainers_ChooseTrainer, (void *)TRAINERS_DEBUG_SELECTION_TRAINER1},
    { COMPOUND_STRING("Opponent 2: {STR_VAR_1}"), DebugAction_Trainers_ChooseTrainer, (void *)TRAINERS_DEBUG_SELECTION_TRAINER2},
    { COMPOUND_STRING("Co-op Partner: {STR_VAR_1}"), DebugAction_Trainers_ChooseTrainer,  (void *)TRAINERS_DEBUG_SELECTION_PARTNER},
    { COMPOUND_STRING("Level Cap: {STR_VAR_1}"), DebugAction_ToggleFlag, DebugAction_Trainers_ToggleLevelCap },
    { COMPOUND_STRING("Best Of: {STR_VAR_1}"), DebugAction_ToggleFlag, DebugAction_Trainers_ToggleBestOf },
    { COMPOUND_STRING("VGC Mode: {STR_VAR_1}"), DebugAction_ToggleFlag, DebugAction_Trainers_ToggleVGC },
    { COMPOUND_STRING("Tournament: {STR_VAR_1}"), DebugAction_ToggleFlag, DebugAction_Trainers_ToggleTournament },
    { COMPOUND_STRING("Double Battle: {STR_VAR_1}"), DebugAction_ToggleFlag, DebugAction_Trainers_SwitchDoublesFlag },
    { COMPOUND_STRING("Pilot Player: {STR_VAR_1}"), DebugAction_ToggleFlag, DebugAction_Trainers_TogglePilotMode },
    { COMPOUND_STRING("Matches {STR_VAR_1}/{STR_VAR_2}"), DebugAction_ToggleFlag, DebugAction_Trainers_SetRematch },
    { COMPOUND_STRING("Rematch Ready {STR_VAR_1}"), DebugAction_ToggleFlag, DebugAction_Trainers_SetRematchReadiness },
    { COMPOUND_STRING("Try Battle"), DebugAction_Trainers_TryBattle },
    { COMPOUND_STRING("Recharge VS Seeker"), DebugAction_Trainers_RechargeVsSeeker },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_Sound[] =
{
    { COMPOUND_STRING("SFX…"),   DebugAction_Sound_SE },
    { COMPOUND_STRING("Music…"), DebugAction_Sound_MUS },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_ROMInfo2[] =
{
    { COMPOUND_STRING("Save Block space"),  DebugAction_ExecuteScript, Debug_CheckSaveBlock },
    { COMPOUND_STRING("ROM space"),         DebugAction_ExecuteScript, Debug_CheckROMSpace },
    { COMPOUND_STRING("Expansion Version"), DebugAction_ExecuteScript, Debug_ShowExpansionVersion },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_Flags[] =
{
    [DEBUG_FLAGVAR_MENU_ITEM_FLAGS]                = { COMPOUND_STRING("Set Flag XYZ…"),                     DebugAction_FlagsVars_Flags },
    [DEBUG_FLAGVAR_MENU_ITEM_VARS]                 = { COMPOUND_STRING("Set Var XYZ…"),                      DebugAction_FlagsVars_Vars },
    [DEBUG_FLAGVAR_MENU_ITEM_DEXFLAGS_ALL]         = { COMPOUND_STRING("Pokédex Flags All"),                 DebugAction_FlagsVars_PokedexFlags_All },
    [DEBUG_FLAGVAR_MENU_ITEM_DEXFLAGS_RESET]       = { COMPOUND_STRING("Pokédex Flags Reset"),               DebugAction_FlagsVars_PokedexFlags_Reset },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_POKEDEX]       = { COMPOUND_STRING("Toggle {STR_VAR_1}Pokédex"),         DebugAction_ToggleFlag, DebugAction_FlagsVars_SwitchDex },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_NATDEX]        = { COMPOUND_STRING("Toggle {STR_VAR_1}National Dex"),    DebugAction_ToggleFlag, DebugAction_FlagsVars_SwitchNatDex },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_POKENAV]       = { COMPOUND_STRING("Toggle {STR_VAR_1}PokéNav"),         DebugAction_ToggleFlag, DebugAction_FlagsVars_SwitchPokeNav },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_MATCH_CALL]    = { COMPOUND_STRING("Toggle {STR_VAR_1}Match Call"),      DebugAction_ToggleFlag, DebugAction_FlagsVars_SwitchMatchCall },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_RUN_SHOES]     = { COMPOUND_STRING("Toggle {STR_VAR_1}Running Shoes"),   DebugAction_ToggleFlag, DebugAction_FlagsVars_RunningShoes },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_LOCATIONS]     = { COMPOUND_STRING("Toggle {STR_VAR_1}Fly Flags"),       DebugAction_ToggleFlag, DebugAction_FlagsVars_ToggleFlyFlags },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_BADGES_ALL]    = { COMPOUND_STRING("Toggle {STR_VAR_1}All badges"),      DebugAction_ToggleFlag, DebugAction_FlagsVars_ToggleBadgeFlags },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_GAME_CLEAR]    = { COMPOUND_STRING("Toggle {STR_VAR_1}Game clear"),      DebugAction_ToggleFlag, DebugAction_FlagsVars_ToggleGameClear },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_FRONTIER_PASS] = { COMPOUND_STRING("Toggle {STR_VAR_1}Frontier Pass"),   DebugAction_ToggleFlag, DebugAction_FlagsVars_ToggleFrontierPass },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_COLLISION]     = { COMPOUND_STRING("Toggle {STR_VAR_1}Collision OFF"),   DebugAction_ToggleFlag, DebugAction_FlagsVars_CollisionOnOff },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_ENCOUNTER]     = { COMPOUND_STRING("Toggle {STR_VAR_1}Encounter OFF"),   DebugAction_ToggleFlag, DebugAction_FlagsVars_EncounterOnOff },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_TRAINER_SEE]   = { COMPOUND_STRING("Toggle {STR_VAR_1}Trainer See OFF"), DebugAction_ToggleFlag, DebugAction_FlagsVars_TrainerSeeOnOff },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_CATCHING]      = { COMPOUND_STRING("Toggle {STR_VAR_1}Catching OFF"),    DebugAction_ToggleFlag, DebugAction_FlagsVars_CatchingOnOff },
    [DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_BAG_USE]       = { COMPOUND_STRING("Toggle {STR_VAR_1}Bag Use OFF"),     DebugAction_ToggleFlag, DebugAction_FlagsVars_BagUseOnOff },
    { NULL }
};

static const u8 *const sDebugMenu_Actions_BagUse_Options[] =
{
    COMPOUND_STRING("No Bag: {STR_VAR_1}Inactive"),
    COMPOUND_STRING("No Bag: {STR_VAR_1}VS Trainers"),
    COMPOUND_STRING("No Bag: {STR_VAR_1}Active"),
    COMPOUND_STRING("No Bag: {STR_VAR_1}Invalid value"),
};

// v0.52.2 Phase 2b — Per-mon edit menu. Opens when user picks "Pokémon N"
// from the slot menu. Loads sBuildTrainerWorkMon from saveblock on open,
// commits back on Save & Back. Cancel discards changes.
// v0.52.3 Phase 2c — PxHex-style menu. Each row shows the current value as
// "Label: Value" via the {STR_VAR_1} placeholder, populated by
// Debug_GenerateListBuildTrainerMonMenu on every redraw. Pickers (Species/
// Item/Move/Ability/EVs) push a sub-window scroll picker; cycle editors
// (Level/Nature/Gender) increment in place and call Debug_RefreshListMenu so
// the displayed value updates instantly.
static const struct DebugMenuOption sDebugMenu_Actions_BuildTrainerMon[] =
{
    { COMPOUND_STRING("Species: {STR_VAR_1}"),     DebugAction_BuildTrainer_EditSpecies,  },
    { COMPOUND_STRING("Held Item: {STR_VAR_1}"),   DebugAction_BuildTrainer_EditHeldItem, },
    { COMPOUND_STRING("Ability: {STR_VAR_1}"),     DebugAction_BuildTrainer_EditAbility,  },
    { COMPOUND_STRING("Move 1: {STR_VAR_1}"),      DebugAction_BuildTrainer_EditMove1,    },
    { COMPOUND_STRING("Move 2: {STR_VAR_1}"),      DebugAction_BuildTrainer_EditMove2,    },
    { COMPOUND_STRING("Move 3: {STR_VAR_1}"),      DebugAction_BuildTrainer_EditMove3,    },
    { COMPOUND_STRING("Move 4: {STR_VAR_1}"),      DebugAction_BuildTrainer_EditMove4,    },
    // v0.52.4 — EVs and IVs now open their own per-stat sub-menus. The row
    // label shows the current total so you can see at a glance whether the
    // mon's spread is full or partial.
    { COMPOUND_STRING("EVs: {STR_VAR_1}"),         DebugAction_BuildTrainer_OpenEVsMenu,  },
    { COMPOUND_STRING("IVs: {STR_VAR_1}"),         DebugAction_BuildTrainer_OpenIVsMenu,  },
    { COMPOUND_STRING("Level: {STR_VAR_1}"),       DebugAction_BuildTrainer_EditLevel,    },
    { COMPOUND_STRING("Nature: {STR_VAR_1}"),      DebugAction_BuildTrainer_EditNature,   },
    { COMPOUND_STRING("Gender: {STR_VAR_1}"),      DebugAction_BuildTrainer_EditGender,   },
    // v0.52.4 — toggle shiny rendering for this mon (engine uses isShiny to
    // generate an OT ID that XORs to a shiny personality).
    { COMPOUND_STRING("Shiny: {STR_VAR_1}"),       DebugAction_BuildTrainer_EditShiny,    },
    // v1.3 — Showdown team-code import. Reuses the existing pokeemerald
    // naming screen (NAMING_SCREEN_TEAMCODE) instead of a from-scratch
    // keyboard, so the GBA's CHAR_* charset handles font rendering natively.
    // On confirm, gSimImportCodePending flag drives a field_control_avatar
    // hook that decodes the buffer via Sim_DecodeTeamCode and re-opens this
    // per-mon editor with the imported values populated.
    { COMPOUND_STRING("Import from code…"),        DebugAction_BuildTrainer_OpenImportCode },
    { COMPOUND_STRING("Save & Back"),              DebugAction_BuildTrainer_MonSaveBack,  },
    { COMPOUND_STRING("Cancel"),                   DebugAction_BuildTrainer_MonCancel,    },
    { NULL }
};

// v0.52.4 — Per-stat EV editor. Each stat row cycles by +4 per A-press
// (0→4→8→...→252→0). Cap at 252 per stat; total may exceed 510 in the
// editor — engine SetMonData clamps individual stats to 252 regardless.
// Reset wipes all 6 to 0; Back returns to the per-mon editor.
static const struct DebugMenuOption sDebugMenu_Actions_BuildTrainerEVs[] =
{
    { COMPOUND_STRING("HP:  {STR_VAR_1}"),  DebugAction_BuildTrainer_EditEV_HP,  },
    { COMPOUND_STRING("Atk: {STR_VAR_1}"),  DebugAction_BuildTrainer_EditEV_Atk, },
    { COMPOUND_STRING("Def: {STR_VAR_1}"),  DebugAction_BuildTrainer_EditEV_Def, },
    { COMPOUND_STRING("SpA: {STR_VAR_1}"),  DebugAction_BuildTrainer_EditEV_SpA, },
    { COMPOUND_STRING("SpD: {STR_VAR_1}"),  DebugAction_BuildTrainer_EditEV_SpD, },
    { COMPOUND_STRING("Spe: {STR_VAR_1}"),  DebugAction_BuildTrainer_EditEV_Spe, },
    { COMPOUND_STRING("Total: {STR_VAR_1}"),DebugAction_BuildTrainer_EVs_Back,   },
    { COMPOUND_STRING("Reset All to 0"),    DebugAction_BuildTrainer_EVs_Reset,  },
    { COMPOUND_STRING("Back"),              DebugAction_BuildTrainer_EVs_Back,   },
    { NULL }
};

// v0.52.4 — Per-stat IV editor. Each stat row cycles by +1 (0..31 wrap).
// Max All sets all 6 stats to 31 (perfect IVs); Zero All sets to 0.
static const struct DebugMenuOption sDebugMenu_Actions_BuildTrainerIVs[] =
{
    { COMPOUND_STRING("HP:  {STR_VAR_1}"),  DebugAction_BuildTrainer_EditIV_HP,  },
    { COMPOUND_STRING("Atk: {STR_VAR_1}"),  DebugAction_BuildTrainer_EditIV_Atk, },
    { COMPOUND_STRING("Def: {STR_VAR_1}"),  DebugAction_BuildTrainer_EditIV_Def, },
    { COMPOUND_STRING("SpA: {STR_VAR_1}"),  DebugAction_BuildTrainer_EditIV_SpA, },
    { COMPOUND_STRING("SpD: {STR_VAR_1}"),  DebugAction_BuildTrainer_EditIV_SpD, },
    { COMPOUND_STRING("Spe: {STR_VAR_1}"),  DebugAction_BuildTrainer_EditIV_Spe, },
    { COMPOUND_STRING("Max All (31)"),      DebugAction_BuildTrainer_IVs_MaxAll, },
    { COMPOUND_STRING("Zero All (0)"),      DebugAction_BuildTrainer_IVs_ZeroAll,},
    { COMPOUND_STRING("Back"),              DebugAction_BuildTrainer_IVs_Back,   },
    { NULL }
};

// v0.52.2 Phase 2b — Slot edit menu. Lists 6 Pokémon slots + Save / Reset.
// Save marks the slot inUse=1 and persists current monCount. Reset wipes
// the slot back to placeholder Magikarp.
// v0.52.3 Phase 2c — Slot picker shows the saved species per slot via
// {STR_VAR_1}, populated by Debug_GenerateListBuildTrainerSlotMenu. Empty
// slots show "-" so the user can see at a glance which slots are filled.
// v0.52.5 — Trainer-identity rows (Name + Sprite) at the top.
static const struct DebugMenuOption sDebugMenu_Actions_BuildTrainerSlot[] =
{
    { COMPOUND_STRING("Name: {STR_VAR_1}"),      DebugAction_BuildTrainer_EditName,        },
    { COMPOUND_STRING("Sprite: {STR_VAR_1}"),    DebugAction_BuildTrainer_EditTrainerPic,  },
    { COMPOUND_STRING("Pokémon 1: {STR_VAR_1}"), DebugAction_BuildTrainer_OpenMon1,        },
    { COMPOUND_STRING("Pokémon 2: {STR_VAR_1}"), DebugAction_BuildTrainer_OpenMon2,        },
    { COMPOUND_STRING("Pokémon 3: {STR_VAR_1}"), DebugAction_BuildTrainer_OpenMon3,        },
    { COMPOUND_STRING("Pokémon 4: {STR_VAR_1}"), DebugAction_BuildTrainer_OpenMon4,        },
    { COMPOUND_STRING("Pokémon 5: {STR_VAR_1}"), DebugAction_BuildTrainer_OpenMon5,        },
    { COMPOUND_STRING("Pokémon 6: {STR_VAR_1}"), DebugAction_BuildTrainer_OpenMon6,        },
    // v1.1 — Copy a preset trainer's full team into this slot, then edit
    // individual mons from the per-Pokémon rows above. Reuses the regular
    // trainer picker via DebugAction_Trainers_ChooseTrainer with the
    // COPY_TO_CUSTOM token; A confirms the copy, B cancels.
    { COMPOUND_STRING("Copy preset team…"),      DebugAction_Trainers_ChooseTrainer, (void *)TRAINERS_DEBUG_SELECTION_COPY_TO_CUSTOM },
    // v1.15 — bulk set every mon's level to 50. Fan request: tedious to
    // open each per-mon editor and bump level individually after import.
    { COMPOUND_STRING("Set all to Lv50"),         DebugAction_BuildTrainer_SetAllLvl50,     },
    { COMPOUND_STRING("Save Slot"),              DebugAction_BuildTrainer_SaveSlot,        },
    { COMPOUND_STRING("Reset Slot"),             DebugAction_BuildTrainer_ResetSlot,       },
    { COMPOUND_STRING("Back"),                   DebugAction_BuildTrainer_BackToWrapper,   },
    { NULL }
};

// v0.52 Phase 2 — Custom slot picker.
static const struct DebugMenuOption sDebugMenu_Actions_BuildTrainer[] =
{
    { COMPOUND_STRING("Custom 1"),       DebugAction_BuildTrainer_OpenSlot1,       },
    { COMPOUND_STRING("Custom 2"),       DebugAction_BuildTrainer_OpenSlot2,       },
    { COMPOUND_STRING("Custom 3"),       DebugAction_BuildTrainer_OpenSlot3,       },
    { COMPOUND_STRING("Custom 4"),       DebugAction_BuildTrainer_OpenSlot4,       },
    { COMPOUND_STRING("Custom 5"),       DebugAction_BuildTrainer_OpenSlot5,       },
    { COMPOUND_STRING("Custom 6"),       DebugAction_BuildTrainer_OpenSlot6,       },
    { NULL }
};

// v0.52 Phase 2 — Top-level Trainers wrapper. Split between editing custom
// trainers and launching the existing simulation picker.
static const struct DebugMenuOption sDebugMenu_Actions_TrainersWrapper[] =
{
    { COMPOUND_STRING("Build Trainer…"),  DebugAction_OpenSubMenuBuildTrainer, sDebugMenu_Actions_BuildTrainer, },
    { COMPOUND_STRING("Run Simulation…"), DebugAction_OpenSubMenuTrainers,     sDebugMenu_Actions_Trainers,     },
    // v1.7 — Frontier Challenge: skip the sim setup screen entirely, open
    // the trainer picker directly in FRONTIER selection mode. On confirm,
    // the chosen trainer's full team loads into gPlayerParty and the user
    // warps to the Battle Frontier hub so they can walk into any facility.
    { COMPOUND_STRING("{COLOR LIGHT_BLUE}Frontier Challenge…"), DebugAction_Trainers_ChooseTrainer, (void *)TRAINERS_DEBUG_SELECTION_FRONTIER, },
    { NULL }
};

static const struct DebugMenuOption sDebugMenu_Actions_Main[] =
{
    { COMPOUND_STRING("Utilities…"),    DebugAction_OpenSubMenu, sDebugMenu_Actions_Utilities, },
    { COMPOUND_STRING("PC/Bag…"),       DebugAction_OpenSubMenu, sDebugMenu_Actions_PCBag, },
    { COMPOUND_STRING("Party…"),        DebugAction_OpenSubMenu, sDebugMenu_Actions_Party, },
    { COMPOUND_STRING("Give X…"),       DebugAction_OpenSubMenu, sDebugMenu_Actions_Give, },
    { COMPOUND_STRING("Player…"),       DebugAction_OpenSubMenu, sDebugMenu_Actions_Player, },
    { COMPOUND_STRING("Scripts…"),      DebugAction_OpenSubMenu, sDebugMenu_Actions_Scripts, },
    // v0.52: Trainers now opens a 2-option wrapper (Build Trainer / Run Simulation).
    { COMPOUND_STRING("Trainers…"),     DebugAction_OpenSubMenu, sDebugMenu_Actions_TrainersWrapper, },
    { COMPOUND_STRING("Flags & Vars…"), DebugAction_OpenSubMenuFlagsVars, sDebugMenu_Actions_Flags, },
    { COMPOUND_STRING("Sound…"),        DebugAction_OpenSubMenu, sDebugMenu_Actions_Sound, },
    { COMPOUND_STRING("ROM Info…"),     DebugAction_OpenSubMenu, sDebugMenu_Actions_ROMInfo2, },
    { COMPOUND_STRING("Cancel"),        DebugAction_Cancel, },
    { NULL }
};

// *******************************
// Windows
static const struct WindowTemplate sDebugMenuWindowTemplateMain =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = DEBUG_MENU_WIDTH_MAIN,
    .height = 2 * DEBUG_MENU_HEIGHT_MAIN,
    .paletteNum = 15,
    .baseBlock = 1,
};

static const struct WindowTemplate sDebugMenuWindowTemplateExtra =
{
    .bg = 0,
    .tilemapLeft = 30 - DEBUG_MENU_WIDTH_EXTRA - 1,
    .tilemapTop = 1,
    .width = DEBUG_MENU_WIDTH_EXTRA,
    .height = 2 * DEBUG_MENU_HEIGHT_EXTRA,
    .paletteNum = 15,
    .baseBlock = 1,
};

static const struct WindowTemplate sDebugMenuWindowTemplateWeather =
{
    .bg = 0,
    .tilemapLeft = 30 - DEBUG_MENU_WIDTH_WEATHER - 1,
    .tilemapTop = 1,
    .width = DEBUG_MENU_WIDTH_WEATHER,
    .height = 2 * DEBUG_MENU_HEIGHT_WEATHER,
    .paletteNum = 15,
    .baseBlock = 1,
};

static const struct WindowTemplate sDebugMenuWindowTemplateSound =
{
    .bg = 0,
    .tilemapLeft = 30 - DEBUG_MENU_WIDTH_SOUND - 1,
    .tilemapTop = 1,
    .width = DEBUG_MENU_WIDTH_SOUND,
    .height = DEBUG_MENU_HEIGHT_SOUND,
    .paletteNum = 15,
    .baseBlock = 1,
};

static bool32 Debug_SaveCallbackMenu(struct DebugMenuOption *callbackItems);

// *******************************
// Functions universal
void Debug_ShowMainMenu(void)
{
    sDebugMenuListData = AllocZeroed(sizeof(*sDebugMenuListData));
    sDebugMenuListData->listId = 0;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_Main);
}

// Forward declaration for Debug_ShowTrainersSubMenu below.
static void Debug_Trainers_ResetTrainersData(void);

// =====================================================================
// Battle Simulator: Visual Tournament Bracket (CB2_SimBracketScreen)
// =====================================================================
// Custom callback that draws the 8-trainer single-elim bracket on its own
// screen (taking over from the lobby's field CB) between tournament
// matches. Inspired by the Battle Dome's tourney tree (src/battle_dome.c)
// and the PWT B2W2 bracket layout, but rebuilt from scratch for our cup
// format so we don't have to pollute gSaveBlock2Ptr->frontier.domeTrainers.
//
// Layout (240x160 px, 30x20 tiles):
//   Row 0-1   Cup name header (centered, 24 tiles wide)
//   Row 2-3   Round name ("Quarterfinals" / "Semifinals" / "FINAL")
//   Row 5-13  TWO-COLUMN bracket of 8 slots, 14 tiles wide each:
//                 Left  col (rows 5-6, 7-8, 10-11, 12-13): slots 0,1,2,3
//                 Right col (rows 5-6, 7-8, 10-11, 12-13): slots 4,5,6,7
//                 1-row gap at row 9 between the two QF-pair groups so
//                 adjacent slots that share a QF match read as a pair.
//             Prefix conveys status (color also conveys it for accessibility):
//                 "->" = the player    (player palette)
//                 "<-" = current opp   (red palette)
//                 "o " = eliminated    (green palette)
//                 "  " = upcoming      (default palette)
//   Row 17-18 "Press A to fight!" centered prompt.
//
// Iteration goal: this is v0.8 MVP — name strips + status indicators.
// Bracket-connecting tile-art lines + trainer face sprites are deferred.

// BG and window templates.
static const struct BgTemplate sSimBracketBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 24,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
};

#define SIM_BRACKET_WIN_HEADER     0
#define SIM_BRACKET_WIN_ROUND      1
#define SIM_BRACKET_WIN_SLOT_0     2
#define SIM_BRACKET_WIN_SLOT_1     3
#define SIM_BRACKET_WIN_SLOT_2     4
#define SIM_BRACKET_WIN_SLOT_3     5
#define SIM_BRACKET_WIN_SLOT_4     6
#define SIM_BRACKET_WIN_SLOT_5     7
#define SIM_BRACKET_WIN_SLOT_6     8
#define SIM_BRACKET_WIN_SLOT_7     9
#define SIM_BRACKET_WIN_PROMPT     10
#define SIM_BRACKET_WIN_COUNT      11

static const struct WindowTemplate sSimBracketWindowTemplates[] =
{
    // Cup name header — top center, 24 tiles wide.
    [SIM_BRACKET_WIN_HEADER] = {
        .bg = 0, .tilemapLeft = 3, .tilemapTop = 0,
        .width = 24, .height = 2,
        .paletteNum = 15, .baseBlock = 0x001,
    },
    // Round name (Quarterfinals / Semifinals / Final).
    [SIM_BRACKET_WIN_ROUND]  = {
        .bg = 0, .tilemapLeft = 3, .tilemapTop = 2,
        .width = 24, .height = 2,
        .paletteNum = 15, .baseBlock = 0x031,
    },
    // Eight trainer name strips, two-column layout:
    //   left  col: tilemapLeft = 1, width = 14
    //   right col: tilemapLeft = 15, width = 14
    // Each strip is 2 tiles (16 px) tall = 1 line of FONT_NORMAL text.
    // Per-slot tile budget: 14 * 2 = 28 tiles = 0x1c.
    [SIM_BRACKET_WIN_SLOT_0] = { .bg = 0, .tilemapLeft = 1,  .tilemapTop = 5,  .width = 14, .height = 2, .paletteNum = 15, .baseBlock = 0x061 },
    [SIM_BRACKET_WIN_SLOT_1] = { .bg = 0, .tilemapLeft = 1,  .tilemapTop = 7,  .width = 14, .height = 2, .paletteNum = 15, .baseBlock = 0x07d },
    [SIM_BRACKET_WIN_SLOT_2] = { .bg = 0, .tilemapLeft = 1,  .tilemapTop = 10, .width = 14, .height = 2, .paletteNum = 15, .baseBlock = 0x099 },
    [SIM_BRACKET_WIN_SLOT_3] = { .bg = 0, .tilemapLeft = 1,  .tilemapTop = 12, .width = 14, .height = 2, .paletteNum = 15, .baseBlock = 0x0b5 },
    [SIM_BRACKET_WIN_SLOT_4] = { .bg = 0, .tilemapLeft = 15, .tilemapTop = 5,  .width = 14, .height = 2, .paletteNum = 15, .baseBlock = 0x0d1 },
    [SIM_BRACKET_WIN_SLOT_5] = { .bg = 0, .tilemapLeft = 15, .tilemapTop = 7,  .width = 14, .height = 2, .paletteNum = 15, .baseBlock = 0x0ed },
    [SIM_BRACKET_WIN_SLOT_6] = { .bg = 0, .tilemapLeft = 15, .tilemapTop = 10, .width = 14, .height = 2, .paletteNum = 15, .baseBlock = 0x109 },
    [SIM_BRACKET_WIN_SLOT_7] = { .bg = 0, .tilemapLeft = 15, .tilemapTop = 12, .width = 14, .height = 2, .paletteNum = 15, .baseBlock = 0x125 },
    // Bottom prompt — widened to 22 tiles so "Fight <TRAINER NAME>!" fits
    // even for the longest names (e.g. "Fight WATTSON PWT!" = ~18 chars).
    [SIM_BRACKET_WIN_PROMPT] = {
        .bg = 0, .tilemapLeft = 4, .tilemapTop = 17,
        .width = 22, .height = 2,
        .paletteNum = 15, .baseBlock = 0x141,
    },
    DUMMY_WIN_TEMPLATE,
};

// Text colors: foreground / background / shadow.
static const u8 sSimBracketColor_Default[3]   = { 0, 2, 3 };   // dark gray on white
static const u8 sSimBracketColor_Player[3]    = { 0, 8, 9 };   // emphasize player slot
static const u8 sSimBracketColor_Defeated[3]  = { 0, 6, 7 };   // green for defeated
static const u8 sSimBracketColor_Current[3]   = { 0, 4, 5 };   // red for current matchup

// Forward decl.
static void Sim_DrawBracketContent(void);
static void Sim_BracketTeardownAndLaunch(void);

static void VBlankCB_SimBracketScreen(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

// Top-level CB2 — state machine driving the bracket screen lifecycle.
// State 0: init BGs / windows / palettes
// State 1: render the bracket content + start fade-in
// State 2: wait for fade-in
// State 3: wait for player input (A/B/Start)
// State 4: start fade-out
// State 5: launch the next tournament match
static void CB2_SimBracketScreen(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        // Wipe video memory to avoid showing prior screen residue.
        DmaFill16(3, 0, (void *)VRAM, VRAM_SIZE);
        DmaFill32(3, 0, (void *)OAM, OAM_SIZE);
        DmaFill16(3, 0, (void *)(PLTT + 2), PLTT_SIZE - 2);
        ResetPaletteFade();
        ResetBgsAndClearDma3BusyFlags(0);
        ResetTasks();
        ResetSpriteData();
        FreeAllSpritePalettes();
        InitBgsFromTemplates(0, sSimBracketBgTemplates, ARRAY_COUNT(sSimBracketBgTemplates));
        InitWindows(sSimBracketWindowTemplates);
        DeactivateAllTextPrinters();
        // Load standard menu frame tiles (font + window border).
        LoadMessageBoxAndBorderGfx();
        ShowBg(0);
        gMain.state++;
        break;
    case 1:
        Sim_DrawBracketContent();
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB_SimBracketScreen);
        EnableInterrupts(1);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_BG0_ON | DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        gMain.state++;
        break;
    case 2:
        if (!gPaletteFade.active)
            gMain.state++;
        break;
    case 3:
        if (JOY_NEW(A_BUTTON | B_BUTTON | START_BUTTON))
        {
            PlaySE(SE_SELECT);
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gMain.state++;
        }
        break;
    case 4:
        if (!gPaletteFade.active)
        {
            Sim_BracketTeardownAndLaunch();
        }
        break;
    }
    RunTextPrinters();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    RunTasks();
}

// Compute opponent slot for each bracket position (helper for status).
// Returns slot of the trainer who lost to `slot` in a previous round, or
// SIM_TOURNAMENT_SLOT_TBD if `slot` hasn't been eliminated.
static bool32 Sim_SlotIsEliminated(u8 slot)
{
    u8 playerQfMatch = gSimTournamentPlayerSlot / 2;
    u8 myQfMatch = slot / 2;

    // Check QF results — losers of QF are eliminated.
    if (gSimTournamentResults[myQfMatch] != SIM_TOURNAMENT_SLOT_TBD
        && gSimTournamentResults[myQfMatch] != slot)
        return TRUE;

    // SF results: losers of SF are eliminated.
    u8 mySfMatch = (myQfMatch < 2) ? 4 : 5;
    if (gSimTournamentResults[mySfMatch] != SIM_TOURNAMENT_SLOT_TBD
        && gSimTournamentResults[mySfMatch] != slot
        && gSimTournamentResults[myQfMatch] == slot)
        return TRUE;

    // Final: losers of F are eliminated.
    if (gSimTournamentResults[6] != SIM_TOURNAMENT_SLOT_TBD
        && gSimTournamentResults[6] != slot
        && gSimTournamentResults[mySfMatch] == slot)
        return TRUE;

    // Unused to silence "set but not used" — kept for future highlighting.
    (void)playerQfMatch;
    return FALSE;
}

// Is this slot the player's CURRENT opponent (the trainer they're about to fight)?
static bool32 Sim_SlotIsCurrentOpponent(u8 slot)
{
    if (gSimTournamentDone) return FALSE;
    return (slot == Sim_GetPlayerOpponentSlot());
}

// Render the bracket: cup name, round name, 8 trainer slot windows with
// status-colored backgrounds, and the bottom prompt.
static void Sim_DrawBracketContent(void)
{
    // Initialize all windows with a clean fill + frame.
    for (u8 w = 0; w < SIM_BRACKET_WIN_COUNT; w++)
    {
        FillWindowPixelBuffer(w, PIXEL_FILL(1));   // white background
        PutWindowTilemap(w);
    }

    // ---- Cup header ----
    StringCopy(gStringVar4, sSimCups[gSimTournamentCup].name);
    StringAppend(gStringVar4, COMPOUND_STRING(" CUP"));
    AddTextPrinterParameterized3(SIM_BRACKET_WIN_HEADER, FONT_NORMAL,
        GetStringCenterAlignXOffset(FONT_NORMAL, gStringVar4, 24 * 8),
        1, sSimBracketColor_Current, TEXT_SKIP_DRAW, gStringVar4);

    // ---- Round name with match counter ----
    // Compute the player's match number within the full 7-match bracket so
    // the user has a "you are HERE" indicator. QF = matches 1..4, SF = 5..6,
    // Final = 7. The player's QF position (gSimTournamentPlayerSlot / 2)
    // gives QF index 0..3; SF index is 0 (top half) or 1 (bottom half).
    u8 playerQfIdx = gSimTournamentPlayerSlot / 2;
    if (gSimTournamentRound == 1)
    {
        u8 matchNum = playerQfIdx + 1; // 1..4
        ConvertIntToDecimalStringN(gStringVar1, matchNum, STR_CONV_MODE_LEFT_ALIGN, 1);
        StringCopy(gStringVar4, COMPOUND_STRING("Quarterfinal "));
        StringAppend(gStringVar4, gStringVar1);
        StringAppend(gStringVar4, COMPOUND_STRING("/4"));
    }
    else if (gSimTournamentRound == 2)
    {
        u8 sfNum = (playerQfIdx < 2) ? 1 : 2;
        ConvertIntToDecimalStringN(gStringVar1, sfNum, STR_CONV_MODE_LEFT_ALIGN, 1);
        StringCopy(gStringVar4, COMPOUND_STRING("Semifinal "));
        StringAppend(gStringVar4, gStringVar1);
        StringAppend(gStringVar4, COMPOUND_STRING("/2"));
    }
    else
    {
        StringCopy(gStringVar4, COMPOUND_STRING("FINAL MATCH"));
    }
    AddTextPrinterParameterized3(SIM_BRACKET_WIN_ROUND, FONT_NORMAL,
        GetStringCenterAlignXOffset(FONT_NORMAL, gStringVar4, 24 * 8),
        1, sSimBracketColor_Default, TEXT_SKIP_DRAW, gStringVar4);

    // ---- 8 trainer slots ----
    for (u8 slot = 0; slot < SIM_TOURNAMENT_BRACKET_SIZE; slot++)
    {
        u8 winId = SIM_BRACKET_WIN_SLOT_0 + slot;

        // Pick text color by status.
        const u8 *color = sSimBracketColor_Default;
        if (slot == gSimTournamentPlayerSlot)
            color = sSimBracketColor_Player;
        else if (Sim_SlotIsCurrentOpponent(slot))
            color = sSimBracketColor_Current;
        else if (Sim_SlotIsEliminated(slot))
            color = sSimBracketColor_Defeated;

        // Compose label: "<icon> NAME" — single-char status icon + space.
        // Icons (each renders as a distinct glyph in the FONT_NORMAL set):
        //   CHAR_RIGHT_ARROW (0x7C)  ▶  = player slot
        //   CHAR_LEFT_ARROW  (0x7B)  ◀  = current opponent
        //   CHAR_X           (0xD2)  X  = eliminated trainer (clearer than the
        //                                 tiny dash CHAR_BULLET renders as)
        //   CHAR_SPACE                  = upcoming / future match
        gStringVar4[0] = CHAR_SPACE;
        gStringVar4[1] = CHAR_SPACE;
        gStringVar4[2] = EOS;
        if (slot == gSimTournamentPlayerSlot)
        {
            gStringVar4[0] = CHAR_RIGHT_ARROW;
        }
        else if (Sim_SlotIsCurrentOpponent(slot))
        {
            gStringVar4[0] = CHAR_LEFT_ARROW;
        }
        else if (Sim_SlotIsEliminated(slot))
        {
            gStringVar4[0] = CHAR_X;
        }
        StringAppend(gStringVar4, GetTrainerNameFromId(gSimTournamentBracket[slot]));

        AddTextPrinterParameterized3(winId, FONT_NORMAL, 0, 1, color, TEXT_SKIP_DRAW, gStringVar4);
    }

    // ---- Bottom prompt with next-opponent name ----
    // Replace the boring static prompt with the actual opponent the player
    // is about to face. Keeps the red emphasis color so it still reads as
    // "this is the action button" while also conveying info.
    StringCopy(gStringVar4, COMPOUND_STRING("Fight "));
    StringAppend(gStringVar4, GetTrainerNameFromId(gSimTournamentBracket[Sim_GetPlayerOpponentSlot()]));
    StringAppend(gStringVar4, COMPOUND_STRING("!"));
    AddTextPrinterParameterized3(SIM_BRACKET_WIN_PROMPT, FONT_NORMAL,
        GetStringCenterAlignXOffset(FONT_NORMAL, gStringVar4, 22 * 8),
        1, sSimBracketColor_Current, TEXT_SKIP_DRAW, gStringVar4);

    // Push all windows to VRAM.
    for (u8 w = 0; w < SIM_BRACKET_WIN_COUNT; w++)
        CopyWindowToVram(w, COPYWIN_FULL);
}

// Clean up windows and transition to next match's battle.
static void Sim_BracketTeardownAndLaunch(void)
{
    for (u8 w = 0; w < SIM_BRACKET_WIN_COUNT; w++)
        RemoveWindow(w);
    sSimMatchOpponent1 = gSimTournamentBracket[Sim_GetPlayerOpponentSlot()];
    Sim_TriggerNextMatchRound();
}

// Battle Simulator: short helper — append a trainer's name to gStringVar4,
// truncated by GetTrainerNameFromId. Kept inline in the bracket builder.
static void Sim_AppendTrainer(u8 slot)
{
    StringAppend(gStringVar4, GetTrainerNameFromId(gSimTournamentBracket[slot]));
}

// Battle Simulator: build the 8-trainer single-elim bracket announcement.
// Three pages, walked through with A presses:
//
//   PAGE 1 — Cup header + round-of-3 + player's matchup announcement
//   PAGE 2 — Quarterfinals results (4 matches, side-match winners with a "*"
//             after the winner; player's QF marked with ">" prefix)
//   PAGE 3 — Semifinals + Final (only if at least SF reached) — bracket
//             continuation, with TBD slots shown as "?"
//
// Color codes: LIGHT_RED for accents, GREEN for the player's wins, DARK_GRAY
// for body text. The "*" character marks the winner of each match line; "?"
// means the match hasn't happened yet.
static void Sim_BuildBracketMessage(void)
{
    // ===== PAGE 1: cup header + round name + matchup =====================
    StringCopy(gStringVar4, COMPOUND_STRING("{COLOR LIGHT_RED}"));
    StringAppend(gStringVar4, sSimCups[gSimTournamentCup].name);
    StringAppend(gStringVar4, COMPOUND_STRING(" CUP{COLOR DARK_GRAY}\n"));
    if (gSimTournamentRound == 1)
        StringAppend(gStringVar4, COMPOUND_STRING("Quarterfinals"));
    else if (gSimTournamentRound == 2)
        StringAppend(gStringVar4, COMPOUND_STRING("Semifinals"));
    else
        StringAppend(gStringVar4, COMPOUND_STRING("{COLOR LIGHT_RED}FINAL ROUND{COLOR DARK_GRAY}"));
    StringAppend(gStringVar4, COMPOUND_STRING("!\l"));
    StringAppend(gStringVar4, GetTrainerNameFromId(sSimMatchPlayerAI));
    StringAppend(gStringVar4, COMPOUND_STRING(" vs {COLOR LIGHT_RED}"));
    StringAppend(gStringVar4, GetTrainerNameFromId(gSimTournamentBracket[Sim_GetPlayerOpponentSlot()]));
    StringAppend(gStringVar4, COMPOUND_STRING("{COLOR DARK_GRAY}!"));

    // ===== PAGE 2: Quarterfinals bracket =================================
    StringAppend(gStringVar4, COMPOUND_STRING("\pQuarterfinals:"));
    u8 playerQfMatch = gSimTournamentPlayerSlot / 2;
    for (u8 qf = 0; qf < 4; qf++)
    {
        u8 slotA = qf * 2;
        u8 slotB = qf * 2 + 1;
        u8 winner = gSimTournamentResults[qf];

        if (qf == playerQfMatch)
            StringAppend(gStringVar4, COMPOUND_STRING("\n{COLOR LIGHT_RED}> {COLOR DARK_GRAY}"));
        else
            StringAppend(gStringVar4, COMPOUND_STRING("\n  "));

        // Slot A name, plus "*" if they won.
        if (winner == slotA)
            StringAppend(gStringVar4, COMPOUND_STRING("{COLOR GREEN}"));
        Sim_AppendTrainer(slotA);
        if (winner == slotA)
            StringAppend(gStringVar4, COMPOUND_STRING("{COLOR DARK_GRAY}"));

        StringAppend(gStringVar4, COMPOUND_STRING(" v "));

        if (winner == slotB)
            StringAppend(gStringVar4, COMPOUND_STRING("{COLOR GREEN}"));
        Sim_AppendTrainer(slotB);
        if (winner == slotB)
            StringAppend(gStringVar4, COMPOUND_STRING("{COLOR DARK_GRAY}"));
    }

    // ===== PAGE 3: Semifinals + Final (only show once SF reached) =======
    if (gSimTournamentRound >= 2)
    {
        StringAppend(gStringVar4, COMPOUND_STRING("\pSemifinals:"));
        u8 mySfMatch = (playerQfMatch < 2) ? 4 : 5;
        for (u8 sf = 4; sf < 6; sf++)
        {
            u8 qfA = (sf == 4) ? 0 : 2;
            u8 qfB = qfA + 1;
            u8 slotA = gSimTournamentResults[qfA];
            u8 slotB = gSimTournamentResults[qfB];
            u8 winner = gSimTournamentResults[sf];

            if (sf == mySfMatch)
                StringAppend(gStringVar4, COMPOUND_STRING("\n{COLOR LIGHT_RED}> {COLOR DARK_GRAY}"));
            else
                StringAppend(gStringVar4, COMPOUND_STRING("\n  "));

            if (winner == slotA)
                StringAppend(gStringVar4, COMPOUND_STRING("{COLOR GREEN}"));
            Sim_AppendTrainer(slotA);
            if (winner == slotA)
                StringAppend(gStringVar4, COMPOUND_STRING("{COLOR DARK_GRAY}"));

            StringAppend(gStringVar4, COMPOUND_STRING(" v "));

            if (slotB == SIM_TOURNAMENT_SLOT_TBD)
            {
                StringAppend(gStringVar4, COMPOUND_STRING("?"));
            }
            else
            {
                if (winner == slotB)
                    StringAppend(gStringVar4, COMPOUND_STRING("{COLOR GREEN}"));
                Sim_AppendTrainer(slotB);
                if (winner == slotB)
                    StringAppend(gStringVar4, COMPOUND_STRING("{COLOR DARK_GRAY}"));
            }
        }

        // Final
        StringAppend(gStringVar4, COMPOUND_STRING("\pFinal:"));
        u8 finalSlotA = gSimTournamentResults[4];
        u8 finalSlotB = gSimTournamentResults[5];
        u8 finalWinner = gSimTournamentResults[6];
        StringAppend(gStringVar4, COMPOUND_STRING("\n  "));
        if (finalSlotA == SIM_TOURNAMENT_SLOT_TBD)
        {
            StringAppend(gStringVar4, COMPOUND_STRING("?"));
        }
        else
        {
            if (finalWinner == finalSlotA)
                StringAppend(gStringVar4, COMPOUND_STRING("{COLOR GREEN}"));
            Sim_AppendTrainer(finalSlotA);
            if (finalWinner == finalSlotA)
                StringAppend(gStringVar4, COMPOUND_STRING("{COLOR DARK_GRAY}"));
        }
        StringAppend(gStringVar4, COMPOUND_STRING(" v "));
        if (finalSlotB == SIM_TOURNAMENT_SLOT_TBD)
        {
            StringAppend(gStringVar4, COMPOUND_STRING("?"));
        }
        else
        {
            if (finalWinner == finalSlotB)
                StringAppend(gStringVar4, COMPOUND_STRING("{COLOR GREEN}"));
            Sim_AppendTrainer(finalSlotB);
            if (finalWinner == finalSlotB)
                StringAppend(gStringVar4, COMPOUND_STRING("{COLOR DARK_GRAY}"));
        }
    }
}

// Polling task: wait for the bracket message to appear, then wait for the
// player to dismiss it, then auto-launch the next tournament round.
static void Task_Sim_BracketWaitAndLaunch(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (data[0] == 0)
    {
        // Phase 0: wait for the message box to actually be on-screen. Without
        // this we'd race the message-box init and DestroyTask too early.
        if (!IsFieldMessageBoxHidden())
            data[0] = 1;
    }
    else
    {
        // Phase 1: user dismissed the message — kick off the next round.
        if (IsFieldMessageBoxHidden())
        {
            DestroyTask(taskId);
            // Re-arm the next opponent based on the bracket + current round.
            sSimMatchOpponent1 = gSimTournamentBracket[Sim_GetPlayerOpponentSlot()];
            Sim_TriggerNextMatchRound();
        }
    }
}

// Battle Simulator: open the Trainers picker directly (skipping the main debug menu).
// Three auto-launch paths run before falling back to the picker:
//   1. Tournament Mode: if a cup is mid-run, show a bracket message + auto-launch.
//   2. Best-Of-N: if a Best-Of match is in progress and undecided, replay the
//      same matchup for the next round.
//   3. Otherwise, show the normal picker.
void Debug_ShowTrainersSubMenu(void)
{
    // Battle Simulator: first-run default for the sim level cap. EWRAM .sbss
    // forces gSimLevelCap to 0 on boot ("off"); set it to 50 the first time
    // the picker opens so the default is VGC-standard. After that, the user's
    // L/R cycle (50 -> 75 -> 100 -> off) is preserved across picker opens.
    {
        static bool8 sLevelCapInitialized;
        if (!sLevelCapInitialized)
        {
            gSimLevelCap = 50;
            sLevelCapInitialized = TRUE;
        }
    }

    if (Sim_IsTournamentActive())
    {
        // Load the next opponent based on bracket + current round.
        sSimMatchOpponent1 = gSimTournamentBracket[Sim_GetPlayerOpponentSlot()];
        // v0.8: hand off to the visual bracket screen. CB2_SimBracketScreen
        // takes the screen over, renders the 8-trainer bracket with status
        // colors, waits for A press, and transitions to the next match.
        gMain.state = 0;
        SetMainCallback2(CB2_SimBracketScreen);
        return;
        // Old text-based bracket message path kept below for reference;
        // unreachable now that CB2_SimBracketScreen handles the in-between.
        Sim_BuildBracketMessage();
        ShowFieldMessage(gStringVar4);
        u8 taskId = CreateTask(Task_Sim_BracketWaitAndLaunch, 80);
        gTasks[taskId].data[0] = 0;
        return;
    }
    if (Sim_IsBestOfActive() && !Sim_IsMatchDecided())
    {
        Sim_TriggerNextMatchRound();
        return;
    }
    sDebugMenuListData = AllocZeroed(sizeof(*sDebugMenuListData));
    sDebugMenuListData->listId = 2;
    Debug_Trainers_ResetTrainersData();
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_Trainers);
}

// v1.19: callnative entry point for the Battle Tower lobby NPC. Just queues
// the wrapper to auto-open on the next overworld input poll — same channel as
// the boot path uses. Keeps the script engine out of the menu's lifecycle.
void SimRequestWrapperReopen(void)
{
    gSimAutoOpenPending = TRUE;
}

// v0.52: top-level wrapper auto-opened on boot. The two-option split between
// "Build Trainer..." and "Run Simulation..." is what the player sees first;
// "Run Simulation..." routes into the existing Battle Theater picker
// (Debug_ShowTrainersSubMenu's old direct path).
void Debug_ShowTrainersWrapper(void)
{
    // Mirror the picker's first-run level-cap default so the wrapper path
    // doesn't lose that initialization.
    {
        static bool8 sLevelCapInitialized;
        if (!sLevelCapInitialized)
        {
            gSimLevelCap = 50;
            sLevelCapInitialized = TRUE;
        }
    }

    // In-progress tournament / best-of matches should jump straight to the
    // next round, bypassing the wrapper (same as the legacy entry).
    if (Sim_IsTournamentActive() || (Sim_IsBestOfActive() && !Sim_IsMatchDecided()))
    {
        Debug_ShowTrainersSubMenu();
        return;
    }

    sDebugMenuListData = AllocZeroed(sizeof(*sDebugMenuListData));
    sDebugMenuListData->listId = 0;
    Debug_Trainers_ResetTrainersData();
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_TrainersWrapper);
}

// v0.52.5 — Entry point used by field_control_avatar to re-open the Build
// Trainer slot edit menu after returning from the naming screen. Mirrors
// Debug_ShowTrainersWrapper's setup (alloc the list-data block, init listId,
// then show) but lands on the slot menu directly so the user picks up where
// they left off. sBuildTrainerActiveSlot is preserved across the round-trip
// because it lives in EWRAM.
void Debug_ReopenBuildTrainerSlotMenu(void)
{
    sDebugMenuListData = AllocZeroed(sizeof(*sDebugMenuListData));
    sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_SLOT;
    Debug_Trainers_ResetTrainersData();
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerSlot);
}

#define tMenuTaskId          data[0]
#define tWindowId            data[1]
#define tSubWindowId         data[2]
#define tInput               data[3]
#define tDigit               data[4]

static bool32 Debug_SaveCallbackMenu(struct DebugMenuOption *callbackItems)
{
    bool32 success = FALSE;
    for (u32 i = 0; i < DEBUG_MAX_SUB_MENU_LEVELS; i++)
    {
        if (sDebugMenuListData->subMenuItems[i] == NULL)
        {
            sDebugMenuListData->subMenuItems[i] = callbackItems;
            success = TRUE;
            break;
        }
    }
    return success;
}

static u32 Debug_RemoveCallbackMenu(void)
{
    u32 freeCallbackLevel = 0;
    for (u32 i = DEBUG_MAX_SUB_MENU_LEVELS; i > 0; i--)
    {
        if (sDebugMenuListData->subMenuItems[i - 1] != NULL)
        {
            sDebugMenuListData->subMenuItems[i - 1] = NULL;
            freeCallbackLevel = i - 1;
            break;
        }
    }
    return freeCallbackLevel;
}

static const struct DebugMenuOption *Debug_GetCurrentCallbackMenu(void)
{
    const struct DebugMenuOption *callbackItems = NULL;
    for (u32 i = DEBUG_MAX_SUB_MENU_LEVELS; i > 0; i--)
    {
        if (sDebugMenuListData->subMenuItems[i - 1] != NULL)
        {
            callbackItems = sDebugMenuListData->subMenuItems[i - 1];
            break;
        }
    }
    return callbackItems;
}

static bool32 IsSubMenuAction(const void *action)
{
    return action == DebugAction_OpenSubMenu
        || action == DebugAction_OpenSubMenuFlagsVars
        || action == DebugAction_OpenSubMenuFakeRTC
        || action == DebugAction_OpenSubMenuCreateFollowerNPC
        || action == DebugAction_OpenSubMenuTrainers
        // v0.52.3 — must be in this list so DebugTask_HandleMenuInput_General
        // dispatches through the 2-arg (DebugSubmenuFunc) branch and passes
        // the items pointer in r1. Without it, the function receives garbage
        // r1 leftover from a prior call — usually "works" by accident but
        // crashed for some users with addr 0x01340134-style faults.
        || action == DebugAction_OpenSubMenuBuildTrainer;
}

static void Debug_ShowMenu(DebugFunc HandleInput, const struct DebugMenuOption *items)
{
    struct ListMenuTemplate menuTemplate = {0};
    u8 windowId;
    u8 menuTaskId;
    u8 inputTaskId;

    if (items != NULL)
        Debug_SaveCallbackMenu((struct DebugMenuOption *)items);
    else
        items = Debug_GetCurrentCallbackMenu();

    // create window
    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateMain);
    DrawStdWindowFrame(windowId, FALSE);
    CopyWindowToVram(windowId, COPYWIN_GFX);

    u32 totalItems = 0;

    if (sDebugMenuListData->listId == 2)
    {
        totalItems = Debug_GenerateListTrainerMenu();
    }
    else if (sDebugMenuListData->listId == 1)
    {
        totalItems = Debug_GenerateListMenuNames();
    }
    // v0.52.3 Phase 2c — PxHex-style "Label: Value" rows for the Build Trainer
    // slot picker and per-mon editor. Each row formats gStringVar1 with the
    // current saveblock / work-buffer value then expands the COMPOUND_STRING.
    else if (sDebugMenuListData->listId == DEBUG_LISTID_BUILD_TRAINER_SLOT)
    {
        totalItems = Debug_GenerateListBuildTrainerSlotMenu();
    }
    else if (sDebugMenuListData->listId == DEBUG_LISTID_BUILD_TRAINER_MON)
    {
        totalItems = Debug_GenerateListBuildTrainerMonMenu();
    }
    else if (sDebugMenuListData->listId == DEBUG_LISTID_BUILD_TRAINER_EVS)
    {
        totalItems = Debug_GenerateListBuildTrainerEVsMenu();
    }
    else if (sDebugMenuListData->listId == DEBUG_LISTID_BUILD_TRAINER_IVS)
    {
        totalItems = Debug_GenerateListBuildTrainerIVsMenu();
    }
    else
    {
        for (u32 i = 0; items[i].text != NULL; i++)
        {
            sDebugMenuListData->listItems[i].id = i;
            StringExpandPlaceholders(gStringVar4, items[i].text);
            if (IsSubMenuAction(items[i].action))
                StringAppend(gStringVar4, sDebugText_Arrow);
            StringCopy(&sDebugMenuListData->itemNames[i][0], gStringVar4);
            sDebugMenuListData->listItems[i].name = &sDebugMenuListData->itemNames[i][0];
            totalItems++;
        }
    }

    // create list menu
    menuTemplate.items = sDebugMenuListData->listItems;
    menuTemplate.moveCursorFunc = ListMenuDefaultCursorMoveFunc;
    menuTemplate.totalItems = totalItems;
    menuTemplate.maxShowed = DEBUG_MENU_HEIGHT_MAIN;
    menuTemplate.windowId = windowId;
    menuTemplate.header_X = 0;
    menuTemplate.item_X = 8;
    menuTemplate.cursor_X = 0;
    menuTemplate.upText_Y = 1;
    menuTemplate.cursorPal = 2;
    menuTemplate.fillValue = 1;
    menuTemplate.cursorShadowPal = 3;
    menuTemplate.lettersSpacing = 1;
    menuTemplate.itemVerticalPadding = 0;
    menuTemplate.scrollMultiple = LIST_NO_MULTIPLE_SCROLL;
    menuTemplate.fontId = DEBUG_MENU_FONT;
    menuTemplate.cursorKind = 0;
    menuTaskId = ListMenuInit(&menuTemplate, 0, 0);

    // create input handler task
    inputTaskId = CreateTask(HandleInput, 3);
    gTasks[inputTaskId].tMenuTaskId = menuTaskId;
    gTasks[inputTaskId].tWindowId = windowId;
    gTasks[inputTaskId].tSubWindowId = 0;

    Debug_RefreshListMenu(inputTaskId);

    // draw everything
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void Debug_DestroyMenu(u8 taskId)
{
    DestroyListMenuTask(gTasks[taskId].tMenuTaskId, NULL, NULL);
    RemoveWindow(gTasks[taskId].tWindowId);
    DestroyTask(taskId);
}

static void Debug_DestroyMenu_Full(u8 taskId)
{
    if (gTasks[taskId].tSubWindowId != 0)
    {
        ClearStdWindowAndFrame(gTasks[taskId].tSubWindowId, FALSE);
        DebugAction_DestroyExtraWindow(taskId);
    }
    DestroyListMenuTask(gTasks[taskId].tMenuTaskId, NULL, NULL);
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);
    DestroyTask(taskId);
    UnfreezeObjectEvents();
    Free(sDebugMenuListData);
    // v0.52.13.1 — must NULL the pointer too. Free() leaves it dangling, and
    // Sim_SetupMatchRound's best-of rematch path keys on (sDebugMenuListData
    // == NULL) to know when to read from the snapshot cache instead. Without
    // this NULL, round 2 reads garbage data[5]/data[7] off the freed heap
    // address — the singles→doubles + pilot-mode regression.
    sDebugMenuListData = NULL;
    // Battle Simulator: when the user closes the menu in the Battle Tower
    // lobby (B-press or after a confirm dialog), re-arm the auto-open flag
    // so the next field-input tick reopens the Trainers submenu. The lobby
    // has no other gameplay — the menu is the only useful surface there.
    // The Debug_DestroyMenu_Full_Script path runs AFTER this, so it can
    // unset the flag if a script needs the overworld open.
    if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY)
     && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY))
    {
        gSimAutoOpenPending = TRUE;
    }
}

static void Debug_DestroyMenu_Full_Script(u8 taskId, const u8 *script)
{
    Debug_DestroyMenu_Full(taskId);
    // Battle Simulator: a script is taking control of the overworld (battle
    // launch, dialog, etc.) — suppress the lobby auto-reopen we just armed
    // in Debug_Destroy_Menu_Full so the script gets exclusive control.
    gSimAutoOpenPending = FALSE;
    LockPlayerFieldControls();
    FreezeObjectEvents();
    ScriptContext_SetupScript(script);
}

static void Debug_HandleInput_Numeric(u8 taskId, s32 min, s32 max, u32 digits)
{
    if (JOY_NEW(DPAD_UP))
    {
        gTasks[taskId].tInput += sPowersOfTen[gTasks[taskId].tDigit];
        if (gTasks[taskId].tInput > max)
            gTasks[taskId].tInput = max;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        gTasks[taskId].tInput -= sPowersOfTen[gTasks[taskId].tDigit];
        if (gTasks[taskId].tInput < min)
            gTasks[taskId].tInput = min;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (gTasks[taskId].tDigit > 0)
            gTasks[taskId].tDigit -= 1;
    }
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (gTasks[taskId].tDigit < digits - 1)
            gTasks[taskId].tDigit += 1;
    }
}

static void DebugAction_Cancel(u8 taskId)
{
    Debug_DestroyMenu_Full(taskId);
    ScriptContext_Enable();
    // Battle Simulator: the picker is the only thing to do in the Battle Tower.
    // Re-open it on the next field-input tick so the user can't end up wandering.
    gSimAutoOpenPending = TRUE;
}

static void DebugAction_DestroyExtraWindow(u8 taskId)
{
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    ClearStdWindowAndFrame(gTasks[taskId].tSubWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tSubWindowId);

    DestroyListMenuTask(gTasks[taskId].tMenuTaskId, NULL, NULL);
    DestroyTask(taskId);
    ScriptContext_Enable();
    UnfreezeObjectEvents();
}

static u8 DebugNativeStep_CreateDebugWindow(void)
{
    u8 windowId;

    LockPlayerFieldControls();
    FreezeObjectEvents();
    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);
    CopyWindowToVram(windowId, COPYWIN_FULL);

    return windowId;
}

static void DebugNativeStep_CloseDebugWindow(u8 taskId)
{
    ClearStdWindowAndFrame(gTasks[taskId].tSubWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tSubWindowId);
    DestroyTask(taskId);
    UnfreezeObjectEvents();
    UnlockPlayerFieldControls();
}

static u8 Debug_GenerateListTrainerMenu(void)
{
    u32 trainer1Id = sDebugMenuListData->data[0];
    u32 trainer2Id = sDebugMenuListData->data[2];
    u32 partnerId = sDebugMenuListData->data[4];

    s32 rematchTableId = sDebugMenuListData->data[1];
    bool32 isRealFight = sDebugMenuListData->data[3];
    bool32 noDraw;
    u32 offset = 0;
    u8 totalItems = 0;
    for (u32 i = 0; i < (ARRAY_COUNT(sDebugMenu_Actions_Trainers) - 1); i++)
    {
        noDraw = FALSE;
        switch (i)
        {
        case 1:
            if (sDebugMenuListData->data[6] == TRAINER_NONE)
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR RED}None"));
            else
            {
                u8 *end = StringCopy(gStringVar1, GetSimTierColorPrefix(sDebugMenuListData->data[6]));
                end = StringCopy(end, GetTrainerNameFromId(sDebugMenuListData->data[6]));
                StringCopy(end, GetSimSourceSuffix(sDebugMenuListData->data[6]));
            }
            break;
        case 2:
            if (trainer1Id == TRAINER_NONE)
                StringCopy(gStringVar1, COMPOUND_STRING("None"));
            else
            {
                u8 *end = StringCopy(gStringVar1, GetSimTierColorPrefix(trainer1Id));
                end = StringCopy(end, GetTrainerNameFromId(trainer1Id));
                StringCopy(end, GetSimSourceSuffix(trainer1Id));
            }
            break;
        case 3:
            if (trainer2Id == TRAINER_NONE)
                StringCopy(gStringVar1, COMPOUND_STRING("None"));
            else
            {
                u8 *end = StringCopy(gStringVar1, GetSimTierColorPrefix(trainer2Id));
                end = StringCopy(end, GetTrainerNameFromId(trainer2Id));
                StringCopy(end, GetSimSourceSuffix(trainer2Id));
            }
            break;
        case 4:
            if (partnerId == PARTNER_NONE)
                StringCopy(gStringVar1, COMPOUND_STRING("None"));
            else if (partnerId >= PARTNER_COUNT)
            {
                u8 *end = StringCopy(gStringVar1, GetSimTierColorPrefix(partnerId));
                end = StringCopy(end, GetTrainerNameFromId(partnerId));
                StringCopy(end, GetSimSourceSuffix(partnerId));
            }
            else
                ConvertIntToDecimalStringN(gStringVar1, partnerId, STR_CONV_MODE_LEADING_ZEROS, 3);
            break;
        case 5: // Level Cap
            if (gSimLevelCap == 0)
                StringCopy(gStringVar1, COMPOUND_STRING("Off"));
            else
                ConvertIntToDecimalStringN(gStringVar1, gSimLevelCap, STR_CONV_MODE_LEFT_ALIGN, 3);
            break;
        case 6: // Best Of (1 = single, otherwise show "X (a-b)" current score)
            if (gSimBestOf <= 1)
            {
                StringCopy(gStringVar1, COMPOUND_STRING("Off"));
            }
            else if (sSimMatchActive && (gSimT1Wins > 0 || gSimT2Wins > 0))
            {
                u8 buf[4];
                ConvertIntToDecimalStringN(gStringVar1, gSimBestOf, STR_CONV_MODE_LEFT_ALIGN, 1);
                StringAppend(gStringVar1, COMPOUND_STRING(" "));
                ConvertIntToDecimalStringN(buf, gSimT1Wins, STR_CONV_MODE_LEFT_ALIGN, 1);
                StringAppend(gStringVar1, buf);
                StringAppend(gStringVar1, COMPOUND_STRING("-"));
                ConvertIntToDecimalStringN(buf, gSimT2Wins, STR_CONV_MODE_LEFT_ALIGN, 1);
                StringAppend(gStringVar1, buf);
            }
            else
            {
                ConvertIntToDecimalStringN(gStringVar1, gSimBestOf, STR_CONV_MODE_LEFT_ALIGN, 1);
            }
            break;
        case 7: // VGC Mode
            if (gSimVGCMode)
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR GREEN} ON"));
            else
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR RED} OFF"));
            break;
        case 8: // Tournament — shows cup name + round progress (e.g. "ORAS 3/7")
            if (gSimTournamentCup == 0 || gSimTournamentCup >= SIM_CUP_COUNT)
            {
                StringCopy(gStringVar1, COMPOUND_STRING("Off"));
            }
            else
            {
                StringCopy(gStringVar1, sSimCups[gSimTournamentCup].name);
                if (gSimTournamentRound > 0 && !gSimTournamentDone)
                {
                    u8 buf[4];
                    StringAppend(gStringVar1, COMPOUND_STRING(" "));
                    ConvertIntToDecimalStringN(buf, gSimTournamentRound, STR_CONV_MODE_LEFT_ALIGN, 1);
                    StringAppend(gStringVar1, buf);
                    StringAppend(gStringVar1, COMPOUND_STRING("/"));
                    ConvertIntToDecimalStringN(buf, gSimTournamentMaxRounds, STR_CONV_MODE_LEFT_ALIGN, 1);
                    StringAppend(gStringVar1, buf);
                }
                else if (gSimTournamentDone)
                {
                    if (gSimTournamentEliminated)
                        StringAppend(gStringVar1, COMPOUND_STRING(" X"));
                    else
                        StringAppend(gStringVar1, COMPOUND_STRING(" WIN"));
                }
            }
            break;
        case 9: // Double Battle
            if (sDebugMenuListData->data[5] || trainer2Id != TRAINER_NONE || partnerId != PARTNER_NONE)
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR GREEN} TRUE"));
            else
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR RED} FALSE"));
            break;
        case 10: // Pilot Mode (v0.51.1) — player slot becomes human-controlled
            if (sDebugMenuListData->data[7])
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR GREEN} TRUE"));
            else
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR RED} FALSE"));
            break;
        case 11: // Matches
            if (!isRealFight)
            {
                noDraw = TRUE;
                break;
            }
            if (rematchTableId == -1)
            {
                if (HasTrainerBeenFought(trainer1Id))
                    StringCopy(gStringVar1, COMPOUND_STRING("1"));
                else
                    StringCopy(gStringVar1, COMPOUND_STRING("0"));
                StringCopy(gStringVar2, COMPOUND_STRING("1"));
            }
            else
            {
                ConvertIntToDecimalStringN(gStringVar1, CountBattledRematchTeams(rematchTableId), STR_CONV_MODE_LEADING_ZEROS, 1);
                ConvertIntToDecimalStringN(gStringVar2, CountMaxPossibleRematch(rematchTableId), STR_CONV_MODE_LEADING_ZEROS, 1);
            }
            break;
        case 12: // Rematch Ready
            if (FREE_MATCH_CALL || I_VS_SEEKER_CHARGING || !isRealFight || rematchTableId == -1)
            {
                noDraw = TRUE;
                break;
            }
            if (GetActiveTrainerRematches(rematchTableId))
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR GREEN} TRUE"));
            else
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR RED} FALSE"));
            break;
        case 14: // Recharge VS Seeker (visibility)
            if (FREE_MATCH_CALL || I_VS_SEEKER_CHARGING == 0)
                noDraw = TRUE;
            break;
        }

        StringExpandPlaceholders(gStringVar4, sDebugMenu_Actions_Trainers[i].text);
        StringCopy(&sDebugMenuListData->itemNames[i][0], gStringVar4);

        if (noDraw)
        {
            offset++;
        }
        else
        {
            sDebugMenuListData->listItems[i - offset].name = &sDebugMenuListData->itemNames[i][0];
            sDebugMenuListData->listItems[i - offset].id = i;
            totalItems++;
        }
    }
    return totalItems;
}

static const u16 sLocationFlags[] =
{
    FLAG_VISITED_LITTLEROOT_TOWN,
    FLAG_VISITED_OLDALE_TOWN,
    FLAG_VISITED_DEWFORD_TOWN,
    FLAG_VISITED_LAVARIDGE_TOWN,
    FLAG_VISITED_FALLARBOR_TOWN,
    FLAG_VISITED_VERDANTURF_TOWN,
    FLAG_VISITED_PACIFIDLOG_TOWN,
    FLAG_VISITED_PETALBURG_CITY,
    FLAG_VISITED_SLATEPORT_CITY,
    FLAG_VISITED_MAUVILLE_CITY,
    FLAG_VISITED_RUSTBORO_CITY,
    FLAG_VISITED_FORTREE_CITY,
    FLAG_VISITED_LILYCOVE_CITY,
    FLAG_VISITED_MOSSDEEP_CITY,
    FLAG_VISITED_SOOTOPOLIS_CITY,
    FLAG_VISITED_EVER_GRANDE_CITY,
    FLAG_LANDMARK_POKEMON_LEAGUE,
    FLAG_LANDMARK_BATTLE_FRONTIER,
    FLAG_WORLD_MAP_PALLET_TOWN,
    FLAG_WORLD_MAP_VIRIDIAN_CITY,
    FLAG_WORLD_MAP_PEWTER_CITY,
    FLAG_WORLD_MAP_CERULEAN_CITY,
    FLAG_WORLD_MAP_LAVENDER_TOWN,
    FLAG_WORLD_MAP_VERMILION_CITY,
    FLAG_WORLD_MAP_CELADON_CITY,
    FLAG_WORLD_MAP_FUCHSIA_CITY,
    FLAG_WORLD_MAP_CINNABAR_ISLAND,
    FLAG_WORLD_MAP_INDIGO_PLATEAU_EXTERIOR,
    FLAG_WORLD_MAP_SAFFRON_CITY,
    FLAG_WORLD_MAP_ONE_ISLAND,
    FLAG_WORLD_MAP_TWO_ISLAND,
    FLAG_WORLD_MAP_THREE_ISLAND,
    FLAG_WORLD_MAP_FOUR_ISLAND,
    FLAG_WORLD_MAP_FIVE_ISLAND,
    FLAG_WORLD_MAP_SEVEN_ISLAND,
    FLAG_WORLD_MAP_SIX_ISLAND,
    FLAG_WORLD_MAP_ROUTE4_POKEMON_CENTER_1F,
    FLAG_WORLD_MAP_ROUTE10_POKEMON_CENTER_1F,
};

static u32 Debug_CheckToggleFlags(u8 id)
{
    u32 result = FALSE;

    switch (id)
    {
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_POKEDEX:
        result = FlagGet(FLAG_SYS_POKEDEX_GET);
        break;
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_NATDEX:
        result = IsNationalPokedexEnabled();
        break;
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_POKENAV:
        result = FlagGet(FLAG_SYS_POKENAV_GET);
        break;
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_MATCH_CALL:
        result = FlagGet(FLAG_ADDED_MATCH_CALL_TO_POKENAV) && FlagGet(FLAG_HAS_MATCH_CALL);
        break;
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_RUN_SHOES:
        result = FlagGet(FLAG_SYS_B_DASH);
        break;
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_LOCATIONS:
        result = TRUE;
        for (u32 i = 0; i < ARRAY_COUNT(sLocationFlags); i++)
        {
            if (sLocationFlags[i] == 0) // Location flags for Frlg are set to flag 0 in Emerald and vice versa
                continue;

            if (!FlagGet(sLocationFlags[i]))
            {
                result = FALSE;
                break;
            }
        }
        break;
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_BADGES_ALL:
        result = TRUE;
        for (u32 i = 0; i < ARRAY_COUNT(gBadgeFlags); i++)
        {
            if (!FlagGet(gBadgeFlags[i]))
            {
                result = FALSE;
                break;
            }
        }
        break;
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_GAME_CLEAR:
        result = FlagGet(FLAG_SYS_GAME_CLEAR);
        break;
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_FRONTIER_PASS:
        result = FlagGet(FLAG_SYS_FRONTIER_PASS);
        break;
    #if OW_FLAG_NO_COLLISION != 0
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_COLLISION:
        result = FlagGet(OW_FLAG_NO_COLLISION);
        break;
    #endif
    #if OW_FLAG_NO_ENCOUNTER != 0
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_ENCOUNTER:
        result = FlagGet(OW_FLAG_NO_ENCOUNTER);
        break;
    #endif
    #if OW_FLAG_NO_TRAINER_SEE != 0
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_TRAINER_SEE:
        result = FlagGet(OW_FLAG_NO_TRAINER_SEE);
        break;
    #endif
    #if B_FLAG_NO_CATCHING != 0
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_CATCHING:
        result = FlagGet(B_FLAG_NO_CATCHING);
        break;
    #endif
    case DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_BAG_USE:
        result = VarGet(B_VAR_NO_BAG_USE);
        if (result >= NO_BAG_INVALID_VALUE)
            result = NO_BAG_INVALID_VALUE;
        break;
    default:
        result = 0xFF;
        break;
    }

    return result;
}

static u8 Debug_GenerateListMenuNames(void)
{
    const u8 sColor_Red[] = _("{COLOR RED}");
    const u8 sColor_Green[] = _("{COLOR GREEN}");
    u32 i, flagResult = 0;
    u8 const *name = NULL;

    u8 totalItems = 0;
    if (sDebugMenuListData->listId == 1)
        // Failsafe to prevent memory corruption
        totalItems = min(ARRAY_COUNT(sDebugMenu_Actions_Flags) - 1, DEBUG_MAX_MENU_ITEMS);

    // Copy item names for all entries but the last (which is Cancel)
    for (i = 0; i < totalItems; i++)
    {
        if (sDebugMenuListData->listId == 1)
        {
            flagResult = Debug_CheckToggleFlags(i);
            if (i == DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_BAG_USE)
                name = sDebugMenu_Actions_BagUse_Options[flagResult];
            else
                name = sDebugMenu_Actions_Flags[i].text;
        }

        if (i == DEBUG_FLAGVAR_MENU_ITEM_TOGGLE_BAG_USE && flagResult == NO_BAG_INVALID_VALUE)
            flagResult = FALSE;

        if (flagResult == 0xFF)
        {
            StringCopy(&sDebugMenuListData->itemNames[i][0], name);
        }
        else if (flagResult)
        {
            StringCopy(gStringVar1, sColor_Green);
            StringExpandPlaceholders(gStringVar4, name);
            StringCopy(&sDebugMenuListData->itemNames[i][0], gStringVar4);
        }
        else
        {
            StringCopy(gStringVar1, sColor_Red);
            StringExpandPlaceholders(gStringVar4, name);
            StringCopy(&sDebugMenuListData->itemNames[i][0], gStringVar4);
        }

        sDebugMenuListData->listItems[i].name = &sDebugMenuListData->itemNames[i][0];
        sDebugMenuListData->listItems[i].id = i;
    }
    return totalItems;
}

static void Debug_RefreshListMenu(u8 taskId)
{
    u8 totalItems = Debug_GenerateListMenuNames();

    // Set list menu data
    gMultiuseListMenuTemplate.items = sDebugMenuListData->listItems;
    gMultiuseListMenuTemplate.totalItems = totalItems;
    gMultiuseListMenuTemplate.maxShowed = DEBUG_MENU_HEIGHT_MAIN;
    gMultiuseListMenuTemplate.windowId = gTasks[taskId].tWindowId;
    gMultiuseListMenuTemplate.header_X = 0;
    gMultiuseListMenuTemplate.item_X = 8;
    gMultiuseListMenuTemplate.cursor_X = 0;
    gMultiuseListMenuTemplate.upText_Y = 1;
    gMultiuseListMenuTemplate.cursorPal = 2;
    gMultiuseListMenuTemplate.fillValue = 1;
    gMultiuseListMenuTemplate.cursorShadowPal = 3;
    gMultiuseListMenuTemplate.lettersSpacing = 1;
    gMultiuseListMenuTemplate.itemVerticalPadding = 0;
    gMultiuseListMenuTemplate.scrollMultiple = LIST_NO_MULTIPLE_SCROLL;
    gMultiuseListMenuTemplate.fontId = 1;
    gMultiuseListMenuTemplate.cursorKind = 0;
}

static void DebugTask_HandleMenuInput_General(u8 taskId)
{
    const struct DebugMenuOption *options = Debug_GetCurrentCallbackMenu();
    u32 input = ListMenu_ProcessInput(gTasks[taskId].tMenuTaskId);
    struct DebugMenuOption option = options[input];

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (option.action != NULL)
        {
            if (IsSubMenuAction(option.action))
            {
                ((DebugSubmenuFunc)option.action)(taskId, option.actionParams);
            }
            else if (option.action == DebugAction_ExecuteScript)
            {
                Debug_DestroyMenu_Full_Script(taskId, (const u8 *)option.actionParams);
            }
            else if (option.action == DebugAction_ToggleFlag)
            {
                ((DebugFunc)option.actionParams)(taskId);
                DebugAction_ToggleFlag(taskId);
            }
            else if (option.action == DebugAction_Trainers_ChooseTrainer)
            {
                DebugAction_Trainers_ChooseTrainer(taskId, (u32)option.actionParams);
            }
            else
            {
                ((DebugFunc)option.action)(taskId);
            }
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (Debug_GetCurrentCallbackMenu() != NULL && Debug_RemoveCallbackMenu() != 0)
        {
            Debug_DestroyMenu(taskId);
            // v1.2 — restore the right listId based on what menu we're
            // returning to. The old code always reset to 0, which made any
            // menu with dynamic-value rows render with raw {STR_VAR_1}
            // placeholders (blank values) after a B-back. Every menu whose
            // generator lives in the listId switch needs to be listed here.
            const struct DebugMenuOption *newTop = Debug_GetCurrentCallbackMenu();
            if (newTop == sDebugMenu_Actions_BuildTrainerMon)
                sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_MON;
            else if (newTop == sDebugMenu_Actions_BuildTrainerSlot)
                sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_SLOT;
            else if (newTop == sDebugMenu_Actions_BuildTrainerEVs)
                sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_EVS;
            else if (newTop == sDebugMenu_Actions_BuildTrainerIVs)
                sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_IVS;
            else if (newTop == sDebugMenu_Actions_Trainers)
                sDebugMenuListData->listId = 2;  // sim trainer-config menu
            else if (newTop == sDebugMenu_Actions_Flags)
                sDebugMenuListData->listId = 1;  // flag-toggle menu
            else if (sDebugMenuListData->listId != 0)
                sDebugMenuListData->listId = 0;
            Debug_ShowMenu(DebugTask_HandleMenuInput_General, NULL);
        }
        else
        {
            Debug_DestroyMenu_Full(taskId);
            ScriptContext_Enable();
        }
    }
}

static void DebugAction_OpenSubMenuTrainers(u8 taskId, const struct DebugMenuOption *items)
{
    Debug_DestroyMenu(taskId);
    sDebugMenuListData->listId = 2;
    sDebugMenuListData->data[0] = TRAINER_NONE;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, items);
}

// v0.52 Phase 2 — Open the Build Trainer slot picker. Mirrors
// DebugAction_OpenSubMenuTrainers but uses listId=0 (no special handler).
static void DebugAction_OpenSubMenuBuildTrainer(u8 taskId, const struct DebugMenuOption *items)
{
    Debug_DestroyMenu(taskId);
    sDebugMenuListData->listId = 0;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, items);
}

// v0.52 Phase 2 — When the user picks a slot, remember which one is active
// and open the per-slot edit menu (Phase 2a: Reset + Back; Phase 3 adds the
// per-mon/name editors).
// v0.52.12 — Sanitize a SimCustomTrainer slot read from saveblock. Forces
// every field into a valid range so stale save data (from before the struct
// grew across v0.51 → v0.52.5) can't crash menus or the synthesizer.
//
// Specifically defends against:
//   * Garbage `inUse` byte → wipe the whole slot if non-{0,1}
//   * Garbage `monCount` > 6 → wipe
//   * `name[]` without EOS terminator → force EOS at the array end
//   * `trainerPic` out of range → 0 (synthesizer falls back to default)
//   * Per-mon species / move / nature / ability / EV / IV / gender out of
//     range → clamp to safe defaults
//
// Cheap to run on every slot read (a few hundred bounds checks).
static void BuildTrainer_SanitizeSlot(struct SimCustomTrainer *slot)
{
    if (slot->inUse > 1 || slot->monCount > 6)
    {
        memset(slot, 0, sizeof(*slot));
        return;
    }
    // Force EOS at the end of the name array. StringCopy walks until EOS;
    // an untermined name would overflow gStringVar1 and corrupt EWRAM.
    slot->name[SIM_CUSTOM_TRAINER_NAME_LEN] = EOS;
    if (slot->trainerPic >= TRAINER_PIC_FRONT_COUNT)
        slot->trainerPic = 0;
    if (!slot->inUse)
        return;  // unused slots: don't bother sanitizing mons
    for (u8 i = 0; i < slot->monCount; i++)
    {
        struct SimCustomTrainerMon *m = &slot->mons[i];
        if (m->species >= NUM_SPECIES)      m->species = SPECIES_NONE;
        if (m->heldItem >= ITEMS_COUNT)     m->heldItem = ITEM_NONE;
        if (m->nature >= NUM_NATURES)       m->nature = NATURE_HARDY;
        if (m->level > 100)                 m->level = 50;
        if (m->gender > 2)                  m->gender = 0;
        if (m->abilityNum >= NUM_ABILITY_SLOTS) m->abilityNum = 0;
        if (m->shiny > 1)                   m->shiny = 0;
        for (u8 j = 0; j < 6; j++)
        {
            if (m->evs[j] > 252) m->evs[j] = 0;
            if (m->ivs[j] > 31)  m->ivs[j] = 0;
        }
        for (u8 j = 0; j < 4; j++)
            if (m->moves[j] >= MOVES_COUNT) m->moves[j] = MOVE_NONE;
    }
}

static void DebugAction_BuildTrainer_OpenSlotN(u8 taskId, u8 slot)
{
    sBuildTrainerActiveSlot = slot;
    // v0.52.12 — Migrate / sanitize stale save data BEFORE rendering any
    // slot rows. This is the first place the user lands on this slot's
    // saveblock data, so it's the right gate.
    BuildTrainer_SanitizeSlot(&gSaveBlock3Ptr->simCustomTrainers[slot]);
    Debug_DestroyMenu(taskId);
    // v0.52.3 — listId 3 routes the redraw through
    // Debug_GenerateListBuildTrainerSlotMenu so each row shows the saved
    // species name (or "-" when empty).
    sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_SLOT;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerSlot);
}

static void DebugAction_BuildTrainer_OpenSlot1(u8 taskId) { DebugAction_BuildTrainer_OpenSlotN(taskId, 0); }
static void DebugAction_BuildTrainer_OpenSlot2(u8 taskId) { DebugAction_BuildTrainer_OpenSlotN(taskId, 1); }
static void DebugAction_BuildTrainer_OpenSlot3(u8 taskId) { DebugAction_BuildTrainer_OpenSlotN(taskId, 2); }
static void DebugAction_BuildTrainer_OpenSlot4(u8 taskId) { DebugAction_BuildTrainer_OpenSlotN(taskId, 3); }
static void DebugAction_BuildTrainer_OpenSlot5(u8 taskId) { DebugAction_BuildTrainer_OpenSlotN(taskId, 4); }
static void DebugAction_BuildTrainer_OpenSlot6(u8 taskId) { DebugAction_BuildTrainer_OpenSlotN(taskId, 5); }

// v0.52 Phase 2 — Wipe the active custom-trainer slot's saveblock entry.
// Sets inUse=0 so Sim_GetCustomTrainerStruct falls back to the placeholder
// Magikarp team for that slot.
static void DebugAction_BuildTrainer_ResetSlot(u8 taskId)
{
    if (sBuildTrainerActiveSlot >= SIM_NUM_CUSTOM_TRAINERS)
        return;
    memset(&gSaveBlock3Ptr->simCustomTrainers[sBuildTrainerActiveSlot], 0,
           sizeof(gSaveBlock3Ptr->simCustomTrainers[sBuildTrainerActiveSlot]));
    // Stay on the slot menu so the user can confirm Reset took effect by
    // backing out and seeing the placeholder team in a battle.
}

// v1.15 — bulk set every populated mon in the current slot to level 50.
// Skips empty slots (species == SPECIES_NONE). Plays a success cue so the
// user gets immediate feedback without having to back out and re-open
// each mon to verify the change took.
static void DebugAction_BuildTrainer_SetAllLvl50(u8 taskId)
{
    if (sBuildTrainerActiveSlot >= SIM_NUM_CUSTOM_TRAINERS)
        return;
    struct SimCustomTrainer *slot = &gSaveBlock3Ptr->simCustomTrainers[sBuildTrainerActiveSlot];
    u32 touched = 0;
    for (u32 i = 0; i < 6; i++)
    {
        if (slot->mons[i].species != SPECIES_NONE)
        {
            slot->mons[i].level = 50;
            touched++;
        }
    }
    PlaySE(touched > 0 ? SE_SUCCESS : SE_FAILURE);
}

// v0.52 Phase 2 — Pop back to the Build Trainer slot picker from the
// per-slot edit menu. Resets listId so the Build Trainer slot-1/2/3 picker
// renders as a plain static menu (it doesn't need dynamic values).
//
// v1.4 — also flushes SaveBlock3 to flash on exit. Any slot-level edits
// (name, sprite, Copy-Preset) that didn't already hit CommitWorkBufferToSaveblock
// get persisted here. Silent save (no SE/popup) since "Back" should feel
// instantaneous.
static void DebugAction_BuildTrainer_BackToWrapper(u8 taskId)
{
    TrySavingData(SAVE_NORMAL);
    Debug_DestroyMenu(taskId);
    sDebugMenuListData->listId = 0;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainer);
}

// v0.52.5 — Launch the engine's naming screen targeting the current slot's
// saveblock name buffer. The naming screen overwrites destBuffer in place
// (length-capped to the template's MAX_NAME_LENGTH), then calls the return
// callback. We set gSimBuildTrainerReopenSlot so field_control_avatar's
// input poll re-enters the slot menu the next frame.
//
// NAMING_SCREEN_PLAYER reuses the "YOUR NAME?" UI — close enough for a
// trainer name. SaveBlock3's name field is SIM_CUSTOM_TRAINER_NAME_LEN+1
// bytes wide, matching TRAINER_NAME_LENGTH so no truncation occurs.
static void DebugAction_BuildTrainer_EditName(u8 taskId)
{
    PlaySE(SE_SELECT);
    struct SimCustomTrainer *slot = &gSaveBlock3Ptr->simCustomTrainers[sBuildTrainerActiveSlot];
    // Mark the slot inUse so the synthesizer reads the new name. The user
    // explicitly hit "Name" — they're configuring this slot.
    slot->inUse = 1;
    if (slot->monCount == 0)
        slot->monCount = 1;  // ensure placeholder mon exists
    // Tear down the debug menu so we can switch CB2 to the naming screen.
    Debug_DestroyMenu_Full(taskId);
    gSimBuildTrainerReopenSlot = TRUE;
    DoNamingScreen(NAMING_SCREEN_PLAYER, slot->name,
                   gSaveBlock2Ptr->playerGender, 0, 0, CB2_ReturnToField);
}

// v0.52.5 — Open the trainer-sprite picker. Mirrors the species picker's
// sub-window flow (DebugAction_BuildTrainer_EditSpecies). Up/Down ±1, L/R
// ±25 fast scroll, A commits, B reverts to the entry value.
//
// v0.52.11 — Use a small static map from TRAINER_PIC_FRONT_* → display
// string. v0.52.10 tried to derive the name by iterating gTrainers[][1090]
// at every redraw, which crashed ("Jumped to invalid address: E1340004")
// on saves migrated from older struct layouts and was suspect under load
// anyway. The table covers the trainer pics most likely to be picked
// (Hoenn cast, gym leaders, Champions, common classes, rivals); unmapped
// pics fall through to "Pic NNN".
struct BuildTrainerPicLabel { u16 picId; const u8 *name; };
static const struct BuildTrainerPicLabel sBuildTrainerPicLabels[] =
{
    { TRAINER_PIC_FRONT_HIKER,                 COMPOUND_STRING("Hiker") },
    { TRAINER_PIC_FRONT_POKEMON_BREEDER_F,     COMPOUND_STRING("Breeder F") },
    { TRAINER_PIC_FRONT_COOLTRAINER_M,         COMPOUND_STRING("Cool Trnr M") },
    { TRAINER_PIC_FRONT_COOLTRAINER_F,         COMPOUND_STRING("Cool Trnr F") },
    { TRAINER_PIC_FRONT_BIRD_KEEPER,           COMPOUND_STRING("Bird Keeper") },
    { TRAINER_PIC_FRONT_COLLECTOR,             COMPOUND_STRING("Collector") },
    { TRAINER_PIC_FRONT_SWIMMER_M,             COMPOUND_STRING("Swimmer M") },
    { TRAINER_PIC_FRONT_SWIMMER_F,             COMPOUND_STRING("Swimmer F") },
    { TRAINER_PIC_FRONT_EXPERT_M,              COMPOUND_STRING("Expert M") },
    { TRAINER_PIC_FRONT_EXPERT_F,              COMPOUND_STRING("Expert F") },
    { TRAINER_PIC_FRONT_BLACK_BELT,            COMPOUND_STRING("Black Belt") },
    { TRAINER_PIC_FRONT_HEX_MANIAC,            COMPOUND_STRING("Hex Maniac") },
    { TRAINER_PIC_FRONT_AROMA_LADY,            COMPOUND_STRING("Aroma Lady") },
    { TRAINER_PIC_FRONT_RUIN_MANIAC,           COMPOUND_STRING("Ruin Maniac") },
    { TRAINER_PIC_FRONT_TUBER_M,               COMPOUND_STRING("Tuber M") },
    { TRAINER_PIC_FRONT_TUBER_F,               COMPOUND_STRING("Tuber F") },
    { TRAINER_PIC_FRONT_LADY,                  COMPOUND_STRING("Lady") },
    { TRAINER_PIC_FRONT_BEAUTY,                COMPOUND_STRING("Beauty") },
    { TRAINER_PIC_FRONT_RICH_BOY,              COMPOUND_STRING("Rich Boy") },
    { TRAINER_PIC_FRONT_POKEMANIAC,            COMPOUND_STRING("PkmnManiac") },
    { TRAINER_PIC_FRONT_GUITARIST,             COMPOUND_STRING("Guitarist") },
    { TRAINER_PIC_FRONT_KINDLER,               COMPOUND_STRING("Kindler") },
    { TRAINER_PIC_FRONT_CAMPER,                COMPOUND_STRING("Camper") },
    { TRAINER_PIC_FRONT_PICNICKER,             COMPOUND_STRING("Picnicker") },
    { TRAINER_PIC_FRONT_BUG_MANIAC,            COMPOUND_STRING("Bug Maniac") },
    { TRAINER_PIC_FRONT_POKEMON_BREEDER_M,     COMPOUND_STRING("Breeder M") },
    { TRAINER_PIC_FRONT_PSYCHIC_M,             COMPOUND_STRING("Psychic M") },
    { TRAINER_PIC_FRONT_PSYCHIC_F,             COMPOUND_STRING("Psychic F") },
    { TRAINER_PIC_FRONT_GENTLEMAN,             COMPOUND_STRING("Gentleman") },
    { TRAINER_PIC_FRONT_ELITE_FOUR_SIDNEY,     COMPOUND_STRING("E4 Sidney") },
    { TRAINER_PIC_FRONT_ELITE_FOUR_PHOEBE,     COMPOUND_STRING("E4 Phoebe") },
    { TRAINER_PIC_FRONT_ELITE_FOUR_GLACIA,     COMPOUND_STRING("E4 Glacia") },
    { TRAINER_PIC_FRONT_ELITE_FOUR_DRAKE,      COMPOUND_STRING("E4 Drake") },
    { TRAINER_PIC_FRONT_LEADER_ROXANNE,        COMPOUND_STRING("Ldr Roxanne") },
    { TRAINER_PIC_FRONT_LEADER_BRAWLY,         COMPOUND_STRING("Ldr Brawly") },
    { TRAINER_PIC_FRONT_LEADER_WATTSON,        COMPOUND_STRING("Ldr Wattson") },
    { TRAINER_PIC_FRONT_LEADER_FLANNERY,       COMPOUND_STRING("Ldr Flannery") },
    { TRAINER_PIC_FRONT_LEADER_NORMAN,         COMPOUND_STRING("Ldr Norman") },
    { TRAINER_PIC_FRONT_LEADER_WINONA,         COMPOUND_STRING("Ldr Winona") },
    { TRAINER_PIC_FRONT_LEADER_TATE_AND_LIZA,  COMPOUND_STRING("Ldr T&L") },
    { TRAINER_PIC_FRONT_LEADER_JUAN,           COMPOUND_STRING("Ldr Juan") },
    { TRAINER_PIC_FRONT_SCHOOL_KID_M,          COMPOUND_STRING("SchoolKid M") },
    { TRAINER_PIC_FRONT_SCHOOL_KID_F,          COMPOUND_STRING("SchoolKid F") },
    { TRAINER_PIC_FRONT_SR_AND_JR,             COMPOUND_STRING("Sr & Jr") },
    { TRAINER_PIC_FRONT_POKEFAN_M,             COMPOUND_STRING("PkmnFan M") },
    { TRAINER_PIC_FRONT_POKEFAN_F,             COMPOUND_STRING("PkmnFan F") },
    { TRAINER_PIC_FRONT_YOUNGSTER,             COMPOUND_STRING("Youngster") },
    { TRAINER_PIC_FRONT_CHAMPION_WALLACE,      COMPOUND_STRING("Champ Wallace") },
    { TRAINER_PIC_FRONT_FISHERMAN,             COMPOUND_STRING("Fisherman") },
    { TRAINER_PIC_FRONT_DRAGON_TAMER,          COMPOUND_STRING("Dragon Tamer") },
    { TRAINER_PIC_FRONT_NINJA_BOY,             COMPOUND_STRING("Ninja Boy") },
    { TRAINER_PIC_FRONT_BATTLE_GIRL,           COMPOUND_STRING("Battle Girl") },
    { TRAINER_PIC_FRONT_PARASOL_LADY,          COMPOUND_STRING("Parasol Lady") },
    { TRAINER_PIC_FRONT_TWINS,                 COMPOUND_STRING("Twins") },
    { TRAINER_PIC_FRONT_SAILOR,                COMPOUND_STRING("Sailor") },
    { TRAINER_PIC_FRONT_MAGMA_LEADER_MAXIE,    COMPOUND_STRING("Maxie") },
    { TRAINER_PIC_FRONT_AQUA_LEADER_ARCHIE,    COMPOUND_STRING("Archie") },
    { TRAINER_PIC_FRONT_WALLY,                 COMPOUND_STRING("Wally") },
    { TRAINER_PIC_FRONT_BRENDAN,               COMPOUND_STRING("Brendan") },
    { TRAINER_PIC_FRONT_MAY,                   COMPOUND_STRING("May") },
    { TRAINER_PIC_FRONT_LASS,                  COMPOUND_STRING("Lass") },
    { TRAINER_PIC_FRONT_BUG_CATCHER,           COMPOUND_STRING("Bug Catcher") },
    { TRAINER_PIC_FRONT_POKEMON_RANGER_M,      COMPOUND_STRING("Ranger M") },
    { TRAINER_PIC_FRONT_POKEMON_RANGER_F,      COMPOUND_STRING("Ranger F") },
    { TRAINER_PIC_FRONT_STEVEN,                COMPOUND_STRING("Steven") },
    { TRAINER_PIC_FRONT_RED,                   COMPOUND_STRING("Red") },
    { TRAINER_PIC_FRONT_LEAF,                  COMPOUND_STRING("Leaf") },
    // v1.1 — Hoenn team grunts / admins, interviewer
    { TRAINER_PIC_FRONT_AQUA_GRUNT_M,          COMPOUND_STRING("Aqua Grunt M") },
    { TRAINER_PIC_FRONT_AQUA_GRUNT_F,          COMPOUND_STRING("Aqua Grunt F") },
    { TRAINER_PIC_FRONT_MAGMA_GRUNT_M,         COMPOUND_STRING("Magma GrntM") },
    { TRAINER_PIC_FRONT_MAGMA_GRUNT_F,         COMPOUND_STRING("Magma GrntF") },
    { TRAINER_PIC_FRONT_AQUA_ADMIN_M,          COMPOUND_STRING("Aqua Admin M") },
    { TRAINER_PIC_FRONT_AQUA_ADMIN_F,          COMPOUND_STRING("Aqua Admin F") },
    { TRAINER_PIC_FRONT_MAGMA_ADMIN,           COMPOUND_STRING("Magma Admin") },
    { TRAINER_PIC_FRONT_INTERVIEWER,           COMPOUND_STRING("Interviewer") },
    // v1.1 — Hoenn triathletes (cycling / running / swimming)
    { TRAINER_PIC_FRONT_CYCLING_TRIATHLETE_M,  COMPOUND_STRING("Cycling M") },
    { TRAINER_PIC_FRONT_CYCLING_TRIATHLETE_F,  COMPOUND_STRING("Cycling F") },
    { TRAINER_PIC_FRONT_RUNNING_TRIATHLETE_M,  COMPOUND_STRING("Running M") },
    { TRAINER_PIC_FRONT_RUNNING_TRIATHLETE_F,  COMPOUND_STRING("Running F") },
    { TRAINER_PIC_FRONT_SWIMMING_TRIATHLETE_M, COMPOUND_STRING("Swim Tri M") },
    { TRAINER_PIC_FRONT_SWIMMING_TRIATHLETE_F, COMPOUND_STRING("Swim Tri F") },
    // v1.1 — Hoenn couples / pairs
    { TRAINER_PIC_FRONT_YOUNG_COUPLE,          COMPOUND_STRING("Young Couple") },
    { TRAINER_PIC_FRONT_OLD_COUPLE,            COMPOUND_STRING("Old Couple") },
    { TRAINER_PIC_FRONT_SIS_AND_BRO,           COMPOUND_STRING("Sis & Bro") },
    // v1.1 — Emerald Battle Frontier brains
    { TRAINER_PIC_FRONT_SALON_MAIDEN_ANABEL,   COMPOUND_STRING("Anabel BF") },
    { TRAINER_PIC_FRONT_DOME_ACE_TUCKER,       COMPOUND_STRING("Tucker") },
    { TRAINER_PIC_FRONT_PALACE_MAVEN_SPENSER,  COMPOUND_STRING("Spenser") },
    { TRAINER_PIC_FRONT_ARENA_TYCOON_GRETA,    COMPOUND_STRING("Greta") },
    { TRAINER_PIC_FRONT_FACTORY_HEAD_NOLAND,   COMPOUND_STRING("Noland") },
    { TRAINER_PIC_FRONT_PIKE_QUEEN_LUCY,       COMPOUND_STRING("Lucy") },
    { TRAINER_PIC_FRONT_PYRAMID_KING_BRANDON,  COMPOUND_STRING("Brandon") },
    // v1.1 — Ruby/Sapphire-style player sprites
    { TRAINER_PIC_FRONT_RS_BRENDAN,            COMPOUND_STRING("RS Brendan") },
    { TRAINER_PIC_FRONT_RS_MAY,                COMPOUND_STRING("RS May") },
    // v1.1 — FRLG variants of Hoenn-shared classes (suffix "FR" disambiguates)
    { TRAINER_PIC_FRONT_POKEMON_RANGER_M_FRLG, COMPOUND_STRING("Ranger M FR") },
    { TRAINER_PIC_FRONT_AROMA_LADY_FRLG,       COMPOUND_STRING("Aroma F FR") },
    { TRAINER_PIC_FRONT_YOUNGSTER_FRLG,        COMPOUND_STRING("Young FR") },
    { TRAINER_PIC_FRONT_BUG_CATCHER_FRLG,      COMPOUND_STRING("BugCtch FR") },
    { TRAINER_PIC_FRONT_LASS_FRLG,             COMPOUND_STRING("Lass FR") },
    { TRAINER_PIC_FRONT_SAILOR_FRLG,           COMPOUND_STRING("Sailor FR") },
    { TRAINER_PIC_FRONT_CAMPER_FRLG,           COMPOUND_STRING("Camper FR") },
    { TRAINER_PIC_FRONT_PICNICKER_FRLG,        COMPOUND_STRING("Picnic FR") },
    { TRAINER_PIC_FRONT_POKEMANIAC_FRLG,       COMPOUND_STRING("PkmMan FR") },
    { TRAINER_PIC_FRONT_HIKER_FRLG,            COMPOUND_STRING("Hiker FR") },
    { TRAINER_PIC_FRONT_FISHERMAN_FRLG,        COMPOUND_STRING("Fish FR") },
    { TRAINER_PIC_FRONT_SWIMMER_M_FRLG,        COMPOUND_STRING("Swim M FR") },
    { TRAINER_PIC_FRONT_BEAUTY_FRLG,           COMPOUND_STRING("Beauty FR") },
    { TRAINER_PIC_FRONT_SWIMMER_F_FRLG,        COMPOUND_STRING("Swim F FR") },
    { TRAINER_PIC_FRONT_PSYCHIC_M_FRLG,        COMPOUND_STRING("Psy M FR") },
    { TRAINER_PIC_FRONT_BIRD_KEEPER_FRLG,      COMPOUND_STRING("Bird FR") },
    { TRAINER_PIC_FRONT_BLACK_BELT_FRLG,       COMPOUND_STRING("BBelt FR") },
    { TRAINER_PIC_FRONT_COOLTRAINER_M_FRLG,    COMPOUND_STRING("Cool M FR") },
    { TRAINER_PIC_FRONT_COOLTRAINER_F_FRLG,    COMPOUND_STRING("Cool F FR") },
    { TRAINER_PIC_FRONT_GENTLEMAN_FRLG,        COMPOUND_STRING("Gent FR") },
    { TRAINER_PIC_FRONT_TWINS_FRLG,            COMPOUND_STRING("Twins FR") },
    { TRAINER_PIC_FRONT_YOUNG_COUPLE_FRLG,     COMPOUND_STRING("Couple FR") },
    { TRAINER_PIC_FRONT_SIS_AND_BRO_FRLG,      COMPOUND_STRING("S&B FR") },
    { TRAINER_PIC_FRONT_PSYCHIC_F_FRLG,        COMPOUND_STRING("Psy F FR") },
    { TRAINER_PIC_FRONT_TUBER_F_FRLG,          COMPOUND_STRING("Tuber F FR") },
    { TRAINER_PIC_FRONT_POKEMON_RANGER_F_FRLG, COMPOUND_STRING("Ranger F FR") },
    { TRAINER_PIC_FRONT_RUIN_MANIAC_FRLG,      COMPOUND_STRING("Ruin FR") },
    { TRAINER_PIC_FRONT_LADY_FRLG,             COMPOUND_STRING("Lady FR") },
    { TRAINER_PIC_FRONT_POKEMON_BREEDER_FRLG,  COMPOUND_STRING("Breeder FR") },
    // v1.1 — FRLG-unique classes (no Hoenn counterpart, no suffix needed)
    { TRAINER_PIC_FRONT_SUPER_NERD_FRLG,       COMPOUND_STRING("Super Nerd") },
    { TRAINER_PIC_FRONT_BIKER_FRLG,            COMPOUND_STRING("Biker") },
    { TRAINER_PIC_FRONT_BURGLAR_FRLG,          COMPOUND_STRING("Burglar") },
    { TRAINER_PIC_FRONT_ENGINEER_FRLG,         COMPOUND_STRING("Engineer") },
    { TRAINER_PIC_FRONT_CUE_BALL_FRLG,         COMPOUND_STRING("Cue Ball") },
    { TRAINER_PIC_FRONT_GAMER_FRLG,            COMPOUND_STRING("Gamer") },
    { TRAINER_PIC_FRONT_ROCKER_FRLG,           COMPOUND_STRING("Rocker") },
    { TRAINER_PIC_FRONT_JUGGLER_FRLG,          COMPOUND_STRING("Juggler") },
    { TRAINER_PIC_FRONT_TAMER_FRLG,            COMPOUND_STRING("Tamer") },
    { TRAINER_PIC_FRONT_SCIENTIST_FRLG,        COMPOUND_STRING("Scientist") },
    { TRAINER_PIC_FRONT_CHANNELER_FRLG,        COMPOUND_STRING("Channeler") },
    { TRAINER_PIC_FRONT_COOL_COUPLE_FRLG,      COMPOUND_STRING("Cool Couple") },
    { TRAINER_PIC_FRONT_CRUSH_KIN_FRLG,        COMPOUND_STRING("Crush Kin") },
    { TRAINER_PIC_FRONT_CRUSH_GIRL_FRLG,       COMPOUND_STRING("Crush Girl") },
    { TRAINER_PIC_FRONT_PAINTER_FRLG,          COMPOUND_STRING("Painter") },
    // v1.1 — FRLG named NPCs (rivals, gym leaders, E4, Giovanni, Oak, Team Rocket)
    { TRAINER_PIC_FRONT_RIVAL_EARLY_FRLG,      COMPOUND_STRING("Rival Early") },
    { TRAINER_PIC_FRONT_RIVAL_LATE_FRLG,       COMPOUND_STRING("Rival Late") },
    { TRAINER_PIC_FRONT_CHAMPION_RIVAL_FRLG,   COMPOUND_STRING("Champ Blue") },
    { TRAINER_PIC_FRONT_PROFESSOR_OAK_FRLG,    COMPOUND_STRING("Prof Oak") },
    { TRAINER_PIC_FRONT_LEADER_GIOVANNI_FRLG,  COMPOUND_STRING("Ldr Giovanni") },
    { TRAINER_PIC_FRONT_LEADER_BROCK_FRLG,     COMPOUND_STRING("Ldr Brock") },
    { TRAINER_PIC_FRONT_LEADER_MISTY_FRLG,     COMPOUND_STRING("Ldr Misty") },
    { TRAINER_PIC_FRONT_LEADER_LT_SURGE_FRLG,  COMPOUND_STRING("Ldr Surge") },
    { TRAINER_PIC_FRONT_LEADER_ERIKA_FRLG,     COMPOUND_STRING("Ldr Erika") },
    { TRAINER_PIC_FRONT_LEADER_KOGA_FRLG,      COMPOUND_STRING("Ldr Koga") },
    { TRAINER_PIC_FRONT_LEADER_BLAINE_FRLG,    COMPOUND_STRING("Ldr Blaine") },
    { TRAINER_PIC_FRONT_LEADER_SABRINA_FRLG,   COMPOUND_STRING("Ldr Sabrina") },
    { TRAINER_PIC_FRONT_ELITE_FOUR_LORELEI_FRLG, COMPOUND_STRING("E4 Lorelei") },
    { TRAINER_PIC_FRONT_ELITE_FOUR_BRUNO_FRLG, COMPOUND_STRING("E4 Bruno") },
    { TRAINER_PIC_FRONT_ELITE_FOUR_AGATHA_FRLG,COMPOUND_STRING("E4 Agatha") },
    { TRAINER_PIC_FRONT_ELITE_FOUR_LANCE_FRLG, COMPOUND_STRING("E4 Lance") },
    { TRAINER_PIC_FRONT_ROCKET_GRUNT_M_FRLG,   COMPOUND_STRING("Rocket M") },
    { TRAINER_PIC_FRONT_ROCKET_GRUNT_F_FRLG,   COMPOUND_STRING("Rocket F") },
};

static void Debug_Display_BuildTrainerPic(u16 picId, u8 windowId)
{
    const u8 *className = NULL;
    // Linear scan of ~155 entries (v1.1 expansion — full TRAINER_PIC_FRONT_*
    // coverage) — still well under any conceivable performance bound, and
    // crucially does NOT touch the gTrainers[][1090] table.
    for (u32 i = 0; i < ARRAY_COUNT(sBuildTrainerPicLabels); i++)
    {
        if (sBuildTrainerPicLabels[i].picId == picId)
        {
            className = sBuildTrainerPicLabels[i].name;
            break;
        }
    }
    if (className != NULL)
        StringCopy(gStringVar1, className);
    else
        StringCopy(gStringVar1, COMPOUND_STRING("(other)"));
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    ConvertIntToDecimalStringN(gStringVar3, picId, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Pic: {STR_VAR_3}\n{STR_VAR_1}{CLEAR_TO 90}\nA OK  B Cancel{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static u16 BuildTrainer_StepTrainerPic(u16 current, s32 direction, s32 step)
{
    // Clamp to FRONT range — opponent slots can't use BACK sprites.
    s32 candidate = (s32)current;
    s32 remaining = step;
    s32 maxIter = 2 * TRAINER_PIC_FRONT_COUNT;
    while (remaining > 0 && maxIter-- > 0)
    {
        candidate += direction;
        if (candidate <= 0)
            candidate = TRAINER_PIC_FRONT_COUNT - 1;
        else if (candidate >= TRAINER_PIC_FRONT_COUNT)
            candidate = 1;
        remaining--;
    }
    return (u16)candidate;
}

static void DebugAction_BuildTrainer_TrainerPicPicker_Select(u8 taskId)
{
    struct SimCustomTrainer *slot = &gSaveBlock3Ptr->simCustomTrainers[sBuildTrainerActiveSlot];
    bool32 redraw = FALSE;
    if (JOY_NEW(L_BUTTON))
    {
        PlaySE(SE_SELECT);
        slot->trainerPic = BuildTrainer_StepTrainerPic(slot->trainerPic, -1, 25);
        redraw = TRUE;
    }
    else if (JOY_NEW(R_BUTTON))
    {
        PlaySE(SE_SELECT);
        slot->trainerPic = BuildTrainer_StepTrainerPic(slot->trainerPic, +1, 25);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        slot->trainerPic = BuildTrainer_StepTrainerPic(slot->trainerPic, +1, 1);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        slot->trainerPic = BuildTrainer_StepTrainerPic(slot->trainerPic, -1, 1);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        slot->trainerPic = BuildTrainer_StepTrainerPic(slot->trainerPic, +1, 5);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        PlaySE(SE_SELECT);
        slot->trainerPic = BuildTrainer_StepTrainerPic(slot->trainerPic, -1, 5);
        redraw = TRUE;
    }

    if (redraw)
    {
        Debug_Display_BuildTrainerPic(slot->trainerPic, gTasks[taskId].tSubWindowId);
        return;
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        slot->inUse = 1;  // picking a sprite implies configuring this slot
        ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
        RemoveWindow(gTasks[taskId].tWindowId);
        ClearStdWindowAndFrame(gTasks[taskId].tSubWindowId, TRUE);
        RemoveWindow(gTasks[taskId].tSubWindowId);
        DestroyListMenuTask(gTasks[taskId].tMenuTaskId, NULL, NULL);
        DestroyTask(taskId);
        Debug_RemoveCallbackMenu();
        sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_SLOT;
        Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerSlot);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        // Restore the original (we stashed it in sBuildTrainerPickerOriginal).
        slot->trainerPic = sBuildTrainerPickerOriginal;
        ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
        RemoveWindow(gTasks[taskId].tWindowId);
        ClearStdWindowAndFrame(gTasks[taskId].tSubWindowId, TRUE);
        RemoveWindow(gTasks[taskId].tSubWindowId);
        DestroyListMenuTask(gTasks[taskId].tMenuTaskId, NULL, NULL);
        DestroyTask(taskId);
        Debug_RemoveCallbackMenu();
        sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_SLOT;
        Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerSlot);
    }
}

static void DebugAction_BuildTrainer_EditTrainerPic(u8 taskId)
{
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);
    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    u32 windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);
    CopyWindowToVram(windowId, COPYWIN_FULL);

    struct SimCustomTrainer *slot = &gSaveBlock3Ptr->simCustomTrainers[sBuildTrainerActiveSlot];
    // v0.52.11 — Clamp to a valid FRONT-pic range. Saves migrated from
    // before v0.52.5 added the trainerPic field can have garbage values at
    // the old padding offsets, and stepping from a wildly-out-of-range
    // starting point used to leave the picker in a confused state.
    if (slot->trainerPic == 0 || slot->trainerPic >= TRAINER_PIC_FRONT_COUNT)
        slot->trainerPic = TRAINER_PIC_FRONT_COOLTRAINER_M;
    sBuildTrainerPickerOriginal = slot->trainerPic;

    gTasks[taskId].func = DebugAction_BuildTrainer_TrainerPicPicker_Select;
    gTasks[taskId].tSubWindowId = windowId;

    Debug_Display_BuildTrainerPic(slot->trainerPic, windowId);
}

// =============================================================================
// v0.52.2 Phase 2b — Per-mon editor
// =============================================================================

// Helper: load the saveblock mon at sBuildTrainerActiveSlot[sBuildTrainerActiveMon]
// into the working buffer. If the slot is unconfigured or the mon slot is
// empty, initialize the buffer with sensible defaults (placeholder Magikarp).
static void BuildTrainer_LoadWorkBufferFromSaveblock(void)
{
    struct SimCustomTrainer *slot = &gSaveBlock3Ptr->simCustomTrainers[sBuildTrainerActiveSlot];
    // v0.52.12 — defend per-mon editor against stale save data (matches the
    // slot menu's own sanitizer)
    BuildTrainer_SanitizeSlot(slot);
    if (slot->inUse && sBuildTrainerActiveMon < slot->monCount
        && slot->mons[sBuildTrainerActiveMon].species != SPECIES_NONE)
    {
        sBuildTrainerWorkMon = slot->mons[sBuildTrainerActiveMon];
    }
    else
    {
        // Fresh defaults (placeholder Magikarp Lv 50, Hardy, Any gender)
        memset(&sBuildTrainerWorkMon, 0, sizeof(sBuildTrainerWorkMon));
        sBuildTrainerWorkMon.species = SPECIES_MAGIKARP;
        sBuildTrainerWorkMon.level = 50;
        sBuildTrainerWorkMon.nature = NATURE_HARDY;
        sBuildTrainerWorkMon.gender = 0;  // Any
        sBuildTrainerWorkMon.moves[0] = MOVE_SPLASH;
    }
}

// Helper: commit the working buffer back into the slot's saveblock data.
// Grows monCount if necessary. Marks the slot as inUse so it persists across
// reboots and the synthesized Trainer struct picks it up.
static void BuildTrainer_CommitWorkBufferToSaveblock(void)
{
    struct SimCustomTrainer *slot = &gSaveBlock3Ptr->simCustomTrainers[sBuildTrainerActiveSlot];
    if (sBuildTrainerActiveMon >= 6) return;

    // v1.6 — recompute item-driven form changes before commit. The editor
    // mutates species and heldItem independently and the engine's normal
    // item-equip event doesn't fire, so the stored species used to stay at
    // the base form even after the player attached a form-changing item
    // (the Arceus + Plate bug a player reported on Reddit — workaround was
    // to take the plate off and re-equip). Build a FormChangeContext from
    // the work buffer, ask the engine for the correct species, and overwrite
    // if it changed. Mirrors what users were doing manually by picking
    // SPECIES_ARCEUS_FIRE etc. directly in the species picker. Covers all
    // FORM_CHANGE_ITEM_HOLD mons: Arceus + Plates / Z-crystals, Silvally +
    // Memories, Genesect + Drives, Giratina + Griseous Orb.
    {
        struct FormChangeContext ctx =
        {
            .method = FORM_CHANGE_ITEM_HOLD,
            .currentSpecies = sBuildTrainerWorkMon.species,
            .heldItem = sBuildTrainerWorkMon.heldItem,
            .ability = GetAbilityBySpecies(sBuildTrainerWorkMon.species, sBuildTrainerWorkMon.abilityNum),
            .partyItemUsed = ITEM_NONE,
            .multichoiceSelection = 0,
            .status = 0,
        };
        u32 newSpecies = GetFormChangeTargetSpecies_Internal(ctx);
        if (newSpecies != 0 && newSpecies != sBuildTrainerWorkMon.species)
            sBuildTrainerWorkMon.species = newSpecies;
    }

    slot->mons[sBuildTrainerActiveMon] = sBuildTrainerWorkMon;
    if (sBuildTrainerActiveMon >= slot->monCount)
        slot->monCount = sBuildTrainerActiveMon + 1;
    slot->inUse = 1;
    // v1.4 — flush SaveBlock3 to flash so edits persist across power-off.
    // The mon-commit path is the canonical "user just confirmed a mon edit"
    // hook; saving here means every confirmed edit becomes durable. Silent
    // save (no UI popup) to keep editor flow uninterrupted.
    TrySavingData(SAVE_NORMAL);
}

// v1.1 — Copy a preset trainer's full team into the active custom slot.
// Community ask: lets the user start from "Cynthia's team" or "Volo's team"
// and edit individual mons instead of rebuilding from scratch. Wipes the
// current slot first.
//
// IV order conversion: TrainerMon's packed u32 `iv` uses engine order
// (HP/Atk/Def/Spe/SpA/SpD). SimCustomTrainerMon's `ivs[6]` uses sim order
// (HP/Atk/Def/SpA/SpD/Spe) to mirror the EV array. EV order matches between
// both structs (engine HP/Atk/Def/SpA/SpD/Spe per battle_main.c L2111-2116).
//
// Returns FALSE if the trainer ID is invalid / has no party; caller can
// SE_FAILURE on FALSE so the user knows the copy didn't take.
static bool32 BuildTrainer_CopyFromTrainer(u16 trainerId)
{
    const struct Trainer *src = GetTrainerStructFromId(trainerId);
    if (src == NULL || src->party == NULL || src->partySize == 0)
        return FALSE;
    struct SimCustomTrainer *slot = &gSaveBlock3Ptr->simCustomTrainers[sBuildTrainerActiveSlot];
    memset(slot, 0, sizeof(*slot));
    // Trainer name → slot name (truncate at SIM_CUSTOM_TRAINER_NAME_LEN).
    for (u8 i = 0; i < SIM_CUSTOM_TRAINER_NAME_LEN; i++)
    {
        u8 c = src->trainerName[i];
        slot->name[i] = c;
        if (c == EOS) break;
    }
    slot->name[SIM_CUSTOM_TRAINER_NAME_LEN] = EOS;
    slot->trainerPic = src->trainerPic;
    u8 partySize = (src->partySize > 6) ? 6 : src->partySize;
    slot->monCount = partySize;
    for (u8 i = 0; i < partySize; i++)
    {
        const struct TrainerMon *psrc = &src->party[i];
        struct SimCustomTrainerMon *pdst = &slot->mons[i];
        memset(pdst, 0, sizeof(*pdst));
        pdst->species  = psrc->species;
        pdst->heldItem = psrc->heldItem;
        for (u8 j = 0; j < 4; j++)
            pdst->moves[j] = psrc->moves[j];
        pdst->nature   = psrc->nature;
        pdst->gender   = psrc->gender;   // 0=Any/Genderless, 1=Male, 2=Female
        pdst->shiny    = psrc->isShiny;
        pdst->level    = (psrc->lvl == 0) ? 50 : psrc->lvl;
        // abilityNum default: 0 (primary ability). The TrainerMon stores the
        // resolved Ability enum, not the slot index, so a full reverse-lookup
        // would need per-species iteration. Defaulting to 0 keeps the copy
        // functional; user can A-cycle through the per-mon editor's Ability
        // row to find the original.
        pdst->abilityNum = 0;
        // IV unpack (engine order → sim order).
        u32 packedIv = psrc->iv;
        u8 hp  = (packedIv >> 0)  & 0x1F;
        u8 atk = (packedIv >> 5)  & 0x1F;
        u8 def = (packedIv >> 10) & 0x1F;
        u8 spe = (packedIv >> 15) & 0x1F;
        u8 spa = (packedIv >> 20) & 0x1F;
        u8 spd = (packedIv >> 25) & 0x1F;
        // If the trainer entry left iv=0, that means "default perfect 31s"
        // in the .party convention. Preserve that intent.
        if (packedIv == 0)
        {
            for (u8 j = 0; j < 6; j++) pdst->ivs[j] = 31;
        }
        else
        {
            pdst->ivs[0] = hp;
            pdst->ivs[1] = atk;
            pdst->ivs[2] = def;
            pdst->ivs[3] = spa;
            pdst->ivs[4] = spd;
            pdst->ivs[5] = spe;
        }
        // EVs: ev is a const u8* (may be NULL = all-zero). Same order as sim.
        if (psrc->ev != NULL)
        {
            for (u8 j = 0; j < 6; j++) pdst->evs[j] = psrc->ev[j];
        }
    }
    slot->inUse = 1;
    return TRUE;
}

// Open the per-mon editor for the given Pokémon index (0-5).
// v0.52.3 — Uses listId 4 so each row is generated through
// Debug_GenerateListBuildTrainerMonMenu, which formats the current work-buffer
// value (Species name, Item name, Ability, Move 1-4, EV total, Level, Nature,
// Gender) into gStringVar1 before the COMPOUND_STRING expands.
static void BuildTrainer_OpenMonEditor(u8 taskId, u8 monIndex)
{
    sBuildTrainerActiveMon = monIndex;
    BuildTrainer_LoadWorkBufferFromSaveblock();
    Debug_DestroyMenu(taskId);
    sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_MON;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerMon);
}

static void DebugAction_BuildTrainer_OpenMon1(u8 taskId) { BuildTrainer_OpenMonEditor(taskId, 0); }
static void DebugAction_BuildTrainer_OpenMon2(u8 taskId) { BuildTrainer_OpenMonEditor(taskId, 1); }
static void DebugAction_BuildTrainer_OpenMon3(u8 taskId) { BuildTrainer_OpenMonEditor(taskId, 2); }
static void DebugAction_BuildTrainer_OpenMon4(u8 taskId) { BuildTrainer_OpenMonEditor(taskId, 3); }
static void DebugAction_BuildTrainer_OpenMon5(u8 taskId) { BuildTrainer_OpenMonEditor(taskId, 4); }
static void DebugAction_BuildTrainer_OpenMon6(u8 taskId) { BuildTrainer_OpenMonEditor(taskId, 5); }

// "Save Slot" from the slot menu — explicit mark inUse=1 in case the user
// hasn't edited any mons but wants the slot recognized.
static void DebugAction_BuildTrainer_SaveSlot(u8 taskId)
{
    struct SimCustomTrainer *slot = &gSaveBlock3Ptr->simCustomTrainers[sBuildTrainerActiveSlot];
    slot->inUse = 1;
    if (slot->monCount == 0)
        slot->monCount = 1;  // ensure at least placeholder Magikarp shows in battle
    // v1.4 — flush SaveBlock3 for slot-level metadata edits (name/sprite/
    // Copy-Preset) that don't go through CommitWorkBufferToSaveblock.
    TrySavingData(SAVE_NORMAL);
    PlaySE(SE_SUCCESS);
}

// "Save & Back" from the per-mon editor — commit work buffer + return to
// the slot menu. Listid 3 so the slot menu redraws with the new species name
// in the Pokémon N row.
static void DebugAction_BuildTrainer_MonSaveBack(u8 taskId)
{
    BuildTrainer_CommitWorkBufferToSaveblock();
    PlaySE(SE_SUCCESS);
    Debug_DestroyMenu(taskId);
    sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_SLOT;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerSlot);
}

// "Cancel" from the per-mon editor — discard work buffer + return to slot.
static void DebugAction_BuildTrainer_MonCancel(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_SLOT;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerSlot);
}

// Cycle level by +5 (caps at 100, wraps to 5). Refreshes the menu via
// Debug_RefreshListMenu so the "Level: NN" row updates in place.
static void DebugAction_BuildTrainer_EditLevel(u8 taskId)
{
    sBuildTrainerWorkMon.level += 5;
    if (sBuildTrainerWorkMon.level > 100)
        sBuildTrainerWorkMon.level = 5;
    PlaySE(SE_SELECT);
    Debug_GenerateListBuildTrainerMonMenu();
    RedrawListMenu(gTasks[taskId].tMenuTaskId);
}

// Cycle nature (0-24 = 25 natures). Wraps.
static void DebugAction_BuildTrainer_EditNature(u8 taskId)
{
    sBuildTrainerWorkMon.nature = (sBuildTrainerWorkMon.nature + 1) % NUM_NATURES;
    PlaySE(SE_SELECT);
    Debug_GenerateListBuildTrainerMonMenu();
    RedrawListMenu(gTasks[taskId].tMenuTaskId);
}

// Cycle gender: Any → Male → Female → Any.
static void DebugAction_BuildTrainer_EditGender(u8 taskId)
{
    sBuildTrainerWorkMon.gender = (sBuildTrainerWorkMon.gender + 1) % 3;
    PlaySE(SE_SELECT);
    Debug_GenerateListBuildTrainerMonMenu();
    RedrawListMenu(gTasks[taskId].tMenuTaskId);
}

// v0.52.3 Phase 2c — Cycle the ability slot through the species' available
// abilities (slots 0, 1, and Hidden 2). Skips empty slots so the user only
// lands on real abilities the mon can have.
static void DebugAction_BuildTrainer_EditAbility(u8 taskId)
{
    u8 start = sBuildTrainerWorkMon.abilityNum;
    for (u8 i = 0; i < NUM_ABILITY_SLOTS; i++)
    {
        u8 candidate = (start + 1 + i) % NUM_ABILITY_SLOTS;
        if (GetSpeciesAbility(sBuildTrainerWorkMon.species, candidate) != ABILITY_NONE)
        {
            sBuildTrainerWorkMon.abilityNum = candidate;
            break;
        }
    }
    PlaySE(SE_SELECT);
    Debug_GenerateListBuildTrainerMonMenu();
    RedrawListMenu(gTasks[taskId].tMenuTaskId);
}

// v0.52.4 — Toggle the shiny flag. Engine reads this in
// CreateNPCTrainerPartyFromTrainer and uses OT_ID_PRESET to make personality
// XOR otId hit a shiny roll.
static void DebugAction_BuildTrainer_EditShiny(u8 taskId)
{
    sBuildTrainerWorkMon.shiny = !sBuildTrainerWorkMon.shiny;
    PlaySE(SE_SELECT);
    Debug_GenerateListBuildTrainerMonMenu();
    RedrawListMenu(gTasks[taskId].tMenuTaskId);
}

// =============================================================================
// v0.52.4 — EV / IV sub-menus
// =============================================================================

// Open the per-stat EV editor. Tears down the per-mon list and shows the
// EVs menu with listId 5 so each row formats the current stat value.
//
// v1.2 — DON'T pop Mon before pushing EVs. v1.1.1 did that thinking it would
// prevent stack growth, but it meant the callback stack went
// [BT, Slot, EVs] instead of [BT, Slot, Mon, EVs] — so when the user pressed
// B (or hit the Back row), they'd return to Slot instead of Mon. The right
// fix for stack growth is in EVs_Back / the generic B handler: pop the EVs
// entry and use Debug_ShowMenu(handler, NULL) so it reads Mon from the now-
// top callback stack entry without re-pushing.
static void DebugAction_BuildTrainer_OpenEVsMenu(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_EVS;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerEVs);
}

// Open the per-stat IV editor (listId 6). See OpenEVsMenu comment for why
// we leave Mon on the callback stack instead of popping it.
static void DebugAction_BuildTrainer_OpenIVsMenu(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_IVS;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerIVs);
}

// Cycle one EV stat by +4. Wraps from 252 back to 0 so the user can scroll
// to a low value without backing out. Stats are u8 so values are clamped to
// 252; engine SetMonData also clamps. Refreshes the EVs menu (which also
// shows the running total).
static void BuildTrainer_CycleEV(u8 taskId, u8 statIndex)
{
    u8 v = sBuildTrainerWorkMon.evs[statIndex];
    if (v >= 252)
        v = 0;
    else if (v > 248)
        v = 252;  // snap to 252 from 248+
    else
        v += 4;
    sBuildTrainerWorkMon.evs[statIndex] = v;
    PlaySE(SE_SELECT);
    Debug_GenerateListBuildTrainerEVsMenu();
    RedrawListMenu(gTasks[taskId].tMenuTaskId);
}

static void DebugAction_BuildTrainer_EditEV_HP (u8 taskId) { BuildTrainer_CycleEV(taskId, 0); }
static void DebugAction_BuildTrainer_EditEV_Atk(u8 taskId) { BuildTrainer_CycleEV(taskId, 1); }
static void DebugAction_BuildTrainer_EditEV_Def(u8 taskId) { BuildTrainer_CycleEV(taskId, 2); }
static void DebugAction_BuildTrainer_EditEV_SpA(u8 taskId) { BuildTrainer_CycleEV(taskId, 3); }
static void DebugAction_BuildTrainer_EditEV_SpD(u8 taskId) { BuildTrainer_CycleEV(taskId, 4); }
static void DebugAction_BuildTrainer_EditEV_Spe(u8 taskId) { BuildTrainer_CycleEV(taskId, 5); }

static void DebugAction_BuildTrainer_EVs_Reset(u8 taskId)
{
    memset(sBuildTrainerWorkMon.evs, 0, sizeof(sBuildTrainerWorkMon.evs));
    PlaySE(SE_SUCCESS);
    Debug_GenerateListBuildTrainerEVsMenu();
    RedrawListMenu(gTasks[taskId].tMenuTaskId);
}

static void DebugAction_BuildTrainer_EVs_Back(u8 taskId)
{
    // v1.2 — Match the generic B-button handler exactly. Stack flow:
    // before EV-open: [BT, Slot, Mon]
    // after EV-open : [BT, Slot, Mon, EVs]
    // after Back    : pop EVs → [BT, Slot, Mon], read Mon from stack top
    //                 (items=NULL means Debug_ShowMenu uses callback top),
    //                 so we don't re-push Mon and the stack stays clean.
    // EV cycle handlers write straight to sBuildTrainerWorkMon, so no
    // commit/reload is needed here — the work buffer persists across menu
    // opens until Save & Back commits it in the per-mon editor.
    Debug_DestroyMenu(taskId);
    Debug_RemoveCallbackMenu();
    sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_MON;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, NULL);
}

// Cycle one IV stat by +1. Wraps 0..31 (5-bit cap).
static void BuildTrainer_CycleIV(u8 taskId, u8 statIndex)
{
    u8 v = sBuildTrainerWorkMon.ivs[statIndex];
    v = (v + 1) % 32;
    sBuildTrainerWorkMon.ivs[statIndex] = v;
    PlaySE(SE_SELECT);
    Debug_GenerateListBuildTrainerIVsMenu();
    RedrawListMenu(gTasks[taskId].tMenuTaskId);
}

static void DebugAction_BuildTrainer_EditIV_HP (u8 taskId) { BuildTrainer_CycleIV(taskId, 0); }
static void DebugAction_BuildTrainer_EditIV_Atk(u8 taskId) { BuildTrainer_CycleIV(taskId, 1); }
static void DebugAction_BuildTrainer_EditIV_Def(u8 taskId) { BuildTrainer_CycleIV(taskId, 2); }
static void DebugAction_BuildTrainer_EditIV_SpA(u8 taskId) { BuildTrainer_CycleIV(taskId, 3); }
static void DebugAction_BuildTrainer_EditIV_SpD(u8 taskId) { BuildTrainer_CycleIV(taskId, 4); }
static void DebugAction_BuildTrainer_EditIV_Spe(u8 taskId) { BuildTrainer_CycleIV(taskId, 5); }

static void DebugAction_BuildTrainer_IVs_MaxAll(u8 taskId)
{
    for (u8 i = 0; i < 6; i++) sBuildTrainerWorkMon.ivs[i] = 31;
    PlaySE(SE_SUCCESS);
    Debug_GenerateListBuildTrainerIVsMenu();
    RedrawListMenu(gTasks[taskId].tMenuTaskId);
}

static void DebugAction_BuildTrainer_IVs_ZeroAll(u8 taskId)
{
    memset(sBuildTrainerWorkMon.ivs, 0, sizeof(sBuildTrainerWorkMon.ivs));
    PlaySE(SE_SUCCESS);
    Debug_GenerateListBuildTrainerIVsMenu();
    RedrawListMenu(gTasks[taskId].tMenuTaskId);
}

static void DebugAction_BuildTrainer_IVs_Back(u8 taskId)
{
    // v1.2 — mirror EVs_Back. See its comment for the stack-flow explanation.
    Debug_DestroyMenu(taskId);
    Debug_RemoveCallbackMenu();
    sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_MON;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, NULL);
}

// =============================================================================
// v0.52.3 Phase 2c — Dynamic label generators (PxHex-style "Label: Value")
// =============================================================================

// Slot menu: for each "Pokémon N" row, show the species name from the saved
// slot, or "-" if that slot is empty. Save Slot / Reset Slot / Back rows
// have no value suffix.
//
// NOTE: ARRAY_COUNT includes the trailing `{ NULL }` sentinel — must subtract
// 1 or StringExpandPlaceholders dereferences a NULL .text and the GBA reads
// garbage as a function pointer (observed crash: 0x01340134).
static u8 Debug_GenerateListBuildTrainerSlotMenu(void)
{
    struct SimCustomTrainer *slot = &gSaveBlock3Ptr->simCustomTrainers[sBuildTrainerActiveSlot];
    u8 totalItems = ARRAY_COUNT(sDebugMenu_Actions_BuildTrainerSlot) - 1;
    for (u32 i = 0; i < totalItems; i++)
    {
        gStringVar1[0] = EOS;
        // v0.52.5 — row indices shifted: 0 = Name, 1 = Sprite, 2-7 = Pokémon
        // 1-6, 8+ = Save / Reset / Back. Match the order in the menu array.
        if (i == 0)  // Name
        {
            if (slot->inUse && slot->name[0] != EOS)
                StringCopy(gStringVar1, slot->name);
            else
                StringCopy(gStringVar1, COMPOUND_STRING("(default)"));
        }
        else if (i == 1)  // Sprite — look up class name via static label
                          //          table (v0.52.11). Old gTrainers[] scan
                          //          was crashing on migrated saves.
        {
            u16 pic = slot->trainerPic;
            if (!slot->inUse || pic == 0 || pic >= TRAINER_PIC_FRONT_COUNT)
            {
                StringCopy(gStringVar1, COMPOUND_STRING("Cool Trnr M"));
            }
            else
            {
                const u8 *className = NULL;
                for (u32 t = 0; t < ARRAY_COUNT(sBuildTrainerPicLabels); t++)
                {
                    if (sBuildTrainerPicLabels[t].picId == pic)
                    {
                        className = sBuildTrainerPicLabels[t].name;
                        break;
                    }
                }
                if (className != NULL)
                    StringCopy(gStringVar1, className);
                else
                    ConvertIntToDecimalStringN(gStringVar1, pic, STR_CONV_MODE_LEADING_ZEROS, 3);
            }
        }
        else if (i >= 2 && i < 8)  // Pokémon 1-6 rows
        {
            u8 monIndex = i - 2;
            if (slot->inUse && monIndex < slot->monCount
                && slot->mons[monIndex].species != SPECIES_NONE)
            {
                StringCopy(gStringVar1, GetSpeciesName(slot->mons[monIndex].species));
            }
            else
            {
                StringCopy(gStringVar1, COMPOUND_STRING("-"));
            }
        }
        StringExpandPlaceholders(gStringVar4, sDebugMenu_Actions_BuildTrainerSlot[i].text);
        StringCopy(&sDebugMenuListData->itemNames[i][0], gStringVar4);
        sDebugMenuListData->listItems[i].name = &sDebugMenuListData->itemNames[i][0];
        sDebugMenuListData->listItems[i].id = i;
    }
    return totalItems;
}

// Per-mon menu: one switch case per editable field, formats the current
// work-buffer value into gStringVar1, then StringExpandPlaceholders rolls it
// into the row label. Subtracts 1 from ARRAY_COUNT to skip the `{ NULL }`
// sentinel (same hazard as the slot menu — see note there).
static u8 Debug_GenerateListBuildTrainerMonMenu(void)
{
    u8 totalItems = ARRAY_COUNT(sDebugMenu_Actions_BuildTrainerMon) - 1;
    for (u32 i = 0; i < totalItems; i++)
    {
        gStringVar1[0] = EOS;
        switch (i)
        {
        case 0:  // Species
            StringCopy(gStringVar1, GetSpeciesName(sBuildTrainerWorkMon.species));
            break;
        case 1:  // Held Item
            if (sBuildTrainerWorkMon.heldItem == ITEM_NONE)
                StringCopy(gStringVar1, COMPOUND_STRING("None"));
            else
                CopyItemName(sBuildTrainerWorkMon.heldItem, gStringVar1);
            break;
        case 2:  // Ability
            {
                enum Ability ab = GetSpeciesAbility(sBuildTrainerWorkMon.species,
                                                    sBuildTrainerWorkMon.abilityNum);
                if (ab == ABILITY_NONE)
                    StringCopy(gStringVar1, COMPOUND_STRING("-"));
                else
                    StringCopy(gStringVar1, gAbilitiesInfo[ab].name);
            }
            break;
        case 3:  // Move 1
        case 4:  // Move 2
        case 5:  // Move 3
        case 6:  // Move 4
            {
                u16 mv = sBuildTrainerWorkMon.moves[i - 3];
                if (mv == MOVE_NONE)
                    StringCopy(gStringVar1, COMPOUND_STRING("-"));
                else
                    StringCopy(gStringVar1, GetMoveName(mv));
            }
            break;
        case 7:  // EVs — running total / 510 (shows at a glance how full
                 //       the spread is; expand sub-menu for per-stat editing)
            {
                u16 total = 0;
                for (u8 j = 0; j < 6; j++) total += sBuildTrainerWorkMon.evs[j];
                u8 buf[4];
                ConvertIntToDecimalStringN(buf, total, STR_CONV_MODE_LEFT_ALIGN, 3);
                StringCopy(gStringVar1, buf);
                StringAppend(gStringVar1, COMPOUND_STRING("/510"));
            }
            break;
        case 8:  // IVs — show count of perfect 31s, or "perfect" if all 6.
                 //       Visualizes IV quality in one glance.
            {
                u8 perfectCount = 0;
                for (u8 j = 0; j < 6; j++)
                    if (sBuildTrainerWorkMon.ivs[j] == 31) perfectCount++;
                bool32 allZero = TRUE;
                for (u8 j = 0; j < 6; j++)
                    if (sBuildTrainerWorkMon.ivs[j] != 0) { allZero = FALSE; break; }
                if (allZero)
                {
                    // Backward-compat: all-zero is treated as "perfect 31s" by
                    // the synthesizer so legacy saves don't suddenly look bad.
                    StringCopy(gStringVar1, COMPOUND_STRING("31s (def)"));
                }
                else if (perfectCount == 6)
                {
                    StringCopy(gStringVar1, COMPOUND_STRING("6x 31 (max)"));
                }
                else
                {
                    u8 buf[4];
                    ConvertIntToDecimalStringN(buf, perfectCount, STR_CONV_MODE_LEFT_ALIGN, 1);
                    StringCopy(gStringVar1, buf);
                    StringAppend(gStringVar1, COMPOUND_STRING("x 31"));
                }
            }
            break;
        case 9:  // Level
            ConvertIntToDecimalStringN(gStringVar1, sBuildTrainerWorkMon.level,
                                       STR_CONV_MODE_LEFT_ALIGN, 3);
            break;
        case 10:  // Nature
            StringCopy(gStringVar1, gNaturesInfo[sBuildTrainerWorkMon.nature].name);
            break;
        case 11:  // Gender
            if (sBuildTrainerWorkMon.gender == 0)
                StringCopy(gStringVar1, COMPOUND_STRING("Any"));
            else if (sBuildTrainerWorkMon.gender == 1)
                StringCopy(gStringVar1, COMPOUND_STRING("Male"));
            else
                StringCopy(gStringVar1, COMPOUND_STRING("Female"));
            break;
        case 12:  // Shiny
            if (sBuildTrainerWorkMon.shiny)
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR GREEN}YES"));
            else
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR RED}NO"));
            break;
        default:
            // Save & Back / Cancel — no value
            break;
        }
        StringExpandPlaceholders(gStringVar4, sDebugMenu_Actions_BuildTrainerMon[i].text);
        StringCopy(&sDebugMenuListData->itemNames[i][0], gStringVar4);
        sDebugMenuListData->listItems[i].name = &sDebugMenuListData->itemNames[i][0];
        sDebugMenuListData->listItems[i].id = i;
    }
    return totalItems;
}

// v0.52.4 — Per-stat EV editor generator. Each stat row shows its current
// value (0-252). The "Total: N" row shows the running sum (useful to see
// when you've maxed at 510 and shouldn't allocate more). Reset / Back rows
// have no value suffix.
static u8 Debug_GenerateListBuildTrainerEVsMenu(void)
{
    u8 totalItems = ARRAY_COUNT(sDebugMenu_Actions_BuildTrainerEVs) - 1;
    u16 total = 0;
    for (u8 j = 0; j < 6; j++) total += sBuildTrainerWorkMon.evs[j];

    for (u32 i = 0; i < totalItems; i++)
    {
        gStringVar1[0] = EOS;
        if (i < 6)
        {
            ConvertIntToDecimalStringN(gStringVar1, sBuildTrainerWorkMon.evs[i],
                                       STR_CONV_MODE_LEFT_ALIGN, 3);
        }
        else if (i == 6)  // Total
        {
            u8 buf[4];
            ConvertIntToDecimalStringN(buf, total, STR_CONV_MODE_LEFT_ALIGN, 3);
            // Color-code: green if at or under 510, red if exceeding (illegal
            // but allowed in the editor — engine SetMonData still caps each
            // stat at 252).
            if (total <= 510)
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR GREEN}"));
            else
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR RED}"));
            StringAppend(gStringVar1, buf);
            StringAppend(gStringVar1, COMPOUND_STRING("/510"));
        }
        StringExpandPlaceholders(gStringVar4, sDebugMenu_Actions_BuildTrainerEVs[i].text);
        StringCopy(&sDebugMenuListData->itemNames[i][0], gStringVar4);
        sDebugMenuListData->listItems[i].name = &sDebugMenuListData->itemNames[i][0];
        sDebugMenuListData->listItems[i].id = i;
    }
    return totalItems;
}

// v0.52.4 — Per-stat IV editor generator. Each stat row shows 0-31 with a
// color hint (green at 31 = perfect, red at 0 = lowest).
static u8 Debug_GenerateListBuildTrainerIVsMenu(void)
{
    u8 totalItems = ARRAY_COUNT(sDebugMenu_Actions_BuildTrainerIVs) - 1;
    for (u32 i = 0; i < totalItems; i++)
    {
        gStringVar1[0] = EOS;
        if (i < 6)
        {
            u8 iv = sBuildTrainerWorkMon.ivs[i];
            u8 buf[4];
            ConvertIntToDecimalStringN(buf, iv, STR_CONV_MODE_LEFT_ALIGN, 2);
            if (iv == 31)
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR GREEN}"));
            else if (iv == 0)
                StringCopy(gStringVar1, COMPOUND_STRING("{COLOR RED}"));
            StringAppend(gStringVar1, buf);
        }
        StringExpandPlaceholders(gStringVar4, sDebugMenu_Actions_BuildTrainerIVs[i].text);
        StringCopy(&sDebugMenuListData->itemNames[i][0], gStringVar4);
        sDebugMenuListData->listItems[i].name = &sDebugMenuListData->itemNames[i][0];
        sDebugMenuListData->listItems[i].id = i;
    }
    return totalItems;
}

// =============================================================================
// v0.52.3 Phase 2c — Scrollable pickers (Species / Item / Move)
//
// Pattern mirrors DebugAction_Trainers_ChooseTrainer / DebugAction_ChooseTrainerID_Select:
//   * Open: tear down the per-mon list window, create a sub-window via
//     sDebugMenuWindowTemplateExtra, set task->func to the picker's select
//     handler, stash original value for cancel-rollback.
//   * Select: D-Up/Down step ±1, L/R step ±100 (or section jump for moves),
//     A confirms (commits to work buffer), B cancels (restores original).
//   * Both A and B return to the per-mon editor by rebuilding it via
//     Debug_ShowMenu(sDebugMenu_Actions_BuildTrainerMon).
// =============================================================================

#define DEBUG_PICKER_DIGITS_4   4

// Skip species IDs that have a placeholder/empty name. Some IDs in the
// gSpeciesInfo table are reserved (e.g. unused form-slot holes), and stepping
// onto them shows an empty row. Walk forward/backward until we hit a real
// one. Hard cap at 2*NUM_SPECIES iterations as an infinite-loop guard in
// case every entry in a stretch is empty (shouldn't happen in practice but
// cheap insurance).
static u16 BuildTrainerPicker_StepSpecies(u16 current, s32 direction, s32 step)
{
    s32 candidate = (s32)current;
    s32 remaining = step;
    s32 maxIter = 2 * NUM_SPECIES;
    while (remaining > 0 && maxIter-- > 0)
    {
        candidate += direction;
        if (candidate <= 0)
            candidate = NUM_SPECIES - 1;
        else if (candidate >= NUM_SPECIES)
            candidate = 1;
        // Accept anything that has a non-empty species name string.
        const u8 *nm = GetSpeciesName((u16)candidate);
        if (nm != NULL && nm[0] != EOS)
            remaining--;
    }
    return (u16)candidate;
}

// Sub-window display for the species picker — shows national-dex-style ID
// number plus the current species name, plus a control hint line.
static void Debug_Display_BuildTrainerSpecies(u16 species, u8 windowId)
{
    u8 *end;
    end = StringCopy(gStringVar1, GetSpeciesName(species));
    WrapFontIdToFit(gStringVar1, end, DEBUG_MENU_FONT, WindowWidthPx(windowId));
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    ConvertIntToDecimalStringN(gStringVar3, species, STR_CONV_MODE_LEADING_ZEROS, DEBUG_PICKER_DIGITS_4);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Sp: {STR_VAR_3}\n{STR_VAR_1}{CLEAR_TO 90}\nA OK  B Cancel{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

// Return from a picker to the per-mon editor. Centralized teardown so each
// picker's select handler stays small. Reuses the trainer-picker exit recipe.
static void BuildTrainerPicker_ReturnToMonEditor(u8 taskId)
{
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);
    ClearStdWindowAndFrame(gTasks[taskId].tSubWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tSubWindowId);
    DestroyListMenuTask(gTasks[taskId].tMenuTaskId, NULL, NULL);
    DestroyTask(taskId);
    Debug_RemoveCallbackMenu();
    sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_MON;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerMon);
}

// Picker select-input loop for Species. D-Up/Down ±1, L/R ±50 (fast scroll).
// A commits (sBuildTrainerWorkMon.species already tracks current), B cancels
// by restoring sBuildTrainerPickerOriginal.
// Forward decl — defined further down with the move picker. Needed here so
// the species-change confirm can wipe moves that the new species can't learn.
static bool32 BuildTrainerPicker_IsLearnable(u16 species, u16 move);

// v1.2 — Iterate moves[] and clear any the current species can't learn.
// Run after a species change so the user doesn't end up with stale moves
// from the previous species (which would also make the Move picker open at
// an invalid value and scroll-find the next-higher-ID learnable, often a
// TM-style move several hundred IDs away from where useful moves live).
static void BuildTrainer_DropIllegalMoves(void)
{
    for (u8 i = 0; i < 4; i++)
    {
        if (sBuildTrainerWorkMon.moves[i] != MOVE_NONE
            && !BuildTrainerPicker_IsLearnable(sBuildTrainerWorkMon.species, sBuildTrainerWorkMon.moves[i]))
        {
            sBuildTrainerWorkMon.moves[i] = MOVE_NONE;
        }
    }
}

static void DebugAction_BuildTrainer_SpeciesPicker_Select(u8 taskId)
{
    bool32 redraw = FALSE;
    if (JOY_NEW(L_BUTTON))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.species = BuildTrainerPicker_StepSpecies(sBuildTrainerWorkMon.species, -1, 50);
        redraw = TRUE;
    }
    else if (JOY_NEW(R_BUTTON))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.species = BuildTrainerPicker_StepSpecies(sBuildTrainerWorkMon.species, +1, 50);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.species = BuildTrainerPicker_StepSpecies(sBuildTrainerWorkMon.species, +1, 1);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.species = BuildTrainerPicker_StepSpecies(sBuildTrainerWorkMon.species, -1, 1);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.species = BuildTrainerPicker_StepSpecies(sBuildTrainerWorkMon.species, +1, 10);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.species = BuildTrainerPicker_StepSpecies(sBuildTrainerWorkMon.species, -1, 10);
        redraw = TRUE;
    }

    if (redraw)
    {
        // Stepping species may invalidate the ability slot for the new mon —
        // snap abilityNum back to 0 so the per-mon menu shows a real ability
        // when the user backs out.
        if (GetSpeciesAbility(sBuildTrainerWorkMon.species, sBuildTrainerWorkMon.abilityNum) == ABILITY_NONE)
            sBuildTrainerWorkMon.abilityNum = 0;
        Debug_Display_BuildTrainerSpecies(sBuildTrainerWorkMon.species, gTasks[taskId].tSubWindowId);
        return;
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        // v1.2 — wipe moves the new species can't learn. Catches the common
        // case of "Magikarp default has Splash, user picks Tyranitar, opens
        // Move 1 and sees Splash + TMs because Splash isn't in Tyranitar's
        // learnset and the picker has nothing valid nearby to land on."
        if (sBuildTrainerWorkMon.species != sBuildTrainerPickerOriginal)
            BuildTrainer_DropIllegalMoves();
        BuildTrainerPicker_ReturnToMonEditor(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.species = sBuildTrainerPickerOriginal;
        BuildTrainerPicker_ReturnToMonEditor(taskId);
    }
}

// Open the species picker. Tears down the list-menu window, opens the picker
// sub-window, swaps task->func to the picker handler.
static void DebugAction_BuildTrainer_EditSpecies(u8 taskId)
{
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);
    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    u32 windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);
    CopyWindowToVram(windowId, COPYWIN_FULL);

    if (sBuildTrainerWorkMon.species == SPECIES_NONE)
        sBuildTrainerWorkMon.species = SPECIES_BULBASAUR;
    sBuildTrainerPickerOriginal = sBuildTrainerWorkMon.species;

    gTasks[taskId].func = DebugAction_BuildTrainer_SpeciesPicker_Select;
    gTasks[taskId].tSubWindowId = windowId;

    Debug_Display_BuildTrainerSpecies(sBuildTrainerWorkMon.species, windowId);
}

// -----------------------------------------------------------------------------
// Held-item picker
// -----------------------------------------------------------------------------

// v0.52.7 / v0.52.9 — Only stop on items the engine treats as battle-meaningful.
// Three gates:
//   1) ITEM_NONE passes (sentinel for clearing the slot)
//   2) Pocket must NOT be POKE_BALLS / TM_HM / KEY_ITEMS — those are obviously
//      not held in battle, and the hold-effect check alone wasn't excluding
//      them strongly enough for some users (v0.52.7 still surfaced balls).
//   3) HoldEffect must not be HOLD_EFFECT_NONE — filters out medicine, repels,
//      escape rope, etc. from POCKET_ITEMS.
//
// Net result: ~150 entries (berries, plates, Z-Crystals, Mega Stones, choice
// items, stat boosters, inducers, signature items, battle utility).
static bool32 BuildTrainerPicker_IsBattleHeldItem(u16 itemId)
{
    if (itemId == ITEM_NONE)
        return TRUE;  // sentinel: clears the slot
    if (itemId >= ITEMS_COUNT)
        return FALSE;
    enum Pocket pocket = GetItemPocket(itemId);
    if (pocket == POCKET_POKE_BALLS || pocket == POCKET_TM_HM || pocket == POCKET_KEY_ITEMS)
        return FALSE;
    return GetItemHoldEffect(itemId) != HOLD_EFFECT_NONE;
}

static u16 BuildTrainerPicker_StepItem(u16 current, s32 direction, s32 step)
{
    s32 candidate = (s32)current;
    s32 remaining = step;
    // Hard cap iterations in case the whitelist ends up empty (paranoid
    // guard — there are >100 battle items, so this won't trip in practice).
    s32 maxIter = 2 * ITEMS_COUNT;
    while (remaining > 0 && maxIter-- > 0)
    {
        candidate += direction;
        if (candidate < 0)
            candidate = ITEMS_COUNT - 1;
        else if (candidate >= ITEMS_COUNT)
            candidate = 0;  // wrap through ITEM_NONE for "no item"
        if (BuildTrainerPicker_IsBattleHeldItem((u16)candidate))
            remaining--;
    }
    return (u16)candidate;
}

static void Debug_Display_BuildTrainerItem(u16 itemId, u8 windowId)
{
    u8 *end;
    if (itemId == ITEM_NONE)
        end = StringCopy(gStringVar1, COMPOUND_STRING("None"));
    else
        end = CopyItemName(itemId, gStringVar1);
    WrapFontIdToFit(gStringVar1, end, DEBUG_MENU_FONT, WindowWidthPx(windowId));
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    ConvertIntToDecimalStringN(gStringVar3, itemId, STR_CONV_MODE_LEADING_ZEROS, DEBUG_PICKER_DIGITS_4);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("It: {STR_VAR_3}\n{STR_VAR_1}{CLEAR_TO 90}\nA OK  B Cancel{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_BuildTrainer_ItemPicker_Select(u8 taskId)
{
    bool32 redraw = FALSE;
    if (JOY_NEW(L_BUTTON))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.heldItem = BuildTrainerPicker_StepItem(sBuildTrainerWorkMon.heldItem, -1, 50);
        redraw = TRUE;
    }
    else if (JOY_NEW(R_BUTTON))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.heldItem = BuildTrainerPicker_StepItem(sBuildTrainerWorkMon.heldItem, +1, 50);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.heldItem = BuildTrainerPicker_StepItem(sBuildTrainerWorkMon.heldItem, +1, 1);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.heldItem = BuildTrainerPicker_StepItem(sBuildTrainerWorkMon.heldItem, -1, 1);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.heldItem = BuildTrainerPicker_StepItem(sBuildTrainerWorkMon.heldItem, +1, 10);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.heldItem = BuildTrainerPicker_StepItem(sBuildTrainerWorkMon.heldItem, -1, 10);
        redraw = TRUE;
    }

    if (redraw)
    {
        Debug_Display_BuildTrainerItem(sBuildTrainerWorkMon.heldItem, gTasks[taskId].tSubWindowId);
        return;
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        BuildTrainerPicker_ReturnToMonEditor(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        sBuildTrainerWorkMon.heldItem = sBuildTrainerPickerOriginal;
        BuildTrainerPicker_ReturnToMonEditor(taskId);
    }
}

static void DebugAction_BuildTrainer_EditHeldItem(u8 taskId)
{
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);
    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    u32 windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);
    CopyWindowToVram(windowId, COPYWIN_FULL);

    sBuildTrainerPickerOriginal = sBuildTrainerWorkMon.heldItem;

    // v0.52.9 — If the entry value is not a battle-legal held item (e.g. a
    // Pokéball left over from an older save), snap to the next valid one
    // so the picker never *displays* a forbidden item. B-Cancel still
    // restores the entry value via sBuildTrainerPickerOriginal.
    if (!BuildTrainerPicker_IsBattleHeldItem(sBuildTrainerWorkMon.heldItem))
        sBuildTrainerWorkMon.heldItem = BuildTrainerPicker_StepItem(sBuildTrainerWorkMon.heldItem, +1, 1);

    gTasks[taskId].func = DebugAction_BuildTrainer_ItemPicker_Select;
    gTasks[taskId].tSubWindowId = windowId;

    Debug_Display_BuildTrainerItem(sBuildTrainerWorkMon.heldItem, windowId);
}

// -----------------------------------------------------------------------------
// Move picker (one handler for all 4 slots, slot stashed in
// sBuildTrainerActiveMoveSlot)
// -----------------------------------------------------------------------------

// v0.52.8 — Move filter: only stop on moves the current species can actually
// learn in this build. Checks the three learnset sources the engine uses
// elsewhere (level-up, teachable/TM/tutor, egg moves) plus MOVE_NONE as a
// sentinel for clearing the slot. Mirrors what GetLevelUpMovesBySpecies +
// CanLearnTeachableMove + MoveIsEggMove (daycare.c) consult — so anything
// shown in the picker is legal as a TrainerMon move.
static bool32 BuildTrainerPicker_IsLearnable(u16 species, u16 move)
{
    if (move == MOVE_NONE)
        return TRUE;  // sentinel: clears the slot
    if (move >= MOVES_COUNT)
        return FALSE;
    if (species == SPECIES_NONE || species >= NUM_SPECIES)
        return FALSE;

    // Level-up learnset (any level, since trainer mons aren't level-gated for
    // legality — the engine just hands them the moves).
    const struct LevelUpMove *lvUp = GetSpeciesLevelUpLearnset(species);
    if (lvUp != NULL)
    {
        for (u32 i = 0; lvUp[i].move != LEVEL_UP_MOVE_END; i++)
            if (lvUp[i].move == move)
                return TRUE;
    }

    // Teachable (TM/HM/tutor) — uses the engine's own check so we mirror its
    // species-vs-move legality table exactly.
    if (CanLearnTeachableMove(species, (enum Move)move))
        return TRUE;

    // Egg moves
    const u16 *egg = GetSpeciesEggMoves(species);
    if (egg != NULL)
    {
        for (u32 i = 0; egg[i] != MOVE_UNAVAILABLE; i++)
            if (egg[i] == move)
                return TRUE;
    }

    return FALSE;
}

static u16 BuildTrainerPicker_StepMove(u16 current, s32 direction, s32 step)
{
    u16 species = sBuildTrainerWorkMon.species;
    s32 candidate = (s32)current;
    s32 remaining = step;
    // Cap iterations to one full sweep of MOVES_COUNT in case a species has
    // an unusually thin learnset; we'll wrap once and stop on whatever's
    // closest. (Unown-style mons with one move still work.)
    s32 maxIter = 2 * MOVES_COUNT;
    while (remaining > 0 && maxIter-- > 0)
    {
        candidate += direction;
        if (candidate < 0)
            candidate = MOVES_COUNT - 1;
        else if (candidate >= MOVES_COUNT)
            candidate = 0;
        if (BuildTrainerPicker_IsLearnable(species, (u16)candidate))
            remaining--;
    }
    return (u16)candidate;
}

static void Debug_Display_BuildTrainerMove(u16 moveId, u8 windowId)
{
    u8 *end;
    if (moveId == MOVE_NONE)
        end = StringCopy(gStringVar1, COMPOUND_STRING("None"));
    else
        end = StringCopy(gStringVar1, GetMoveName(moveId));
    WrapFontIdToFit(gStringVar1, end, DEBUG_MENU_FONT, WindowWidthPx(windowId));
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    ConvertIntToDecimalStringN(gStringVar3, moveId, STR_CONV_MODE_LEADING_ZEROS, DEBUG_PICKER_DIGITS_4);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Mv: {STR_VAR_3}\n{STR_VAR_1}{CLEAR_TO 90}\nA OK  B Cancel{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_BuildTrainer_MovePicker_Select(u8 taskId)
{
    u16 *targetMove = &sBuildTrainerWorkMon.moves[sBuildTrainerActiveMoveSlot];
    bool32 redraw = FALSE;
    if (JOY_NEW(L_BUTTON))
    {
        PlaySE(SE_SELECT);
        *targetMove = BuildTrainerPicker_StepMove(*targetMove, -1, 50);
        redraw = TRUE;
    }
    else if (JOY_NEW(R_BUTTON))
    {
        PlaySE(SE_SELECT);
        *targetMove = BuildTrainerPicker_StepMove(*targetMove, +1, 50);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        *targetMove = BuildTrainerPicker_StepMove(*targetMove, +1, 1);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        *targetMove = BuildTrainerPicker_StepMove(*targetMove, -1, 1);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        *targetMove = BuildTrainerPicker_StepMove(*targetMove, +1, 10);
        redraw = TRUE;
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        PlaySE(SE_SELECT);
        *targetMove = BuildTrainerPicker_StepMove(*targetMove, -1, 10);
        redraw = TRUE;
    }

    if (redraw)
    {
        Debug_Display_BuildTrainerMove(*targetMove, gTasks[taskId].tSubWindowId);
        return;
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        BuildTrainerPicker_ReturnToMonEditor(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        *targetMove = sBuildTrainerPickerOriginal;
        BuildTrainerPicker_ReturnToMonEditor(taskId);
    }
}

static void BuildTrainer_OpenMovePicker(u8 taskId, u8 slot)
{
    sBuildTrainerActiveMoveSlot = slot;

    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);
    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    u32 windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);
    CopyWindowToVram(windowId, COPYWIN_FULL);

    // v1.2 — Defensive snap: if the slot currently holds a move the species
    // can't learn (e.g. import path bypassed the species-picker sanitizer),
    // clear it before opening the picker. Without this the picker starts at
    // an illegal move and StepMove finds the next-higher learnable ID, which
    // is usually a TM rather than the more useful low-ID level-up moves.
    if (sBuildTrainerWorkMon.moves[slot] != MOVE_NONE
        && !BuildTrainerPicker_IsLearnable(sBuildTrainerWorkMon.species, sBuildTrainerWorkMon.moves[slot]))
    {
        sBuildTrainerWorkMon.moves[slot] = MOVE_NONE;
    }
    sBuildTrainerPickerOriginal = sBuildTrainerWorkMon.moves[slot];

    gTasks[taskId].func = DebugAction_BuildTrainer_MovePicker_Select;
    gTasks[taskId].tSubWindowId = windowId;

    Debug_Display_BuildTrainerMove(sBuildTrainerWorkMon.moves[slot], windowId);
}

static void DebugAction_BuildTrainer_EditMove1(u8 taskId) { BuildTrainer_OpenMovePicker(taskId, 0); }
static void DebugAction_BuildTrainer_EditMove2(u8 taskId) { BuildTrainer_OpenMovePicker(taskId, 1); }
static void DebugAction_BuildTrainer_EditMove3(u8 taskId) { BuildTrainer_OpenMovePicker(taskId, 2); }
static void DebugAction_BuildTrainer_EditMove4(u8 taskId) { BuildTrainer_OpenMovePicker(taskId, 3); }

// =============================================================================

static void DebugAction_OpenSubMenuFlagsVars(u8 taskId, const struct DebugMenuOption *items)
{
    Debug_DestroyMenu(taskId);
    sDebugMenuListData->listId = 1;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, items);
}

static void DebugAction_OpenSubMenu(u8 taskId, const struct DebugMenuOption *items)
{
    Debug_DestroyMenu(taskId);
    sDebugMenuListData->listId = 0;
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, items);
}

static void DebugAction_OpenSubMenuFakeRTC(u8 taskId, const struct DebugMenuOption *items)
{
    if (!OW_USE_FAKE_RTC)
    {
        Debug_DestroyMenu_Full_Script(taskId, Debug_EventScript_FakeRTCNotEnabled);
    }
    else
    {
        Debug_DestroyMenu(taskId);
        Debug_ShowMenu(DebugTask_HandleMenuInput_General, items);
    }
}

static void DebugAction_ExecuteScript(u8 taskId, const u8 *script)
{
    Debug_DestroyMenu_Full_Script(taskId, script);
}

static void DebugAction_ToggleFlag(u8 taskId)
{
    if (sDebugMenuListData->listId == 2)
        Debug_GenerateListTrainerMenu();
    else
        Debug_GenerateListMenuNames();

    RedrawListMenu(gTasks[taskId].tMenuTaskId);
}

static void DebugAction_OpenSubMenuCreateFollowerNPC(u8 taskId, const struct DebugMenuOption *items)
{
    if (FNPC_ENABLE_NPC_FOLLOWERS)
    {
        Debug_DestroyMenu(taskId);
        Debug_ShowMenu(DebugTask_HandleMenuInput_General, items);
    }
    else
    {
        Debug_DestroyMenu_Full_Script(taskId, Debug_Follower_NPC_Not_Enabled);
    }
}

// *******************************
// Actions Utilities

static void DebugAction_Util_Fly(u8 taskId)
{
    Debug_DestroyMenu_Full(taskId);
    SetMainCallback2(CB2_OpenFlyMap);
}

#define tMapGroup  data[5]
#define tMapNum    data[6]
#define tWarp      data[7]

#define LAST_MAP_GROUP (MAP_GROUPS_COUNT - 1)

static void DebugAction_Util_Warp_Warp(u8 taskId)
{
    u8 windowId;

    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, 3);
    ConvertIntToDecimalStringN(gStringVar2, LAST_MAP_GROUP, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringExpandPlaceholders(gStringVar1, sDebugText_Util_WarpToMap_SelMax);
    StringCopy(gStringVar3, gText_DigitIndicator[0]);
    StringExpandPlaceholders(gStringVar4, sDebugText_Util_WarpToMap_SelectMapGroup);
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);

    gTasks[taskId].func = DebugAction_Util_Warp_SelectMapGroup;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tInput = 0;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tMapGroup = 0;
    gTasks[taskId].tMapNum = 0;
    gTasks[taskId].tWarp = 0;
}

static void DebugAction_Util_Warp_SelectMapGroup(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        Debug_HandleInput_Numeric(taskId, 0, LAST_MAP_GROUP, 3);

        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, 3);
        ConvertIntToDecimalStringN(gStringVar2, LAST_MAP_GROUP, STR_CONV_MODE_LEADING_ZEROS, 3);
        StringExpandPlaceholders(gStringVar1, sDebugText_Util_WarpToMap_SelMax);
        StringCopy(gStringVar3, gText_DigitIndicator[gTasks[taskId].tDigit]);
        StringExpandPlaceholders(gStringVar4, sDebugText_Util_WarpToMap_SelectMapGroup);
        AddTextPrinterParameterized(gTasks[taskId].tSubWindowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].tMapGroup = gTasks[taskId].tInput;
        gTasks[taskId].tInput = 0;
        gTasks[taskId].tDigit = 0;

        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, (MAP_GROUP_COUNT[gTasks[taskId].tMapGroup] - 1 >= 100) ? 3 : 2);
        ConvertIntToDecimalStringN(gStringVar2, MAP_GROUP_COUNT[gTasks[taskId].tMapGroup] - 1, STR_CONV_MODE_LEADING_ZEROS, (MAP_GROUP_COUNT[gTasks[taskId].tMapGroup] - 1 >= 100) ? 3 : 2);
        StringExpandPlaceholders(gStringVar1, sDebugText_Util_WarpToMap_SelMax);
        GetMapName(gStringVar2, Overworld_GetMapHeaderByGroupAndId(gTasks[taskId].tMapGroup, gTasks[taskId].tInput)->regionMapSectionId, 0);
        StringCopy(gStringVar3, gText_DigitIndicator[gTasks[taskId].tDigit]);
        StringExpandPlaceholders(gStringVar4, sDebugText_Util_WarpToMap_SelectMap);
        AddTextPrinterParameterized(gTasks[taskId].tSubWindowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);

        gTasks[taskId].func = DebugAction_Util_Warp_SelectMap;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void DebugAction_Util_Warp_SelectMap(u8 taskId)
{
    u8 max_value = MAP_GROUP_COUNT[gTasks[taskId].tMapGroup]; //maps in the selected map group

    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        Debug_HandleInput_Numeric(taskId, 0, max_value - 1, 3);

        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, (max_value >= 100) ? 3 : 2);
        ConvertIntToDecimalStringN(gStringVar2, MAP_GROUP_COUNT[gTasks[taskId].tMapGroup] - 1, STR_CONV_MODE_LEADING_ZEROS, (max_value >= 100) ? 3 : 2);
        StringExpandPlaceholders(gStringVar1, sDebugText_Util_WarpToMap_SelMax);
        GetMapName(gStringVar2, Overworld_GetMapHeaderByGroupAndId(gTasks[taskId].tMapGroup, gTasks[taskId].tInput)->regionMapSectionId, 0);
        StringCopy(gStringVar3, gText_DigitIndicator[gTasks[taskId].tDigit]);
        StringExpandPlaceholders(gStringVar4, sDebugText_Util_WarpToMap_SelectMap);
        AddTextPrinterParameterized(gTasks[taskId].tSubWindowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].tMapNum = gTasks[taskId].tInput;
        gTasks[taskId].tInput = 0;
        gTasks[taskId].tDigit = 0;

        StringCopy(gStringVar3, gText_DigitIndicator[gTasks[taskId].tDigit]);
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, 3);
        StringExpandPlaceholders(gStringVar4, sDebugText_Util_WarpToMap_SelectWarp);
        AddTextPrinterParameterized(gTasks[taskId].tSubWindowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
        gTasks[taskId].func = DebugAction_Util_Warp_SelectWarp;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void DebugAction_Util_Warp_SelectWarp(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        if (JOY_NEW(DPAD_UP))
        {
            gTasks[taskId].tInput += sPowersOfTen[gTasks[taskId].tDigit];
            if (gTasks[taskId].tInput > 10)
                gTasks[taskId].tInput = 10;
        }
        if (JOY_NEW(DPAD_DOWN))
        {
            gTasks[taskId].tInput -= sPowersOfTen[gTasks[taskId].tDigit];
            if (gTasks[taskId].tInput < 0)
                gTasks[taskId].tInput = 0;
        }

        StringCopy(gStringVar3, gText_DigitIndicator[gTasks[taskId].tDigit]);
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, 3);
        StringExpandPlaceholders(gStringVar4, sDebugText_Util_WarpToMap_SelectWarp);
        AddTextPrinterParameterized(gTasks[taskId].tSubWindowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].tWarp = gTasks[taskId].tInput;
        //If there's no warp with the number available, warp to the center of the map.
        SetWarpDestinationToMapWarp(gTasks[taskId].tMapGroup, gTasks[taskId].tMapNum, gTasks[taskId].tWarp);
        DoWarp();
        ResetInitialPlayerAvatarState();
        DebugAction_DestroyExtraWindow(taskId);
        ScriptContext_Stop();
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

#undef tMapGroup
#undef tMapNum
#undef tWarp

void CheckSaveBlock1Size(struct ScriptContext *ctx)
{
    u32 currSb1Size = sizeof(struct SaveBlock1);
    u32 maxSb1Size = SECTOR_DATA_SIZE * (SECTOR_ID_SAVEBLOCK1_END - SECTOR_ID_SAVEBLOCK1_START + 1);
    ConvertIntToDecimalStringN(gStringVar1, currSb1Size, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar2, maxSb1Size, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar3, maxSb1Size - currSb1Size, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar4, 1, STR_CONV_MODE_LEFT_ALIGN, 6);
}

void CheckSaveBlock2Size(struct ScriptContext *ctx)
{
    u32 currSb2Size = (sizeof(struct SaveBlock2));
    u32 maxSb2Size = SECTOR_DATA_SIZE;
    ConvertIntToDecimalStringN(gStringVar1, currSb2Size, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar2, maxSb2Size, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar3, maxSb2Size - currSb2Size, STR_CONV_MODE_LEFT_ALIGN, 6);
}

void CheckSaveBlock3Size(struct ScriptContext *ctx)
{
    u32 currSb3Size = (sizeof(struct SaveBlock3));
    u32 maxSb3Size = SAVE_BLOCK_3_CHUNK_SIZE * NUM_SECTORS_PER_SLOT;
    ConvertIntToDecimalStringN(gStringVar1, currSb3Size, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar2, maxSb3Size, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar3, maxSb3Size - currSb3Size, STR_CONV_MODE_LEFT_ALIGN, 6);
}

void CheckPokemonStorageSize(struct ScriptContext *ctx)
{
    u32 currPkmnStorageSize = sizeof(struct PokemonStorage);
    u32 maxPkmnStorageSize = SECTOR_DATA_SIZE * (SECTOR_ID_PKMN_STORAGE_END - SECTOR_ID_PKMN_STORAGE_START + 1);
    ConvertIntToDecimalStringN(gStringVar1, currPkmnStorageSize, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar2, maxPkmnStorageSize, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar3, maxPkmnStorageSize - currPkmnStorageSize, STR_CONV_MODE_LEFT_ALIGN, 6);
}

enum RoundMode
{
    ROUND_CEILING,
    ROUND_NEAREST,
    ROUND_FLOOR,
};

static u8 *ConvertQ22_10ToDecimalString(u8 *string, u32 q22_10, u32 decimalDigits, enum RoundMode roundMode)
{
    string = ConvertIntToDecimalStringN(string, q22_10 >> 10, STR_CONV_MODE_LEFT_ALIGN, 10);

    if (decimalDigits == 0)
        return string;

    *string++ = CHAR_PERIOD;

    q22_10 &= (1 << 10) - 1;
    while (decimalDigits-- > 1)
    {
        q22_10 *= 10;
        *string++ = CHAR_0 + (q22_10 >> 10);
        q22_10 &= (1 << 10) - 1;
    }

    q22_10 *= 10;
    switch (roundMode)
    {
    case ROUND_CEILING: q22_10 += (1 << 10) - 1; break;
    case ROUND_NEAREST: q22_10 += 1 << (10 - 1); break;
    case ROUND_FLOOR:                            break;
    }
    *string++ = CHAR_0 + (q22_10 >> 10);

    *string++ = EOS;

    return string;
}

void CheckROMSize(struct ScriptContext *ctx)
{
    extern u8 __rom_end[];
    u32 currROMSizeB = __rom_end - (const u8 *)ROM_START;
    u32 currROMSizeKB = (currROMSizeB + 1023) / 1024;
    u32 currROMFreeKB = ((const u8 *)ROM_END - __rom_end) / 1024;
    ConvertQ22_10ToDecimalString(gStringVar1, currROMSizeKB, 2, ROUND_CEILING);
    ConvertQ22_10ToDecimalString(gStringVar2, currROMFreeKB, 2, ROUND_FLOOR);
}

static void DebugAction_Util_Weather(u8 taskId)
{
    u8 windowId;

    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateWeather);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    //Display initial ID
    StringCopy(gStringVar2, gText_DigitIndicator[0]);
    ConvertIntToDecimalStringN(gStringVar3, 1, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringCopyPadded(gStringVar1, GetWeatherName(0), CHAR_SPACE, 30);
    StringExpandPlaceholders(gStringVar4, sDebugText_Util_Weather_ID);
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);

    gTasks[taskId].func = DebugAction_Util_Weather_SelectId;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tInput = 0;
    gTasks[taskId].tDigit = 0;
}

static void DebugAction_Util_Weather_SelectId(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        Debug_HandleInput_Numeric(taskId, WEATHER_NONE, WEATHER_COUNT - 1, 3);

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].tDigit]);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, 2);

        StringCopyPadded(gStringVar1, GetWeatherName(gTasks[taskId].tInput), CHAR_SPACE, 30);

        StringExpandPlaceholders(gStringVar4, sDebugText_Util_Weather_ID);
        AddTextPrinterParameterized(gTasks[taskId].tSubWindowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].tInput <= 14 || gTasks[taskId].tInput >= 20)
        {
            gTasks[taskId].data[5] = gTasks[taskId].tInput;
            SetWeather(gTasks[taskId].data[5]);
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void DebugAction_Util_WatchCredits(u8 taskId)
{
    Debug_DestroyMenu_Full(taskId);
    SetMainCallback2(CB2_StartCreditsSequence);
}

static void DebugAction_Player_Name(u8 taskId)
{
    DoNamingScreen(NAMING_SCREEN_PLAYER, gSaveBlock2Ptr->playerName, gSaveBlock2Ptr->playerGender, 0, 0, CB2_ReturnToFieldContinueScript);
}

static void DebugAction_Player_Gender(u8 taskId)
{
    if (gSaveBlock2Ptr->playerGender == MALE)
        gSaveBlock2Ptr->playerGender = FEMALE;
    else
        gSaveBlock2Ptr->playerGender = MALE;
    Debug_DestroyMenu_Full(taskId);
    ScriptContext_Enable();
}

static void DebugAction_Player_Id(u8 taskId)
{
    u32 trainerId = Random32();
    SetTrainerId(trainerId, gSaveBlock2Ptr->playerTrainerId);
    Debug_DestroyMenu_Full(taskId);
    ScriptContext_Enable();
}

static void DebugAction_Util_CheatStart(u8 taskId)
{
    if (!FlagGet(FLAG_SYS_CLOCK_SET))
        RtcInitLocalTimeOffset(0, 0);

    InitTimeBasedEvents();
    if (IS_FRLG)
        Debug_DestroyMenu_Full_Script(taskId, Debug_CheatStartFrlg);
    else
        Debug_DestroyMenu_Full_Script(taskId, Debug_CheatStart);
}

void BufferExpansionVersion(struct ScriptContext *ctx)
{
    static const u8 sText_Released[] = _("\nRelease Build");
    static const u8 sText_Unreleased[] = _("\nDevelopment Build");
    u8 *string = gStringVar1;
    *string++ = CHAR_v;
    string = ConvertIntToDecimalStringN(string, EXPANSION_VERSION_MAJOR, STR_CONV_MODE_LEFT_ALIGN, 3);
    *string++ = CHAR_PERIOD;
    string = ConvertIntToDecimalStringN(string, EXPANSION_VERSION_MINOR, STR_CONV_MODE_LEFT_ALIGN, 3);
    *string++ = CHAR_PERIOD;
    string = ConvertIntToDecimalStringN(string, EXPANSION_VERSION_PATCH, STR_CONV_MODE_LEFT_ALIGN, 3);
    if (EXPANSION_TAGGED_RELEASE)
        string = StringCopy(string, sText_Released);
    else
        string = StringCopy(string, sText_Unreleased);
}

void DebugMenu_CalculateTime(struct ScriptContext *ctx)
{
    if (OW_USE_FAKE_RTC)
    {
        struct SiiRtcInfo *rtc = FakeRtc_GetCurrentTime();
        StringExpandPlaceholders(gStringVar1, gDayNameStringsTable[rtc->dayOfWeek]);
        ConvertIntToDecimalStringN(gStringVar2, rtc->hour, STR_CONV_MODE_LEFT_ALIGN, 3);
        ConvertIntToDecimalStringN(gStringVar3, rtc->minute, STR_CONV_MODE_LEADING_ZEROS, 2);
    }
    else
    {
        u32 day = ((gLocalTime.days - 1) + 6) % 7 ;
        RtcCalcLocalTime();
        StringExpandPlaceholders(gStringVar1, gDayNameStringsTable[day]);
        ConvertIntToDecimalStringN(gStringVar2, gLocalTime.hours, STR_CONV_MODE_LEFT_ALIGN, 3);
        ConvertIntToDecimalStringN(gStringVar3, gLocalTime.minutes, STR_CONV_MODE_LEADING_ZEROS, 2);
    }
}

void DebugMenu_CalculateTimeOfDay(struct ScriptContext *ctx)
{
    enum TimeOfDay timeOfDay = GetTimeOfDay();
    switch (timeOfDay)
    {
    case TIME_MORNING:
    case TIME_DAY:
    case TIME_EVENING:
    case TIME_NIGHT:
        StringExpandPlaceholders(gStringVar1, gTimeOfDayStringsTable[timeOfDay]);
        break;
    case TIMES_OF_DAY_COUNT:
            break;
    }
}


// *******************************
// Actions Trainers

static void Debug_Trainers_ResetTrainersData(void)
{
    // Battle Simulator: only the PLAYER AI slot is pinned to TRAINER_LOGAN
    // (so the user's team is one A-press away). All other sim slots stay at
    // TRAINER_NONE / PARTNER_NONE so tournament mode and 1v1 double battles
    // do not get an accidental opponent / partner pre-selected. Discoverability
    // of new trainers is handled by reordering sSimulatorRoster[] so that
    // DPAD-UP from TRAINER_NONE lands directly on Logan/Green LGPE/RR/Alola
    // (i.e. v0.9 new content is now at the FRONT of the roster).
    sDebugMenuListData->data[0] = TRAINER_NONE;     // Opponent
    sDebugMenuListData->data[1] = -1;
    sDebugMenuListData->data[2] = TRAINER_NONE;     // Opponent 2
    sDebugMenuListData->data[3] = -1;
    sDebugMenuListData->data[4] = PARTNER_NONE;     // Co-op Partner
    sDebugMenuListData->data[5] = FALSE;            // Double Battle toggle
    // v1.1: Player AI default. Dev builds pin Logan so his team is one A-press
    // away. RELEASE_BUILD strips Logan from sSimulatorRoster — defaulting to him
    // there would land on a trainer that isn't in the visible picker (jumps out
    // of bounds + corrupts the displayed label). Use TRAINER_NONE for release so
    // the player explicitly picks their side.
#ifdef RELEASE_BUILD
    sDebugMenuListData->data[6] = TRAINER_NONE;
#else
    sDebugMenuListData->data[6] = TRAINER_LOGAN;    // Player AI (pinned)
#endif
    sDebugMenuListData->data[7] = FALSE;            // v0.51.1 Pilot Mode toggle
}

void SetMultiTrainerBattle(struct ScriptContext *ctx);

static void ParseObjectEventScript(const u8 *script)
{
    struct ScriptContext *ctx = AllocZeroed(sizeof(struct ScriptContext));
    if (script[0] == SCR_OP_TRAINERBATTLE)
    {
        TrainerBattleLoadArgs(script + 1);
    }
    else if (Script_MatchesCallNative(script, NativeVsSeekerRematchId, TRUE))
    {
        ctx->scriptPtr = script + 5;
        sDebugMenuListData->data[0] = ScriptPeekHalfword(ctx);
    }
    else if (Script_MatchesSpecial(script, SavePlayerParty) && Script_MatchesCallNative(script + 3, SetMultiTrainerBattle, FALSE))
    {
        ctx->scriptPtr = script + 8;
        SetMultiTrainerBattle(ctx);
    }
    Free(ctx);
    if (!TRAINER_BATTLE_PARAM.opponentA && !TRAINER_BATTLE_PARAM.opponentB)
        return;
    sDebugMenuListData->data[0] = TRAINER_BATTLE_PARAM.opponentA;
    sDebugMenuListData->data[2] = TRAINER_BATTLE_PARAM.opponentB;
    if (gPartnerTrainerId)
        sDebugMenuListData->data[4] = gPartnerTrainerId - MAX_TRAINERS_COUNT;
    InitTrainerBattleParameter();
    gPartnerTrainerId = 0;
}

static void Debug_Display_LocalTrainer(u32 localId, u32 digit, u8 windowId)
{
    u32 trainerID = sDebugMenuListData->data[0];
    StringCopy(gStringVar2, gText_DigitIndicator[digit]);
    u8 *end;
    if (trainerID == TRAINER_NONE)
        end = StringCopy(gStringVar1, COMPOUND_STRING("Not a Trainer"));
    else
        end = StringCopy(gStringVar1, GetTrainerNameFromId(trainerID));
    WrapFontIdToFit(gStringVar1, end, DEBUG_MENU_FONT, WindowWidthPx(windowId));
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    ConvertIntToDecimalStringN(gStringVar3, localId, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_LOCALID);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Local ID: {STR_VAR_3}\n{STR_VAR_1}{CLEAR_TO 90}\n\n{STR_VAR_2}{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void GetTrainerIdFromLocalId(u32 localId)
{
    Debug_Trainers_ResetTrainersData();
    ParseObjectEventScript(gMapHeader.events->objectEvents[localId - 1].script);
    if (GetTrainerBattleType(sDebugMenuListData->data[0]) == TRAINER_BATTLE_TYPE_DOUBLES)
        sDebugMenuListData->data[5] = TRUE;
}

#define TRAINER_TAG 0xFDF3
#define tSpriteId   data[5]
#define LOCAL_ID_MIN 1
#define LOCAL_ID_MAX (gMapHeader.events->objectEventCount)

static void DebugAction_ChooseFromMap_Select(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        u32 previousInput = gTasks[taskId].tInput;

        do {
            Debug_HandleInput_Numeric(taskId, LOCAL_ID_MIN, LOCAL_ID_MAX, DEBUG_NUMBER_DIGITS_LOCALID);
            GetTrainerIdFromLocalId(gTasks[taskId].tInput);
        } while (sDebugMenuListData->data[0] == TRAINER_NONE && gTasks[taskId].tInput != LOCAL_ID_MIN && gTasks[taskId].tInput != LOCAL_ID_MAX);

        if (sDebugMenuListData->data[0] == TRAINER_NONE)
        {
            s32 sign = previousInput > gTasks[taskId].tInput ? 1 : -1;

            PlaySE(SE_FAILURE);

            while (gTasks[taskId].tInput != previousInput && sDebugMenuListData->data[0] == TRAINER_NONE)
            {
                gTasks[taskId].tInput += sign;
                GetTrainerIdFromLocalId(gTasks[taskId].tInput);
            }
        }

        FreeSpritePaletteByTag(TRAINER_TAG);
        DestroySprite(&gSprites[gTasks[taskId].tSpriteId]);
        Debug_Display_LocalTrainer(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
        u32 graphicsId = gMapHeader.events->objectEvents[gTasks[taskId].tInput - 1].graphicsId;
        gTasks[taskId].tSpriteId = CreateObjectGraphicsSprite(graphicsId, SpriteCallbackDummy, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4);
        StartSpriteAnim(&gSprites[gTasks[taskId].tSpriteId], ANIM_STD_GO_SOUTH);
        gSprites[gTasks[taskId].tSpriteId].oam.priority = 0;
        gSprites[gTasks[taskId].tSpriteId].oam.paletteNum =  LoadObjectEventPaletteCopy(gSprites[gTasks[taskId].tSpriteId].template->paletteTag, TRAINER_TAG);
    }

    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        sDebugMenuListData->data[1] = FirstBattleTrainerIdToRematchTableId(gRematchTable, sDebugMenuListData->data[0]);
        sDebugMenuListData->data[3] = TRUE;
        FreeSpritePaletteByTag(TRAINER_TAG);
        DestroySprite(&gSprites[gTasks[taskId].tSpriteId]);
        ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
        RemoveWindow(gTasks[taskId].tWindowId);
        ClearStdWindowAndFrame(gTasks[taskId].tSubWindowId, TRUE);
        RemoveWindow(gTasks[taskId].tSubWindowId);
        DestroyListMenuTask(gTasks[taskId].tMenuTaskId, NULL, NULL);
        DestroyTask(taskId);

        PlaySE(SE_SELECT);
        sDebugMenuListData->listId = 2;
        Debug_RemoveCallbackMenu();
        Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_Trainers);
    }
}

static void DebugAction_Trainers_ChooseFromMap(u8 taskId)
{
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    u32 windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    // Display initial object event
    u32 localId = LOCAL_ID_MIN;
    GetTrainerIdFromLocalId(localId);
    Debug_Display_LocalTrainer(localId, 0, windowId);

    //Set task data
    gTasks[taskId].func = DebugAction_ChooseFromMap_Select;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tInput = localId;
    gTasks[taskId].tDigit = 0;

    u32 graphicsId = gMapHeader.events->objectEvents[localId - 1].graphicsId;
    u32 spriteId = CreateObjectGraphicsSprite(graphicsId, SpriteCallbackDummy, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4);
    StartSpriteAnim(&gSprites[spriteId], ANIM_STD_GO_SOUTH);
    gSprites[spriteId].oam.priority = 0;
    gSprites[spriteId].oam.paletteNum =  LoadObjectEventPaletteCopy(gSprites[spriteId].template->paletteTag, TRAINER_TAG);
    gTasks[taskId].tSpriteId = spriteId;
}

#undef TRAINER_TAG
#undef tSpriteId
#undef LOCAL_ID_MIN
#undef LOCAL_ID_MAX

#define tSelection  data[5]
#define tInitial    data[6]

static u8 GetTrainerMaxLevel(u32 trainerID)
{
    const struct Trainer *t;
    u8 maxLvl = 0;
    if (trainerID == 0 || trainerID >= TRAINERS_COUNT)
        return 0;
    t = GetTrainerStructFromId(trainerID);
    for (u32 i = 0; i < t->partySize; i++)
        if (t->party[i].lvl > maxLvl)
            maxLvl = t->party[i].lvl;
    return maxLvl;
}

static void Debug_Display_TrainerID(u32 trainerID, u32 selection, u32 digit, u8 windowId)
{
    // Partner slot: only wrap small values (real partner table IDs) into TRAINER_PARTNER.
    // Curated-roster regular trainer IDs (>= PARTNER_COUNT) are valid as-is.
    if (selection == TRAINERS_DEBUG_SELECTION_PARTNER && trainerID < PARTNER_COUNT)
        trainerID = TRAINER_PARTNER(trainerID);
    StringCopy(gStringVar2, gText_DigitIndicator[digit]);
    u8 *end;
    if (trainerID == TRAINER_NONE || trainerID == TRAINER_PARTNER(PARTNER_NONE))
        end = StringCopy(gStringVar1, COMPOUND_STRING("None"));
    else
        end = StringCopy(gStringVar1, GetTrainerNameFromId(trainerID));
    // Battle Simulator: tag where the team came from (HGSS rematches, BW2 PWT,
    // ORAS post-game) so you can tell the variants apart at a glance.
    if (trainerID < TRAINER_PARTNER(PARTNER_NONE))
        end = StringAppend(gStringVar1, GetSimSourceSuffix(trainerID));
    // Battle Simulator: append top-mon level so you can see how strong they are.
    u8 maxLvl = (trainerID < TRAINER_PARTNER(PARTNER_NONE)) ? GetTrainerMaxLevel(trainerID) : 0;
    if (maxLvl > 0)
    {
        u8 lvBuf[8];
        end = StringAppend(gStringVar1, COMPOUND_STRING(" Lv"));
        ConvertIntToDecimalStringN(lvBuf, maxLvl, STR_CONV_MODE_LEFT_ALIGN, 3);
        end = StringAppend(gStringVar1, lvBuf);
    }
    WrapFontIdToFit(gStringVar1, end, DEBUG_MENU_FONT, WindowWidthPx(windowId));
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    ConvertIntToDecimalStringN(gStringVar3, trainerID, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_TRAINERS);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("ID: {STR_VAR_3}\n{STR_VAR_1}{CLEAR_TO 90}\n\n{STR_VAR_2}{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static u32 SimulatorRoster_Step(u32 currentId, s32 direction)
{
    s32 currentIdx = -1;
    for (u32 i = 0; i < SIMULATOR_ROSTER_COUNT; i++)
    {
        if (sSimulatorRoster[i] == currentId)
        {
            currentIdx = (s32)i;
            break;
        }
    }
    s32 newIdx;
    if (currentIdx < 0)
        newIdx = (direction > 0) ? 0 : (s32)(SIMULATOR_ROSTER_COUNT - 1);
    else
    {
        newIdx = currentIdx + direction;
        if (newIdx < 0)
            newIdx = (s32)(SIMULATOR_ROSTER_COUNT - 1);
        else if (newIdx >= (s32)SIMULATOR_ROSTER_COUNT)
            newIdx = 0;
    }
    return sSimulatorRoster[newIdx];
}

// Battle Simulator: roster section boundaries — when the user presses L or R
// in the trainer picker, the cursor jumps to the start of the prev / next
// section. Each entry is an index into sSimulatorRoster[] where a new section
// begins. Sections are defined in game/region order matching the roster
// layout (top-to-bottom in the array). Update this when sSimulatorRoster is
// reordered. v0.10.2: combined Alola sub-sections into one Alola section,
// added HGSS / FRLG / Emerald sections.
// v0.54 — In RELEASE_BUILD the Custom challengers section is gone (Logan +
// Taylor stripped from sSimulatorRoster[] above), so every other section
// index shifts down by 2. CUSTOM_OFFSET handles the math without splitting
// this array into two ifdef'd variants.
#ifdef RELEASE_BUILD
#define CUSTOM_OFFSET 0
#else
#define CUSTOM_OFFSET 2
#endif
// v1.5 — type bumped u8 → u16 since the LA section now starts past index 255.
// Total roster is approaching 260 with the new PWT entries + VGC 2012 cup.
static const u16 sSimulatorRosterSectionStarts[] = {
#ifndef RELEASE_BUILD
    0,                       // Custom challengers — Logan, Taylor (2) — dev only
#endif
    CUSTOM_OFFSET,           // LGPE — Green + Lorelei/Agatha/Lance/Red/Blue/Bruno + Trace (9)
    9 + CUSTOM_OFFSET,       // FRLG — Indigo E4 + Blue starter variants (7)
    16 + CUSTOM_OFFSET,      // HGSS — Red + Blue HGSS + 3 Silvers + gym/E4/Champion (25)
    41 + CUSTOM_OFFSET,      // Emerald — Hoenn 5 gyms + E4 + Steven + Wallace (14)
    55 + CUSTOM_OFFSET,      // ORAS — Hoenn ORAS E4 + Wally/Steven/May/Brendan (8)
    63 + CUSTOM_OFFSET,      // Platinum — 3 Barrys + Battleground + stat + E4 + Cynthia Pt (21)
    84 + CUSTOM_OFFSET,      // BDSP — 3 Barrys + 8 gym + 4 E4 + Cynthia + 3 Lucas + 3 Dawn (22)
    106 + CUSTOM_OFFSET,     // SwSh — Leon x3 + Hop x6 + Mustard x2 + Marnie/Bede + 9 gym + Klara/Avery/Peony (25)
    131 + CUSTOM_OFFSET,     // Alola — Trial Captains + Kahunas + E4 + Champion + Variants + v0.48 USUM (32)
    163 + CUSTOM_OFFSET,     // Rainbow Rocket — Episode RR bosses (10)
    173 + CUSTOM_OFFSET,     // PWT Kanto (8)
    181 + CUSTOM_OFFSET,     // PWT Hoenn (9)
    190 + CUSTOM_OFFSET,     // PWT Johto (8)
    198 + CUSTOM_OFFSET,     // PWT Sinnoh (8)
    206 + CUSTOM_OFFSET,     // PWT Unova (8 base + 5 v1.5 = 13: Lenora..Marlon + Cilan/Chili/Cress/Cheren/Roxie)
    219 + CUSTOM_OFFSET,     // PWT Champs — Red/Blue/Lance/Steven/Wallace + Cynthia/Iris/Alder + v1.5 Bianca World Leaders (9)
    228 + CUSTOM_OFFSET,     // BW — N x2 + Alder + Cheren x3 + Bianca x3 + Hugh x3 + Unova E4 + Ghetsis + Colress (18)
    246 + CUSTOM_OFFSET,     // XY — Diantha/Serena/Calem (3)
    249 + CUSTOM_OFFSET,     // Anime — Ash World Champion (1)
    250 + CUSTOM_OFFSET,     // VGC — v1.5: Wolfe Glick + Ray Rizo (2012 World Finals) (2)
    252 + CUSTOM_OFFSET,     // Custom — v0.51 + v1.1 user-built slots (6: 3 original + 3 added)
    258 + CUSTOM_OFFSET,     // Legends Arceus — v0.53 Volo/Adaman/Irida/Ingo/Akari + v0.53.2 Kamado/Zisu/Beni/Rei (9)
    267 + CUSTOM_OFFSET,     // RGBY — v1.6: Prof. Oak Glitch x3 (Venusaur/Charizard/Blastoise variants based on player's starter)
};
#define SIMULATOR_ROSTER_SECTION_COUNT (sizeof(sSimulatorRosterSectionStarts) / sizeof(sSimulatorRosterSectionStarts[0]))

// Battle Simulator: jump the picker to the start of the previous / next
// section in sSimulatorRoster[]. Lets the user fast-scroll through ~120
// trainers by region/game origin instead of mashing DPAD-UP.
static u32 SimulatorRoster_JumpSection(u32 currentId, s32 direction)
{
    // Find the current roster index for currentId. If not in roster (e.g.
    // TRAINER_NONE default), seed to section 0 (Logan).
    s32 currentIdx = -1;
    for (u32 i = 0; i < SIMULATOR_ROSTER_COUNT; i++)
    {
        if (sSimulatorRoster[i] == currentId)
        {
            currentIdx = (s32)i;
            break;
        }
    }
    if (currentIdx < 0)
        return sSimulatorRoster[sSimulatorRosterSectionStarts[0]];

    // Find which section currentIdx falls into (largest start <= currentIdx).
    s32 currentSection = 0;
    for (s32 i = 0; i < (s32)SIMULATOR_ROSTER_SECTION_COUNT; i++)
    {
        if (sSimulatorRosterSectionStarts[i] <= (u16)currentIdx)
            currentSection = i;
        else
            break;
    }

    s32 newSection;
    if (direction > 0)
    {
        // R press: if we're not already at the section's first entry, snap to it
        // first (so the user lands on the section head). Otherwise advance.
        if ((u16)currentIdx != sSimulatorRosterSectionStarts[currentSection])
            newSection = currentSection;
        else
            newSection = (currentSection + 1) % (s32)SIMULATOR_ROSTER_SECTION_COUNT;
    }
    else
    {
        // L press: jump backward to previous section's head.
        newSection = currentSection - 1;
        if (newSection < 0)
            newSection = (s32)SIMULATOR_ROSTER_SECTION_COUNT - 1;
    }
    return sSimulatorRoster[sSimulatorRosterSectionStarts[newSection]];
}

static void DebugAction_ChooseTrainerID_Select(u8 taskId)
{
    // Battle Simulator: L_BUTTON / R_BUTTON in the picker jump the cursor
    // to the previous / next ROSTER SECTION (e.g. RR -> Alola Trial ->
    // Alola E4 -> PWT Johto). Lets the user fast-find a region/game origin
    // without scrolling through ~120 individual trainers.
    bool32 isSimSlot = (gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_PLAYER
                     || gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_TRAINER1
                     || gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_TRAINER2
                     || gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_PARTNER
                     // v1.1 — copy-to-custom uses the same roster scroll +
                     // L/R section jump UX as the sim slot pickers.
                     || gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_COPY_TO_CUSTOM
                     // v1.7 — Frontier Challenge picker also wants the
                     // curated roster scroll + L/R region jump. Without this
                     // the picker falls through to the generic 0..1310
                     // numeric scroll and shows every vanilla Emerald
                     // trainer ID, not just our ~120 curated entries.
                     || gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_FRONTIER);
    // v1.1 — copy mode is read-only against sDebugMenuListData->data[] (it
    // never writes back into a sim slot); cached separately so the scroll
    // handlers can skip the data[X] = tInput write for COPY entries.
    // v1.7 — same goes for FRONTIER mode.
    bool32 isCopyMode = (gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_COPY_TO_CUSTOM
                      || gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_FRONTIER);
    if (isSimSlot && (JOY_NEW(L_BUTTON) || JOY_NEW(R_BUTTON)))
    {
        PlaySE(SE_SELECT);
        s32 dir = JOY_NEW(R_BUTTON) ? 1 : -1;
        gTasks[taskId].tInput = SimulatorRoster_JumpSection(gTasks[taskId].tInput, dir);
        if (!isCopyMode)  // v1.1 — copy mode never writes back into sim slots
        {
            switch (gTasks[taskId].tSelection)
            {
            case TRAINERS_DEBUG_SELECTION_TRAINER1:
                sDebugMenuListData->data[0] = gTasks[taskId].tInput; break;
            case TRAINERS_DEBUG_SELECTION_TRAINER2:
                sDebugMenuListData->data[2] = gTasks[taskId].tInput; break;
            case TRAINERS_DEBUG_SELECTION_PARTNER:
                sDebugMenuListData->data[4] = gTasks[taskId].tInput; break;
            case TRAINERS_DEBUG_SELECTION_PLAYER:
                sDebugMenuListData->data[6] = gTasks[taskId].tInput; break;
            default: break;
            }
        }
        Debug_Display_TrainerID(gTasks[taskId].tInput, gTasks[taskId].tSelection, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
        // Intentionally do not fall through to DPAD handling for this frame.
        return;
    }

    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        u32 min = 1;
        u32 max = TRAINERS_COUNT - 1;
        // Roster scroll applies to every trainer slot — Co-op Partner accepts curated
        // regular trainer IDs in sim mode (TryBattle detects values >= PARTNER_COUNT
        // and loads the team from the regular trainer table).
        if (gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_TRAINER2
         || gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_PLAYER)
        {
            min = 0;
            max = TRAINERS_COUNT - 1;
        }
        else if (gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_PARTNER)
        {
            min = 0;
            max = PARTNER_COUNT - 1;
        }
        if (isSimSlot && (JOY_NEW(DPAD_UP) || JOY_NEW(DPAD_DOWN)))
        {
            s32 dir = JOY_NEW(DPAD_UP) ? 1 : -1;
            gTasks[taskId].tInput = SimulatorRoster_Step(gTasks[taskId].tInput, dir);
        }
        else
            Debug_HandleInput_Numeric(taskId, min, max, DEBUG_NUMBER_DIGITS_TRAINERS);
        switch (gTasks[taskId].tSelection)
        {
        case TRAINERS_DEBUG_SELECTION_TRAINER1:
            sDebugMenuListData->data[0] = gTasks[taskId].tInput;
            break;
        case TRAINERS_DEBUG_SELECTION_TRAINER2:
            sDebugMenuListData->data[2] = gTasks[taskId].tInput;
            break;
        case TRAINERS_DEBUG_SELECTION_PARTNER:
            sDebugMenuListData->data[4] = gTasks[taskId].tInput;
            break;
        case TRAINERS_DEBUG_SELECTION_PLAYER:
            sDebugMenuListData->data[6] = gTasks[taskId].tInput;
            break;
        }
        Debug_Display_TrainerID(gTasks[taskId].tInput, gTasks[taskId].tSelection, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
    }

    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        // v1.1 — copy-to-custom commits on A, cancels on B, and returns to the
        // slot menu instead of the sim menu.
        bool32 isCopyConfirm = (gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_COPY_TO_CUSTOM);
        // v1.7 — Frontier Challenge picker. A launches the warp + team load,
        // B cancels back to the wrapper menu (not the sim menu, since the
        // user came in from the wrapper).
        bool32 isFrontier = (gTasks[taskId].tSelection == TRAINERS_DEBUG_SELECTION_FRONTIER);
        bool32 frontierLaunch = isFrontier && JOY_NEW(A_BUTTON);
        if (isCopyConfirm && JOY_NEW(A_BUTTON))
        {
            if (BuildTrainer_CopyFromTrainer((u16)gTasks[taskId].tInput))
                PlaySE(SE_SUCCESS);
            else
                PlaySE(SE_FAILURE);
        }
        else if (frontierLaunch)
        {
            // Launches the warp into the Battle Frontier hub with the chosen
            // trainer's team loaded into gPlayerParty. The function pushes a
            // new MainCallback2 (CB2_LoadMap) so the menu-reopen branch below
            // is a no-op for this path — the warp takes over the next frame.
            Sim_StartFrontierChallenge((u16)gTasks[taskId].tInput);
        }
        if (!isCopyConfirm && !isFrontier && gTasks[taskId].tInput != gTasks[taskId].tInitial)
        {
            sDebugMenuListData->data[3] = FALSE;
            sDebugMenuListData->data[1] = -1;
        }
        ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
        RemoveWindow(gTasks[taskId].tWindowId);
        ClearStdWindowAndFrame(gTasks[taskId].tSubWindowId, TRUE);
        RemoveWindow(gTasks[taskId].tSubWindowId);
        DestroyListMenuTask(gTasks[taskId].tMenuTaskId, NULL, NULL);
        DestroyTask(taskId);
        Debug_RemoveCallbackMenu();
        if (isCopyConfirm)
        {
            sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_SLOT;
            Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerSlot);
        }
        else if (frontierLaunch)
        {
            // Sim_StartFrontierChallenge already pushed CB2_LoadMap — don't
            // reopen any menu, the warp takes over.
        }
        else if (isFrontier)
        {
            // B-button cancel: return to the wrapper menu the user came from.
            sDebugMenuListData->listId = 0;
            Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_TrainersWrapper);
        }
        else
        {
            sDebugMenuListData->listId = 2;
            Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_Trainers);
        }
    }
}

static void DebugAction_Trainers_ChooseTrainer(u8 taskId, u32 selection)
{
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    u32 windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    //Set task data
    gTasks[taskId].func = DebugAction_ChooseTrainerID_Select;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tSelection = (s32)selection;

    switch (gTasks[taskId].tSelection)
    {
    case TRAINERS_DEBUG_SELECTION_TRAINER1:
        gTasks[taskId].tInput = sDebugMenuListData->data[0];
        break;
    case TRAINERS_DEBUG_SELECTION_TRAINER2:
        gTasks[taskId].tInput = sDebugMenuListData->data[2];
        break;
    case TRAINERS_DEBUG_SELECTION_PARTNER:
        gTasks[taskId].tInput = sDebugMenuListData->data[4];
        break;
    case TRAINERS_DEBUG_SELECTION_PLAYER:
        gTasks[taskId].tInput = sDebugMenuListData->data[6];
        break;
    case TRAINERS_DEBUG_SELECTION_FRONTIER:
        // v1.7 — Frontier Challenge picker. No saved selection to restore;
        // land on the first roster trainer (skipping all the vanilla Emerald
        // trainer IDs that aren't part of our curated sSimulatorRoster).
        // Mirrors COPY_TO_CUSTOM's initial-value logic.
        gTasks[taskId].tInput = sSimulatorRoster[0];
        break;
    case TRAINERS_DEBUG_SELECTION_COPY_TO_CUSTOM:
        // v1.1 — copy picker has no "saved selection" to restore. Land on the
        // first roster trainer so L/R section-jump and D-pad scroll work
        // immediately.
        gTasks[taskId].tInput = sSimulatorRoster[0];
        break;
    }
    gTasks[taskId].tInitial = gTasks[taskId].tInput;
    // Display initial trainer
    Debug_Display_TrainerID(gTasks[taskId].tInput, gTasks[taskId].tSelection, 0, windowId);
}

#undef tSelection
#undef tInitial

static void DebugAction_Trainers_SwitchDoublesFlag(u8 taskId)
{
    if (sDebugMenuListData->data[2] != TRAINER_NONE || sDebugMenuListData->data[4] != PARTNER_NONE)
        return;
    if (sDebugMenuListData->data[5])
        sDebugMenuListData->data[5] = FALSE;
    else
        sDebugMenuListData->data[5] = TRUE;
}

// Battle Simulator v0.51.1: toggle Pilot Mode. When ON, the player AI slot
// becomes a human-controlled trainer — you pick moves / switch mons via the
// normal in-battle menu using the Player AI's team as your party. This works
// because the engine routes the player slot through SetControllerToPlayer
// (instead of the AiSingles / PlayerPartner AI controllers) when the
// B_FLAG_AI_VS_AI_BATTLE flag isn't set. Sim_SetupMatchRound checks data[7]
// and skips setting that flag when Pilot Mode is on.
static void DebugAction_Trainers_TogglePilotMode(u8 taskId)
{
    if (sDebugMenuListData->data[7])
        sDebugMenuListData->data[7] = FALSE;
    else
        sDebugMenuListData->data[7] = TRUE;
}

// Battle Simulator: cycle Off -> 50 -> 75 -> 100 -> Off
static void DebugAction_Trainers_ToggleLevelCap(u8 taskId)
{
    if (gSimLevelCap == 0)
        gSimLevelCap = 50;
    else if (gSimLevelCap == 50)
        gSimLevelCap = 75;
    else if (gSimLevelCap == 75)
        gSimLevelCap = 100;
    else
        gSimLevelCap = 0;
}

// Battle Simulator: cycle Best Of 1 (Off) -> 3 -> 5 -> 1. Editing this resets the
// in-flight match score so the new format starts cleanly.
static void DebugAction_Trainers_ToggleBestOf(u8 taskId)
{
    if (gSimBestOf <= 1)
        gSimBestOf = 3;
    else if (gSimBestOf == 3)
        gSimBestOf = 5;
    else
        gSimBestOf = 1;
    gSimT1Wins = 0;
    gSimT2Wins = 0;
    sSimMatchActive = FALSE;
}

// Battle Simulator: toggle VGC mode. When ON the engine treats every match as
// "bring 6 pick 4" doubles at Lv 50 — caps party builds to 4 mons, forces the
// doubles flag, and snaps the level cap to 50. Toggling OFF leaves the level
// cap where it is (so the user can keep the cap manually if they want).
static void DebugAction_Trainers_ToggleVGC(u8 taskId)
{
    gSimVGCMode = !gSimVGCMode;
    if (gSimVGCMode)
    {
        // Snap to Lv50 cap — VGC is always a Lv50 format. The Level Cap toggle
        // can still cycle to a different value after if the user wants.
        gSimLevelCap = 50;
        // Force Doubles flag on (data[5] is the picker's "force doubles" toggle).
        if (sDebugMenuListData)
            sDebugMenuListData->data[5] = TRUE;
    }
    // Reset any in-flight best-of match so the format change doesn't mix counts.
    gSimT1Wins = 0;
    gSimT2Wins = 0;
    sSimMatchActive = FALSE;
}

// Battle Simulator: tournament cup query/status helpers.
static bool32 Sim_IsTournamentActive(void)
{
    return (gSimTournamentCup > 0 && gSimTournamentCup < SIM_CUP_COUNT
            && gSimTournamentRound > 0 && !gSimTournamentDone);
}

static bool32 UNUSED Sim_IsTournamentDone(void)
{
    return (gSimTournamentCup > 0 && gSimTournamentDone);
}

// Battle Simulator: cycle Tournament cup selection. Toggles through Off and
// each defined cup, resetting in-flight tournament state. 8-trainer
// single-elim is fixed at 3 rounds (QF / SF / Final) for the player.
static void DebugAction_Trainers_ToggleTournament(u8 taskId)
{
    gSimTournamentCup++;
    if (gSimTournamentCup >= SIM_CUP_COUNT)
        gSimTournamentCup = 0;
    // Reset any in-flight tournament when the selection changes.
    gSimTournamentRound = 0;
    gSimTournamentDone = FALSE;
    gSimTournamentEliminated = FALSE;
    gSimTournamentMaxRounds = 3;  // 8-person single-elim: QF, SF, Final.
    // Also clear any active Best Of match — the two modes are mutually
    // exclusive at the round-dispatcher level.
    gSimT1Wins = 0;
    gSimT2Wins = 0;
    sSimMatchActive = FALSE;
}

// Battle Simulator: pick a winner between two bracket slots, using trainer-
// tier weighting + RNG. Higher-tier trainers win more often (Champion > E4 >
// Gym > default), but upsets still happen. Returns the WINNING slot index.
static u8 Sim_SimulateMatch(u8 slotA, u8 slotB)
{
    // Tier value: 3 = Champion, 2 = E4, 1 = Gym, 0 = default.
    s32 weightA = (s32)Sim_GetTier(gSimTournamentBracket[slotA]);
    s32 weightB = (s32)Sim_GetTier(gSimTournamentBracket[slotB]);
    // Base 50/50 with each tier difference adding ~15% advantage.
    s32 spread = (weightA - weightB) * 15;     // -45 .. +45
    s32 chanceA = 50 + spread;                  // 5 .. 95
    if (chanceA < 5)  chanceA = 5;
    if (chanceA > 95) chanceA = 95;
    u32 roll = Random() % 100;
    return (roll < (u32)chanceA) ? slotA : slotB;
}

// Battle Simulator: build the 8-trainer single-elim bracket. Shuffles the
// cup's roster into 8 slots (excluding the Player AI's trainer if present),
// places the Player AI into a random slot, then pre-simulates the 4 SIDE
// QF matches that aren't the player's. SF and F outcomes are simulated
// lazily as the player advances. Returns TRUE on success.
static bool32 Sim_BuildTournamentBracket(u8 cupIndex, s32 playerSideId)
{
    if (cupIndex == 0 || cupIndex >= SIM_CUP_COUNT)
        return FALSE;
    const struct SimCup *cup = &sSimCups[cupIndex];
    if (cup->trainers == NULL || cup->size == 0)
        return FALSE;
    if (playerSideId == TRAINER_NONE)
        return FALSE;

    // Reset bracket + results.
    for (u8 i = 0; i < SIM_TOURNAMENT_BRACKET_SIZE; i++)
        gSimTournamentBracket[i] = 0;
    for (u8 i = 0; i < SIM_TOURNAMENT_MATCH_COUNT; i++)
        gSimTournamentResults[i] = SIM_TOURNAMENT_SLOT_TBD;

    // Copy candidate list, dropping the Player AI's own trainer ID if it
    // happens to be in the cup (so they aren't paired against themselves).
    // Cap raised from 16 -> 64 in v0.10 so the PWT World Leaders cup (which
    // pools all 40+ PWT gym leaders + champions) can include its full roster
    // and the Fisher-Yates shuffle below picks a different 7 each tournament.
    u16 candidates[64];
    u8 candidateCount = 0;
    for (u8 i = 0; i < cup->size && candidateCount < ARRAY_COUNT(candidates); i++)
    {
        if ((s32)cup->trainers[i] == playerSideId)
            continue;
        candidates[candidateCount++] = cup->trainers[i];
    }
    // Need at least 7 challengers (player fills the 8th slot).
    if (candidateCount < 7)
    {
        // Pad by repeating the last candidate. Cup is too small for a full
        // 8-bracket, but we make the best of it.
        if (candidateCount == 0)
            return FALSE;
        while (candidateCount < 7 && candidateCount < ARRAY_COUNT(candidates))
        {
            candidates[candidateCount] = candidates[candidateCount % candidateCount];
            candidateCount++;
        }
    }

    // Fisher-Yates shuffle for random pairing each tournament run.
    for (s32 i = candidateCount - 1; i > 0; i--)
    {
        u32 j = Random() % (i + 1);
        u16 swap = candidates[i];
        candidates[i] = candidates[j];
        candidates[j] = swap;
    }

    // Place player AI into a random slot (0-7). The other 7 slots take the
    // first 7 shuffled candidates in order.
    gSimTournamentPlayerSlot = Random() & 7;
    u8 candidateIdx = 0;
    for (u8 slot = 0; slot < SIM_TOURNAMENT_BRACKET_SIZE; slot++)
    {
        if (slot == gSimTournamentPlayerSlot)
            gSimTournamentBracket[slot] = (u16)playerSideId;
        else
            gSimTournamentBracket[slot] = candidates[candidateIdx++];
    }

    // Pre-simulate the 4 QF matches that AREN'T the player's. The match
    // index for QF round is slot/2 (slot 0+1 → match 0, slot 2+3 → match 1,
    // slot 4+5 → match 2, slot 6+7 → match 3).
    u8 playerQfMatch = gSimTournamentPlayerSlot / 2;
    for (u8 qf = 0; qf < 4; qf++)
    {
        if (qf == playerQfMatch)
            continue;  // player plays this one live
        u8 slotA = qf * 2;
        u8 slotB = qf * 2 + 1;
        gSimTournamentResults[qf] = Sim_SimulateMatch(slotA, slotB);
    }

    return TRUE;
}

// Battle Simulator: returns the slot index of the player's current opponent
// based on which round they're in and the simulated bracket results so far.
static u8 Sim_GetPlayerOpponentSlot(void)
{
    u8 myMatch = gSimTournamentPlayerSlot / 2;       // QF match index 0-3
    if (gSimTournamentRound == 1)
    {
        // QF: opponent is your bracket pair (XOR 1 flips even↔odd).
        return gSimTournamentPlayerSlot ^ 1;
    }
    if (gSimTournamentRound == 2)
    {
        // SF: opponent is the winner of the sibling QF in the player's half.
        // Player's half = matches 0+1 (slots 0-3) or matches 2+3 (slots 4-7).
        u8 siblingQfMatch = myMatch ^ 1;
        return gSimTournamentResults[siblingQfMatch];
    }
    // Round 3 (Final): opponent is the winner of the other SF.
    // The player's SF match index is 4 (if myMatch < 2) or 5 (otherwise).
    // Lazily simulate the OTHER SF if it hasn't been computed yet.
    u8 mySfMatch = (myMatch < 2) ? 4 : 5;
    u8 otherSfMatch = 4 + (5 - mySfMatch);  // 4 → 5, 5 → 4
    if (gSimTournamentResults[otherSfMatch] == SIM_TOURNAMENT_SLOT_TBD)
    {
        // Other half's SF pairs winners of QF0+QF1 (for SF4) or QF2+QF3 (SF5).
        u8 qfA = (otherSfMatch == 4) ? 0 : 2;
        u8 qfB = qfA + 1;
        gSimTournamentResults[otherSfMatch] = Sim_SimulateMatch(
            gSimTournamentResults[qfA], gSimTournamentResults[qfB]);
    }
    return gSimTournamentResults[otherSfMatch];
}

// Battle Simulator: return the match index (0-6) for the player's current
// round. Used to record the player's match result into gSimTournamentResults.
static u8 Sim_GetPlayerMatchIdx(void)
{
    u8 playerQfMatch = gSimTournamentPlayerSlot / 2;
    if (gSimTournamentRound == 1) return playerQfMatch;
    if (gSimTournamentRound == 2) return (playerQfMatch < 2) ? 4 : 5;
    return 6;  // Final
}

// Battle Simulator: record the player's just-finished match result and
// advance the tournament state. Win → mark player slot as winner, bump
// round (or set champion). Lose → mark opponent slot as winner, mark as
// eliminated. Draws/forfeits leave the round alone for a retry.
void Sim_AdvanceTournamentAfterMatch(bool32 playerWon)
{
    if (gSimTournamentCup == 0 || gSimTournamentRound == 0 || gSimTournamentDone)
        return;

    u8 matchIdx = Sim_GetPlayerMatchIdx();
    u8 opponentSlot = Sim_GetPlayerOpponentSlot();

    if (playerWon)
    {
        gSimTournamentResults[matchIdx] = gSimTournamentPlayerSlot;
        if (gSimTournamentRound >= gSimTournamentMaxRounds)
        {
            // Won the final — champion!
            gSimTournamentDone = TRUE;
            gSimTournamentEliminated = FALSE;
        }
        else
        {
            gSimTournamentRound++;
        }
    }
    else
    {
        gSimTournamentResults[matchIdx] = opponentSlot;
        gSimTournamentDone = TRUE;
        gSimTournamentEliminated = TRUE;
    }
}

// Battle Simulator: start a tournament run — build the 8-trainer bracket and
// arm round 1 (Quarter Finals).
static void Sim_StartTournament(s32 playerSideId)
{
    if (!Sim_BuildTournamentBracket(gSimTournamentCup, playerSideId))
    {
        gSimTournamentCup = 0;
        return;
    }
    gSimTournamentMaxRounds = 3;
    gSimTournamentRound = 1;
    gSimTournamentDone = FALSE;
    gSimTournamentEliminated = FALSE;
}

static void DebugAction_Trainers_SetRematch(u8 taskId)
{
    s32 rematchId = sDebugMenuListData->data[1];

    if (rematchId == -1)
    {
        FlagToggle(TRAINER_FLAGS_START + sDebugMenuListData->data[0]);
        return;
    }

    for (u32 i = 0; i < REMATCHES_COUNT; i++)
    {
        if (gRematchTable[rematchId].trainerIds[i] == 0)
            break;

        if (!HasTrainerBeenFought(gRematchTable[rematchId].trainerIds[i]))
        {
            FlagToggle(TRAINER_FLAGS_START + gRematchTable[rematchId].trainerIds[i]);
            return;
        }
    }

    for (u32 i = 0; i < REMATCHES_COUNT; i++)
    {
        if (gRematchTable[rematchId].trainerIds[i] == 0)
            break;

        FlagToggle(TRAINER_FLAGS_START + gRematchTable[rematchId].trainerIds[i]);
    }
}

static void DebugAction_Trainers_SetRematchReadiness(u8 taskId)
{
    if (sDebugMenuListData->data[1] == -1)
        return;
    if (GetActiveTrainerRematches(sDebugMenuListData->data[1]))
        SetActiveTrainerRematches(sDebugMenuListData->data[1], FALSE);
    else
        SetActiveTrainerRematches(sDebugMenuListData->data[1], TRUE);
}

// ============================================================================
// Battle Simulator: Team Preview / Pick Logic
// ============================================================================
// Each AI in a multi/VGC sim scores its own 6 mons against the opposing
// teamsheet (the union of the other side's trainer parties) and brings the
// top N. The score combines:
//   - move/STAB type effectiveness vs the opposing types (offense)
//   - opposing best-move type effectiveness vs our typing (defense)
//   - speed tier (faster mons get a small bump)
//   - held-item value (Mega Stone / Choice / Life Orb get bonuses since
//     they directly upscale the mon in battle)
// The pick queue is consumed in order by CreateNPCTrainerPartyFromTrainer,
// which is called once per trainer being built up — Partner first, then
// Player AI from our Sim_SetupMatchRound, then Opp A and Opp B from the
// engine's battle-init path.

// Look up the best STAB-type effectiveness one of our moves has against a
// defender's typing. STAB filter keeps the score honest about what we can
// realistically threaten with — a Psychic with one Bug move shouldn't score
// huge into Dark targets unless that one move is actually a primary threat.
static uq4_12_t Sim_BestOffensiveModifier(const struct TrainerMon *atk, enum Type defT1, enum Type defT2)
{
    uq4_12_t best = UQ_4_12(0.0);
    for (u8 m = 0; m < MAX_MON_MOVES; m++)
    {
        u16 move = atk->moves[m];
        if (move == MOVE_NONE)
            continue;
        // Skip status moves; they don't deal damage, so type matchup is moot.
        if (gMovesInfo[move].category == DAMAGE_CATEGORY_STATUS)
            continue;
        if (gMovesInfo[move].power == 0)
            continue;
        enum Type mt = gMovesInfo[move].type;
        uq4_12_t eff = GetTypeModifier(mt, defT1);
        if (defT1 != defT2)
            eff = uq4_12_multiply(eff, GetTypeModifier(mt, defT2));
        if (eff > best)
            best = eff;
    }
    // Fall back to species STAB types if no usable damaging moves were specified.
    if (best == UQ_4_12(0.0))
    {
        enum Type atkT1 = gSpeciesInfo[atk->species].types[0];
        enum Type atkT2 = gSpeciesInfo[atk->species].types[1];
        uq4_12_t off1 = GetTypeModifier(atkT1, defT1);
        if (defT1 != defT2)
            off1 = uq4_12_multiply(off1, GetTypeModifier(atkT1, defT2));
        uq4_12_t off2 = off1;
        if (atkT1 != atkT2)
        {
            off2 = GetTypeModifier(atkT2, defT1);
            if (defT1 != defT2)
                off2 = uq4_12_multiply(off2, GetTypeModifier(atkT2, defT2));
        }
        best = (off1 > off2) ? off1 : off2;
    }
    return best;
}

// Held-item bonus in score-units. Mega Stones are biggest by far because Mega
// Evolution changes the mon's stats + ability outright. Choice items + Life Orb
// are flat damage multipliers, still meaningful. Returns 0..1500.
static u32 Sim_HeldItemBonus(u16 itemId)
{
    if (itemId == ITEM_NONE)
        return 0;
    enum HoldEffect he = gItemsInfo[itemId].holdEffect;
    if (he == HOLD_EFFECT_MEGA_STONE)
        return 1500;
    if (he == HOLD_EFFECT_Z_CRYSTAL)
        return 1200;
    if (he == HOLD_EFFECT_CHOICE_BAND
     || he == HOLD_EFFECT_CHOICE_SCARF
     || itemId == ITEM_CHOICE_SPECS)
        return 600;
    if (he == HOLD_EFFECT_LIFE_ORB)
        return 500;
    if (he == HOLD_EFFECT_FOCUS_SASH || he == HOLD_EFFECT_FOCUS_BAND)
        return 300;
    if (he == HOLD_EFFECT_LEFTOVERS || he == HOLD_EFFECT_BLACK_SLUDGE)
        return 200;
    if (he == HOLD_EFFECT_ASSAULT_VEST)
        return 250;
    if (he == HOLD_EFFECT_EXPERT_BELT || he == HOLD_EFFECT_WIDE_LENS)
        return 200;
    return 100;  // any held item is slightly better than none
}

// Score one of our mons against the opposing teamsheet. Higher is better — this
// becomes the ordering key for the bring-N picks.
//
// Score breakdown:
//   - For each opposing mon: +1000 * offensiveModifier - 800 * defensiveModifier
//     (modifiers are UQ4.12; UQ_4_12(2.0)=8192 means super-effective)
//   - Speed bonus: + (baseSpeed * 4)
//   - Item bonus per Sim_HeldItemBonus
static u32 Sim_ScoreMonForPick(const struct TrainerMon *me, const u16 *oppSpecies, const struct TrainerMon * const *oppMons, u8 oppCount)
{
    s32 score = 5000;  // baseline so we never go negative on bad matchups
    enum Type myT1 = gSpeciesInfo[me->species].types[0];
    enum Type myT2 = gSpeciesInfo[me->species].types[1];

    for (u8 i = 0; i < oppCount; i++)
    {
        if (oppSpecies[i] == SPECIES_NONE)
            continue;
        enum Type enT1 = gSpeciesInfo[oppSpecies[i]].types[0];
        enum Type enT2 = gSpeciesInfo[oppSpecies[i]].types[1];

        uq4_12_t myOff = Sim_BestOffensiveModifier(me, enT1, enT2);
        // (myOff units: UQ4.12 → 1.0x = 4096, 2.0x = 8192, 4.0x = 16384, 0.5x = 2048)
        // Map to score: 1.0x ≈ +1000, 2.0x ≈ +2000, 0x ≈ 0, 4x ≈ +4000.
        score += (s32)((myOff * 1000) >> UQ_4_12_SHIFT);

        // Their offensive into us. Use their stored moves the same way.
        uq4_12_t enOff = Sim_BestOffensiveModifier(oppMons[i], myT1, myT2);
        score -= (s32)((enOff * 800) >> UQ_4_12_SHIFT);
    }

    // Speed: 0..160 base speed → 0..640 score.
    score += gSpeciesInfo[me->species].baseSpeed * 4;

    // Held item value.
    score += (s32)Sim_HeldItemBonus(me->heldItem);

    return (score < 0) ? 0 : (u32)score;
}

// Selection-sort top N indices by score (descending). Stable for ties.
static void Sim_PickTopN(const struct Trainer *me, const u16 *oppSpecies, const struct TrainerMon * const *oppMons, u8 oppCount, u8 pickCount, u8 *outIndices, u8 *outCount)
{
    u8 partySize = me->partySize;
    if (partySize == 0)
    {
        *outCount = 0;
        return;
    }
    if (pickCount > partySize)
        pickCount = partySize;
    if (pickCount > SIM_PICK_INDICES_MAX)
        pickCount = SIM_PICK_INDICES_MAX;

    // Score every mon once.
    u32 scores[SIM_PICK_INDICES_MAX] = {0};
    u8 cap = partySize > SIM_PICK_INDICES_MAX ? SIM_PICK_INDICES_MAX : partySize;
    for (u8 i = 0; i < cap; i++)
        scores[i] = Sim_ScoreMonForPick(&me->party[i], oppSpecies, oppMons, oppCount);

    // Track which indices we've already picked.
    bool8 taken[SIM_PICK_INDICES_MAX] = {0};

    for (u8 slot = 0; slot < pickCount; slot++)
    {
        s32 bestIdx = -1;
        u32 bestScore = 0;
        for (u8 i = 0; i < cap; i++)
        {
            if (taken[i])
                continue;
            if (bestIdx < 0 || scores[i] > bestScore)
            {
                bestIdx = i;
                bestScore = scores[i];
            }
        }
        if (bestIdx < 0)
            break;
        outIndices[slot] = (u8)bestIdx;
        taken[bestIdx] = TRUE;
    }
    *outCount = pickCount;
}

// Build a flat species[] + mon-pointer[] list for an opposing pair so each AI
// can score against the combined teamsheet. tA/tB may be NULL (no opp on that
// slot, e.g. VGC singles vs trainer or partner-less player AI).
// Build the opposing teamsheet for scoring. If a "filter" is supplied for either
// opposing trainer, only the indices in that filter are added to the teamsheet
// (e.g. the 4 mons they brought in the previous round, for adaptive Game 2+
// picks). NULL filter or 0 filterCount means "include the whole trainer's team".
static u8 Sim_BuildOpposingTeamsheet(
    const struct Trainer *tA, const u8 *filterA, u8 filterACount,
    const struct Trainer *tB, const u8 *filterB, u8 filterBCount,
    u16 *outSpecies, const struct TrainerMon **outMons)
{
    u8 count = 0;
    if (tA != NULL)
    {
        if (filterA != NULL && filterACount > 0)
        {
            for (u8 i = 0; i < filterACount && count < 12; i++)
            {
                u8 idx = filterA[i];
                if (idx >= tA->partySize)
                    continue;
                outSpecies[count] = tA->party[idx].species;
                outMons[count] = &tA->party[idx];
                count++;
            }
        }
        else
        {
            for (u8 i = 0; i < tA->partySize && count < 12; i++)
            {
                outSpecies[count] = tA->party[i].species;
                outMons[count] = &tA->party[i];
                count++;
            }
        }
    }
    if (tB != NULL)
    {
        if (filterB != NULL && filterBCount > 0)
        {
            for (u8 i = 0; i < filterBCount && count < 12; i++)
            {
                u8 idx = filterB[i];
                if (idx >= tB->partySize)
                    continue;
                outSpecies[count] = tB->party[idx].species;
                outMons[count] = &tB->party[idx];
                count++;
            }
        }
        else
        {
            for (u8 i = 0; i < tB->partySize && count < 12; i++)
            {
                outSpecies[count] = tB->party[i].species;
                outMons[count] = &tB->party[i];
                count++;
            }
        }
    }
    return count;
}

// Look up a trainer's previous-round picks by trainer ID. Returns the number
// of picks found (0 if none / first-round). Used for adaptive scoring.
static u8 Sim_FindPrevPicks(u16 trainerId, u8 *outIndices)
{
    if (!sSimPrevPicksValid || trainerId == 0)
        return 0;
    for (u8 r = 0; r < sSimPrevPickRowCount; r++)
    {
        if (sSimPrevPickTrainerIds[r] == trainerId && sSimPrevPickCounts[r] > 0)
        {
            u8 cnt = sSimPrevPickCounts[r];
            if (cnt > SIM_PICK_INDICES_MAX)
                cnt = SIM_PICK_INDICES_MAX;
            for (u8 i = 0; i < cnt; i++)
                outIndices[i] = sSimPrevPickIndices[r][i];
            return cnt;
        }
    }
    return 0;
}

// Compute the pick row for one trainer vs an opposing trainer pair, push it
// to the queue. Adapts scoring by trying to find each opposing trainer's
// previous-round brings; if found, score is restricted to that subset.
static void Sim_QueuePicksFor(u16 meId, const struct Trainer *me,
                              u16 oppAId, const struct Trainer *oppA,
                              u16 oppBId, const struct Trainer *oppB,
                              u8 pickCount, u8 row)
{
    u8 prevA[SIM_PICK_INDICES_MAX];
    u8 prevB[SIM_PICK_INDICES_MAX];
    u8 prevACount = Sim_FindPrevPicks(oppAId, prevA);
    u8 prevBCount = Sim_FindPrevPicks(oppBId, prevB);

    u16 oppSpecies[12];
    const struct TrainerMon *oppMons[12];
    u8 oppCount = Sim_BuildOpposingTeamsheet(
        oppA, prevACount > 0 ? prevA : NULL, prevACount,
        oppB, prevBCount > 0 ? prevB : NULL, prevBCount,
        oppSpecies, oppMons);

    u8 written = 0;
    Sim_PickTopN(me, oppSpecies, oppMons, oppCount, pickCount, gSimPickIndices[row], &written);

    // Battle Simulator: Taylor always leads with Pelipper. Pelipper is at party
    // index 0 in TRAINER_TAYLOR_BRO's team. If the matchup-aware Sim_PickTopN
    // didn't put index 0 at slot 0 of the picks, swap it forward. If Pelipper
    // wasn't picked at all, force-replace slot 0 with index 0.
    if (meId == TRAINER_TAYLOR_BRO && written > 0)
    {
        u8 pelipperPos = 0xFF;
        for (u8 i = 0; i < written; i++)
        {
            if (gSimPickIndices[row][i] == 0)
            {
                pelipperPos = i;
                break;
            }
        }
        if (pelipperPos == 0xFF)
        {
            // Pelipper not picked -> force it into slot 0, push others back.
            gSimPickIndices[row][0] = 0;
        }
        else if (pelipperPos != 0)
        {
            // Swap to slot 0.
            u8 tmp = gSimPickIndices[row][0];
            gSimPickIndices[row][0] = gSimPickIndices[row][pelipperPos];
            gSimPickIndices[row][pelipperPos] = tmp;
        }
    }

    gSimPickCounts[row] = written;
    sSimCurrentPickTrainerIds[row] = meId;
}

// Battle Simulator: snapshot the just-built/just-played pick queue into the
// "previous picks" memory so the NEXT call to Sim_PrepareTeamPicks can adapt.
// Called from CB2_EndDebugBattle when more rounds (Best-Of or Tournament) are
// coming. Idempotent — safe to call even if no picks were made this round.
void Sim_SnapshotPicksForNextRound(void)
{
    sSimPrevPickRowCount = gSimPickQueueLen;
    if (sSimPrevPickRowCount > SIM_PICK_QUEUE_MAX)
        sSimPrevPickRowCount = SIM_PICK_QUEUE_MAX;
    for (u8 r = 0; r < sSimPrevPickRowCount; r++)
    {
        sSimPrevPickTrainerIds[r] = sSimCurrentPickTrainerIds[r];
        sSimPrevPickCounts[r] = gSimPickCounts[r];
        u8 cnt = gSimPickCounts[r];
        if (cnt > SIM_PICK_INDICES_MAX)
            cnt = SIM_PICK_INDICES_MAX;
        for (u8 i = 0; i < cnt; i++)
            sSimPrevPickIndices[r][i] = gSimPickIndices[r][i];
    }
    sSimPrevPicksValid = (sSimPrevPickRowCount > 0);
}

// Battle Simulator: forget all previous-pick history. Called when a fresh
// matchup is starting (TryBattle from the picker) so we don't carry over
// stale memory from a prior session.
static void Sim_ResetPickHistory(void)
{
    sSimPrevPicksValid = FALSE;
    sSimPrevPickRowCount = 0;
}

// Look up trainer struct safely. Returns NULL for stock partner / sentinel IDs.
static const struct Trainer *Sim_GetTrainerStruct(s32 id)
{
    if (id == TRAINER_NONE || id == PARTNER_NONE)
        return NULL;
    if (id < 0 || id >= TRAINERS_COUNT)
        return NULL;
    return GetTrainerStructFromId((u16)id);
}

// =============================================================================
// v1.3 — Team-code import via NAMING_SCREEN_TEAMCODE
// =============================================================================
// The v1.2 attempt at a from-scratch keyboard widget shipped unreadable glyphs
// because pokeemerald's text engine speaks CHAR_* (custom Pokemon charset),
// not ASCII. v1.3 sidesteps that by reusing the existing naming screen —
// it already maps the Pokemon charset to letters, has cursor logic, page
// swap (uppercase / lowercase / symbols), backspace, OK/Cancel. We added
// `_` to the symbols page and a NAMING_SCREEN_TEAMCODE template with 30-char
// maxChars. The flow:
//   1. User picks "Import from code…" in the per-mon editor
//   2. We tear down the debug menu, set gSimImportCodePending = TRUE, and
//      hand control to DoNamingScreen
//   3. User types the PB...code with A-Z / a-z / 0-9 / - / _ across the 3
//      keyboard pages, presses START (OK button) to confirm
//   4. Naming screen writes the typed string into gSimTeamCodeBuffer and
//      returns to the field via CB2_ReturnToField
//   5. field_control_avatar sees gSimImportCodePending, calls
//      Debug_DecodeImportedTeamCode below, which validates the code,
//      writes into sBuildTrainerWorkMon, and re-opens the per-mon editor.
//   On decode failure the slot is untouched and the error is shown via the
//   menu re-open's first redraw (the per-mon row labels show the previous
//   work-buffer state, so the user sees nothing changed).

static void DebugAction_BuildTrainer_OpenImportCode(u8 taskId)
{
    PlaySE(SE_SELECT);
    // Start with an empty buffer so the naming screen's existing-string copy
    // path doesn't pre-fill with garbage. User types the full PB-prefix.
    memset(gSimTeamCodeBuffer, EOS, sizeof(gSimTeamCodeBuffer));
    // Tear down the debug menu before switching CB2 to the naming screen.
    // The work buffer in sBuildTrainerWorkMon and the active mon/slot indices
    // both live in EWRAM so they survive the round-trip.
    Debug_DestroyMenu_Full(taskId);
    gSimImportCodePending = TRUE;
    // v1.3 — Ditto's the icon. You're pasting a code that turns into a
    // Pokémon; Ditto literally transforms into other Pokémon. Thematic
    // match + a single consistent icon across every import screen.
    // iconFunction=3 in sTeamCodeScreenTemplate hands this off to the same
    // Pokémon-icon renderer that NAMING_SCREEN_CAUGHT_MON uses.
    DoNamingScreen(NAMING_SCREEN_TEAMCODE, gSimTeamCodeBuffer,
                   SPECIES_DITTO, 0, 0, CB2_ReturnToField);
}

// Called from field_control_avatar after the naming screen returns. Decodes
// the buffer, writes into the work mon on success, then re-opens the per-mon
// editor. On failure the work buffer is unchanged.
void Debug_DecodeImportedTeamCodeAndReopen(void)
{
    struct SimCustomTrainerMon mon;
    enum SimTeamCodeResult res = Sim_DecodeTeamCode(gSimTeamCodeBuffer, &mon);
    if (res == SIM_CODE_OK)
    {
        sBuildTrainerWorkMon = mon;
        BuildTrainer_CommitWorkBufferToSaveblock();
        PlaySE(SE_SUCCESS);
    }
    else
    {
        PlaySE(SE_FAILURE);
    }
    // Re-open the per-mon editor by simulating the OpenMonEditor path.
    // sDebugMenuListData was freed by Debug_DestroyMenu_Full when we left for
    // the naming screen, so allocate fresh + reload the work buffer.
    sDebugMenuListData = AllocZeroed(sizeof(*sDebugMenuListData));
    sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_MON;
    BuildTrainer_LoadWorkBufferFromSaveblock();
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerMon);
}

// v1.2 dead-code keyboard widget block kept under #if 0 below in case the
// from-scratch approach is ever needed (e.g. for a feature the naming screen
// can't support). Skip to the next === banner.
#if 0
// On-screen base64 keyboard for the "Import from code…" row in the per-mon
// editor. The user types a "PB..." code (typ. 24-34 chars) using D-pad +
// A/B/L/R/START/SELECT. On START we decode via Sim_DecodeTeamCode and
// overwrite sBuildTrainerWorkMon. On SELECT we cancel and return to the
// per-mon editor untouched.
//
// Layout (28x18 tile window, ~224x144 px):
//   Row 0:  "Code:" header
//   Row 1:  typed buffer (first 26 chars)
//   Row 2:  typed buffer (overflow, chars 26-51)
//   Row 3:  status line (last error, or hint when fresh)
//   Rows 4-11: 8 rows of 8 chars each = 64-char base64 grid. Cursor uses
//              square brackets around the selected char.
//   Rows 12-13: control hints
//
// Cursor moves: D-pad. A inserts char, B deletes last, L/R jump 2 rows
// (vertical fast-nav), START confirms, SELECT cancels.

// 64-char URL-safe base64 alphabet. Indexed by (cursorY * 8 + cursorX).
static const u8 sCodeKeyboardChars[64] = {
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','-','_',
};

#define CODE_KB_MAX_LEN 52   // PB (2) + worst-case v2 base64 (~50)

static EWRAM_DATA u8 sCodeKbBuffer[CODE_KB_MAX_LEN + 1] = {0};
static EWRAM_DATA u8 sCodeKbLen = 0;
static EWRAM_DATA u8 sCodeKbCursorX = 0;
static EWRAM_DATA u8 sCodeKbCursorY = 0;
// 0xFF = no status to display, else SimTeamCodeResult value. EWRAM_DATA must
// initialize to zero in .sbss; OpenImportCode sets it to 0xFF before the
// first paint, so the initializer here is just a placeholder.
static EWRAM_DATA u8 sCodeKbStatus = 0;

// Window for the keyboard widget. Wider/taller than sDebugMenuWindowTemplateExtra
// so the 8x8 grid + buffer display all fit on one screen.
static const struct WindowTemplate sCodeKeyboardWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 28,
    .height = 17,
    .paletteNum = 15,
    .baseBlock = 1,
};

// Render the keyboard + buffer + status into the window. Called on every
// state change (cursor move, char add/del, status set).
static void CodeKeyboard_Draw(u8 windowId)
{
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));

    // --- Buffer display (rows 0-2) ---
    u8 line[27];  // 26 chars + EOS per line
    StringCopy(gStringVar1, COMPOUND_STRING("Code:"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar1, 0, 0, 0, NULL);

    u8 *p = sCodeKbBuffer;
    u32 remaining = sCodeKbLen;
    // Line 1: chars 0-25
    u32 take = remaining > 26 ? 26 : remaining;
    for (u32 i = 0; i < take; i++) line[i] = p[i];
    line[take] = EOS;
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, line, 32, 0, 0, NULL);
    // Line 2: chars 26-51 (overflow)
    if (remaining > 26)
    {
        take = remaining - 26;
        if (take > 26) take = 26;
        for (u32 i = 0; i < take; i++) line[i] = p[26 + i];
        line[take] = EOS;
        AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, line, 32, 14, 0, NULL);
    }

    // --- Status line (row 3) ---
    if (sCodeKbStatus != 0xFF)
    {
        AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT,
            Sim_TeamCodeResultLabel((enum SimTeamCodeResult)sCodeKbStatus),
            0, 30, 0, NULL);
    }
    else
    {
        AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT,
            COMPOUND_STRING("A:type B:del START:OK SEL:X"), 0, 30, 0, NULL);
    }

    // --- Keyboard grid (rows 4-11): 8x8, each cell shows " X " or "[X]" ---
    for (u32 row = 0; row < 8; row++)
    {
        u8 *out = line;
        for (u32 col = 0; col < 8; col++)
        {
            u8 ch = sCodeKeyboardChars[row * 8 + col];
            if (row == sCodeKbCursorY && col == sCodeKbCursorX)
            {
                // Pokemon's text charset has no square brackets, so use
                // parens to mark the cursor cell: "(X)" vs " X ".
                *out++ = CHAR_LEFT_PAREN;
                *out++ = ch;
                *out++ = CHAR_RIGHT_PAREN;
            }
            else
            {
                *out++ = CHAR_SPACE;
                *out++ = ch;
                *out++ = CHAR_SPACE;
            }
        }
        *out = EOS;
        AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, line, 8, 48 + row * 10, 0, NULL);
    }

    CopyWindowToVram(windowId, COPYWIN_GFX);
}

// Tear down the keyboard window + return to the per-mon editor's list menu.
// `commitWorkBuffer` controls whether to also persist the work buffer to
// the saveblock (TRUE only on a successful import).
static void CodeKeyboard_ReturnToMonEditor(u8 taskId, bool32 commitWorkBuffer)
{
    if (commitWorkBuffer)
        BuildTrainer_CommitWorkBufferToSaveblock();
    ClearStdWindowAndFrame(gTasks[taskId].tSubWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tSubWindowId);
    gTasks[taskId].tSubWindowId = 0;
    sCodeKbStatus = 0xFF;
    Debug_DestroyMenu(taskId);
    sDebugMenuListData->listId = DEBUG_LISTID_BUILD_TRAINER_MON;
    Debug_GenerateListBuildTrainerMonMenu();
    Debug_ShowMenu(DebugTask_HandleMenuInput_General, sDebugMenu_Actions_BuildTrainerMon);
}

// Per-frame input handler for the keyboard.
static void Task_CodeKeyboard(u8 taskId)
{
    bool32 redraw = FALSE;
    u8 windowId = gTasks[taskId].tSubWindowId;

    if (JOY_NEW(DPAD_LEFT))  { sCodeKbCursorX = (sCodeKbCursorX + 7) % 8; redraw = TRUE; PlaySE(SE_SELECT); }
    if (JOY_NEW(DPAD_RIGHT)) { sCodeKbCursorX = (sCodeKbCursorX + 1) % 8; redraw = TRUE; PlaySE(SE_SELECT); }
    if (JOY_NEW(DPAD_UP))    { sCodeKbCursorY = (sCodeKbCursorY + 7) % 8; redraw = TRUE; PlaySE(SE_SELECT); }
    if (JOY_NEW(DPAD_DOWN))  { sCodeKbCursorY = (sCodeKbCursorY + 1) % 8; redraw = TRUE; PlaySE(SE_SELECT); }
    // L/R jumps 2 rows for fast nav (charset is laid out so 2 rows ≈ a
    // visual "group" — uppercase / lowercase / digits+sym).
    if (JOY_NEW(L_BUTTON))   { sCodeKbCursorY = (sCodeKbCursorY + 6) % 8; redraw = TRUE; PlaySE(SE_SELECT); }
    if (JOY_NEW(R_BUTTON))   { sCodeKbCursorY = (sCodeKbCursorY + 2) % 8; redraw = TRUE; PlaySE(SE_SELECT); }

    if (JOY_NEW(A_BUTTON))
    {
        if (sCodeKbLen < CODE_KB_MAX_LEN)
        {
            sCodeKbBuffer[sCodeKbLen++] = sCodeKeyboardChars[sCodeKbCursorY * 8 + sCodeKbCursorX];
            sCodeKbBuffer[sCodeKbLen] = '\0';
            sCodeKbStatus = 0xFF;
            redraw = TRUE;
            PlaySE(SE_SELECT);
        }
    }
    if (JOY_NEW(B_BUTTON))
    {
        // Don't delete past the "PB" prefix; we re-insert it on every open.
        if (sCodeKbLen > 2)
        {
            sCodeKbBuffer[--sCodeKbLen] = '\0';
            sCodeKbStatus = 0xFF;
            redraw = TRUE;
            PlaySE(SE_SELECT);
        }
    }

    if (JOY_NEW(START_BUTTON))
    {
        struct SimCustomTrainerMon mon;
        enum SimTeamCodeResult res = Sim_DecodeTeamCode(sCodeKbBuffer, &mon);
        if (res == SIM_CODE_OK)
        {
            PlaySE(SE_SUCCESS);
            sBuildTrainerWorkMon = mon;
            CodeKeyboard_ReturnToMonEditor(taskId, TRUE);
            return;
        }
        else
        {
            PlaySE(SE_FAILURE);
            sCodeKbStatus = (u8)res;
            CodeKeyboard_Draw(windowId);
            return;
        }
    }
    if (JOY_NEW(SELECT_BUTTON))
    {
        PlaySE(SE_SELECT);
        CodeKeyboard_ReturnToMonEditor(taskId, FALSE);
        return;
    }

    if (redraw)
        CodeKeyboard_Draw(windowId);
}

// Open the keyboard sub-window. Pre-fills the buffer with the magic "PB"
// prefix so the user can't accidentally delete it. Cursor starts at A (0,0).
static void DebugAction_BuildTrainer_OpenImportCode(u8 taskId)
{
    Debug_DestroyMenu(taskId);
    Debug_RemoveCallbackMenu();
    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    u8 windowId = AddWindow(&sCodeKeyboardWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);

    sCodeKbBuffer[0] = 'P';
    sCodeKbBuffer[1] = 'B';
    sCodeKbBuffer[2] = '\0';
    sCodeKbLen = 2;
    sCodeKbCursorX = 0;
    sCodeKbCursorY = 0;
    sCodeKbStatus = 0xFF;

    u8 newTaskId = CreateTask(Task_CodeKeyboard, 3);
    gTasks[newTaskId].tSubWindowId = windowId;
    gTasks[newTaskId].tWindowId = windowId;

    CodeKeyboard_Draw(windowId);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}
#endif  // v1.2 keyboard widget disabled until v1.3 charmap fix

// =============================================================================
// v0.51 — Custom user-built trainer slots
// =============================================================================
// Storage lives in SaveBlock3 (struct SimCustomTrainer simCustomTrainers[3]).
// At battle time, GetTrainerStructFromId() in include/data.h dispatches to
// Sim_GetCustomTrainerStruct() when it sees a custom-range ID. We synthesize
// a struct Trainer + its TrainerMon party in static EWRAM buffers, populated
// from the saveblock on each call.
//
// Phase 1 (v0.51): back-end only. If the slot is empty (inUse=0), a fallback
// placeholder team is generated (single Magikarp Lv5) so battles don't crash
// against unconfigured slots. Phase 2+ will add the in-game edit UI.

static EWRAM_DATA struct TrainerMon sSimCustomTrainerMons[SIM_NUM_CUSTOM_TRAINERS][6] = {0};
static EWRAM_DATA u8 sSimCustomTrainerEVs[SIM_NUM_CUSTOM_TRAINERS][6][6] = {0};
static EWRAM_DATA struct Trainer sSimCustomTrainerStructs[SIM_NUM_CUSTOM_TRAINERS] = {0};

const struct Trainer *Sim_GetCustomTrainerStruct(u16 trainerId)
{
    // v1.1 — slots 1-3 are at TRAINER_SIM_CUSTOM_1..3 (1087-1089). Slots 4-6
    // jumped to TRAINER_SIM_CUSTOM_4..6 (1099-1101) so legacy save data for
    // slots 1-3 didn't shift when the LA trainers were inserted. Map both
    // windows back to a contiguous 0-5 saveblock index.
    u8 slot;
    if (trainerId >= TRAINER_SIM_CUSTOM_1 && trainerId <= TRAINER_SIM_CUSTOM_3)
        slot = trainerId - TRAINER_SIM_CUSTOM_1;
    else if (trainerId >= TRAINER_SIM_CUSTOM_4 && trainerId <= TRAINER_SIM_CUSTOM_6)
        slot = (trainerId - TRAINER_SIM_CUSTOM_4) + 3;
    else
        return NULL;
    if (slot >= SIM_NUM_CUSTOM_TRAINERS)
        return NULL;

    struct SimCustomTrainer *saved = &gSaveBlock3Ptr->simCustomTrainers[slot];
    // v0.52.12 — sanitize stale save data here too, so battles initiated
    // against an old save's custom slot never read garbage.
    BuildTrainer_SanitizeSlot(saved);
    struct Trainer *t = &sSimCustomTrainerStructs[slot];

    // Zero out the trainer struct; we'll fill in only the fields we need.
    memset(t, 0, sizeof(*t));

    // Trainer name: from saveblock, or default placeholder
    if (saved->inUse)
    {
        for (u8 i = 0; i < SIM_CUSTOM_TRAINER_NAME_LEN; i++)
            t->trainerName[i] = saved->name[i];
        t->trainerName[SIM_CUSTOM_TRAINER_NAME_LEN] = EOS;
    }
    else
    {
        // "CUSTOM 1/2/3/4/5/6" default name when slot is unconfigured.
        // v1.1: expanded to 6 entries to match SIM_NUM_CUSTOM_TRAINERS bump.
        static const u8 sDefaultNames[SIM_NUM_CUSTOM_TRAINERS][9] = {
            _("CUSTOM 1"), _("CUSTOM 2"), _("CUSTOM 3"),
            _("CUSTOM 4"), _("CUSTOM 5"), _("CUSTOM 6"),
        };
        StringCopy(t->trainerName, sDefaultNames[slot]);
    }

    // Build party. Empty slot → 1 placeholder Magikarp so battles don't crash.
    u8 monCount = (saved->inUse && saved->monCount > 0) ? saved->monCount : 1;
    if (monCount > 6) monCount = 6;

    for (u8 i = 0; i < monCount; i++)
    {
        struct TrainerMon *dst = &sSimCustomTrainerMons[slot][i];
        memset(dst, 0, sizeof(*dst));

        if (saved->inUse && i < saved->monCount && saved->mons[i].species != SPECIES_NONE)
        {
            struct SimCustomTrainerMon *src = &saved->mons[i];
            dst->species = src->species;
            dst->heldItem = src->heldItem;
            for (u8 j = 0; j < 4; j++) dst->moves[j] = src->moves[j];
            dst->lvl = (src->level == 0) ? 50 : src->level;
            dst->nature = src->nature;
            // v0.52.4 — pack per-stat IVs into the u32 expected by
            // MON_DATA_IVS. Engine order is HP/Atk/Def/Spe/SpA/SpD; we store
            // HP/Atk/Def/SpA/SpD/Spe to mirror the EV array, so swap slots 3-5
            // when packing.
            //
            // Back-compat: a freshly-defaulted mon (all-zero ivs[]) means "old
            // save / never edited" — give perfect 31s so legacy slots don't
            // suddenly battle with 0 IVs after the v0.52.4 upgrade.
            {
                bool32 allZero = TRUE;
                for (u8 j = 0; j < 6; j++) if (src->ivs[j] != 0) { allZero = FALSE; break; }
                if (allZero)
                {
                    dst->iv = (31u) | (31u << 5) | (31u << 10) | (31u << 15) | (31u << 20) | (31u << 25);
                }
                else
                {
                    u32 hp  = src->ivs[0] & 0x1F;
                    u32 atk = src->ivs[1] & 0x1F;
                    u32 def = src->ivs[2] & 0x1F;
                    u32 spa = src->ivs[3] & 0x1F;
                    u32 spd = src->ivs[4] & 0x1F;
                    u32 spe = src->ivs[5] & 0x1F;
                    dst->iv = hp | (atk << 5) | (def << 10) | (spe << 15) | (spa << 20) | (spd << 25);
                }
            }
            // Copy EVs into static buffer; struct TrainerMon takes a pointer.
            for (u8 j = 0; j < 6; j++) sSimCustomTrainerEVs[slot][i][j] = src->evs[j];
            dst->ev = sSimCustomTrainerEVs[slot][i];
            dst->ball = ITEM_POKE_BALL;
            dst->friendship = MAX_FRIENDSHIP;
            dst->gender = src->gender;
            // v0.52.4 — propagate the shiny flag. The engine uses this in
            // battle_main.c CreateNPCTrainerPartyFromTrainer to set an OT_ID_PRESET
            // so personality XOR otId hits the shiny rolls.
            dst->isShiny = (src->shiny != 0);
            // ability resolved from species's abilities table; ABILITY_NONE
            // means "use the default ability for this species/slot".
            dst->ability = ABILITY_NONE;
        }
        else
        {
            // Placeholder mon for empty/blank slot.
            dst->species = SPECIES_MAGIKARP;
            dst->lvl = 5;
            dst->iv = 0;
            dst->ability = ABILITY_NONE;
            dst->ball = ITEM_POKE_BALL;
            dst->friendship = MAX_FRIENDSHIP;
            dst->moves[0] = MOVE_SPLASH;
            // ev pointer stays NULL → default zero EVs
        }
    }

    t->party = sSimCustomTrainerMons[slot];
    t->partySize = monCount;
    t->trainerClass = TRAINER_CLASS_COOLTRAINER;
    // v0.52.5 — honor saved sprite class. 0 = legacy/unset, so fall back to
    // COOLTRAINER_M (the historical default). Clamp to the FRONT range so
    // we never accidentally use a back pic for the opponent slot.
    if (saved->inUse && saved->trainerPic != 0 && saved->trainerPic < TRAINER_PIC_FRONT_COUNT)
        t->trainerPic = saved->trainerPic;
    else
        t->trainerPic = TRAINER_PIC_FRONT_COOLTRAINER_M;
    t->encounterMusic = TRAINER_ENCOUNTER_MUSIC_COOL;
    t->gender = 0;  // male default
    t->aiFlags = AI_FLAG_CHECK_BAD_MOVE
               | AI_FLAG_TRY_TO_FAINT
               | AI_FLAG_CHECK_VIABILITY
               | AI_FLAG_SMART_SWITCHING;
    t->items[0] = ITEM_FULL_RESTORE;
    t->items[1] = ITEM_FULL_RESTORE;
    t->items[2] = ITEM_FULL_RESTORE;
    t->items[3] = ITEM_FULL_RESTORE;
    t->battleType = TRAINER_BATTLE_TYPE_SINGLES;

    return t;
}
// =============================================================================

// Public: populate the pick queue for the upcoming match-round build. Call
// once per Sim_SetupMatchRound BEFORE any CreateNPCTrainerPartyFromTrainer
// calls. Skipped for non-AI-vs-AI battles.
static void Sim_PrepareTeamPicks(s32 trainer1Id, s32 trainer2Id, s32 partnerId, s32 playerSideId, u8 pickCount)
{
    gSimPickQueueActive = FALSE;
    gSimPickQueueHead = 0;
    gSimPickQueueLen = 0;
    for (u8 r = 0; r < SIM_PICK_QUEUE_MAX; r++)
        gSimPickCounts[r] = 0;

    // Skip when there's no Player AI side to fight — we only do team preview
    // when the simulator is driving both sides.
    if (playerSideId == TRAINER_NONE)
        return;

    const struct Trainer *opp1 = Sim_GetTrainerStruct(trainer1Id);
    const struct Trainer *opp2 = Sim_GetTrainerStruct(trainer2Id);
    const struct Trainer *partner = (partnerId >= PARTNER_COUNT) ? Sim_GetTrainerStruct(partnerId) : NULL;
    const struct Trainer *player = Sim_GetTrainerStruct(playerSideId);

    // Trainer IDs are needed alongside the structs so the adaptive scoring
    // can look up each opposing trainer's prior brings from sSimPrevPickIds[].
    u16 partnerTrainerId = (partner != NULL) ? (u16)partnerId : 0;
    u16 playerTrainerId  = (player != NULL)  ? (u16)playerSideId : 0;
    u16 opp1TrainerId    = (opp1 != NULL)    ? (u16)trainer1Id : 0;
    u16 opp2TrainerId    = (opp2 != NULL)    ? (u16)trainer2Id : 0;

    u8 row = 0;
    // Order MUST match the order CreateNPCTrainerPartyFromTrainer gets called:
    //   1. Partner (Sim_SetupMatchRound's partner block, if curated)
    //   2. Player AI (Sim_SetupMatchRound's player AI block)
    //   3. Opp A (battle init)
    //   4. Opp B (battle init, only if BATTLE_TYPE_TWO_OPPONENTS)
    if (partner != NULL)
    {
        Sim_QueuePicksFor(partnerTrainerId, partner,
                          opp1TrainerId, opp1,
                          opp2TrainerId, opp2,
                          pickCount, row);
        row++;
    }
    if (player != NULL)
    {
        Sim_QueuePicksFor(playerTrainerId, player,
                          opp1TrainerId, opp1,
                          opp2TrainerId, opp2,
                          pickCount, row);
        row++;
    }
    if (opp1 != NULL)
    {
        Sim_QueuePicksFor(opp1TrainerId, opp1,
                          playerTrainerId, player,
                          partnerTrainerId, partner,
                          pickCount, row);
        row++;
    }
    if (opp2 != NULL)
    {
        Sim_QueuePicksFor(opp2TrainerId, opp2,
                          playerTrainerId, player,
                          partnerTrainerId, partner,
                          pickCount, row);
        row++;
    }

    gSimPickQueueLen = row;
    gSimPickQueueActive = (row > 0);
}

// Public: consumer hook used by CreateNPCTrainerPartyFromTrainer. Returns TRUE
// (and copies picks into outIndices) when a row is available for the next
// trainer in the call order. FALSE means "fall back to default selection".
bool32 Sim_ConsumeNextPickRow(u8 *outIndices, u8 maxIndices)
{
    if (!gSimPickQueueActive)
        return FALSE;
    if (gSimPickQueueHead >= gSimPickQueueLen)
    {
        gSimPickQueueActive = FALSE;
        return FALSE;
    }
    u8 row = gSimPickQueueHead++;
    u8 cnt = gSimPickCounts[row];
    if (cnt == 0)
        return FALSE;
    if (cnt > maxIndices)
        cnt = maxIndices;
    for (u8 i = 0; i < cnt; i++)
        outIndices[i] = gSimPickIndices[row][i];
    return TRUE;
}

// Battle Simulator: build battle params + parties for one round of an AI-vs-AI sim.
// Shared between the picker's "Try Battle" action and the Best-Of-N auto-rematch
// =============================================================================
// v1.1 — Player name override for AI-vs-AI sim battles.
// =============================================================================
// Battle dialogue reads gSaveBlock2Ptr->playerName for every "[Player] sent
// out X!" / "[Player] is about to use Y!" line. In AI-vs-AI sim mode the
// player slot is actually a curated trainer (Logan, Cynthia, Volo, etc.), so
// the user expects to see THAT trainer's name in the dialogue — not whatever
// name happens to be sitting in SaveBlock2.
//
// Worse: this ROM boots straight into the Battle Tower lobby without a
// naming-screen step, so a fresh save's playerName is whatever uninitialized
// EWRAM bytes were sitting in that slot at boot (renders as glitched / random
// characters in dialogue — the "Known bug" in the README).
//
// Fix: snapshot the existing playerName at sim battle start, overwrite with
// the player AI trainer's name, restore at battle end. Pilot mode keeps the
// user's name (they're actively piloting — their name is correct).
static EWRAM_DATA u8 sSimSavedPlayerName[PLAYER_NAME_LENGTH + 1] = {0};
static EWRAM_DATA bool8 sSimPlayerNameOverridden = FALSE;

void Sim_OverridePlayerName(u16 playerSideId, bool32 pilotMode)
{
    // Don't override twice (paranoid: a future code path could call this from
    // inside an active sim).
    if (sSimPlayerNameOverridden)
        return;
    // Always snapshot so restore is safe — even in pilot mode, we may need to
    // patch a corrupted default name in-place.
    for (u32 i = 0; i < PLAYER_NAME_LENGTH + 1; i++)
        sSimSavedPlayerName[i] = gSaveBlock2Ptr->playerName[i];
    sSimPlayerNameOverridden = TRUE;

    if (pilotMode)
    {
        // Pilot mode: user is actively playing — use their name. But if it's
        // empty / uninitialized (first byte 0x00 or 0xFF), fall back to a
        // sane default so dialogue isn't glitched.
        u8 first = gSaveBlock2Ptr->playerName[0];
        if (first == 0x00 || first == EOS)
        {
            static const u8 sDefaultPlayerName[] = _("PLAYER");
            StringCopyN(gSaveBlock2Ptr->playerName, sDefaultPlayerName, PLAYER_NAME_LENGTH);
            gSaveBlock2Ptr->playerName[PLAYER_NAME_LENGTH] = EOS;
        }
        return;
    }
    // AI-vs-AI: copy the player AI trainer's name into playerName. Trainer
    // names are up to TRAINER_NAME_LENGTH (10) but playerName is only
    // PLAYER_NAME_LENGTH (7), so the copy is bounded + EOS-terminated.
    if (playerSideId != TRAINER_NONE)
    {
        const struct Trainer *t = GetTrainerStructFromId(playerSideId);
        if (t != NULL)
        {
            StringCopyN(gSaveBlock2Ptr->playerName, t->trainerName, PLAYER_NAME_LENGTH);
            gSaveBlock2Ptr->playerName[PLAYER_NAME_LENGTH] = EOS;
        }
    }
}

void Sim_RestorePlayerName(void)
{
    if (!sSimPlayerNameOverridden)
        return;
    for (u32 i = 0; i < PLAYER_NAME_LENGTH + 1; i++)
        gSaveBlock2Ptr->playerName[i] = sSimSavedPlayerName[i];
    sSimPlayerNameOverridden = FALSE;
}

// path (Sim_TriggerNextMatchRound). Always force doubles since singles AI-vs-AI is
// engine-broken. Save state for the matchup must be reset by the caller as needed.
static void Sim_SetupMatchRound(s32 trainer1Id, s32 trainer2Id, s32 partnerId, s32 playerSideId)
{
    // Battle Simulator v0.50.4: top up the player's bag with all four
    // gimmick-key items every match so the engine never short-circuits a
    // gimmick activation on "player doesn't have the key." Belt-and-suspenders
    // for v0.48.1 / v0.50.3 (Z-Power Ring engine bypass) and v0.15 (Dynamax) —
    // even if a code path with a CheckBagHasItem gate slipped past our bypass
    // review (Ash's Pikachu Z-Move wasn't firing despite both bypasses
    // being in place), the item is just present.
    AddBagItem(ITEM_MEGA_RING, 1);
    AddBagItem(ITEM_Z_POWER_RING, 1);
    AddBagItem(ITEM_DYNAMAX_BAND, 1);
    AddBagItem(ITEM_TERA_ORB, 1);

    // Battle Simulator v0.47: singles toggle restored, backed by the new
    // AiSingles controller (BATTLE_CONTROLLER_AI_SINGLES). The controller
    // registers as Player for BattlerIsPlayer() so BufferStringBattle's
    // PRINTSTRING routing takes the player-substitution arms instead of the
    // partner arms — those arms call GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)
    // which returns gBattlersCount in singles and crashes downstream.
    //
    // v0.52.13 — in best-of-N rounds 2+ sDebugMenuListData has been freed
    // (Debug_DestroyMenu_Full at round 1 setup-end), so prefer the snapshot
    // cache whenever we're on a rematch. The "rematch" condition is
    // sSimMatchActive AND at least one win on the board — round 1 has
    // gSimT1Wins == gSimT2Wins == 0 and reads live picker state. Reading
    // off the freed pointer (still non-NULL until v0.52.13.1) was randomly
    // flipping singles → doubles + enabling pilot mode every rematch.
    bool32 isRematch = sSimMatchActive && (gSimT1Wins > 0 || gSimT2Wins > 0);
    bool32 forceDouble;
    bool32 pilotMode;
    if (isRematch)
    {
        forceDouble = sSimMatchForceDouble;
        pilotMode   = sSimMatchPilotMode;
    }
    else
    {
        forceDouble = (sDebugMenuListData != NULL && sDebugMenuListData->data[5] != 0);
        pilotMode   = (sDebugMenuListData != NULL && sDebugMenuListData->data[7] != 0);
    }
    if (gSimVGCMode)
        forceDouble = TRUE;
    // v0.52.15 — publish pilot mode globally so the level-cap path in
    // battle_main.c can detect it (the cap's existing AI-vs-AI flag gate
    // is bypassed by pilot mode by design).
    gSimPilotMode = pilotMode;
    // v1.1 — Swap gSaveBlock2Ptr->playerName for the player AI trainer's name
    // so battle dialogue ("[Player] sent out X!") shows "LOGAN" / "CYNTHIA"
    // instead of the user's overworld name (or garbage on fresh saves).
    // Pilot mode keeps the user's name but patches obviously-empty defaults.
    // Restored in CB2_EndDebugBattle.
    Sim_OverridePlayerName((u16)playerSideId, pilotMode);
    // v0.53.3 — Grant all 8 Hoenn badges in pilot mode so loaner mons obey at
    // any level. The vanilla obedience table caps controllable level at 30 /
    // 50 / 70 / 90 / unlimited based on badge count — pilot mode hands the
    // player a Champion-tier trainer's full team (often Lv 50+ aces and Lv
    // 70+ legendaries), so without all 8 badges the player loses turns to
    // disobedience every battle. Cleared again after the battle so the
    // overworld save-state isn't permanently altered.
    if (pilotMode)
    {
        for (u32 i = 0; i < NUM_BADGES; i++)
            FlagSet(FLAG_BADGE01_GET + i);
    }

    // Battle Simulator: precompute team-preview picks BEFORE any party-build
    // calls. Multi battles bring 3 of 6; VGC mode brings 4 of 6 (the picker
    // already forces doubles + no two-opponents). Single trainer vs single
    // trainer in non-VGC mode brings the full party (no pick needed).
    {
        u8 pickCount = 3;  // multi default
        if (gSimVGCMode)
            pickCount = 4;
        Sim_PrepareTeamPicks(trainer1Id, trainer2Id, partnerId, playerSideId, pickCount);
    }

    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    TRAINER_BATTLE_PARAM.opponentA = trainer1Id;
    TRAINER_BATTLE_PARAM.opponentB = 0xFFFF;
    // Doubles mandatory when partner / two-opponents in play; otherwise honor
    // the picker toggle.
    if (partnerId != PARTNER_NONE || trainer2Id != TRAINER_NONE || forceDouble)
        gBattleTypeFlags |= BATTLE_TYPE_DOUBLE;
    if (trainer2Id != TRAINER_NONE)
    {
        TRAINER_BATTLE_PARAM.opponentB = trainer2Id;
        gBattleTypeFlags |= BATTLE_TYPE_TWO_OPPONENTS;
    }
    // v0.51.1: Pilot Mode (data[7]) skips the AI-vs-AI flag so the player slot
    // uses SetControllerToPlayer (human input via standard battle menu) instead
    // of SetControllerToAiSingles. The playerSideId trainer's team still loads
    // into gPlayerParty below, so you're piloting THAT trainer's mons.
    // v0.52.13: pilotMode is now decided above (cache vs live picker data).
    if (B_FLAG_AI_VS_AI_BATTLE && !pilotMode && (playerSideId != TRAINER_NONE || partnerId >= PARTNER_COUNT))
        FlagSet(B_FLAG_AI_VS_AI_BATTLE);
    if (partnerId != PARTNER_NONE)
    {
        SavePlayerParty();
        gBattleTypeFlags |= BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER;
        for (u32 i = 0; i < MAX_FRONTIER_PARTY_SIZE; i++)
        {
            gSelectedOrderFromParty[i] = i + 1;
            gSaveBlock2Ptr->frontier.selectedPartyMons[i] = gSelectedOrderFromParty[i];
        }
        if (partnerId >= PARTNER_COUNT)
        {
            gPartnerTrainerId = partnerId;
            CreateNPCTrainerPartyFromTrainer(&gPlayerParty[PARTY_SIZE / 2], GetTrainerStructFromId(partnerId), FALSE, gBattleTypeFlags);
        }
        else
        {
            gPartnerTrainerId = TRAINER_PARTNER(partnerId);
            FillPartnerParty(gPartnerTrainerId);
        }
    }
    if (playerSideId != TRAINER_NONE)
    {
        if (partnerId == PARTNER_NONE)
            SavePlayerParty();
        // v0.47: doubles no longer forced — AiSingles controller handles singles.
        // v0.51.1: skip AI-vs-AI flag in pilot mode so player has human controls.
        if (B_FLAG_AI_VS_AI_BATTLE && !pilotMode)
            FlagSet(B_FLAG_AI_VS_AI_BATTLE);
        CreateNPCTrainerPartyFromTrainer(gPlayerParty, GetTrainerStructFromId(playerSideId), TRUE, gBattleTypeFlags);
        gPartnerTrainerId = playerSideId;
        for (u32 i = 0; i < MAX_FRONTIER_PARTY_SIZE; i++)
        {
            gSelectedOrderFromParty[i] = i + 1;
            gSaveBlock2Ptr->frontier.selectedPartyMons[i] = gSelectedOrderFromParty[i];
        }
    }
    // Battle Simulator v0.38: LiYun rotation is the permanent BG. After 17+
    // versions of debugging the custom theater BG -- gbagfx PNG conversion,
    // smol vs LZ77 compression, POW2 tile sheets, raw .4bpp matching LiYun's
    // format exactly, every sim-mode register fix -- horizontal stripe
    // artifacts persisted on every custom asset attempt. LiYun's pack is
    // community-validated and renders cleanly. BATTLE_THEATER env entry is
    // still wired up (data, enum, infotable) so if the missing piece is ever
    // found, swap this one line back to BATTLE_ENVIRONMENT_BATTLE_THEATER.
    {
        static u8 sLiYunRotation = 0;
        static const u8 sLiYunBgs[] = {
            BATTLE_ENVIRONMENT_LIYUN_INDOOR,
            BATTLE_ENVIRONMENT_LIYUN_OUTDOOR,
            BATTLE_ENVIRONMENT_LIYUN_CRAGGY,
        };
        gBattleEnvironment = sLiYunBgs[sLiYunRotation % ARRAY_COUNT(sLiYunBgs)];
        sLiYunRotation++;
    }
    CalculateEnemyPartyCount();
    // Battle Simulator: pre-battle BGM sting. Fires just after both sides'
    // parties are committed and right before the battle scene takes over,
    // so the opening notes of the gym leader battle theme play during the
    // transition into the encounter and pump up the match start.
    m4aSongNumStart(MUS_VS_GYM_LEADER);
}

// Battle Simulator: best-of-N match status helpers. A best-of-N match is "decided"
// once either side has the majority (e.g. 2 wins for best-of-3, 3 wins for best-of-5).
static bool32 Sim_IsBestOfActive(void)
{
    return sSimMatchActive && gSimBestOf > 1;
}

static bool32 Sim_IsMatchDecided(void)
{
    if (!Sim_IsBestOfActive())
        return TRUE;
    u8 needed = (gSimBestOf + 1) / 2;
    return (gSimT1Wins >= needed || gSimT2Wins >= needed);
}

// Battle Simulator: launch the next round of a best-of-N rematch using the stashed
// matchup IDs. Called from Debug_ShowTrainersSubMenu when an undecided match is
// pending — bypasses the picker entirely so the user just sees fades into Round 2/3.
void Sim_TriggerNextMatchRound(void)
{
    Sim_SetupMatchRound(sSimMatchOpponent1, sSimMatchOpponent2, sSimMatchPartner, sSimMatchPlayerAI);
    BattleSetup_StartTrainerBattle_Debug();
}

static void DebugAction_Trainers_TryBattle(u8 taskId)
{
    s32 trainer1Id = sDebugMenuListData->data[0];
    s32 trainer2Id = sDebugMenuListData->data[2];
    s32 partnerId = sDebugMenuListData->data[4];
    s32 playerSideId = sDebugMenuListData->data[6];
    s32 rematchId = sDebugMenuListData->data[1];
    if (sDebugMenuListData->data[1] != -1)
    {
        s32 lastMatch = CountBattledRematchTeams(rematchId);
        if (lastMatch == REMATCHES_COUNT || gRematchTable[rematchId].trainerIds[lastMatch] == 0)
            lastMatch -= 1;
        trainer1Id = gRematchTable[rematchId].trainerIds[lastMatch];
    }
    // Battle Simulator: if Tournament Mode is armed, kick off a fresh cup run.
    // Sim_StartTournament builds the 8-trainer single-elim bracket, places the
    // player in a random slot, and pre-simulates the 4 side QF outcomes.
    // The player's actual QF opponent is whoever's in their bracket pair.
    // Mutually exclusive with Best-Of (handled in CB2_EndDebugBattle's dispatcher).
    if (gSimTournamentCup > 0 && playerSideId != TRAINER_NONE)
    {
        Sim_StartTournament(playerSideId);
        if (Sim_IsTournamentActive())
        {
            trainer1Id = gSimTournamentBracket[Sim_GetPlayerOpponentSlot()];
            // Force singles-style 1v1 for cup matches — clear partner & opp2
            // so each fight is a clean Player AI vs cup challenger duel.
            trainer2Id = TRAINER_NONE;
            partnerId = PARTNER_NONE;
        }
    }
    // Battle Simulator: starting a new battle from the picker — reset any in-flight
    // best-of-N state and stash the matchup so CB2_EndDebugBattle can re-trigger it.
    // Also clear adaptive-pick history so Round 1 always scores blind (full
    // teamsheet, no counter-reads from a previous session).
    gSimT1Wins = 0;
    gSimT2Wins = 0;
    sSimMatchActive = (gSimBestOf > 1) && !Sim_IsTournamentActive();
    Sim_ResetPickHistory();
    sSimMatchOpponent1 = trainer1Id;
    sSimMatchOpponent2 = trainer2Id;
    sSimMatchPartner = partnerId;
    sSimMatchPlayerAI = playerSideId;
    // v0.52.13 — snapshot doubles + pilot flags from the menu so rounds 2+
    // of a best-of-N match keep the same config after sDebugMenuListData is
    // freed. Reading directly off the freed pointer was randomly flipping
    // singles → doubles + enabling pilot mode mid-series.
    sSimMatchForceDouble = (sDebugMenuListData != NULL && sDebugMenuListData->data[5] != 0);
    sSimMatchPilotMode   = (sDebugMenuListData != NULL && sDebugMenuListData->data[7] != 0);

    Sim_SetupMatchRound(trainer1Id, trainer2Id, partnerId, playerSideId);
    BattleSetup_StartTrainerBattle_Debug();
    Debug_DestroyMenu_Full(taskId);
}

static void DebugAction_Trainers_RechargeVsSeeker(u8 taskId)
{
    SetTrainerRematchStepCounter(VSSEEKER_RECHARGE_STEPS);
    MapResetTrainerRematches(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum);
    ScriptContext_SetupScript(EventScript_VsSeekerChargingDone);
    Debug_DestroyMenu_Full(taskId);
}

// v1.7 — Battle Frontier Challenge Mode (Option A: minimal).
// Borrows the given trainer's full team and warps the user to the Battle
// Frontier outdoor hub so they can challenge any of the 7 facilities
// (Tower / Dome / Palace / Arena / Factory / Pike / Pyramid) with that team.
// Idea credit: the Reddit player who found the v1.4 forfeit-to-overworld
// exploit and pitched "make it an official feature" as a better fix than
// patching the exit hole.
//
// Called from the picker's A-button confirm when the picker was opened in
// TRAINERS_DEBUG_SELECTION_FRONTIER mode (the new wrapper menu entry sets
// that mode directly without going through the sim battle setup screen).
//
// The chosen trainer's team gets loaded into gPlayerParty via the same
// CreateNPCTrainerPartyFromTrainer call Sim_SetupMatchRound uses. From there,
// the engine's existing SavePlayerParty / LoadPlayerParty specials (already
// wired into every Frontier facility's lobby OnTransition script) handle the
// backup/restore on entry/exit, so no Frontier-internal edits are needed.
//
// Exit caveat (Option A): walking back out of the Frontier reception gate
// leaves the player in the standard overworld. Easiest path back to the
// wrapper menu is a soft reset. Hooking the gate exit to auto-relaunch the
// wrapper menu is queued for v1.7.1 polish.
static void Sim_StartFrontierChallenge(s32 trainerId)
{
    if (trainerId == TRAINER_NONE)
    {
        PlaySE(SE_FAILURE);
        return;
    }
    // v1.7 — manual party loader. Tried CreateNPCTrainerPartyFromTrainer
    // (the engine's standard "load this trainer's party" helper) but it
    // was silently failing to populate gPlayerParty[] for our path —
    // both the START-menu party display AND the saveblock shadow we wrote
    // afterwards came out empty. Probably hits an internal gate we
    // couldn't pin down (the function has special-case branches for
    // FRONTIER / TRAINER_HILL / sim-battle modes that don't fire the
    // population loop). Falling back to manual: iterate TrainerMon[]
    // entries and call CreateMon + SetMonData directly. No gates, no
    // surprises.
    const struct Trainer *trainer = GetTrainerStructFromId(trainerId);
    if (trainer == NULL || trainer->party == NULL || trainer->partySize == 0)
    {
        PlaySE(SE_FAILURE);
        return;
    }
    ZeroPlayerPartyMons();
    u32 partySize = (trainer->partySize > PARTY_SIZE) ? PARTY_SIZE : trainer->partySize;
    for (u32 i = 0; i < partySize; i++)
    {
        const struct TrainerMon *src = &trainer->party[i];
        u32 personality = 0x88 | (i << 8);  // arbitrary distinct per slot
        ModifyPersonalityForNature(&personality, src->nature);
        u32 monLevel = (src->lvl == 0) ? 50 : src->lvl;
        CreateMon(&gPlayerParty[i], src->species, monLevel, personality, OTID_STRUCT_RANDOM_NO_SHINY);
        SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &src->heldItem);
        for (u32 j = 0; j < MAX_MON_MOVES; j++)
        {
            u32 move = src->moves[j];
            SetMonData(&gPlayerParty[i], MON_DATA_MOVE1 + j, &move);
            u32 pp = gMovesInfo[move].pp;
            SetMonData(&gPlayerParty[i], MON_DATA_PP1 + j, &pp);
        }
        if (src->iv != 0)
            SetMonData(&gPlayerParty[i], MON_DATA_IVS, &src->iv);
        if (src->ev != NULL)
        {
            SetMonData(&gPlayerParty[i], MON_DATA_HP_EV,    &src->ev[0]);
            SetMonData(&gPlayerParty[i], MON_DATA_ATK_EV,   &src->ev[1]);
            SetMonData(&gPlayerParty[i], MON_DATA_DEF_EV,   &src->ev[2]);
            SetMonData(&gPlayerParty[i], MON_DATA_SPATK_EV, &src->ev[3]);
            SetMonData(&gPlayerParty[i], MON_DATA_SPDEF_EV, &src->ev[4]);
            SetMonData(&gPlayerParty[i], MON_DATA_SPEED_EV, &src->ev[5]);
        }
        // Recompute stats so the IV/EV/level changes take effect (HP becomes
        // the proper value instead of the base CreateMon estimate).
        CalculateMonStats(&gPlayerParty[i]);
    }
    gPlayerPartyCount = partySize;
    // v1.7 — gPlayerPartyCount is set explicitly above in the manual load
    // loop. Calling CalculatePlayerPartyCount() defensively in case any
    // helper-path produces a non-SPECIES_NONE entry that the manual count
    // missed.
    CalculatePlayerPartyCount();
    // v1.7 — also persist the loaded team to gSaveBlock1Ptr->playerParty
    // (in-memory shadow). Every Frontier facility lobby's OnTransition
    // script calls SavePlayerParty on entry; if the shadow is empty when
    // those scripts later LoadPlayerParty, gPlayerParty gets wiped.
    // Writing the shadow here keeps both views in sync.
    //
    // NOTE: Skipping TrySavingData (flash flush). Tested calling it but
    // it's multi-frame async and the warp/save flows tripped over each
    // other, leaving gPlayerPartyCount=0 in the START menu (receptionist
    // still saw the team because it iterates gPlayerParty directly).
    // In-memory shadow alone is enough — flash will sync next save point.
    SavePlayerParty();
    // v1.7 — flag a post-warp Calculate fixup. Something in the
    // CB2_LoadMap → field-load sequence resets gPlayerPartyCount to 0
    // after our load, which hides POKéMON from the START menu. The
    // field_control_avatar dispatch checks gSimFrontierChallengePending
    // each frame; on first frame after the warp lands it re-runs
    // CalculatePlayerPartyCount and clears the flag.
    gSimFrontierChallengePending = TRUE;
    // v1.19 — sticky flag so the START menu "Sim Menu" entry stays
    // available the whole Frontier Challenge session, not just the
    // first frame after warp.
    gSimFrontierChallengeActive = TRUE;
    // Grant the Frontier Pass so the receptionists let us into facilities.
    FlagSet(FLAG_SYS_FRONTIER_PASS);
    // v1.7 — the smoking-gun fix for the "START menu doesn't show POKéMON
    // even though party is populated" bug. start_menu.c:340 gates the
    // POKéMON entry on FLAG_SYS_POKEMON_GET. That flag is set in vanilla
    // when Birch hands you your starter — never in sim mode since the
    // wrapper menu skips the entire intro. Receptionist NPCs iterate
    // gPlayerParty[] directly so they don't need the flag, but the START
    // menu does. Setting it here unblocks POKéMON in the menu and brings
    // up the loaner team correctly.
    FlagSet(FLAG_SYS_POKEMON_GET);
    // v1.7 polish — give the player running shoes by default. The Frontier
    // maps are sprawling and walking pace everywhere is annoying. Setting
    // this flag enables hold-B-to-run via field_player_avatar.c's existing
    // FlagGet(FLAG_SYS_B_DASH) check.
    FlagSet(FLAG_SYS_B_DASH);
    // v1.8 — borrow the trainer's name as the player's display name for the
    // entire Frontier run. Roleplay request: "if I'm using Iris's team, I
    // want to BE Iris in the Frontier — show 'Iris sent out Hydreigon' in
    // dialogue, receptionists call me Iris, trainer card says Iris, etc."
    //
    // Trainer name is up to TRAINER_NAME_LENGTH (10) chars; playerName is
    // only PLAYER_NAME_LENGTH (7). StringCopyN bounds the copy and we EOS-
    // terminate manually so anything that walks past the cap (battle
    // dialogue formatters, trainer card render, etc.) hits a clean stop.
    //
    // Unlike Sim_OverridePlayerName (used for AI-vs-AI sim battles), this
    // is intentionally a one-way write — no snapshot, no restore. Frontier
    // Challenge is a "leave and come back" mode (player soft-resets or
    // returns to wrapper menu via boot), so we don't need to round-trip
    // back to the user's original name mid-session. Soft reset reloads
    // playerName from SaveBlock2 anyway.
    StringCopyN(gSaveBlock2Ptr->playerName, trainer->trainerName, PLAYER_NAME_LENGTH);
    gSaveBlock2Ptr->playerName[PLAYER_NAME_LENGTH] = EOS;
    // Stash a copy in EWRAM for the field-tick re-apply (see
    // field_control_avatar.c). The map-load callback chain has a
    // window between SetWarpDestination/WarpIntoMap and the player's
    // first field tick where playerName can get overwritten — this
    // buffer survives the whole sequence and the tick fixup restores
    // playerName from it just before the user regains control.
    StringCopyN(gSimFrontierBorrowedName, trainer->trainerName, PLAYER_NAME_LENGTH);
    gSimFrontierBorrowedName[PLAYER_NAME_LENGTH] = EOS;
    // v1.7 — grant all 8 Hoenn badges so the loaner mons obey at any level.
    // Without this, Lv 50+ legendaries on a borrowed Champion team would
    // disobey commands every other turn. Mirrors the same trick pilot
    // mode uses (Sim_SetupMatchRound at line ~7150).
    for (u32 i = 0; i < NUM_BADGES; i++)
        FlagSet(FLAG_BADGE01_GET + i);
    // v1.7 — drop a Mach Bike + gimmick keys in the bag. Bike is optional
    // (running shoes already cover most movement) but the user can press
    // SELECT to swap to bike for the longer hub runs. Mega/Z/Dynamax/Tera
    // keys let any facility that accepts them fire the gimmick (the bag
    // checks here mirror the same set Sim_SetupMatchRound adds for sim
    // battles at lines ~7098-7101).
    AddBagItem(ITEM_MACH_BIKE, 1);
    AddBagItem(ITEM_MEGA_RING, 1);
    AddBagItem(ITEM_Z_POWER_RING, 1);
    AddBagItem(ITEM_DYNAMAX_BAND, 1);
    AddBagItem(ITEM_TERA_ORB, 1);
    // v1.7 — spawn directly inside the Battle Tower lobby instead of the
    // outdoor Frontier hub. The outdoor map triggered Scott's first-visit
    // cutscene + dropped the player into a non-walkable tile in some testing
    // sessions (player got stuck walking into a wall). Battle Tower lobby is
    // the same map sim battles already exit to — known-good spawn coords,
    // no scripted intro to interfere. Player can talk to the Tower
    // receptionist to start a Tower challenge immediately, or walk out (south
    // door) to navigate the Frontier hub and pick a different facility.
    SetWarpDestination(MAP_GROUP(MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY),
                       MAP_NUM(MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY),
                       WARP_ID_NONE, 5, 8);
    WarpIntoMap();
    PlaySE(SE_WARP_IN);
    SetMainCallback2(CB2_LoadMap);
}

// *******************************
// Actions Flags and Vars
static void Debug_Display_FlagInfo(u32 flag, u32 digit, u8 windowId)
{
    ConvertIntToDecimalStringN(gStringVar1, flag, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_FLAGS);
    ConvertIntToHexStringN(gStringVar2, flag, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringExpandPlaceholders(gStringVar1, COMPOUND_STRING("{STR_VAR_1}{CLEAR_TO 90}\n0x{STR_VAR_2}{CLEAR_TO 90}"));
    if (FlagGet(flag))
        StringCopyPadded(gStringVar2, sDebugText_True, CHAR_SPACE, 15);
    else
        StringCopyPadded(gStringVar2, sDebugText_False, CHAR_SPACE, 15);
    StringCopy(gStringVar3, gText_DigitIndicator[digit]);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Flag: {STR_VAR_1}{CLEAR_TO 90}\n{STR_VAR_2}{CLEAR_TO 90}\n{STR_VAR_3}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_FlagsVars_Flags(u8 taskId)
{
    u8 windowId;

    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    // Display initial flag
    Debug_Display_FlagInfo(FLAG_TEMP_1, 0, windowId);

    gTasks[taskId].func = DebugAction_FlagsVars_FlagsSelect;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tInput = FLAG_TEMP_1;
    gTasks[taskId].tDigit = 0;
}

static void DebugAction_FlagsVars_FlagsSelect(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        FlagToggle(gTasks[taskId].tInput);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
        return;
    }

    Debug_HandleInput_Numeric(taskId, 1, FLAGS_COUNT - 1, DEBUG_NUMBER_DIGITS_FLAGS);

    if (JOY_NEW(DPAD_ANY) || JOY_NEW(A_BUTTON))
    {
        Debug_Display_FlagInfo(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
    }
}

#define tVarValue  data[5]

static void DebugAction_FlagsVars_Vars(u8 taskId)
{
    u8 windowId;

    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    // Display initial var
    ConvertIntToDecimalStringN(gStringVar1, VARS_START, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
    ConvertIntToHexStringN(gStringVar2, VARS_START, STR_CONV_MODE_LEFT_ALIGN, 4);
    StringExpandPlaceholders(gStringVar1, sDebugText_FlagsVars_VariableHex);
    ConvertIntToDecimalStringN(gStringVar3, 0, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
    StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
    StringCopy(gStringVar2, gText_DigitIndicator[0]);
    StringExpandPlaceholders(gStringVar4, sDebugText_FlagsVars_Variable);
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);

    gTasks[taskId].func = DebugAction_FlagsVars_Select;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tInput = VARS_START;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tVarValue = 0;
}

static void DebugAction_FlagsVars_Select(u8 taskId)
{
    Debug_HandleInput_Numeric(taskId, VARS_START, VARS_END, DEBUG_NUMBER_DIGITS_VARIABLES);

    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
        ConvertIntToHexStringN(gStringVar2, gTasks[taskId].tInput, STR_CONV_MODE_LEFT_ALIGN, 4);
        StringExpandPlaceholders(gStringVar1, sDebugText_FlagsVars_VariableHex);
        if (VarGetIfExist(gTasks[taskId].tInput) == 0xFFFF)
            gTasks[taskId].tVarValue = 0;
        else
            gTasks[taskId].tVarValue = VarGet(gTasks[taskId].tInput);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].tVarValue, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].tDigit]);

        //Combine str's to full window string
        StringExpandPlaceholders(gStringVar4, sDebugText_FlagsVars_Variable);
        AddTextPrinterParameterized(gTasks[taskId].tSubWindowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].tDigit = 0;

        PlaySE(SE_SELECT);

        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
        ConvertIntToHexStringN(gStringVar2, gTasks[taskId].tInput, STR_CONV_MODE_LEFT_ALIGN, 4);
        StringExpandPlaceholders(gStringVar1, sDebugText_FlagsVars_VariableHex);
        if (VarGetIfExist(gTasks[taskId].tInput) == 0xFFFF)
            gTasks[taskId].tVarValue = 0;
        else
            gTasks[taskId].tVarValue = VarGet(gTasks[taskId].tInput);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].tVarValue, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
        StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].tDigit]);
        StringExpandPlaceholders(gStringVar4, sDebugText_FlagsVars_VariableValueSet);
        AddTextPrinterParameterized(gTasks[taskId].tSubWindowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);

        gTasks[taskId].data[6] = gTasks[taskId].data[5]; //New value selector
        gTasks[taskId].func = DebugAction_FlagsVars_SetValue;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
        return;
    }
}

static void DebugAction_FlagsVars_SetValue(u8 taskId)
{
    if (JOY_NEW(DPAD_UP))
    {
        if (gTasks[taskId].data[6] + sPowersOfTen[gTasks[taskId].tDigit] <= 32000)
            gTasks[taskId].data[6] += sPowersOfTen[gTasks[taskId].tDigit];
        else
            gTasks[taskId].data[6] = 32000 - 1;

        if (gTasks[taskId].data[6] >= 32000)
            gTasks[taskId].data[6] = 32000 - 1;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        gTasks[taskId].data[6] -= sPowersOfTen[gTasks[taskId].tDigit];
        if (gTasks[taskId].data[6] < 0)
            gTasks[taskId].data[6] = 0;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        gTasks[taskId].tDigit -= 1;
        if (gTasks[taskId].tDigit < 0)
            gTasks[taskId].tDigit = 0;
    }
    if (JOY_NEW(DPAD_RIGHT))
    {
        gTasks[taskId].tDigit += 1;
        if (gTasks[taskId].tDigit > 4)
            gTasks[taskId].tDigit = 4;
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        VarSet(gTasks[taskId].tInput, gTasks[taskId].data[6]);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
        return;
    }

    if (JOY_NEW(DPAD_ANY) || JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);

        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
        ConvertIntToHexStringN(gStringVar2, gTasks[taskId].tInput, STR_CONV_MODE_LEFT_ALIGN, 4);
        StringExpandPlaceholders(gStringVar1, sDebugText_FlagsVars_VariableHex);
        StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].data[6], STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_VARIABLES);
        StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].tDigit]);
        StringExpandPlaceholders(gStringVar4, sDebugText_FlagsVars_VariableValueSet);
        AddTextPrinterParameterized(gTasks[taskId].tSubWindowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
    }
}

#undef tVarValue

static void DebugAction_FlagsVars_PokedexFlags_All(u8 taskId)
{
    u16 i;
    for (i = 0; i < NATIONAL_DEX_COUNT; i++)
    {
        GetSetPokedexFlag(i + 1, FLAG_SET_CAUGHT);
        GetSetPokedexFlag(i + 1, FLAG_SET_SEEN);
    }
    Debug_DestroyMenu_Full(taskId);
    ScriptContext_Enable();
}

static void DebugAction_FlagsVars_PokedexFlags_Reset(u8 taskId)
{
    int boxId, boxPosition, partyId;
    u16 species;

    // Reset Pokedex to emtpy
    memset(&gSaveBlock1Ptr->dexCaught, 0, sizeof(gSaveBlock1Ptr->dexCaught));
    memset(&gSaveBlock1Ptr->dexSeen, 0, sizeof(gSaveBlock1Ptr->dexSeen));

    // Add party Pokemon to Pokedex
    for (partyId = 0; partyId < PARTY_SIZE; partyId++)
    {
        if (GetMonData(&gPlayerParty[partyId], MON_DATA_SANITY_HAS_SPECIES))
        {
            species = GetMonData(&gPlayerParty[partyId], MON_DATA_SPECIES);
            GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_SET_CAUGHT);
            GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_SET_SEEN);
        }
    }

    // Add box Pokemon to Pokedex
    for (boxId = 0; boxId < TOTAL_BOXES_COUNT; boxId++)
    {
        for (boxPosition = 0; boxPosition < IN_BOX_COUNT; boxPosition++)
        {
            if (GetBoxMonData(&gPokemonStoragePtr->boxes[boxId][boxPosition], MON_DATA_SANITY_HAS_SPECIES))
            {
                species = GetBoxMonData(&gPokemonStoragePtr->boxes[boxId][boxPosition], MON_DATA_SPECIES);
                GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_SET_CAUGHT);
                GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_SET_SEEN);
            }
        }
    }
    Debug_DestroyMenu_Full(taskId);
    ScriptContext_Enable();
}

static void DebugAction_FlagsVars_SwitchDex(u8 taskId)
{
    if (FlagGet(FLAG_SYS_POKEDEX_GET))
        PlaySE(SE_PC_OFF);
    else
        PlaySE(SE_PC_LOGIN);
    FlagToggle(FLAG_SYS_POKEDEX_GET);
}

static void DebugAction_FlagsVars_SwitchNatDex(u8 taskId)
{
    if (IsNationalPokedexEnabled())
    {
        DisableNationalPokedex();
        PlaySE(SE_PC_OFF);
    }
    else
    {
        EnableNationalPokedex();
        PlaySE(SE_PC_LOGIN);
    }
}

static void DebugAction_FlagsVars_SwitchPokeNav(u8 taskId)
{
    if (FlagGet(FLAG_SYS_POKENAV_GET))
        PlaySE(SE_PC_OFF);
    else
        PlaySE(SE_PC_LOGIN);
    FlagToggle(FLAG_SYS_POKENAV_GET);
}

static void DebugAction_FlagsVars_SwitchMatchCall(u8 taskId)
{
    if (FlagGet(FLAG_ADDED_MATCH_CALL_TO_POKENAV))
    {
        PlaySE(SE_PC_OFF);
        FlagClear(FLAG_ADDED_MATCH_CALL_TO_POKENAV);
        FlagClear(FLAG_HAS_MATCH_CALL);
    }
    else
    {
        PlaySE(SE_PC_LOGIN);
        FlagSet(FLAG_ADDED_MATCH_CALL_TO_POKENAV);
        FlagSet(FLAG_HAS_MATCH_CALL);
    }
}

static void DebugAction_FlagsVars_RunningShoes(u8 taskId)
{
    if (FlagGet(FLAG_SYS_B_DASH))
        PlaySE(SE_PC_OFF);
    else
        PlaySE(SE_PC_LOGIN);
    FlagToggle(FLAG_SYS_B_DASH);
}

static void DebugAction_FlagsVars_ToggleFlyFlags(u8 taskId)
{
    u32 checkedFlag = sLocationFlags[0] == 0 ? sLocationFlags[ARRAY_COUNT(sLocationFlags) - 1] : sLocationFlags[0];
    if (FlagGet(checkedFlag))
    {
        PlaySE(SE_PC_OFF);
        for (u32 i = 0; i < ARRAY_COUNT(sLocationFlags); i++)
            FlagClear(sLocationFlags[i]);
    }
    else
    {
        PlaySE(SE_PC_LOGIN);
        for (u32 i = 0; i < ARRAY_COUNT(sLocationFlags); i++)
            FlagSet(sLocationFlags[i]);
    }
}

static void DebugAction_FlagsVars_ToggleBadgeFlags(u8 taskId)
{
    if (FlagGet(gBadgeFlags[ARRAY_COUNT(gBadgeFlags) - 1]))
    {
        PlaySE(SE_PC_OFF);
        for (u32 i = 0; i < ARRAY_COUNT(gBadgeFlags); i++)
            FlagClear(gBadgeFlags[i]);
    }
    else
    {
        PlaySE(SE_PC_LOGIN);
        for (u32 i = 0; i < ARRAY_COUNT(gBadgeFlags); i++)
            FlagSet(gBadgeFlags[i]);
    }
}

static void DebugAction_FlagsVars_ToggleGameClear(u8 taskId)
{
    // Sound effect
    if (FlagGet(FLAG_SYS_GAME_CLEAR))
        PlaySE(SE_PC_OFF);
    else
        PlaySE(SE_PC_LOGIN);
    FlagToggle(FLAG_SYS_GAME_CLEAR);
}

static void DebugAction_FlagsVars_ToggleFrontierPass(u8 taskId)
{
    // Sound effect
    if (FlagGet(FLAG_SYS_FRONTIER_PASS))
        PlaySE(SE_PC_OFF);
    else
        PlaySE(SE_PC_LOGIN);
    FlagToggle(FLAG_SYS_FRONTIER_PASS);
}

static void DebugAction_FlagsVars_CollisionOnOff(u8 taskId)
{
#if OW_FLAG_NO_COLLISION == 0
    Debug_DestroyMenu_Full_Script(taskId, Debug_FlagsNotSetOverworldConfigMessage);
#else
    if (FlagGet(OW_FLAG_NO_COLLISION))
        PlaySE(SE_PC_OFF);
    else
        PlaySE(SE_PC_LOGIN);
    FlagToggle(OW_FLAG_NO_COLLISION);
#endif
}

static void DebugAction_FlagsVars_EncounterOnOff(u8 taskId)
{
#if OW_FLAG_NO_ENCOUNTER == 0
    Debug_DestroyMenu_Full_Script(taskId, Debug_FlagsNotSetOverworldConfigMessage);
#else
    if (FlagGet(OW_FLAG_NO_ENCOUNTER))
        PlaySE(SE_PC_OFF);
    else
        PlaySE(SE_PC_LOGIN);
    FlagToggle(OW_FLAG_NO_ENCOUNTER);
#endif
}

static void DebugAction_FlagsVars_TrainerSeeOnOff(u8 taskId)
{
#if OW_FLAG_NO_TRAINER_SEE == 0
    Debug_DestroyMenu_Full_Script(taskId, Debug_FlagsNotSetOverworldConfigMessage);
#else
    if (FlagGet(OW_FLAG_NO_TRAINER_SEE))
        PlaySE(SE_PC_OFF);
    else
        PlaySE(SE_PC_LOGIN);
    FlagToggle(OW_FLAG_NO_TRAINER_SEE);
#endif
}

static void DebugAction_FlagsVars_BagUseOnOff(u8 taskId)
{
#if B_VAR_NO_BAG_USE < VARS_START || B_VAR_NO_BAG_USE > VARS_END
    Debug_DestroyMenu_Full_Script(taskId, Debug_VarsNotSetBattleConfigMessage);
#else
    PlaySE(SE_SELECT);
    VarSet(B_VAR_NO_BAG_USE, (VarGet(B_VAR_NO_BAG_USE) + 1) % 3);
#endif
}

static void DebugAction_FlagsVars_CatchingOnOff(u8 taskId)
{
#if B_FLAG_NO_CATCHING == 0
    Debug_DestroyMenu_Full_Script(taskId, Debug_FlagsNotSetBattleConfigMessage);
#else
    if (FlagGet(B_FLAG_NO_CATCHING))
        PlaySE(SE_PC_OFF);
    else
        PlaySE(SE_PC_LOGIN);
    FlagToggle(B_FLAG_NO_CATCHING);
#endif
}

// *******************************
// Actions Give
#define ITEM_TAG 0xFDF3
#define tItemId    data[5]
#define tSpriteId  data[6]

static void Debug_Display_ItemInfo(enum Item itemId, u32 digit, u8 windowId)
{
    StringCopy(gStringVar2, gText_DigitIndicator[digit]);
    u8* end = CopyItemName(itemId, gStringVar1);
    enum Move moveId = ItemIdToBattleMoveId(itemId);
    if (moveId != MOVE_NONE)
    {
        end = StringCopy(end, gText_Space);
        end = StringCopy(end, GetMoveName(moveId));
    }
    else if (CheckIfItemIsTMHMOrEvolutionStone(itemId) == 1)
    {
        end = StringCopy(end, COMPOUND_STRING(" None"));
    }

    WrapFontIdToFit(gStringVar1, end, DEBUG_MENU_FONT, WindowWidthPx(windowId));
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    ConvertIntToDecimalStringN(gStringVar3, itemId, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_ITEMS);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Item ID: {STR_VAR_3}\n{STR_VAR_1}{CLEAR_TO 90}\n\n{STR_VAR_2}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_Give_Item(u8 taskId)
{
    u8 windowId;

    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    // Display initial item
    Debug_Display_ItemInfo(1, 0, windowId);

    gTasks[taskId].func = DebugAction_Give_Item_SelectId;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tInput = 1;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tSpriteId = AddItemIconSprite(ITEM_TAG, ITEM_TAG, gTasks[taskId].tInput);
    gSprites[gTasks[taskId].tSpriteId].x2 = DEBUG_NUMBER_ICON_X+10;
    gSprites[gTasks[taskId].tSpriteId].y2 = DEBUG_NUMBER_ICON_Y+10;
    gSprites[gTasks[taskId].tSpriteId].oam.priority = 0;
}

static void DestroyItemIcon(u8 taskId)
{
    FreeSpriteTilesByTag(ITEM_TAG);
    FreeSpritePaletteByTag(ITEM_TAG);
    FreeSpriteOamMatrix(&gSprites[gTasks[taskId].tSpriteId]);
    DestroySprite(&gSprites[gTasks[taskId].tSpriteId]);
}

static void Debug_Display_ItemQuantity(u32 quantity, u32 digit, u8 windowId)
{
    StringCopy(gStringVar2, gText_DigitIndicator[digit]);
    ConvertIntToDecimalStringN(gStringVar1, quantity, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_ITEM_QUANTITY);
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Quantity:{CLEAR_TO 90}\n{STR_VAR_1}{CLEAR_TO 90}\n\n{STR_VAR_2}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_Give_Item_SelectId(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        Debug_HandleInput_Numeric(taskId, 1, ITEMS_COUNT - 1, DEBUG_NUMBER_DIGITS_ITEMS);
        Debug_Display_ItemInfo(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
        DestroyItemIcon(taskId);
        gTasks[taskId].tSpriteId = AddItemIconSprite(ITEM_TAG, ITEM_TAG, gTasks[taskId].tInput);
        gSprites[gTasks[taskId].tSpriteId].x2 = DEBUG_NUMBER_ICON_X+10;
        gSprites[gTasks[taskId].tSpriteId].y2 = DEBUG_NUMBER_ICON_Y+10;
        gSprites[gTasks[taskId].tSpriteId].oam.priority = 0;
    }

    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].tItemId = gTasks[taskId].tInput;
        gTasks[taskId].tInput = 1;
        gTasks[taskId].tDigit = 0;
        Debug_Display_ItemQuantity(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
        gTasks[taskId].func = DebugAction_Give_Item_SelectQuantity;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        DestroyItemIcon(taskId);

        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void DebugAction_Give_Item_SelectQuantity(u8 taskId)
{
    enum Item itemId = gTasks[taskId].tItemId;

    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        Debug_HandleInput_Numeric(taskId, 1, MAX_BAG_ITEM_CAPACITY, MAX_ITEM_DIGITS);
        Debug_Display_ItemQuantity(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        DestroyItemIcon(taskId);

        PlaySE(MUS_LEVEL_UP);
        AddBagItem(itemId, gTasks[taskId].tInput);
        DebugAction_DestroyExtraWindow(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        DestroyItemIcon(taskId);

        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

#undef tItemId
#undef tSpriteId

//Pokemon
static void ResetMonDataStruct(struct DebugMonData *sDebugMonData)
{
    sDebugMonData->species          = 1;
    sDebugMonData->level            = MIN_LEVEL;
    sDebugMonData->isShiny          = FALSE;
    sDebugMonData->nature           = 0;
    sDebugMonData->abilityNum       = 0;
    sDebugMonData->teraType         = TYPE_NONE;
    sDebugMonData->dynamaxLevel     = 0;
    sDebugMonData->gmaxFactor       = FALSE;
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        sDebugMonData->monIVs[i] = 0;
        sDebugMonData->monEVs[i] = 0;
    }
}

#define tIsComplex  data[5]
#define tSpriteId   data[6]
#define tIterator   data[7]
#define tIsEgg      data[8]

static void Debug_Display_SpeciesInfo(u32 species, u32 number, u32 digit, u8 windowId)
{
    u8 *end;
    StringCopy(gStringVar2, gText_DigitIndicator[digit]);
    if (!IsSpeciesEnabled(species))
    {
        species = SPECIES_NONE;
        end = StringCopy(gStringVar1, COMPOUND_STRING("Species Disabled"));
    }
    else
    {
        end = StringCopy(gStringVar1, GetSpeciesName(species));
    }
    WrapFontIdToFit(gStringVar1, end, DEBUG_MENU_FONT, WindowWidthPx(windowId));
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    ConvertIntToDecimalStringN(gStringVar3, number, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_ITEMS);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Species: {STR_VAR_3}\n{STR_VAR_1}{CLEAR_TO 90}\n\n{STR_VAR_2}{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_Give_PokemonSimple(u8 taskId)
{
    u8 windowId;

    //Mon data struct
    sDebugMonData = AllocZeroed(sizeof(struct DebugMonData));
    ResetMonDataStruct(sDebugMonData);

    //Window initialization
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    // Display initial Pokémon
    u32 species;
    if (!IsSpeciesEnabled(sDebugMonData->species))
        species = SPECIES_NONE;
    else
        species = sDebugMonData->species;

    Debug_Display_SpeciesInfo(species, sDebugMonData->species, 0, windowId);

    //Set task data
    gTasks[taskId].func = DebugAction_Give_Pokemon_SelectId;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tInput = sDebugMonData->species;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tIsComplex = FALSE;
    gTasks[taskId].tIsEgg = FALSE;

    FreeMonIconPalettes();
    LoadMonIconPalettePersonality(species, 0);
    gTasks[taskId].tSpriteId = CreateMonIcon(species, SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0);
    gSprites[gTasks[taskId].tSpriteId].oam.priority = 0;
}

static void DebugAction_Give_PokemonComplex(u8 taskId)
{
    u8 windowId;

    //Mon data struct
    sDebugMonData = AllocZeroed(sizeof(struct DebugMonData));
    ResetMonDataStruct(sDebugMonData);

    //Window initialization
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    // Display initial Pokémon
    u32 species;
    if (!IsSpeciesEnabled(sDebugMonData->species))
        species = SPECIES_NONE;
    else
        species = sDebugMonData->species;

    Debug_Display_SpeciesInfo(species, sDebugMonData->species, 0, windowId);

    gTasks[taskId].func = DebugAction_Give_Pokemon_SelectId;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tInput = 1;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tIsComplex = TRUE;
    gTasks[taskId].tIsEgg = FALSE;

    FreeMonIconPalettes();
    LoadMonIconPalettePersonality(species, 0);
    gTasks[taskId].tSpriteId = CreateMonIcon(species, SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0);
    gSprites[gTasks[taskId].tSpriteId].oam.priority = 0;
    gTasks[taskId].tIterator = 0;
}

static void DebugAction_Give_NewEgg(u8 taskId)
{
    u8 windowId;

    //Mon data struct
    sDebugMonData = AllocZeroed(sizeof(struct DebugMonData));
    ResetMonDataStruct(sDebugMonData);

    //Window initialization
    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    // Display initial Pokémon
    u32 species;
    if (!IsSpeciesEnabled(gTasks[taskId].tInput))
        species = SPECIES_NONE;
    else
        species = sDebugMonData->species;

    Debug_Display_SpeciesInfo(species, gTasks[taskId].tInput, 0, windowId);

    //Set task data
    gTasks[taskId].func = DebugAction_Give_Pokemon_SelectId;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tInput = sDebugMonData->species;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tIsComplex = FALSE;
    gTasks[taskId].tIsEgg = TRUE;

    FreeMonIconPalettes();
    LoadMonIconPalette(species);
    gTasks[taskId].tSpriteId = CreateMonIcon(species, SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0);
    gSprites[gTasks[taskId].tSpriteId].oam.priority = 0;
}

static void Debug_Display_Level(u32 level, u32 digit, u8 windowId)
{
    StringCopy(gStringVar2, gText_DigitIndicator[digit]);
    ConvertIntToDecimalStringN(gStringVar1, level, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Level:{CLEAR_TO 90}\n{STR_VAR_1}{CLEAR_TO 90}\n{CLEAR_TO 90}\n{STR_VAR_2}{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_Give_Pokemon_SelectId(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        Debug_HandleInput_Numeric(taskId, 1, NUM_SPECIES - 1, DEBUG_NUMBER_DIGITS_ITEMS);
        u32 species = gTasks[taskId].tInput;
        if (!IsSpeciesEnabled(species))
            species = SPECIES_NONE;
        Debug_Display_SpeciesInfo(species, gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].tSpriteId]);
        FreeMonIconPalettes();
        LoadMonIconPalettePersonality(species, 0);
        gTasks[taskId].tSpriteId = CreateMonIcon(species, SpriteCB_MonIcon, DEBUG_NUMBER_ICON_X, DEBUG_NUMBER_ICON_Y, 4, 0);
        gSprites[gTasks[taskId].tSpriteId].oam.priority = 0;
    }

    if (JOY_NEW(A_BUTTON))
    {
        if (!IsSpeciesEnabled(gTasks[taskId].tInput))
        {
            PlaySE(SE_PC_OFF);
            return;
        }

        sDebugMonData->species = gTasks[taskId].tInput;
        gTasks[taskId].tInput = 1;
        gTasks[taskId].tDigit = 0;

        if (!gTasks[taskId].tIsEgg)
        {
            Debug_Display_Level(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
            gTasks[taskId].func = DebugAction_Give_Pokemon_SelectLevel;
            return;
        }

        ScriptGiveEgg(sDebugMonData->species);
        PlaySE(SE_SELECT);
        Free(sDebugMonData);
        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].tSpriteId]);
        DebugAction_DestroyExtraWindow(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData);
        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].tSpriteId]);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void Debug_Display_TrueFalse(bool32 value, u8 windowId, const u8 *titleStr)
{
    static const u8 *txtStr;
    txtStr = value ? sDebugText_True : sDebugText_False;
    StringCopyPadded(gStringVar2, txtStr, CHAR_SPACE, 15);
    ConvertIntToDecimalStringN(gStringVar3, value, STR_CONV_MODE_LEADING_ZEROS, 0);
    StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
    StringExpandPlaceholders(gStringVar4, titleStr);
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_Give_Pokemon_SelectLevel(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        Debug_HandleInput_Numeric(taskId, 1, MAX_LEVEL, 3);
        Debug_Display_Level(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].tSpriteId]);
        if (gTasks[taskId].tIsComplex == FALSE)
        {
            PlaySE(MUS_LEVEL_UP);
            ScriptGiveMon(sDebugMonData->species, gTasks[taskId].tInput, ITEM_NONE);
            // Set flag for user convenience
            FlagSet(FLAG_SYS_POKEMON_GET);
            Free(sDebugMonData);
            DebugAction_DestroyExtraWindow(taskId);
        }
        else
        {
            sDebugMonData->level = gTasks[taskId].tInput;
            gTasks[taskId].tInput = 0;
            gTasks[taskId].tDigit = 0;
            Debug_Display_TrueFalse(gTasks[taskId].tInput, gTasks[taskId].tSubWindowId, sDebugText_PokemonShiny);
            gTasks[taskId].func = DebugAction_Give_Pokemon_SelectShiny;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData);
        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].tSpriteId]);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void Debug_Display_Nature(u32 natureId, u32 digit, u8 windowId)
{
    StringCopy(gStringVar2, gText_DigitIndicator[digit]);
    ConvertIntToDecimalStringN(gStringVar3, natureId, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
    if (natureId == 0)
        StringCopy(gStringVar1, COMPOUND_STRING("Random"));
    else
        StringCopy(gStringVar1, gNaturesInfo[natureId - 1].name);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Nature ID: {STR_VAR_3}{CLEAR_TO 90}\n{STR_VAR_1}{CLEAR_TO 90}\n{CLEAR_TO 90}\n{STR_VAR_2}{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_Give_Pokemon_SelectShiny(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].tInput ^= JOY_NEW(DPAD_UP | DPAD_DOWN) > 0;
        Debug_Display_TrueFalse(gTasks[taskId].tInput, gTasks[taskId].tSubWindowId, sDebugText_PokemonShiny);
    }

    if (JOY_NEW(A_BUTTON))
    {
        sDebugMonData->isShiny = gTasks[taskId].tInput;
        gTasks[taskId].tInput = 0;
        gTasks[taskId].tDigit = 0;
        Debug_Display_Nature(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
        gTasks[taskId].func = DebugAction_Give_Pokemon_SelectNature;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void Debug_Display_Ability(u32 abilityNum, u32 digit, u8 windowId)//(u32 natureId, u32 digit, u8 windowId)
{
    enum Ability abilityId = GetAbilityBySpecies(sDebugMonData->species, abilityNum);
    StringCopy(gStringVar2, gText_DigitIndicator[digit]);
    ConvertIntToDecimalStringN(gStringVar3, abilityNum, STR_CONV_MODE_LEFT_ALIGN, 2);
    StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
    u8 *end = StringCopy(gStringVar1, gAbilitiesInfo[abilityId].name);
    WrapFontIdToFit(gStringVar1, end, DEBUG_MENU_FONT, WindowWidthPx(windowId));
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Ability Num: {STR_VAR_3}{CLEAR_TO 90}\n{STR_VAR_1}{CLEAR_TO 90}\n{CLEAR_TO 90}\n{STR_VAR_2}{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_Give_Pokemon_SelectNature(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_NEW(DPAD_UP))
        {
            gTasks[taskId].tInput += sPowersOfTen[gTasks[taskId].tDigit];
            if (gTasks[taskId].tInput > NUM_NATURES)
                gTasks[taskId].tInput = NUM_NATURES;
        }
        if (JOY_NEW(DPAD_DOWN))
        {
            gTasks[taskId].tInput -= sPowersOfTen[gTasks[taskId].tDigit];
            if (gTasks[taskId].tInput < 0)
                gTasks[taskId].tInput = 0;
        }

        Debug_Display_Nature(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].tInput == 0)
            sDebugMonData->nature = NATURE_RANDOM;
        else
            sDebugMonData->nature = gTasks[taskId].tInput - 1;
        gTasks[taskId].tInput = 0;
        gTasks[taskId].tDigit = 0;

        Debug_Display_Ability(0, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);

        gTasks[taskId].func = DebugAction_Give_Pokemon_SelectAbility;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void Debug_Display_TeraType(u32 typeId, u32 digit, u8 windowId)
{
    StringCopy(gStringVar2, gText_DigitIndicator[digit]);
    ConvertIntToDecimalStringN(gStringVar3, typeId, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
    StringCopy(gStringVar1, gTypesInfo[typeId].name);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Tera Type: {STR_VAR_3}{CLEAR_TO 90}\n{STR_VAR_1}{CLEAR_TO 90}\n{CLEAR_TO 90}\n{STR_VAR_2}{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_Give_Pokemon_SelectAbility(u8 taskId)
{
    s32 abilityNum = -1;

    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_NEW(DPAD_UP))
        {
            abilityNum = gTasks[taskId].tInput + 1;
            while (GetSpeciesAbility(sDebugMonData->species, abilityNum) == ABILITY_NONE && abilityNum < NUM_ABILITY_SLOTS)
            {
                abilityNum++;
            }
        }
        if (JOY_NEW(DPAD_DOWN))
        {
            abilityNum = gTasks[taskId].tInput - 1;
            while (GetSpeciesAbility(sDebugMonData->species, abilityNum) == ABILITY_NONE && abilityNum >= 0)
            {
                abilityNum--;
            }
        }

        if (abilityNum >= 0 && abilityNum < NUM_ABILITY_SLOTS)
        {
            gTasks[taskId].tInput = abilityNum;
            Debug_Display_Ability(abilityNum, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
        }
    }

    if (JOY_NEW(A_BUTTON))
    {
        sDebugMonData->abilityNum = gTasks[taskId].tInput;
        gTasks[taskId].tInput = 0;
        gTasks[taskId].tDigit = 0;

        Debug_Display_TeraType(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);

        gTasks[taskId].func = DebugAction_Give_Pokemon_SelectTeraType;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void Debug_Display_DynamaxLevel(u32 level, u32 digit, u8 windowId)
{
    StringCopy(gStringVar2, gText_DigitIndicator[digit]);
    ConvertIntToDecimalStringN(gStringVar1, level, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Dmax Lvl:{CLEAR_TO 90}\n{STR_VAR_1}{CLEAR_TO 90}\n{CLEAR_TO 90}\n{STR_VAR_2}{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_Give_Pokemon_SelectTeraType(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        if (JOY_NEW(DPAD_UP))
        {
            gTasks[taskId].tInput += sPowersOfTen[gTasks[taskId].tDigit];
            if (gTasks[taskId].tInput > NUMBER_OF_MON_TYPES - 1)
                gTasks[taskId].tInput = NUMBER_OF_MON_TYPES - 1;
        }
        if (JOY_NEW(DPAD_DOWN))
        {
            gTasks[taskId].tInput -= sPowersOfTen[gTasks[taskId].tDigit];
            if (gTasks[taskId].tInput < 0)
                gTasks[taskId].tInput = 0;
        }

        Debug_Display_TeraType(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        sDebugMonData->teraType = gTasks[taskId].tInput;
        gTasks[taskId].tInput = 0;
        gTasks[taskId].tDigit = 0;

        Debug_Display_DynamaxLevel(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);

        gTasks[taskId].func = DebugAction_Give_Pokemon_SelectDynamaxLevel;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void Debug_Display_GigantamaxFactor(u32 input, u8 windowId)
{
    Debug_Display_TrueFalse(input, windowId, COMPOUND_STRING("Gmax Factor:{CLEAR_TO 90}\n   {STR_VAR_2}{CLEAR_TO 90}\n{CLEAR_TO 90}\n{CLEAR_TO 90}"));
}

static void DebugAction_Give_Pokemon_SelectDynamaxLevel(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        Debug_HandleInput_Numeric(taskId, 0, MAX_DYNAMAX_LEVEL, 2);
        Debug_Display_DynamaxLevel(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        sDebugMonData->dynamaxLevel = gTasks[taskId].tInput;
        gTasks[taskId].tInput = 0;
        gTasks[taskId].tDigit = 0;
        Debug_Display_GigantamaxFactor(gTasks[taskId].tInput, gTasks[taskId].tSubWindowId);
        gTasks[taskId].func = DebugAction_Give_Pokemon_SelectGigantamaxFactor;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData);
        FreeMonIconPalettes();
        FreeAndDestroyMonIconSprite(&gSprites[gTasks[taskId].tSpriteId]);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void Debug_Display_StatInfo(const u8* text, u32 stat, u32 value, u32 digit, u8 windowId, u32 maxValue)
{
    StringCopy(gStringVar1, gStatNamesTable[stat]);
    StringCopy(gStringVar2, gText_DigitIndicator[digit]);
    ConvertIntToDecimalStringN(gStringVar3, value, STR_CONV_MODE_LEADING_ZEROS, CountDigits(maxValue));
    StringCopyPadded(gStringVar3, gStringVar3, CHAR_SPACE, 15);
    StringExpandPlaceholders(gStringVar4, text);
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_Give_Pokemon_SelectGigantamaxFactor(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].tInput ^= JOY_NEW(DPAD_UP | DPAD_DOWN) > 0;
        Debug_Display_GigantamaxFactor(gTasks[taskId].tInput, gTasks[taskId].tSubWindowId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        sDebugMonData->gmaxFactor = gTasks[taskId].tInput;
        gTasks[taskId].tInput = 0;
        gTasks[taskId].tDigit = 0;
        Debug_Display_StatInfo(sDebugText_IVs, gTasks[taskId].tIterator, gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId, MAX_PER_STAT_IVS);
        gTasks[taskId].func = DebugAction_Give_Pokemon_SelectIVs;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void DebugAction_Give_Pokemon_SelectIVs(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        Debug_HandleInput_Numeric(taskId, 0, MAX_PER_STAT_IVS, 3);
        Debug_Display_StatInfo(sDebugText_IVs, gTasks[taskId].tIterator, gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId, MAX_PER_STAT_IVS);
    }

    //If A or B button
    if (JOY_NEW(A_BUTTON))
    {
        // Set IVs for stat
        sDebugMonData->monIVs[gTasks[taskId].tIterator] = gTasks[taskId].tInput;

        //Check if all IVs set
        if (gTasks[taskId].tIterator != NUM_STATS - 1)
        {
            gTasks[taskId].tIterator++;
            gTasks[taskId].tInput = 0;
            gTasks[taskId].tDigit = 0;

            Debug_Display_StatInfo(sDebugText_IVs, gTasks[taskId].tIterator, gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId, MAX_PER_STAT_IVS);
            gTasks[taskId].func = DebugAction_Give_Pokemon_SelectIVs;
        }
        else
        {
            gTasks[taskId].tInput = 0;
            gTasks[taskId].tDigit = 0;
            gTasks[taskId].tIterator = 0;

            Debug_Display_StatInfo(sDebugText_EVs, gTasks[taskId].tIterator, gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId, MAX_PER_STAT_EVS);
            gTasks[taskId].func = DebugAction_Give_Pokemon_SelectEVs;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static u32 GetDebugPokemonTotalEV(void)
{
    u32 totalEVs = 0;
    for (u32 i = 0; i < NUM_STATS; i++)
        totalEVs += sDebugMonData->monEVs[i];
    return totalEVs;
}

static void Debug_Display_MoveInfo(enum Move moveId, u32 iteration, u32 digit, u8 windowId)
{
    // Doesn't expand placeholdes so a 4th dynamic value can be shown.
    u8 *end;
    if (moveId == MOVES_COUNT)
        end = StringCopy(gStringVar1, COMPOUND_STRING("Default"));
    else
        end = StringCopy(gStringVar1, GetMoveName(moveId));
    WrapFontIdToFit(gStringVar1, end, DEBUG_MENU_FONT, WindowWidthPx(windowId));
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    StringCopy(gStringVar4, COMPOUND_STRING("Move "));
    ConvertIntToDecimalStringN(gStringVar3, iteration, STR_CONV_MODE_LEADING_ZEROS, 1);
    StringAppend(gStringVar4, gStringVar3);
    StringAppend(gStringVar4, COMPOUND_STRING(": "));
    ConvertIntToDecimalStringN(gStringVar3, moveId, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringAppend(gStringVar4, gStringVar3);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n"));
    StringAppend(gStringVar4, gStringVar1);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}\n{CLEAR_TO 90}\n"));
    StringAppend(gStringVar4, gText_DigitIndicator[digit]);
    StringAppend(gStringVar4, COMPOUND_STRING("{CLEAR_TO 90}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_Give_Pokemon_SelectEVs(u8 taskId)
{
    u16 totalEV = GetDebugPokemonTotalEV();

    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        Debug_HandleInput_Numeric(taskId, 0, MAX_PER_STAT_EVS, 4);
        Debug_Display_StatInfo(sDebugText_EVs, gTasks[taskId].tIterator, gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId, MAX_PER_STAT_EVS);
    }

    //If A or B button
    if (JOY_NEW(A_BUTTON))
    {
        // Set EVs for stat
        sDebugMonData->monEVs[gTasks[taskId].tIterator] = gTasks[taskId].tInput;

        //Check if all EVs set
        if (gTasks[taskId].tIterator != NUM_STATS - 1)
        {
            gTasks[taskId].tIterator++;
            gTasks[taskId].tInput = 0;
            gTasks[taskId].tDigit = 0;
            Debug_Display_StatInfo(sDebugText_EVs, gTasks[taskId].tIterator, gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId, MAX_PER_STAT_EVS);
            gTasks[taskId].func = DebugAction_Give_Pokemon_SelectEVs;
        }
        else
        {
            gTasks[taskId].tInput = 0;
            gTasks[taskId].tDigit = 0;
            gTasks[taskId].tIterator = 0;

            if (totalEV > MAX_TOTAL_EVS)
            {
                for (u32 i = 0; i < NUM_STATS; i++)
                {
                    sDebugMonData->monEVs[i] = 0;
                }

                PlaySE(SE_FAILURE);
                Debug_Display_StatInfo(sDebugText_EVs, gTasks[taskId].tIterator, gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId, MAX_PER_STAT_EVS);
                gTasks[taskId].func = DebugAction_Give_Pokemon_SelectEVs;
            }
            else
            {
                Debug_Display_MoveInfo(gTasks[taskId].tInput, gTasks[taskId].tIterator, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
                gTasks[taskId].func = DebugAction_Give_Pokemon_Move;
            }
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void DebugAction_Give_Pokemon_Move(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        Debug_HandleInput_Numeric(taskId, 0, MOVES_COUNT, 4);

        Debug_Display_MoveInfo(gTasks[taskId].tInput, gTasks[taskId].tIterator, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
    }

    if (JOY_NEW(A_BUTTON))
    {
        // Set current value
        if (gTasks[taskId].tInput < MOVES_COUNT)
            sDebugMonData->monMoves[gTasks[taskId].tIterator] = gTasks[taskId].tInput;
        else
            sDebugMonData->monMoves[gTasks[taskId].tIterator] = MOVE_DEFAULT;
        // If MOVE_NONE selected, stop asking for additional moves
        if (gTasks[taskId].tInput == MOVE_NONE)
            gTasks[taskId].tIterator = MAX_MON_MOVES;

        //If NOT last move or selected MOVE_NONE ask for next move, else make mon
        if (gTasks[taskId].tIterator < MAX_MON_MOVES - 1)
        {
            gTasks[taskId].tIterator++;
            gTasks[taskId].tInput = 0;
            gTasks[taskId].tDigit = 0;

            Debug_Display_MoveInfo(gTasks[taskId].tInput, gTasks[taskId].tIterator, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
            gTasks[taskId].func = DebugAction_Give_Pokemon_Move;
        }
        else
        {
            gTasks[taskId].tInput = 0;
            gTasks[taskId].tDigit = 0;

            PlaySE(MUS_LEVEL_UP);
            gTasks[taskId].func = DebugAction_Give_Pokemon_ComplexCreateMon;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Free(sDebugMonData);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

static void DebugAction_Give_Pokemon_ComplexCreateMon(u8 taskId) //https://github.com/ghoulslash/pokeemerald/tree/custom-givemon
{
    struct Pokemon mon;
    u8 i;
    enum Move moves[MAX_MON_MOVES];
    u8 IVs[NUM_STATS];
    u8 iv_val;
    u8 EVs[NUM_STATS];
    u8 ev_val;
    u16 species     = sDebugMonData->species;
    u8 level        = sDebugMonData->level;
    bool8 isShiny   = sDebugMonData->isShiny;
    u8 nature       = sDebugMonData->nature;
    u8 abilityNum   = sDebugMonData->abilityNum;
    u32 teraType    = sDebugMonData->teraType;
    u32 dmaxLevel   = sDebugMonData->dynamaxLevel;
    u32 gmaxFactor  = sDebugMonData->gmaxFactor;
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        moves[i] = sDebugMonData->monMoves[i];
    }
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        EVs[i] = sDebugMonData->monEVs[i];
        IVs[i] = sDebugMonData->monIVs[i];
    }

    //Nature
    u32 personality = GetMonPersonality(species, MON_GENDER_RANDOM, nature, RANDOM_UNOWN_LETTER);
    CreateMon(&mon, species, level, personality, OTID_STRUCT_PLAYER_ID);

    //Shininess
    SetMonData(&mon, MON_DATA_IS_SHINY, &isShiny);

    // Gigantamax factor
    SetMonData(&mon, MON_DATA_GIGANTAMAX_FACTOR, &gmaxFactor);

    // Dynamax Level
    SetMonData(&mon, MON_DATA_DYNAMAX_LEVEL, &dmaxLevel);

    // tera type
    if (teraType == TYPE_NONE || teraType == TYPE_MYSTERY || teraType >= NUMBER_OF_MON_TYPES)
        teraType = GetTeraTypeFromPersonality(&mon);
    SetMonData(&mon, MON_DATA_TERA_TYPE, &teraType);

    //IVs
    for (i = 0; i < NUM_STATS; i++)
    {
        iv_val = IVs[i];
        if (iv_val != USE_RANDOM_IVS && iv_val != 0xFF)
            SetMonData(&mon, MON_DATA_HP_IV + i, &iv_val);
    }

    //EVs
    for (i = 0; i < NUM_STATS; i++)
    {
        ev_val = EVs[i];
        if (ev_val)
            SetMonData(&mon, MON_DATA_HP_EV + i, &ev_val);
    }

    GiveMonInitialMoveset(&mon);
    //Moves
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        // Non-default moveset chosen. Reset moves before setting the chosen moves.
        if (moves[0] != MOVE_NONE)
            SetMonMoveSlot(&mon, MOVE_NONE, i);

        if (moves[i] == MOVE_NONE)
            continue;

        if (moves[i] == MOVE_DEFAULT)
            GiveMonDefaultMove(&mon, i);
        else
            SetMonMoveSlot(&mon, moves[i], i);
    }

    // Ability
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &abilityNum);

    //Update mon stats before giving it to the player
    CalculateMonStats(&mon);

    GiveScriptedMonToPlayer(&mon, PARTY_SIZE);

    // Set flag for user convenience
    FlagSet(FLAG_SYS_POKEMON_GET);

    Free(sDebugMonData);
    DebugAction_DestroyExtraWindow(taskId); //return sentToPc;
}

#undef tIsComplex
#undef tSpriteId
#undef tIterator
#undef tIsEgg

//Decoration
#define tSpriteId  data[6]

static void Debug_Display_DecorationInfo(enum Item itemId, u32 digit, u8 windowId)
{
    StringCopy(gStringVar2, gText_DigitIndicator[digit]);
    u8* end = StringCopy(gStringVar1, gDecorations[itemId].name);
    WrapFontIdToFit(gStringVar1, end, DEBUG_MENU_FONT, WindowWidthPx(windowId));
    StringCopyPadded(gStringVar1, gStringVar1, CHAR_SPACE, 15);
    ConvertIntToDecimalStringN(gStringVar3, itemId, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_ITEMS);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Decor ID: {STR_VAR_3}\n{STR_VAR_1}{CLEAR_TO 90}\n\n{STR_VAR_2}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugAction_Give_Decoration(u8 taskId)
{
    u8 windowId;

    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateExtra);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    // Display initial decoration
    Debug_Display_DecorationInfo(1, 0, windowId);

    gTasks[taskId].func = DebugAction_Give_Decoration_SelectId;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tInput = 1;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tSpriteId = AddDecorationIconObject(gTasks[taskId].tInput, DEBUG_NUMBER_ICON_X+8, DEBUG_NUMBER_ICON_Y+10, 0, ITEM_TAG, ITEM_TAG);
}

static void DestroyDecorationIcon(u8 taskId)
{
    FreeSpriteTilesByTag(ITEM_TAG);
    FreeSpritePaletteByTag(ITEM_TAG);
    FreeSpriteOamMatrix(&gSprites[gTasks[taskId].tSpriteId]);
    DestroySprite(&gSprites[gTasks[taskId].tSpriteId]);
}

static void DebugAction_Give_Decoration_SelectId(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);
        Debug_HandleInput_Numeric(taskId, 1, NUM_DECORATIONS, DEBUG_NUMBER_DIGITS_ITEMS);
        Debug_Display_DecorationInfo(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
        DestroyDecorationIcon(taskId);
        gTasks[taskId].tSpriteId = AddDecorationIconObject(gTasks[taskId].tInput, DEBUG_NUMBER_ICON_X+8, DEBUG_NUMBER_ICON_Y+10, 0, ITEM_TAG, ITEM_TAG);
    }

    if (JOY_NEW(A_BUTTON))
    {
        DestroyItemIcon(taskId);

        PlaySE(MUS_LEVEL_UP);
        DecorationAdd(gTasks[taskId].tInput);
        DebugAction_DestroyExtraWindow(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        DestroyDecorationIcon(taskId);

        PlaySE(SE_SELECT);
        DebugAction_DestroyExtraWindow(taskId);
    }
}

#undef tSpriteId

static void DebugAction_Give_MaxMoney(u8 taskId)
{
    SetMoney(&gSaveBlock1Ptr->money, MAX_MONEY);
}

static void DebugAction_Give_MaxCoins(u8 taskId)
{
    SetCoins(MAX_COINS);

    if (!CheckBagHasItem(ITEM_COIN_CASE, 1))
        AddBagItem(ITEM_COIN_CASE, 1);
}

static void DebugAction_Give_MaxBattlePoints(u8 taskId)
{
    gSaveBlock2Ptr->frontier.battlePoints = MAX_BATTLE_FRONTIER_POINTS;
}

static void DebugAction_Give_DayCareEgg(u8 taskId)
{
    s32 emptySlot = Daycare_FindEmptySpot(&gSaveBlock1Ptr->daycare);
    if (emptySlot == 0) // no daycare mons
        Debug_DestroyMenu_Full_Script(taskId, DebugScript_ZeroDaycareMons);
    else if (emptySlot == 1) // 1 daycare mon
        Debug_DestroyMenu_Full_Script(taskId, DebugScript_OneDaycareMons);
    else if (GetDaycareCompatibilityScore(&gSaveBlock1Ptr->daycare) == PARENTS_INCOMPATIBLE) // not compatible parents
        Debug_DestroyMenu_Full_Script(taskId, DebugScript_DaycareMonsNotCompatible);
    else // 2 pokemon which can have a pokemon baby together
        TriggerPendingDaycareEgg();
}

// *******************************
// Actions TimeMenu

static void DebugAction_TimeMenu_ChangeTimeOfDay(u8 taskId)
{
    u32 input = ListMenu_ProcessInput(gTasks[taskId].tMenuTaskId);

    DebugAction_DestroyExtraWindow(taskId);
    switch (input)
    {
    case TIME_MORNING:
        FakeRtc_ForwardTimeTo(MORNING_HOUR_BEGIN, 0, 0);
        break;
    case TIME_DAY:
        FakeRtc_ForwardTimeTo(DAY_HOUR_BEGIN, 0, 0);
        break;
    case TIME_EVENING:
        FakeRtc_ForwardTimeTo(EVENING_HOUR_BEGIN, 0, 0);
        break;
    case TIME_NIGHT:
            FakeRtc_ForwardTimeTo(NIGHT_HOUR_BEGIN, 0, 0);
            break;
    }
    Debug_DestroyMenu_Full(taskId);
    SetMainCallback2(CB2_LoadMap);
}

static void DebugAction_TimeMenu_ChangeWeekdays(u8 taskId)
{
    u32 input = ListMenu_ProcessInput(gTasks[taskId].tMenuTaskId);
    struct SiiRtcInfo *rtc = FakeRtc_GetCurrentTime();
    u32 daysToAdd = 0;

    DebugAction_DestroyExtraWindow(taskId);
    daysToAdd = ((input - rtc->dayOfWeek) + WEEKDAY_COUNT) % WEEKDAY_COUNT;
    FakeRtc_AdvanceTimeBy(daysToAdd, 0, 0, 0);
    Debug_DestroyMenu_Full(taskId);
    SetMainCallback2(CB2_LoadMap);
}

// *******************************
// Actions PCBag

static void DebugAction_PCBag_Fill_PCBoxes_Fast(u8 taskId) //Credit: Sierraffinity
{
    int boxId, boxPosition;
    struct BoxPokemon boxMon;
    u16 species = SPECIES_BULBASAUR;
    u8 speciesName[POKEMON_NAME_LENGTH + 1];

    CreateBoxMon(&boxMon, species, 100, Random32(), OTID_STRUCT_PLAYER_ID);
    //mons are created with 0 IVs

    for (boxId = 0; boxId < TOTAL_BOXES_COUNT; boxId++)
    {
        for (boxPosition = 0; boxPosition < IN_BOX_COUNT; boxPosition++, species++)
        {
            if (!GetBoxMonData(&gPokemonStoragePtr->boxes[boxId][boxPosition], MON_DATA_SANITY_HAS_SPECIES))
            {
                StringCopy(speciesName, GetSpeciesName(species));
                SetBoxMonData(&boxMon, MON_DATA_NICKNAME, &speciesName);
                SetBoxMonData(&boxMon, MON_DATA_SPECIES, &species);
                GiveBoxMonInitialMoveset(&boxMon);
                gPokemonStoragePtr->boxes[boxId][boxPosition] = boxMon;
            }
        }
    }

    // Set flag for user convenience
    FlagSet(FLAG_SYS_POKEMON_GET);
    Debug_DestroyMenu_Full(taskId);
    ScriptContext_Enable();
}

static void DebugAction_PCBag_Fill_PCBoxes_Slow(u8 taskId)
{
    int boxId, boxPosition;
    struct BoxPokemon boxMon;
    u32 species = SPECIES_BULBASAUR;
    bool8 spaceAvailable = FALSE;

    for (boxId = 0; boxId < TOTAL_BOXES_COUNT; boxId++)
    {
        for (boxPosition = 0; boxPosition < IN_BOX_COUNT; boxPosition++)
        {
            if (!GetBoxMonData(&gPokemonStoragePtr->boxes[boxId][boxPosition], MON_DATA_SANITY_HAS_SPECIES))
            {
                if (!spaceAvailable)
                    PlayBGM(MUS_RG_MYSTERY_GIFT);
                CreateBoxMon(&boxMon, species, 100, Random32(), OTID_STRUCT_PLAYER_ID);
                SetBoxMonIVs(&boxMon, USE_RANDOM_IVS);
                GiveBoxMonInitialMoveset(&boxMon);
                gPokemonStoragePtr->boxes[boxId][boxPosition] = boxMon;
                species = (species < NUM_SPECIES - 1) ? species + 1 : 1;
                spaceAvailable = TRUE;
            }
        }
    }

    // Set flag for user convenience
    FlagSet(FLAG_SYS_POKEMON_GET);
    if (spaceAvailable)
        PlayBGM(GetCurrentMapMusic());

    Debug_DestroyMenu_Full_Script(taskId, Debug_BoxFilledMessage);
}

static void DebugAction_PCBag_Fill_PCItemStorage(u8 taskId)
{
    enum Item itemId;

    for (itemId = 1; itemId < ITEMS_COUNT; itemId++)
    {
        if (!CheckPCHasItem(itemId, MAX_PC_ITEM_CAPACITY))
            AddPCItem(itemId, MAX_PC_ITEM_CAPACITY);
    }
}

static void DebugAction_PCBag_Fill_PocketItems(u8 taskId)
{
    enum Item itemId;

    for (itemId = 1; itemId < ITEMS_COUNT; itemId++)
    {
        if (GetItemPocket(itemId) == POCKET_ITEMS && CheckBagHasSpace(itemId, MAX_BAG_ITEM_CAPACITY))
            AddBagItem(itemId, MAX_BAG_ITEM_CAPACITY);
    }
}

static void DebugAction_PCBag_Fill_PocketPokeBalls(u8 taskId)
{
    for (enum PokeBall ballId = BALL_STRANGE; ballId < POKEBALL_COUNT; ballId++)
    {
        if (CheckBagHasSpace(gPokeBalls[ballId].itemId, MAX_BAG_ITEM_CAPACITY))
            AddBagItem(gPokeBalls[ballId].itemId, MAX_BAG_ITEM_CAPACITY);
    }
}

static void DebugAction_PCBag_Fill_PocketTMHM(u8 taskId)
{
    u16 index, itemId;

    for (index = 0; index < NUM_ALL_MACHINES; index++)
    {
        itemId = GetTMHMItemId(index + 1);
        if (CheckBagHasSpace(itemId, 1) && ItemIdToBattleMoveId(itemId) != MOVE_NONE)
            AddBagItem(itemId, 1);
    }
}

static void DebugAction_PCBag_Fill_PocketBerries(u8 taskId)
{
    enum Item itemId;

    for (itemId = FIRST_BERRY_INDEX; itemId < LAST_BERRY_INDEX; itemId++)
    {
        if (CheckBagHasSpace(itemId, MAX_BAG_ITEM_CAPACITY))
            AddBagItem(itemId, MAX_BAG_ITEM_CAPACITY);
    }
}

static void DebugAction_PCBag_Fill_PocketKeyItems(u8 taskId)
{
    enum Item itemId;

    for (itemId = 1; itemId < ITEMS_COUNT; itemId++)
    {
        if (GetItemPocket(itemId) == POCKET_KEY_ITEMS && CheckBagHasSpace(itemId, 1))
            AddBagItem(itemId, 1);
    }
}

static void DebugAction_PCBag_ClearBag(u8 taskId)
{
    PlaySE(MUS_LEVEL_UP);
    ClearBag();
}

static void DebugAction_PCBag_ClearBoxes(u8 taskId)
{
    ResetPokemonStorageSystem();
    Debug_DestroyMenu_Full(taskId);
    ScriptContext_Enable();
}

// *******************************
// Actions Sound
static const u8 *const sBGMNames[END_MUS - START_MUS + 1];
static const u8 *const sSENames[END_SE + 1];

#define tCurrentSong  data[5]

static void DebugAction_Sound_SE(u8 taskId)
{
    u8 windowId;

    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateSound);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    // Display initial sound effect
    StringCopy(gStringVar2, gText_DigitIndicator[0]);
    ConvertIntToDecimalStringN(gStringVar3, 1, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_ITEMS);
    StringCopyPadded(gStringVar1, sSENames[0], CHAR_SPACE, 35);
    StringExpandPlaceholders(gStringVar4, sDebugText_Sound_SFX_ID);
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);

    StopMapMusic(); //Stop map music to better hear sounds

    gTasks[taskId].func = DebugAction_Sound_SE_SelectId;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tInput = 1;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tCurrentSong = gTasks[taskId].tInput;
}

static void DebugAction_Sound_SE_SelectId(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        const u8 *seName;
        Debug_HandleInput_Numeric(taskId, 1, END_SE, DEBUG_NUMBER_DIGITS_ITEMS);

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].tDigit]);
        seName = sSENames[gTasks[taskId].tInput - 1];
        if (seName == NULL)
            seName = sDebugText_Dashes;
        StringCopyPadded(gStringVar1, seName, CHAR_SPACE, 35);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_ITEMS);
        StringExpandPlaceholders(gStringVar4, sDebugText_Sound_SFX_ID);
        AddTextPrinterParameterized(gTasks[taskId].tSubWindowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        m4aSongNumStop(gTasks[taskId].tCurrentSong);
        gTasks[taskId].tCurrentSong = gTasks[taskId].tInput;
        m4aSongNumStart(gTasks[taskId].tInput);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        m4aSongNumStop(gTasks[taskId].tCurrentSong);
        DebugAction_DestroyExtraWindow(taskId);
    }
    else if (JOY_NEW(START_BUTTON))
    {
        m4aSongNumStop(gTasks[taskId].tCurrentSong);
    }
}

static void DebugAction_Sound_MUS(u8 taskId)
{
    u8 windowId;

    ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
    RemoveWindow(gTasks[taskId].tWindowId);

    HideMapNamePopUpWindow();
    LoadMessageBoxAndBorderGfx();
    windowId = AddWindow(&sDebugMenuWindowTemplateSound);
    DrawStdWindowFrame(windowId, FALSE);

    CopyWindowToVram(windowId, COPYWIN_FULL);

    // Display initial song
    StringCopy(gStringVar2, gText_DigitIndicator[0]);
    ConvertIntToDecimalStringN(gStringVar3, START_MUS, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_ITEMS);
    StringCopyPadded(gStringVar1, sBGMNames[0], CHAR_SPACE, 35);
    StringExpandPlaceholders(gStringVar4, sDebugText_Sound_Music_ID);
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);

    StopMapMusic(); //Stop map music to better hear new music

    gTasks[taskId].func = DebugAction_Sound_MUS_SelectId;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tInput = START_MUS;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tCurrentSong = gTasks[taskId].tInput;
}

static void DebugAction_Sound_MUS_SelectId(u8 taskId)
{
    if (JOY_NEW(DPAD_ANY))
    {
        const u8 *bgmName;
        Debug_HandleInput_Numeric(taskId, START_MUS, END_MUS, DEBUG_NUMBER_DIGITS_ITEMS);

        StringCopy(gStringVar2, gText_DigitIndicator[gTasks[taskId].tDigit]);
        bgmName = sBGMNames[gTasks[taskId].tInput - START_MUS];
        if (bgmName == NULL)
            bgmName = sDebugText_Dashes;
        StringCopyPadded(gStringVar1, bgmName, CHAR_SPACE, 35);
        ConvertIntToDecimalStringN(gStringVar3, gTasks[taskId].tInput, STR_CONV_MODE_LEADING_ZEROS, DEBUG_NUMBER_DIGITS_ITEMS);
        StringExpandPlaceholders(gStringVar4, sDebugText_Sound_Music_ID);
        AddTextPrinterParameterized(gTasks[taskId].tSubWindowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
    }

    if (JOY_NEW(A_BUTTON))
    {
        m4aSongNumStop(gTasks[taskId].tCurrentSong);
        gTasks[taskId].tCurrentSong = gTasks[taskId].tInput;
        m4aSongNumStart(gTasks[taskId].tInput);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        // m4aSongNumStop(gTasks[taskId].tCurrentSong);   //Uncomment if music should stop after leaving menu
        DebugAction_DestroyExtraWindow(taskId);
    }
    else if (JOY_NEW(START_BUTTON))
    {
        m4aSongNumStop(gTasks[taskId].tCurrentSong);
    }
}

static const u32 gDebugFollowerNPCGraphics[] =
{
    OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL,
    OBJ_EVENT_GFX_RIVAL_MAY_NORMAL,
    OBJ_EVENT_GFX_STEVEN,
    OBJ_EVENT_GFX_WALLY,
    OBJ_EVENT_GFX_RED,
    OBJ_EVENT_GFX_LEAF,
};

static void DebugAction_CreateFollowerNPC(u8 taskId)
{
    u32 input = ListMenu_ProcessInput(gTasks[taskId].tMenuTaskId);
    u32 gfx = gDebugFollowerNPCGraphics[input];

    Debug_DestroyMenu_Full(taskId);
    LockPlayerFieldControls();
    DestroyFollowerNPC();
    SetFollowerNPCData(FNPC_DATA_BATTLE_PARTNER, PARTNER_STEVEN);
    CreateFollowerNPC(gfx, FNPC_ALL, Debug_Follower_NPC_Event_Script);
    UnlockPlayerFieldControls();
}

static void DebugAction_DestroyFollowerNPC(u8 taskId)
{
    if (FNPC_ENABLE_NPC_FOLLOWERS)
    {
        Debug_DestroyMenu_Full(taskId);
        LockPlayerFieldControls();
        DestroyFollowerNPC();
        UnlockPlayerFieldControls();
    }
    else
    {
        Debug_DestroyMenu_Full_Script(taskId, Debug_Follower_NPC_Not_Enabled);
    }
}

#undef tCurrentSong


#define SOUND_LIST_BGM              \
    X(MUS_LITTLEROOT_TEST)          \
    X(MUS_GSC_ROUTE38)              \
    X(MUS_CAUGHT)                   \
    X(MUS_VICTORY_WILD)             \
    X(MUS_VICTORY_GYM_LEADER)       \
    X(MUS_VICTORY_LEAGUE)           \
    X(MUS_C_COMM_CENTER)            \
    X(MUS_GSC_PEWTER)               \
    X(MUS_C_VS_LEGEND_BEAST)        \
    X(MUS_ROUTE101)                 \
    X(MUS_ROUTE110)                 \
    X(MUS_ROUTE120)                 \
    X(MUS_PETALBURG)                \
    X(MUS_OLDALE)                   \
    X(MUS_GYM)                      \
    X(MUS_SURF)                     \
    X(MUS_PETALBURG_WOODS)          \
    X(MUS_LEVEL_UP)                 \
    X(MUS_HEAL)                     \
    X(MUS_OBTAIN_BADGE)             \
    X(MUS_OBTAIN_ITEM)              \
    X(MUS_EVOLVED)                  \
    X(MUS_OBTAIN_TMHM)              \
    X(MUS_LILYCOVE_MUSEUM)          \
    X(MUS_ROUTE122)                 \
    X(MUS_OCEANIC_MUSEUM)           \
    X(MUS_EVOLUTION_INTRO)          \
    X(MUS_EVOLUTION)                \
    X(MUS_MOVE_DELETED)             \
    X(MUS_ENCOUNTER_GIRL)           \
    X(MUS_ENCOUNTER_MALE)           \
    X(MUS_ABANDONED_SHIP)           \
    X(MUS_FORTREE)                  \
    X(MUS_BIRCH_LAB)                \
    X(MUS_B_TOWER_RS)               \
    X(MUS_ENCOUNTER_SWIMMER)        \
    X(MUS_CAVE_OF_ORIGIN)           \
    X(MUS_OBTAIN_BERRY)             \
    X(MUS_AWAKEN_LEGEND)            \
    X(MUS_SLOTS_JACKPOT)            \
    X(MUS_SLOTS_WIN)                \
    X(MUS_TOO_BAD)                  \
    X(MUS_ROULETTE)                 \
    X(MUS_LINK_CONTEST_P1)          \
    X(MUS_LINK_CONTEST_P2)          \
    X(MUS_LINK_CONTEST_P3)          \
    X(MUS_LINK_CONTEST_P4)          \
    X(MUS_ENCOUNTER_RICH)           \
    X(MUS_VERDANTURF)               \
    X(MUS_RUSTBORO)                 \
    X(MUS_POKE_CENTER)              \
    X(MUS_ROUTE104)                 \
    X(MUS_ROUTE119)                 \
    X(MUS_CYCLING)                  \
    X(MUS_POKE_MART)                \
    X(MUS_LITTLEROOT)               \
    X(MUS_MT_CHIMNEY)               \
    X(MUS_ENCOUNTER_FEMALE)         \
    X(MUS_LILYCOVE)                 \
    X(MUS_DESERT)                   \
    X(MUS_HELP)                     \
    X(MUS_UNDERWATER)               \
    X(MUS_VICTORY_TRAINER)          \
    X(MUS_TITLE)                    \
    X(MUS_INTRO)                    \
    X(MUS_ENCOUNTER_MAY)            \
    X(MUS_ENCOUNTER_INTENSE)        \
    X(MUS_ENCOUNTER_COOL)           \
    X(MUS_ROUTE113)                 \
    X(MUS_ENCOUNTER_AQUA)           \
    X(MUS_FOLLOW_ME)                \
    X(MUS_ENCOUNTER_BRENDAN)        \
    X(MUS_EVER_GRANDE)              \
    X(MUS_ENCOUNTER_SUSPICIOUS)     \
    X(MUS_VICTORY_AQUA_MAGMA)       \
    X(MUS_CABLE_CAR)                \
    X(MUS_GAME_CORNER)              \
    X(MUS_DEWFORD)                  \
    X(MUS_SAFARI_ZONE)              \
    X(MUS_VICTORY_ROAD)             \
    X(MUS_AQUA_MAGMA_HIDEOUT)       \
    X(MUS_SAILING)                  \
    X(MUS_MT_PYRE)                  \
    X(MUS_SLATEPORT)                \
    X(MUS_MT_PYRE_EXTERIOR)         \
    X(MUS_SCHOOL)                   \
    X(MUS_HALL_OF_FAME)             \
    X(MUS_FALLARBOR)                \
    X(MUS_SEALED_CHAMBER)           \
    X(MUS_CONTEST_WINNER)           \
    X(MUS_CONTEST)                  \
    X(MUS_ENCOUNTER_MAGMA)          \
    X(MUS_INTRO_BATTLE)             \
    X(MUS_ABNORMAL_WEATHER)         \
    X(MUS_WEATHER_GROUDON)          \
    X(MUS_SOOTOPOLIS)               \
    X(MUS_CONTEST_RESULTS)          \
    X(MUS_HALL_OF_FAME_ROOM)        \
    X(MUS_TRICK_HOUSE)              \
    X(MUS_ENCOUNTER_TWINS)          \
    X(MUS_ENCOUNTER_ELITE_FOUR)     \
    X(MUS_ENCOUNTER_HIKER)          \
    X(MUS_CONTEST_LOBBY)            \
    X(MUS_ENCOUNTER_INTERVIEWER)    \
    X(MUS_ENCOUNTER_CHAMPION)       \
    X(MUS_CREDITS)                  \
    X(MUS_END)                      \
    X(MUS_B_FRONTIER)               \
    X(MUS_B_ARENA)                  \
    X(MUS_OBTAIN_B_POINTS)          \
    X(MUS_REGISTER_MATCH_CALL)      \
    X(MUS_B_PYRAMID)                \
    X(MUS_B_PYRAMID_TOP)            \
    X(MUS_B_PALACE)                 \
    X(MUS_RAYQUAZA_APPEARS)         \
    X(MUS_B_TOWER)                  \
    X(MUS_OBTAIN_SYMBOL)            \
    X(MUS_B_DOME)                   \
    X(MUS_B_PIKE)                   \
    X(MUS_B_FACTORY)                \
    X(MUS_VS_RAYQUAZA)              \
    X(MUS_VS_FRONTIER_BRAIN)        \
    X(MUS_VS_MEW)                   \
    X(MUS_B_DOME_LOBBY)             \
    X(MUS_VS_WILD)                  \
    X(MUS_VS_AQUA_MAGMA)            \
    X(MUS_VS_TRAINER)               \
    X(MUS_VS_GYM_LEADER)            \
    X(MUS_VS_CHAMPION)              \
    X(MUS_VS_REGI)                  \
    X(MUS_VS_KYOGRE_GROUDON)        \
    X(MUS_VS_RIVAL)                 \
    X(MUS_VS_ELITE_FOUR)            \
    X(MUS_VS_AQUA_MAGMA_LEADER)     \
    X(MUS_RG_FOLLOW_ME)             \
    X(MUS_RG_GAME_CORNER)           \
    X(MUS_RG_ROCKET_HIDEOUT)        \
    X(MUS_RG_GYM)                   \
    X(MUS_RG_JIGGLYPUFF)            \
    X(MUS_RG_INTRO_FIGHT)           \
    X(MUS_RG_TITLE)                 \
    X(MUS_RG_CINNABAR)              \
    X(MUS_RG_LAVENDER)              \
    X(MUS_RG_HEAL)                  \
    X(MUS_RG_CYCLING)               \
    X(MUS_RG_ENCOUNTER_ROCKET)      \
    X(MUS_RG_ENCOUNTER_GIRL)        \
    X(MUS_RG_ENCOUNTER_BOY)         \
    X(MUS_RG_HALL_OF_FAME)          \
    X(MUS_RG_VIRIDIAN_FOREST)       \
    X(MUS_RG_MT_MOON)               \
    X(MUS_RG_POKE_MANSION)          \
    X(MUS_RG_CREDITS)               \
    X(MUS_RG_ROUTE1)                \
    X(MUS_RG_ROUTE24)               \
    X(MUS_RG_ROUTE3)                \
    X(MUS_RG_ROUTE11)               \
    X(MUS_RG_VICTORY_ROAD)          \
    X(MUS_RG_VS_GYM_LEADER)         \
    X(MUS_RG_VS_TRAINER)            \
    X(MUS_RG_VS_WILD)               \
    X(MUS_RG_VS_CHAMPION)           \
    X(MUS_RG_PALLET)                \
    X(MUS_RG_OAK_LAB)               \
    X(MUS_RG_OAK)                   \
    X(MUS_RG_POKE_CENTER)           \
    X(MUS_RG_SS_ANNE)               \
    X(MUS_RG_SURF)                  \
    X(MUS_RG_POKE_TOWER)            \
    X(MUS_RG_SILPH)                 \
    X(MUS_RG_FUCHSIA)               \
    X(MUS_RG_CELADON)               \
    X(MUS_RG_VICTORY_TRAINER)       \
    X(MUS_RG_VICTORY_WILD)          \
    X(MUS_RG_VICTORY_GYM_LEADER)    \
    X(MUS_RG_VERMILLION)            \
    X(MUS_RG_PEWTER)                \
    X(MUS_RG_ENCOUNTER_RIVAL)       \
    X(MUS_RG_RIVAL_EXIT)            \
    X(MUS_RG_DEX_RATING)            \
    X(MUS_RG_OBTAIN_KEY_ITEM)       \
    X(MUS_RG_CAUGHT_INTRO)          \
    X(MUS_RG_PHOTO)                 \
    X(MUS_RG_GAME_FREAK)            \
    X(MUS_RG_CAUGHT)                \
    X(MUS_RG_NEW_GAME_INSTRUCT)     \
    X(MUS_RG_NEW_GAME_INTRO)        \
    X(MUS_RG_NEW_GAME_EXIT)         \
    X(MUS_RG_POKE_JUMP)             \
    X(MUS_RG_UNION_ROOM)            \
    X(MUS_RG_NET_CENTER)            \
    X(MUS_RG_MYSTERY_GIFT)          \
    X(MUS_RG_BERRY_PICK)            \
    X(MUS_RG_SEVII_CAVE)            \
    X(MUS_RG_TEACHY_TV_SHOW)        \
    X(MUS_RG_SEVII_ROUTE)           \
    X(MUS_RG_SEVII_DUNGEON)         \
    X(MUS_RG_SEVII_123)             \
    X(MUS_RG_SEVII_45)              \
    X(MUS_RG_SEVII_67)              \
    X(MUS_RG_POKE_FLUTE)            \
    X(MUS_RG_VS_DEOXYS)             \
    X(MUS_RG_VS_MEWTWO)             \
    X(MUS_RG_VS_LEGEND)             \
    X(MUS_RG_ENCOUNTER_GYM_LEADER)  \
    X(MUS_RG_ENCOUNTER_DEOXYS)      \
    X(MUS_RG_TRAINER_TOWER)         \
    X(MUS_RG_SLOW_PALLET)           \
    X(MUS_RG_TEACHY_TV_MENU)

#define SOUND_LIST_SE               \
    X(SE_USE_ITEM)                  \
    X(SE_PC_LOGIN)                  \
    X(SE_PC_OFF)                    \
    X(SE_PC_ON)                     \
    X(SE_SELECT)                    \
    X(SE_WIN_OPEN)                  \
    X(SE_WALL_HIT)                  \
    X(SE_DOOR)                      \
    X(SE_EXIT)                      \
    X(SE_LEDGE)                     \
    X(SE_BIKE_BELL)                 \
    X(SE_NOT_EFFECTIVE)             \
    X(SE_EFFECTIVE)                 \
    X(SE_SUPER_EFFECTIVE)           \
    X(SE_BALL_OPEN)                 \
    X(SE_FAINT)                     \
    X(SE_FLEE)                      \
    X(SE_SLIDING_DOOR)              \
    X(SE_SHIP)                      \
    X(SE_BANG)                      \
    X(SE_PIN)                       \
    X(SE_BOO)                       \
    X(SE_BALL)                      \
    X(SE_CONTEST_PLACE)             \
    X(SE_A)                         \
    X(SE_I)                         \
    X(SE_U)                         \
    X(SE_E)                         \
    X(SE_O)                         \
    X(SE_N)                         \
    X(SE_SUCCESS)                   \
    X(SE_FAILURE)                   \
    X(SE_EXP)                       \
    X(SE_BIKE_HOP)                  \
    X(SE_SWITCH)                    \
    X(SE_CLICK)                     \
    X(SE_FU_ZAKU)                   \
    X(SE_CONTEST_CONDITION_LOSE)    \
    X(SE_LAVARIDGE_FALL_WARP)       \
    X(SE_ICE_STAIRS)                \
    X(SE_ICE_BREAK)                 \
    X(SE_ICE_CRACK)                 \
    X(SE_FALL)                      \
    X(SE_UNLOCK)                    \
    X(SE_WARP_IN)                   \
    X(SE_WARP_OUT)                  \
    X(SE_REPEL)                     \
    X(SE_ROTATING_GATE)             \
    X(SE_TRUCK_MOVE)                \
    X(SE_TRUCK_STOP)                \
    X(SE_TRUCK_UNLOAD)              \
    X(SE_TRUCK_DOOR)                \
    X(SE_BERRY_BLENDER)             \
    X(SE_CARD)                      \
    X(SE_SAVE)                      \
    X(SE_BALL_BOUNCE_1)             \
    X(SE_BALL_BOUNCE_2)             \
    X(SE_BALL_BOUNCE_3)             \
    X(SE_BALL_BOUNCE_4)             \
    X(SE_BALL_TRADE)                \
    X(SE_BALL_THROW)                \
    X(SE_NOTE_C)                    \
    X(SE_NOTE_D)                    \
    X(SE_NOTE_E)                    \
    X(SE_NOTE_F)                    \
    X(SE_NOTE_G)                    \
    X(SE_NOTE_A)                    \
    X(SE_NOTE_B)                    \
    X(SE_NOTE_C_HIGH)               \
    X(SE_PUDDLE)                    \
    X(SE_BRIDGE_WALK)               \
    X(SE_ITEMFINDER)                \
    X(SE_DING_DONG)                 \
    X(SE_BALLOON_RED)               \
    X(SE_BALLOON_BLUE)              \
    X(SE_BALLOON_YELLOW)            \
    X(SE_BREAKABLE_DOOR)            \
    X(SE_MUD_BALL)                  \
    X(SE_FIELD_POISON)              \
    X(SE_ESCALATOR)                 \
    X(SE_THUNDERSTORM)              \
    X(SE_THUNDERSTORM_STOP)         \
    X(SE_DOWNPOUR)                  \
    X(SE_DOWNPOUR_STOP)             \
    X(SE_RAIN)                      \
    X(SE_RAIN_STOP)                 \
    X(SE_THUNDER)                   \
    X(SE_THUNDER2)                  \
    X(SE_ELEVATOR)                  \
    X(SE_LOW_HEALTH)                \
    X(SE_EXP_MAX)                   \
    X(SE_ROULETTE_BALL)             \
    X(SE_ROULETTE_BALL2)            \
    X(SE_TAILLOW_WING_FLAP)         \
    X(SE_SHOP)                      \
    X(SE_CONTEST_HEART)             \
    X(SE_CONTEST_CURTAIN_RISE)      \
    X(SE_CONTEST_CURTAIN_FALL)      \
    X(SE_CONTEST_ICON_CHANGE)       \
    X(SE_CONTEST_ICON_CLEAR)        \
    X(SE_CONTEST_MONS_TURN)         \
    X(SE_SHINY)                     \
    X(SE_INTRO_BLAST)               \
    X(SE_MUGSHOT)                   \
    X(SE_APPLAUSE)                  \
    X(SE_VEND)                      \
    X(SE_ORB)                       \
    X(SE_DEX_SCROLL)                \
    X(SE_DEX_PAGE)                  \
    X(SE_POKENAV_ON)                \
    X(SE_POKENAV_OFF)               \
    X(SE_DEX_SEARCH)                \
    X(SE_EGG_HATCH)                 \
    X(SE_BALL_TRAY_ENTER)           \
    X(SE_BALL_TRAY_BALL)            \
    X(SE_BALL_TRAY_EXIT)            \
    X(SE_GLASS_FLUTE)               \
    X(SE_M_THUNDERBOLT)             \
    X(SE_M_THUNDERBOLT2)            \
    X(SE_M_HARDEN)                  \
    X(SE_M_NIGHTMARE)               \
    X(SE_M_VITAL_THROW)             \
    X(SE_M_VITAL_THROW2)            \
    X(SE_M_BUBBLE)                  \
    X(SE_M_BUBBLE2)                 \
    X(SE_M_BUBBLE3)                 \
    X(SE_M_RAIN_DANCE)              \
    X(SE_M_CUT)                     \
    X(SE_M_STRING_SHOT)             \
    X(SE_M_STRING_SHOT2)            \
    X(SE_M_ROCK_THROW)              \
    X(SE_M_GUST)                    \
    X(SE_M_GUST2)                   \
    X(SE_M_DOUBLE_SLAP)             \
    X(SE_M_DOUBLE_TEAM)             \
    X(SE_M_RAZOR_WIND)              \
    X(SE_M_ICY_WIND)                \
    X(SE_M_THUNDER_WAVE)            \
    X(SE_M_COMET_PUNCH)             \
    X(SE_M_MEGA_KICK)               \
    X(SE_M_MEGA_KICK2)              \
    X(SE_M_CRABHAMMER)              \
    X(SE_M_JUMP_KICK)               \
    X(SE_M_FLAME_WHEEL)             \
    X(SE_M_FLAME_WHEEL2)            \
    X(SE_M_FLAMETHROWER)            \
    X(SE_M_FIRE_PUNCH)              \
    X(SE_M_TOXIC)                   \
    X(SE_M_SACRED_FIRE)             \
    X(SE_M_SACRED_FIRE2)            \
    X(SE_M_EMBER)                   \
    X(SE_M_TAKE_DOWN)               \
    X(SE_M_BLIZZARD)                \
    X(SE_M_BLIZZARD2)               \
    X(SE_M_SCRATCH)                 \
    X(SE_M_VICEGRIP)                \
    X(SE_M_WING_ATTACK)             \
    X(SE_M_FLY)                     \
    X(SE_M_SAND_ATTACK)             \
    X(SE_M_RAZOR_WIND2)             \
    X(SE_M_BITE)                    \
    X(SE_M_HEADBUTT)                \
    X(SE_M_SURF)                    \
    X(SE_M_HYDRO_PUMP)              \
    X(SE_M_WHIRLPOOL)               \
    X(SE_M_HORN_ATTACK)             \
    X(SE_M_TAIL_WHIP)               \
    X(SE_M_MIST)                    \
    X(SE_M_POISON_POWDER)           \
    X(SE_M_BIND)                    \
    X(SE_M_DRAGON_RAGE)             \
    X(SE_M_SING)                    \
    X(SE_M_PERISH_SONG)             \
    X(SE_M_PAY_DAY)                 \
    X(SE_M_DIG)                     \
    X(SE_M_DIZZY_PUNCH)             \
    X(SE_M_SELF_DESTRUCT)           \
    X(SE_M_EXPLOSION)               \
    X(SE_M_ABSORB_2)                \
    X(SE_M_ABSORB)                  \
    X(SE_M_SCREECH)                 \
    X(SE_M_BUBBLE_BEAM)             \
    X(SE_M_BUBBLE_BEAM2)            \
    X(SE_M_SUPERSONIC)              \
    X(SE_M_BELLY_DRUM)              \
    X(SE_M_METRONOME)               \
    X(SE_M_BONEMERANG)              \
    X(SE_M_LICK)                    \
    X(SE_M_PSYBEAM)                 \
    X(SE_M_FAINT_ATTACK)            \
    X(SE_M_SWORDS_DANCE)            \
    X(SE_M_LEER)                    \
    X(SE_M_SWAGGER)                 \
    X(SE_M_SWAGGER2)                \
    X(SE_M_HEAL_BELL)               \
    X(SE_M_CONFUSE_RAY)             \
    X(SE_M_SNORE)                   \
    X(SE_M_BRICK_BREAK)             \
    X(SE_M_GIGA_DRAIN)              \
    X(SE_M_PSYBEAM2)                \
    X(SE_M_SOLAR_BEAM)              \
    X(SE_M_PETAL_DANCE)             \
    X(SE_M_TELEPORT)                \
    X(SE_M_MINIMIZE)                \
    X(SE_M_SKETCH)                  \
    X(SE_M_SWIFT)                   \
    X(SE_M_REFLECT)                 \
    X(SE_M_BARRIER)                 \
    X(SE_M_DETECT)                  \
    X(SE_M_LOCK_ON)                 \
    X(SE_M_MOONLIGHT)               \
    X(SE_M_CHARM)                   \
    X(SE_M_CHARGE)                  \
    X(SE_M_STRENGTH)                \
    X(SE_M_HYPER_BEAM)              \
    X(SE_M_WATERFALL)               \
    X(SE_M_REVERSAL)                \
    X(SE_M_ACID_ARMOR)              \
    X(SE_M_SANDSTORM)               \
    X(SE_M_TRI_ATTACK)              \
    X(SE_M_TRI_ATTACK2)             \
    X(SE_M_ENCORE)                  \
    X(SE_M_ENCORE2)                 \
    X(SE_M_BATON_PASS)              \
    X(SE_M_MILK_DRINK)              \
    X(SE_M_ATTRACT)                 \
    X(SE_M_ATTRACT2)                \
    X(SE_M_MORNING_SUN)             \
    X(SE_M_FLATTER)                 \
    X(SE_M_SAND_TOMB)               \
    X(SE_M_GRASSWHISTLE)            \
    X(SE_M_SPIT_UP)                 \
    X(SE_M_DIVE)                    \
    X(SE_M_EARTHQUAKE)              \
    X(SE_M_TWISTER)                 \
    X(SE_M_SWEET_SCENT)             \
    X(SE_M_YAWN)                    \
    X(SE_M_SKY_UPPERCUT)            \
    X(SE_M_STAT_INCREASE)           \
    X(SE_M_HEAT_WAVE)               \
    X(SE_M_UPROAR)                  \
    X(SE_M_HAIL)                    \
    X(SE_M_COSMIC_POWER)            \
    X(SE_M_TEETER_DANCE)            \
    X(SE_M_STAT_DECREASE)           \
    X(SE_M_HAZE)                    \
    X(SE_M_HYPER_BEAM2)             \
    X(SE_RG_DOOR)                   \
    X(SE_RG_CARD_FLIP)              \
    X(SE_RG_CARD_FLIPPING)          \
    X(SE_RG_CARD_OPEN)              \
    X(SE_RG_BAG_CURSOR)             \
    X(SE_RG_BAG_POCKET)             \
    X(SE_RG_BALL_CLICK)             \
    X(SE_RG_SHOP)                   \
    X(SE_RG_SS_ANNE_HORN)           \
    X(SE_RG_HELP_OPEN)              \
    X(SE_RG_HELP_CLOSE)             \
    X(SE_RG_HELP_ERROR)             \
    X(SE_RG_DEOXYS_MOVE)            \
    X(SE_RG_POKE_JUMP_SUCCESS)      \
    X(SE_RG_POKE_JUMP_FAILURE)      \
    X(SE_POKENAV_CALL)              \
    X(SE_POKENAV_HANG_UP)           \
    X(SE_ARENA_TIMEUP1)             \
    X(SE_ARENA_TIMEUP2)             \
    X(SE_PIKE_CURTAIN_CLOSE)        \
    X(SE_PIKE_CURTAIN_OPEN)         \
    X(SE_SUDOWOODO_SHAKE)

// Create BGM list
#define X(songId) static const u8 sBGMName_##songId[] = _(#songId);
SOUND_LIST_BGM
#undef X

#define X(songId) [songId - START_MUS] = sBGMName_##songId,
static const u8 *const sBGMNames[END_MUS - START_MUS + 1] =
{
SOUND_LIST_BGM
};
#undef X

// Create SE list
#define X(songId) static const u8 sSEName_##songId[] = _(#songId);
SOUND_LIST_SE
#undef X

#define X(songId) [songId - 1] = sSEName_##songId,
static const u8 *const sSENames[END_SE + 1] =
{
SOUND_LIST_SE
};
#undef X

// *******************************
// Actions BerryFunctions

static void DebugAction_BerryFunctions_ClearAll(u8 taskId)
{
    u8 i;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (gObjectEvents[i].movementType == MOVEMENT_TYPE_BERRY_TREE_GROWTH)
        {
            RemoveBerryTree(GetObjectEventBerryTreeId(i));
            SetBerryTreeJustPicked(gObjectEvents[i].localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
        }
    }

    ScriptContext_Enable();
    Debug_DestroyMenu_Full(taskId);
}

static void DebugAction_BerryFunctions_Ready(u8 taskId)
{
    u8 i;
    struct BerryTree *tree;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (gObjectEvents[i].movementType == MOVEMENT_TYPE_BERRY_TREE_GROWTH)
        {
            tree = &gSaveBlock1Ptr->berryTrees[GetObjectEventBerryTreeId(i)];
            if (tree->stage != BERRY_STAGE_NO_BERRY)
            {
                tree->stage = BERRY_STAGE_BERRIES - 1;
                BerryTreeGrow(tree);
            }
        }
    }

    ScriptContext_Enable();
    Debug_DestroyMenu_Full(taskId);
}

static void DebugAction_BerryFunctions_NextStage(u8 taskId)
{
    u8 i;
    struct BerryTree *tree;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (gObjectEvents[i].movementType == MOVEMENT_TYPE_BERRY_TREE_GROWTH)
        {
            tree = &gSaveBlock1Ptr->berryTrees[GetObjectEventBerryTreeId(i)];
            BerryTreeGrow(tree);
        }
    }

    ScriptContext_Enable();
    Debug_DestroyMenu_Full(taskId);
}

static void DebugAction_BerryFunctions_Pests(u8 taskId)
{
    u8 i;

    if (!OW_BERRY_PESTS)
    {
        Debug_DestroyMenu_Full_Script(taskId, Debug_BerryPestsDisabled);
        return;
    }

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (gObjectEvents[i].movementType == MOVEMENT_TYPE_BERRY_TREE_GROWTH)
        {
            if (gSaveBlock1Ptr->berryTrees[GetObjectEventBerryTreeId(i)].stage != BERRY_STAGE_PLANTED)
                gSaveBlock1Ptr->berryTrees[GetObjectEventBerryTreeId(i)].pests = TRUE;
        }
    }

    ScriptContext_Enable();
    Debug_DestroyMenu_Full(taskId);
}

static void DebugAction_BerryFunctions_Weeds(u8 taskId)
{
    u8 i;

    if (!OW_BERRY_WEEDS)
    {
        Debug_DestroyMenu_Full_Script(taskId, Debug_BerryWeedsDisabled);
        return;
    }

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (gObjectEvents[i].movementType == MOVEMENT_TYPE_BERRY_TREE_GROWTH)
        {
            gSaveBlock1Ptr->berryTrees[GetObjectEventBerryTreeId(i)].weeds = TRUE;
        }
    }

    ScriptContext_Enable();
    Debug_DestroyMenu_Full(taskId);
}

// *******************************
// Actions Party/Boxes

static void DebugAction_Party_HealParty(u8 taskId)
{
    PlaySE(SE_USE_ITEM);
    HealPlayerParty();
    ScriptContext_Enable();
    Debug_DestroyMenu_Full(taskId);
}

void DebugNative_GetAbilityNames(void)
{
    u32 species = GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_SPECIES);
    StringCopy(gStringVar1, gAbilitiesInfo[GetAbilityBySpecies(species, 0)].name);
    StringCopy(gStringVar2, gAbilitiesInfo[GetAbilityBySpecies(species, 1)].name);
    StringCopy(gStringVar3, gAbilitiesInfo[GetAbilityBySpecies(species, 2)].name);
}

#define tPartyId               data[5]
#define tFriendship            data[6]

static void Debug_Display_FriendshipInfo(s32 oldFriendship, s32 newFriendship, u32 digit, u8 windowId)
{
    ConvertIntToDecimalStringN(gStringVar1, oldFriendship, STR_CONV_MODE_LEADING_ZEROS, 3);
    ConvertIntToDecimalStringN(gStringVar2, newFriendship, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringCopy(gStringVar3, gText_DigitIndicator[digit]);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Friendship:\n{STR_VAR_1} {RIGHT_ARROW} {STR_VAR_2}\n\n{STR_VAR_3}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugNativeStep_Party_SetFriendshipSelect(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].tFriendship = gTasks[taskId].tInput;
        SetMonData(&gPlayerParty[gTasks[taskId].tPartyId], MON_DATA_FRIENDSHIP, &gTasks[taskId].tInput);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugNativeStep_CloseDebugWindow(taskId);
        return;
    }

    Debug_HandleInput_Numeric(taskId, 0, 255, 3);

    if (JOY_NEW(DPAD_ANY) || JOY_NEW(A_BUTTON))
        Debug_Display_FriendshipInfo(gTasks[taskId].tFriendship, gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
}

static void DebugNativeStep_Party_SetFriendshipMain(u8 taskId)
{
    u8 windowId = DebugNativeStep_CreateDebugWindow();
    u32 friendship = GetMonData(&gPlayerParty[gTasks[taskId].tPartyId], MON_DATA_FRIENDSHIP);

    // Display initial flag
    Debug_Display_FriendshipInfo(friendship, friendship, 0, windowId);

    gTasks[taskId].func = DebugNativeStep_Party_SetFriendshipSelect;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tFriendship = friendship;
    gTasks[taskId].tInput = friendship;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tPartyId = 0;
}

void DebugNative_Party_SetFriendship(void)
{
    if (gSpecialVar_0x8004 < PARTY_SIZE)
    {
        u32 taskId = CreateTask(DebugNativeStep_Party_SetFriendshipMain, 1);
        gTasks[taskId].tPartyId = gSpecialVar_0x8004;
    }
}

#undef tFriendship

#define tStrain            data[6]

static void Debug_Display_PokerusDaysLeftInfo(s32 daysLeft, s32 strain, u32 digit, u8 windowId)
{
    ConvertIntToDecimalStringN(gStringVar1, daysLeft, STR_CONV_MODE_LEADING_ZEROS, 2);

    if (daysLeft == 0 && strain)
        StringCopy(gStringVar2, COMPOUND_STRING("Inactive"));
    else if (daysLeft == 0)
        StringCopy(gStringVar2, COMPOUND_STRING("No Pokerus"));
    else
        StringCopy(gStringVar2, COMPOUND_STRING(""));
    StringCopy(gStringVar3, gText_DigitIndicator[digit]);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Days Left:\n{STR_VAR_1}\n{STR_VAR_2}{CLEAR_TO 90}\n{STR_VAR_3}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugNativeStep_Party_SetPokerusDaysLeftSelect(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        SetMonData(&gPlayerParty[gTasks[taskId].tPartyId], MON_DATA_POKERUS_DAYS_LEFT, &gTasks[taskId].tInput);
        DebugNativeStep_CloseDebugWindow(taskId);
        return;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugNativeStep_CloseDebugWindow(taskId);
        return;
    }

    Debug_HandleInput_Numeric(taskId, 0, 15, 2);

    if (JOY_NEW(DPAD_ANY) || JOY_NEW(A_BUTTON))
        Debug_Display_PokerusDaysLeftInfo(gTasks[taskId].tInput, gTasks[taskId].tStrain, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
}

static void Debug_Display_PokerusStrainInfo(s32 strain, u32 digit, u8 windowId)
{
    ConvertIntToDecimalStringN(gStringVar1, strain, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringCopy(gStringVar3, gText_DigitIndicator[digit]);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Strain:\n{STR_VAR_1}\n\n{STR_VAR_3}"));
    AddTextPrinterParameterized(windowId, DEBUG_MENU_FONT, gStringVar4, 0, 0, 0, NULL);
}

static void DebugNativeStep_Party_SetPokerusStrainSelect(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].tStrain = gTasks[taskId].tInput;
        SetMonData(&gPlayerParty[gTasks[taskId].tPartyId], MON_DATA_POKERUS_STRAIN, &gTasks[taskId].tInput);
        gTasks[taskId].tInput = GetMonData(&gPlayerParty[gTasks[taskId].tPartyId], MON_DATA_POKERUS_DAYS_LEFT);
        Debug_Display_PokerusDaysLeftInfo(gTasks[taskId].tInput, gTasks[taskId].tStrain, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
        gTasks[taskId].func = DebugNativeStep_Party_SetPokerusDaysLeftSelect;
        return;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DebugNativeStep_CloseDebugWindow(taskId);
        return;
    }

    Debug_HandleInput_Numeric(taskId, 0, 15, 2);

    if (JOY_NEW(DPAD_ANY) || JOY_NEW(A_BUTTON))
        Debug_Display_PokerusStrainInfo(gTasks[taskId].tInput, gTasks[taskId].tDigit, gTasks[taskId].tSubWindowId);
}

static void DebugNativeStep_Party_SetPokerusMain(u8 taskId)
{
    u8 windowId = DebugNativeStep_CreateDebugWindow();
    u32 strain = GetMonData(&gPlayerParty[gTasks[taskId].tPartyId], MON_DATA_POKERUS_STRAIN);

    // Display initial flag
    Debug_Display_PokerusStrainInfo(strain, 0, windowId);

    gTasks[taskId].func = DebugNativeStep_Party_SetPokerusStrainSelect;
    gTasks[taskId].tSubWindowId = windowId;
    gTasks[taskId].tStrain = strain;
    gTasks[taskId].tInput = strain;
    gTasks[taskId].tDigit = 0;
    gTasks[taskId].tPartyId = 0;
}

void DebugNative_Party_SetPokerus(void)
{
    if (gSpecialVar_0x8004 < PARTY_SIZE)
    {
        u32 taskId = CreateTask(DebugNativeStep_Party_SetPokerusMain, 1);
        gTasks[taskId].tPartyId = gSpecialVar_0x8004;
    }
}

#undef tStrain
#undef tPartyId

#undef tMenuTaskId
#undef tWindowId
#undef tSubWindowId
#undef tInput
#undef tDigit

static void DebugAction_Party_ClearPokerus(u8 taskId)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (!GetMonData(&gPlayerParty[i], MON_DATA_SPECIES))
            continue;
        u32 data = 0;
        SetMonData(&gPlayerParty[i], MON_DATA_POKERUS, &data);
    }
    ScriptContext_Enable();
    Debug_DestroyMenu_Full(taskId);
}

static void DebugAction_Party_ClearParty(u8 taskId)
{
    ZeroPlayerPartyMons();
    ScriptContext_Enable();
    Debug_DestroyMenu_Full(taskId);
}

enum DebugTrainerIds
{
    DEBUG_TRAINER_PLAYER,
    DEBUG_TRAINER_AI,
    DEBUG_TRAINERS_COUNT
};

const struct Trainer sDebugTrainers[DIFFICULTY_COUNT][DEBUG_TRAINERS_COUNT] =
{
#include "data/debug_trainers.h"
};

const struct Trainer* GetDebugAiTrainer(void)
{
    return &sDebugTrainers[DIFFICULTY_NORMAL][DEBUG_TRAINER_AI];
}

static void DebugAction_Party_SetParty(u8 taskId)
{
    ZeroPlayerPartyMons();
    CreateNPCTrainerPartyFromTrainer(gPlayerParty, &sDebugTrainers[DIFFICULTY_NORMAL][DEBUG_TRAINER_PLAYER], TRUE, BATTLE_TYPE_TRAINER);
    ScriptContext_Enable();
    Debug_DestroyMenu_Full(taskId);
}

static void DebugAction_Party_BattleSingle(u8 taskId)
{
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    CreateNPCTrainerPartyFromTrainer(gPlayerParty, &sDebugTrainers[DIFFICULTY_NORMAL][DEBUG_TRAINER_PLAYER], TRUE, BATTLE_TYPE_TRAINER);
    CreateNPCTrainerPartyFromTrainer(gEnemyParty, GetDebugAiTrainer(), FALSE, BATTLE_TYPE_TRAINER);

    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    if (sDebugTrainers[DIFFICULTY_NORMAL][DEBUG_TRAINER_AI].battleType == TRAINER_BATTLE_TYPE_DOUBLES)
        gBattleTypeFlags |= BATTLE_TYPE_DOUBLE;
    gDebugAIFlags = sDebugTrainers[DIFFICULTY_NORMAL][DEBUG_TRAINER_AI].aiFlags;
    gIsDebugBattle = TRUE;
    gBattleEnvironment = BattleSetup_GetEnvironmentId();
    CalculateEnemyPartyCount();
    BattleSetup_StartTrainerBattle_Debug();
    Debug_DestroyMenu_Full(taskId);
}

void CheckEWRAMCounters(struct ScriptContext *ctx)
{
    ConvertIntToDecimalStringN(gStringVar1, gFollowerSteps, STR_CONV_MODE_LEFT_ALIGN, 5);
    ConvertIntToDecimalStringN(gStringVar2, gChainFishingDexNavStreak, STR_CONV_MODE_LEFT_ALIGN, 5);
}
