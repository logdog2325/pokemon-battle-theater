#ifndef GUARD_NAMING_SCREEN_H
#define GUARD_NAMING_SCREEN_H

#include "main.h"

enum {
    NAMING_SCREEN_PLAYER,
    NAMING_SCREEN_BOX,
    NAMING_SCREEN_CAUGHT_MON,
    NAMING_SCREEN_NICKNAME,
    NAMING_SCREEN_WALDA,
    NAMING_SCREEN_CODE,
    NAMING_SCREEN_RIVAL,
    // v1.3 — Pokemon Battle Theater team-code import. 30-char buffer instead
    // of the vanilla 7-12 chars; uses Ditto icon for thematic "transform-into-
    // a-Pokemon-via-code" flavor.
    NAMING_SCREEN_TEAMCODE,
};

extern void BattleMainCB2(void);

void DoNamingScreen(u8 templateNum, u8 *destBuffer, u16 monSpecies, u16 monGender, u32 monPersonality, MainCallback returnCallback);

#endif // GUARD_NAMING_SCREEN_H
