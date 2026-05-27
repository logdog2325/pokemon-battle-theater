// =============================================================================
// v1.2 — Team code decoder. See include/sim_team_code.h + tools/team-codes/SPEC.md
// =============================================================================
//
// Mirrors tools/team-codes/decode.py. Format v2 only — v1 byte-aligned codes
// from pre-release builds are not supported.
//
// Flow:
//   1. Verify "PB" prefix and strip it
//   2. Base64-decode the rest (URL-safe charset, no padding)
//   3. Verify checksum (last byte = XOR of preceding)
//   4. Read the bit-stream into a SimCustomTrainerMon
//   5. Sanity-check field ranges before signalling SIM_CODE_OK
//
// Bit-stream reader is MSB-first within each byte: the first bit of the
// payload is bit 7 of byte 0. Same as the encoder.

#include "global.h"
#include "string_util.h"
#include "sim_team_code.h"
#include "constants/pokemon.h"          // NUM_NATURES
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/species.h"

// Format version the decoder understands. Bumped in lockstep with the encoder.
#define SIM_TEAM_CODE_VERSION   2

// Magic prefix that gates a code as belonging to this format. Two bytes so
// even a typo on the first char is caught immediately.
#define SIM_TEAM_CODE_MAGIC_0   'P'
#define SIM_TEAM_CODE_MAGIC_1   'B'

// Worst-case bit-stream length (v2): 188 bits = 24 bytes + 1 checksum = 25.
// Round up to 32 for slack on potential future-version extensions.
#define MAX_PAYLOAD_BYTES       32
// Minimum: 60-bit bit stream = 8 bytes + 1 checksum = 9 bytes.
#define MIN_PAYLOAD_BYTES       9

// Minimum encoded code length: "PB" + ceil(MIN_PAYLOAD_BYTES * 4 / 3) chars.
// 9 bytes base64-encodes to 12 chars; with the prefix that's 14 chars. The
// decoder rejects anything shorter.
#define MIN_CODE_LENGTH         14

// =============================================================================
// URL-safe base64 char → 6-bit value lookup. Returns 0xFF on invalid char.
// =============================================================================
static u8 Base64DecodeChar(u8 c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '-')             return 62;
    if (c == '_')             return 63;
    return 0xFF;
}

// Decode an ASCII base64 string into raw bytes. Returns number of bytes
// written, or -1 on a bad char. Caller provides a buffer of size MAX_PAYLOAD_BYTES.
static s32 Base64Decode(const u8 *src, u32 srcLen, u8 *dst)
{
    u32 acc = 0;
    u32 accBits = 0;
    s32 outLen = 0;
    for (u32 i = 0; i < srcLen; i++)
    {
        u8 v = Base64DecodeChar(src[i]);
        if (v == 0xFF)
            return -1;
        acc = (acc << 6) | v;
        accBits += 6;
        if (accBits >= 8)
        {
            accBits -= 8;
            if (outLen >= MAX_PAYLOAD_BYTES)
                return -1;   // payload won't fit — code is too long for v2
            dst[outLen++] = (acc >> accBits) & 0xFF;
        }
    }
    // Remaining bits in `acc` (< 8) are zero-padding from the encoder; OK to drop.
    return outLen;
}

// =============================================================================
// Bit-stream reader (MSB-first within each byte).
// =============================================================================
struct BitReader
{
    const u8 *data;
    u32 bitPos;
    u32 totalBits;
    bool32 overflow;  // set if a read went past the end of the stream
};

static void BitReader_Init(struct BitReader *br, const u8 *data, u32 byteLen)
{
    br->data = data;
    br->bitPos = 0;
    br->totalBits = byteLen * 8;
    br->overflow = FALSE;
}

static u32 BitReader_Read(struct BitReader *br, u32 nBits)
{
    u32 value = 0;
    for (u32 i = 0; i < nBits; i++)
    {
        if (br->bitPos >= br->totalBits)
        {
            br->overflow = TRUE;
            return 0;
        }
        u32 byteIdx = br->bitPos >> 3;
        u32 bitIdx = 7 - (br->bitPos & 7);
        u32 bit = (br->data[byteIdx] >> bitIdx) & 1;
        value = (value << 1) | bit;
        br->bitPos++;
    }
    return value;
}

// =============================================================================
// Main entry — decode a code into a SimCustomTrainerMon.
// =============================================================================
enum SimTeamCodeResult Sim_DecodeTeamCode(const u8 *code, struct SimCustomTrainerMon *out)
{
    if (code == NULL || out == NULL)
        return SIM_CODE_BAD_PREFIX;

    // Verify magic prefix.
    if (code[0] != SIM_TEAM_CODE_MAGIC_0 || code[1] != SIM_TEAM_CODE_MAGIC_1)
        return SIM_CODE_BAD_PREFIX;

    // Measure the base64 body (everything after "PB").
    u32 srcLen = 0;
    while (code[2 + srcLen] != '\0' && srcLen < 64)
        srcLen++;
    if (srcLen + 2 < MIN_CODE_LENGTH)
        return SIM_CODE_TOO_SHORT;

    // Base64-decode.
    u8 payload[MAX_PAYLOAD_BYTES];
    s32 payloadLen = Base64Decode(&code[2], srcLen, payload);
    if (payloadLen < 0)
        return SIM_CODE_BAD_CHAR;
    if (payloadLen < MIN_PAYLOAD_BYTES)
        return SIM_CODE_TOO_SHORT;

    // Verify checksum: XOR of every body byte (including the trailing zero-
    // pad) should equal the last byte.
    u8 checksum = 0;
    for (s32 i = 0; i < payloadLen - 1; i++)
        checksum ^= payload[i];
    if (checksum != payload[payloadLen - 1])
        return SIM_CODE_BAD_CHECKSUM;

    // Read the bit stream. The checksum byte itself is excluded.
    struct BitReader br;
    BitReader_Init(&br, payload, payloadLen - 1);

    u32 version = BitReader_Read(&br, 4);
    if (version > SIM_TEAM_CODE_VERSION)
        return SIM_CODE_BAD_VERSION;

    // Read into temporary scratch so we don't half-populate *out on a later
    // field validation failure.
    struct SimCustomTrainerMon m;
    memset(&m, 0, sizeof(m));

    m.species   = BitReader_Read(&br, 11);
    m.heldItem  = BitReader_Read(&br, 10);
    m.nature    = BitReader_Read(&br, 5);
    m.abilityNum= BitReader_Read(&br, 2);
    m.shiny     = BitReader_Read(&br, 1);
    m.level     = BitReader_Read(&br, 7);
    m.gender    = BitReader_Read(&br, 2);

    // Move count + moves
    u32 moveCount = BitReader_Read(&br, 3);
    if (moveCount > 4)
        return SIM_CODE_BAD_FIELD;
    for (u32 i = 0; i < 4; i++)
        m.moves[i] = (i < moveCount) ? BitReader_Read(&br, 11) : MOVE_NONE;

    // EVs: 6-bit mask + 8-bit values for each set bit
    u32 evMask = BitReader_Read(&br, 6);
    for (u32 i = 0; i < 6; i++)
    {
        if (evMask & (1 << i))
        {
            u32 v = BitReader_Read(&br, 8);
            if (v > 252)
                v = 252;
            m.evs[i] = v;
        }
        else
        {
            m.evs[i] = 0;
        }
    }

    // IVs: 1-bit "has deviations" flag. If 0, all IVs are 31.
    u32 ivHasDeviations = BitReader_Read(&br, 1);
    if (ivHasDeviations)
    {
        u32 ivMask = BitReader_Read(&br, 6);
        for (u32 i = 0; i < 6; i++)
        {
            if (ivMask & (1 << i))
            {
                u32 v = BitReader_Read(&br, 5);
                if (v > 31) v = 31;
                m.ivs[i] = v;
            }
            else
            {
                m.ivs[i] = 31;
            }
        }
    }
    else
    {
        for (u32 i = 0; i < 6; i++)
            m.ivs[i] = 31;
    }

    if (br.overflow)
        return SIM_CODE_TOO_SHORT;

    // Field sanity checks. Sim_GetCustomTrainerStruct + the engine handle most
    // invalid IDs gracefully, but reject obvious garbage so the user gets a
    // clear error instead of a Magikarp at level 0 in their slot.
    if (m.species == 0 || m.species >= NUM_SPECIES)
        return SIM_CODE_BAD_FIELD;
    if (m.heldItem >= ITEMS_COUNT)
        return SIM_CODE_BAD_FIELD;
    if (m.nature >= NUM_NATURES)
        return SIM_CODE_BAD_FIELD;
    if (m.abilityNum > 2)
        return SIM_CODE_BAD_FIELD;
    if (m.level == 0 || m.level > 100)
        return SIM_CODE_BAD_FIELD;
    if (m.gender > 2)
        return SIM_CODE_BAD_FIELD;
    for (u32 i = 0; i < 4; i++)
    {
        if (m.moves[i] >= MOVES_COUNT_DYNAMAX)
            return SIM_CODE_BAD_FIELD;
    }

    // All good — commit to caller's struct.
    *out = m;
    return SIM_CODE_OK;
}

// =============================================================================
// Status labels (short — must fit in one window text line, ~20 chars).
// =============================================================================
static const u8 sLabelOk[]            = _("Import OK!");
static const u8 sLabelBadPrefix[]     = _("Missing PB prefix");
static const u8 sLabelBadChar[]       = _("Bad char in code");
static const u8 sLabelTooShort[]      = _("Code too short");
static const u8 sLabelBadChecksum[]   = _("Code corrupt (typo?)");
static const u8 sLabelBadVersion[]    = _("Need newer ROM");
static const u8 sLabelBadField[]      = _("Code data invalid");

const u8 *Sim_TeamCodeResultLabel(enum SimTeamCodeResult result)
{
    switch (result)
    {
    case SIM_CODE_OK:           return sLabelOk;
    case SIM_CODE_BAD_PREFIX:   return sLabelBadPrefix;
    case SIM_CODE_BAD_CHAR:     return sLabelBadChar;
    case SIM_CODE_TOO_SHORT:    return sLabelTooShort;
    case SIM_CODE_BAD_CHECKSUM: return sLabelBadChecksum;
    case SIM_CODE_BAD_VERSION:  return sLabelBadVersion;
    case SIM_CODE_BAD_FIELD:    return sLabelBadField;
    }
    return sLabelBadField;
}
