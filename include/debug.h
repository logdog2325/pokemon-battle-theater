#ifndef GUARD_DEBUG_H
#define GUARD_DEBUG_H

void Debug_ShowMainMenu(void);
void Debug_ShowTrainersSubMenu(void);
// v0.52: opens the two-option wrapper (Build Trainer / Run Simulation).
// Used as the boot auto-open entry point so the user lands on the wrapper
// instead of going straight to the Battle Theater picker.
void Debug_ShowTrainersWrapper(void);
extern const u8 Debug_FlagsAndVarNotSetBattleConfigMessage[];
const u8 *GetWeatherName(u32 weatherId);
const struct Trainer* GetDebugAiTrainer(void);

void DebugNative_GetAbilityNames(void);
void DebugNative_Party_SetFriendship(void);

extern EWRAM_DATA bool8 gIsDebugBattle;
extern EWRAM_DATA u64 gDebugAIFlags;
// v0.51 Custom Trainer slots — synthesizes a struct Trainer from SaveBlock3
// data for TRAINER_SIM_CUSTOM_1/2/3 IDs. Returns NULL if slot is out of
// range. Used by GetTrainerStructFromId in include/data.h to inject custom
// trainers without touching the compiled gTrainers[] table.
struct Trainer;
const struct Trainer *Sim_GetCustomTrainerStruct(u16 trainerId);

extern EWRAM_DATA bool8 gSimAutoOpenPending;
// v0.52.15 — TRUE while a sim battle is running in pilot mode (player slot
// is human-controlled instead of AI). The level cap in battle_main.c is
// gated on B_FLAG_AI_VS_AI_BATTLE, but pilot mode skips that flag — so the
// cap silently fails in pilot battles. Set by Sim_SetupMatchRound, cleared
// by CB2_EndDebugBattle.
extern EWRAM_DATA bool8 gSimPilotMode;
// v0.52.5 — set by DebugAction_BuildTrainer_EditName before invoking
// DoNamingScreen. The naming screen's returnCallback drops the player back
// on the field; field_control_avatar's input poll sees this flag and
// re-enters the Build Trainer slot menu (preserving sBuildTrainerActiveSlot
// across the round-trip).
extern EWRAM_DATA bool8 gSimBuildTrainerReopenSlot;
void Debug_ReopenBuildTrainerSlotMenu(void);
extern EWRAM_DATA s16 gSimLevelCap;
extern EWRAM_DATA u8 gSimBestOf;
extern EWRAM_DATA u8 gSimT1Wins;
extern EWRAM_DATA u8 gSimT2Wins;
extern EWRAM_DATA bool8 gSimVGCMode;
extern EWRAM_DATA u8 gSimTournamentCup;
extern EWRAM_DATA u8 gSimTournamentRound;
extern EWRAM_DATA u8 gSimTournamentMaxRounds;
extern EWRAM_DATA bool8 gSimTournamentDone;
extern EWRAM_DATA bool8 gSimTournamentEliminated;

// Battle Simulator: team-preview pick override consumed by
// CreateNPCTrainerPartyFromTrainer in src/battle_main.c. Returns TRUE and copies
// up to maxIndices into outIndices when a pre-computed pick row is available for
// the current trainer-build call; otherwise leaves the engine on its default
// (or ace-priority) selection path.
bool32 Sim_ConsumeNextPickRow(u8 *outIndices, u8 maxIndices);

// Battle Simulator: pick a dramatic battle track based on the highest-tier
// trainer in the current matchup (Champion / E4 / Gym / default). Called from
// GetBattleBGM in src/pokemon.c when an AI-vs-AI sim battle is active.
u16 Sim_GetBattleMusic(void);

// Battle Simulator: snapshot the just-played round's team-preview picks so the
// next round can adaptively counter them. Called from CB2_EndDebugBattle in
// src/battle_setup.c when a Best-Of or Tournament match still has more rounds.
void Sim_SnapshotPicksForNextRound(void);

// Battle Simulator: record the result of the player's just-finished tournament
// match (TRUE = player won) into the bracket and either advance the round or
// mark the tournament as eliminated. Handles SF lazy-simulation as the player
// moves to the final. Called from CB2_EndDebugBattle.
void Sim_AdvanceTournamentAfterMatch(bool32 playerWon);

// v1.1 — Player name override for sim battles. Swap gSaveBlock2Ptr->playerName
// with the player AI trainer's name (Logan / Cynthia / etc.) so battle
// dialogue reads correctly. Pilot mode keeps the user's name but patches
// obviously-empty defaults. Call Override at sim battle start, Restore at end.
void Sim_OverridePlayerName(u16 playerSideId, bool32 pilotMode);
void Sim_RestorePlayerName(void);

#endif // GUARD_DEBUG_H
