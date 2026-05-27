#ifndef GUARD_SIM_TEAM_CODE_H
#define GUARD_SIM_TEAM_CODE_H

// =============================================================================
// v1.2 — Team code decoder (Showdown import).
// =============================================================================
//
// Decodes a "PB<base64>" string emitted by tools/team-codes/encoder.html
// into a SimCustomTrainerMon. One code per Pokemon; the user types 6 codes
// to import a full team. See tools/team-codes/SPEC.md for the v2 format
// (bit-packed payload, URL-safe base64, XOR checksum byte).

#include "global.h"

enum SimTeamCodeResult
{
    SIM_CODE_OK = 0,            // mon populated
    SIM_CODE_BAD_PREFIX,        // missing "PB" magic
    SIM_CODE_BAD_CHAR,          // non-base64 char in payload
    SIM_CODE_TOO_SHORT,         // payload too short to even hold the bit-stream header
    SIM_CODE_BAD_CHECKSUM,      // XOR didn't match
    SIM_CODE_BAD_VERSION,       // version > supported (user needs newer ROM)
    SIM_CODE_BAD_FIELD,         // species/nature/level/etc. out of range — code corrupt
};

// Decode `code` (null-terminated, ASCII) into `out`. On success returns
// SIM_CODE_OK and *out is fully populated (species, item, ability slot,
// moves, EVs, IVs, nature, gender, level, shiny). On failure *out is
// left untouched.
//
// Caller is responsible for committing *out to the saveblock (e.g. via
// BuildTrainer_CommitWorkBufferToSaveblock after copying into the work
// buffer).
enum SimTeamCodeResult Sim_DecodeTeamCode(const u8 *code, struct SimCustomTrainerMon *out);

// Human-readable error label for the status line on import failure.
// Returns a static const string (no need to free).
const u8 *Sim_TeamCodeResultLabel(enum SimTeamCodeResult result);

#endif // GUARD_SIM_TEAM_CODE_H
