# Pokemon Battle Theater — Team Code Format v2

Encodes a single Pokemon as a short URL-safe base64 string for in-ROM import.
A full team is 6 separate codes (one per Pokemon) — easier to type, easier to
recover from a typo than one giant code.

## At a glance

- **Charset**: URL-safe base64 (`A-Z a-z 0-9 - _`), no padding
- **Length per mon**: 13-34 chars (typical: ~24)
- **6 mons per team** ⇒ ~120-200 chars total typing
- **Magic prefix**: every code starts with `PB` so the user can sanity-check
  what they pasted. Decoder rejects codes missing the prefix.
- **Trainer name + sprite class** are NOT encoded — the user keeps the slot's
  existing name/sprite (set separately in-ROM via the slot menu's Name + Sprite
  rows, or via the Copy Preset Team feature)

## Bit-stream layout

All fields are written as a continuous bit stream, MSB-first within each byte
(the first bit of the payload is bit 7 of byte 0, then bit 6, ..., bit 0 of
byte 0, then bit 7 of byte 1, etc.). The final byte is zero-padded.

| Bits | Field | Notes |
|---|---|---|
| 4 | `version` | Format version, currently `2`. Decoder rejects unknown versions. |
| 11 | `species` | `SPECIES_*` id (0-2047). 0 = "empty mon" (decoder clears the whole slot). |
| 10 | `heldItem` | `ITEM_*` id (0-1023). 0 = no item. |
| 5 | `nature` | 0-24 (Hardy through Quirky). |
| 2 | `abilitySlot` | 0 / 1 / 2 — primary / secondary / hidden. |
| 1 | `shiny` | 0 = not shiny, 1 = shiny. |
| 7 | `level` | 1-100. |
| 2 | `gender` | 0 = Any, 1 = Male, 2 = Female. |
| 3 | `moveCount` | Number of moves filled, 0-4. |
| 11×`moveCount` | `moves[]` | Each `MOVE_*` id (0-2047). Slot order matters. |
| 6 | `evMask` | Bit `i` (0-5) set ⇒ stat `i` has a non-zero EV. Order: HP/Atk/Def/SpA/SpD/Spe. |
| 8×popcount(evMask) | `evValues[]` | One byte per bit set in `evMask`, in stat-index order. 0-252 each. |
| 1 | `ivHasDeviations` | 0 = every IV is 31 (skip the next two sections). 1 = at least one IV is not 31. |
| 6 (if above is 1) | `ivMask` | Bit `i` (0-5) set ⇒ stat `i` IV is NOT 31. |
| 5×popcount(ivMask) | `ivValues[]` | One 5-bit value per bit set in `ivMask`, 0-31 each. |
| 8 | `checksum` | XOR of every payload byte (the byte-aligned representation of the bit stream, **including the trailing zero-pad** in the last byte) BEFORE the checksum byte itself. |

The checksum byte is appended to the payload AFTER the bit-stream has been
flushed to byte boundary. So the final encoded byte sequence is
`[bitstream_bytes...] [checksum]`.

### Size table

Minimum (default mon, 0 moves, no EV investment, all 31 IVs):
```
4 + 11 + 10 + 5 + 2 + 1 + 7 + 2 + 3 + 0 + 6 + 0 + 1 + 8 = 60 bits = 8 bytes
8 bytes → 11 base64 chars + 2 magic = 13 chars
```

Typical (4 moves, 3 EV stats invested, all 31 IVs):
```
4 + 11 + 10 + 5 + 2 + 1 + 7 + 2 + 3 + 44 + 6 + 24 + 1 + 8 = 128 bits = 16 bytes
16 bytes → 22 base64 chars + 2 magic = 24 chars
```

Maximum (4 moves, every EV stat non-zero, every IV stat deviating from 31):
```
4 + 11 + 10 + 5 + 2 + 1 + 7 + 2 + 3 + 44 + 6 + 48 + 1 + 6 + 30 + 8 = 188 bits = 24 bytes
24 bytes → 32 base64 chars + 2 magic = 34 chars
```

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

**Bit-stream (little-endian visualization):**
```
version=2             | 4 bits  | 0010
species=448 (Lucario) | 11 bits | 001 1100 0000
heldItem=479 (LO)     | 10 bits | 01 1101 1111
nature=3 (Adamant)    | 5 bits  | 0 0011
abilitySlot=0         | 2 bits  | 00
shiny=0               | 1 bit   | 0
level=50              | 7 bits  | 011 0010
gender=0 (Any)        | 2 bits  | 00
moveCount=4           | 3 bits  | 100
moves[0]=276 CCom     | 11 bits | 000 1000 1110 (wait — verify against actual ID at encode time)
moves[1]=241 ExSp     | 11 bits | 000 0111 1001
moves[2]=282 BulPun   | 11 bits | 000 1000 1110
moves[3]=242 Crunch   | 11 bits | 000 0111 1010
evMask=0b110010       | 6 bits  | 11 0010
evValues=[252,4,252]  | 24 bits | 1111 1100 / 0000 0100 / 1111 1100
ivHasDeviations=0     | 1 bit   | 0
checksum (xor)        | 8 bits  | ...
```

Total ~140 bits = 18 bytes ⇒ 24 base64 chars ⇒ `PB` + 24 = 26-char code.

## Versioning

`version` field is checked first (top 4 bits of the bit stream). Decoder
refuses to import if `version > supported` and tells the user to upgrade
their ROM. The web encoder always emits the highest version it knows.

Future versions can extend the layout. Old codes from v1 (byte-aligned format
with version=1) are NOT supported by v2 decoders — the format is a hard
break.

## Why not compress the whole 6-mon team together?

DEFLATE on a full team gets the total length down to ~70 chars (vs ~140 for
6 separate codes), but the user explicitly chose per-mon. Trade-off:

| | One big code | 6 per-mon codes |
|---|---|---|
| Total length | ~70 chars | ~140 chars |
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
