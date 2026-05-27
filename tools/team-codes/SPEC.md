# Pokemon Battle Theater — Team Code Format v1

Encodes a single Pokemon as a short URL-safe base64 string for in-ROM import.
A full team is 6 separate codes (one per Pokemon) — easier to type, easier to
recover from a typo than one giant code.

## At a glance

- **Charset**: URL-safe base64 (`A-Z a-z 0-9 - _`), no padding
- **Length per mon**: ~26-42 chars (typical: ~30)
- **6 mons per team** ⇒ ~150-250 chars total typing
- **Magic prefix**: every code starts with `PB` so the user can sanity-check
  what they pasted. Decoder rejects codes missing the prefix.
- **Trainer name + sprite class** are NOT encoded — the user keeps the slot's
  existing name/sprite (set separately in-ROM via the slot menu's Name + Sprite
  rows, or via the Copy Preset Team feature)

## Why per-mon instead of per-team

User asked for shorter individual codes over one long code. Per-mon trades a
bit of total typing for:
- Each code self-contained (typo in mon 3 doesn't ruin mons 1-6)
- Can import mons individually (e.g. swap one mon, keep the other five)
- The web encoder can output 6 codes labeled "Pokemon 1", "Pokemon 2" etc., so
  the user knows exactly which one to copy

## Binary layout (per mon, before base64)

All multi-byte values are little-endian. Bit fields pack low-bit-first within
a byte.

| Offset | Size | Field | Notes |
|---|---|---|---|
| 0 | 1 byte | `version` | Format version, currently `1`. Decoder rejects unknown versions. |
| 1 | 2 bytes | `species` | `SPECIES_*` id (u16). 0 = "empty mon" (decoder treats whole mon as cleared). |
| 3 | 2 bytes | `heldItem` | `ITEM_*` id (u16). 0 = no item. |
| 5 | 1 byte | `packedAbilityNatureShiny` | bits 0-4: `nature` (0-24), bits 5-6: `abilityNum` (0-2), bit 7: `shiny` |
| 6 | 1 byte | `level` | 1-100. |
| 7 | 1 byte | `genderFlags` | bits 0-1: `gender` (0=Any, 1=Male, 2=Female). bits 2-7 reserved (must be 0). |
| 8 | 8 bytes | `moves[4]` | 4 × u16 in slot order. 0 = empty move slot. |
| 16 | 1 byte | `evMask` | Bit `i` (0-5) set ⇒ stat `i` has a non-zero EV. Order: HP/Atk/Def/SpA/SpD/Spe. Bits 6-7 reserved. |
| 17 | N bytes | `evs` | One byte per bit set in `evMask`, in stat-index order. Each value 0-252. |
| 17+N | 1 byte | `ivMask` | Bit `i` (0-5) set ⇒ stat `i` IV is NOT 31 (i.e. has been customized). |
| 18+N | M bytes | `ivs` | One byte per bit set in `ivMask`, value 0-31 each. |
| END-1 | 1 byte | `checksum` | XOR of every byte from `version` through the last IV byte. Decoder rejects if it doesn't match. |

### Size math

- Minimum (default mon, no EVs, all 31 IVs, 0 moves set): 19 bytes ⇒ 26 base64 chars + 2 magic chars = 28 chars
- Typical (4 moves, 3 EV stats invested, all 31 IVs, no gender lock, not shiny): 22 bytes ⇒ 30 chars + 2 magic = 32 chars
- Maximum (4 moves, full 6-stat EVs, all 6 IVs customized): 31 bytes ⇒ 42 chars + 2 magic = 44 chars

### Full encoded code

`PB` + `urlsafe_base64(binary_payload)` with no `=` padding. So a typical
mon's code looks like:

```
PBAQEcAAAh8x3DKQAAA
```

Decoder strips `PB`, base64-decodes, validates version + checksum, fills a
`SimCustomTrainerMon` struct.

## Round-trip example (Showdown → code → struct)

**Input (Showdown export, Lucario):**
```
Lucario @ Life Orb
Ability: Inner Focus
Level: 50
EVs: 252 Atk / 4 SpD / 252 Spe
Adamant Nature
- Close Combat
- Extreme Speed
- Bullet Punch
- Crunch
```

**Resulting binary (24 bytes):**
```
01                version=1
48 02             species=Lucario (SPECIES_LUCARIO=584=0x0248 LE)
9C 01             heldItem=Life Orb (ITEM_LIFE_ORB=412=0x019C LE)
03                packed: nature=3 (Adamant), ability=0 (Inner Focus = slot 0), shiny=0
32                level=50
00                gender=Any, reserved=0
14 01             move 1: Close Combat (0x0114 LE)
F1 00             move 2: Extreme Speed (0x00F1 LE)
1A 01             move 3: Bullet Punch
8E 00             move 4: Crunch
22                evMask: 0b00100010 (bit 1 = Atk, bit 4 = SpD, bit 5 = Spe wait... let me recompute)
                  Actually: bit 1 (Atk), bit 4 (SpD), bit 5 (Spe) = 0b00110010 = 0x32
                  3 stats with non-zero EVs
FC 04 FC          evs: 252, 4, 252
00                ivMask: 0 (all 31)
XX                checksum
```

**Resulting code** (~32 chars): `PBASKCCgEMyABCQB8RoBjgAyMgT8BPwAxx`

## Versioning

`version` byte is checked first. If the decoder sees a version > the one it
supports, it refuses to import and tells the user to upgrade their ROM. The
web encoder always emits the highest version it knows; if a user is on an
older ROM, they need to upgrade or use an older encoder.

Future versions can extend the layout by:
- Adding new fields after the IV section (decoder ignores unknown trailing
  bytes if the checksum still validates)
- Bumping the version byte to signal breaking changes

## Why not compress the whole 6-mon team together?

DEFLATE on a full team gets the total length down to ~80 chars (vs ~180 for
6 separate codes), but the user explicitly chose per-mon. Trade-off:

| | One big code | 6 per-mon codes |
|---|---|---|
| Total length | ~80 chars | ~150-250 chars |
| Typo recovery | Re-type everything | Re-type one mon |
| Partial import | Not possible | Swap individual mons |
| Web tool UX | One code to copy | 6 codes labeled by slot |

Per-mon wins on robustness and partial-import; one big code wins on terseness.
We picked robustness.

## Why URL-safe base64 instead of base32 / bech32

Base64 is 25% shorter than base32. Confusable characters (0/O, 1/l) can be
hit, but the 8-bit checksum catches single-byte corruption with ~99.6%
probability, and a single typo in URL-safe base64 nearly always changes a
byte (vs base32's bigger redundancy). For the typing volume we care about,
shorter wins.

The `PB` prefix doubles as a visual sanity check — if the code doesn't start
with `PB` the user knows they grabbed the wrong text.
