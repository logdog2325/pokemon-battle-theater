#!/usr/bin/env python3
"""
Pokemon Battle Theater — team code encoder (Python reference impl).

Takes a Showdown-format text export and emits 6 URL-safe base64 codes (one
per Pokemon) for in-ROM import. See SPEC.md for the binary format.

Usage:
    python3 encode.py <showdown-export.txt>

The web encoder (tools/team-codes/encoder.html) does the same thing in JS so
users don't need Python. This Python script is the reference impl for
round-trip testing and CI.
"""

from __future__ import annotations
import base64
import dataclasses
import pathlib
import re
import sys

REPO_ROOT = pathlib.Path(__file__).resolve().parents[2]

# Format version emitted. Decoder bumps version when it sees this byte.
# v2 = bit-packed payload (saves ~25% vs v1's byte-aligned format).
FORMAT_VERSION = 2


# ---------------------------------------------------------------------------
# Bit stream — MSB-first writer / reader. Used by the v2 encoder for compact
# field packing. The first bit of the payload is bit 7 of byte 0.
# ---------------------------------------------------------------------------

class BitWriter:
    def __init__(self):
        self.buf = bytearray()
        self.cur = 0   # accumulating byte
        self.cur_bits = 0  # bits already written into self.cur (high-to-low)

    def write(self, value: int, n_bits: int) -> None:
        if value < 0 or value >= (1 << n_bits):
            raise ValueError(f"Value {value} doesn't fit in {n_bits} bits")
        # Write high bits of `value` first
        for i in range(n_bits - 1, -1, -1):
            bit = (value >> i) & 1
            self.cur = (self.cur << 1) | bit
            self.cur_bits += 1
            if self.cur_bits == 8:
                self.buf.append(self.cur)
                self.cur = 0
                self.cur_bits = 0

    def finish(self) -> bytes:
        if self.cur_bits:
            self.cur <<= (8 - self.cur_bits)  # zero-pad the last byte
            self.buf.append(self.cur)
            self.cur = 0
            self.cur_bits = 0
        return bytes(self.buf)


class BitReader:
    def __init__(self, data: bytes):
        self.data = data
        self.pos = 0  # bit position (0 = bit 7 of byte 0)

    def read(self, n_bits: int) -> int:
        value = 0
        for _ in range(n_bits):
            byte_idx = self.pos // 8
            bit_idx = 7 - (self.pos % 8)
            if byte_idx >= len(self.data):
                raise ValueError("Bit stream underflow")
            bit = (self.data[byte_idx] >> bit_idx) & 1
            value = (value << 1) | bit
            self.pos += 1
        return value

# Stat index order, matches SimCustomTrainerMon.evs[] / ivs[] layout
# (HP / Atk / Def / SpA / SpD / Spe). See include/global.h.
STAT_ORDER = ["HP", "Atk", "Def", "SpA", "SpD", "Spe"]
STAT_ALIASES = {
    "hp": 0, "atk": 1, "attack": 1, "def": 2, "defense": 2,
    "spa": 3, "sp.atk": 3, "spatk": 3, "specialattack": 3, "special attack": 3,
    "spd": 4, "sp.def": 4, "spdef": 4, "specialdefense": 4, "special defense": 4,
    "spe": 5, "spd ": 4, "speed": 5,
}

NATURES = [
    "Hardy", "Lonely", "Brave", "Adamant", "Naughty",
    "Bold", "Docile", "Relaxed", "Impish", "Lax",
    "Timid", "Hasty", "Serious", "Jolly", "Naive",
    "Modest", "Mild", "Quiet", "Bashful", "Rash",
    "Calm", "Gentle", "Sassy", "Careful", "Quirky",
]
NATURE_INDEX = {n.lower(): i for i, n in enumerate(NATURES)}

GENDER_MAP = {"any": 0, "": 0, "male": 1, "m": 1, "female": 2, "f": 2}


# ---------------------------------------------------------------------------
# Constant table parsing — pulls SPECIES_* / ITEM_* / MOVE_* / ABILITY_* from
# the pokeemerald-expansion headers so we don't have to hardcode ~3000 IDs.
# ---------------------------------------------------------------------------

_DEFINE_RE = re.compile(r"^\s*#define\s+(\w+)\s+(\w+|\(.*?\)|0x[0-9A-Fa-f]+)")


def _load_defines(path: pathlib.Path, prefix: str) -> dict[str, int]:
    """Parse `#define PREFIX_FOO 123` lines from a C header."""
    out: dict[str, int] = {}
    with path.open() as f:
        for line in f:
            m = _DEFINE_RE.match(line)
            if not m:
                continue
            name, raw = m.group(1), m.group(2)
            if not name.startswith(prefix):
                continue
            try:
                value = int(raw, 0)
            except ValueError:
                # Reference to another constant, e.g. ITEM_LIST_END = 0xFFFF; skip.
                continue
            out[name] = value
    return out


def _load_enum(path: pathlib.Path, prefix: str) -> dict[str, int]:
    """Parse a C header that mixes `#define FOO 123` and enum
    `FOO = OTHER_NAME + 1,` entries. Resolves references against previously-
    seen identifiers from the same file (counter auto-increments inside enums
    when no explicit value is given).

    Returns {NAME: int}. Only entries whose name starts with `prefix` end up
    in the returned dict, but ALL identifiers are tracked internally for
    reference resolution (since e.g. MOVE_ROOST = MOVES_COUNT_GEN3 needs the
    MOVES_COUNT_GEN3 value even though MOVES_COUNT_GEN3 doesn't match the
    MOVE_ prefix).
    """
    symbols: dict[str, int] = {}
    # Inside an enum, the running counter increments on every entry that
    # doesn't have an explicit value. Outside an enum (i.e. for #define), no
    # counter — every entry must have a value.
    counter = 0
    in_enum = False
    # `enum __attribute__((packed)) Item\n{\n` style — the opening brace can
    # land on the next line, so track "saw enum keyword recently" separately.
    saw_enum_keyword = False

    def resolve_expr(expr: str) -> int | None:
        """Resolve a tiny subset of C expressions: literal, identifier,
        identifier + literal. Returns None if unresolvable."""
        expr = expr.strip()
        # Literal
        try:
            return int(expr, 0)
        except ValueError:
            pass
        # Identifier
        if expr in symbols:
            return symbols[expr]
        # Identifier + literal  (e.g. MOVES_COUNT_GEN3 + 1)
        m = re.match(r"(\w+)\s*([+-])\s*(0x[0-9A-Fa-f]+|\d+)$", expr)
        if m:
            base = symbols.get(m.group(1))
            if base is None:
                return None
            offset = int(m.group(3), 0)
            return base + offset if m.group(2) == "+" else base - offset
        return None

    define_re = re.compile(r"^\s*#define\s+(\w+)\s+(.+?)(?:\s*//.*)?$")
    enum_entry_re = re.compile(r"^\s*(\w+)\s*(?:=\s*([^,]+?))?\s*,?\s*(?://.*)?$")

    for line in path.open():
        stripped = line.strip()
        if not stripped or stripped.startswith("//"):
            continue
        # Detect enum boundaries. `enum NAME {` may have the opening brace on
        # the same line OR on the following line.
        if re.search(r"\benum\b", stripped):
            saw_enum_keyword = True
        if saw_enum_keyword and "{" in stripped:
            in_enum = True
            saw_enum_keyword = False
            counter = 0
            # If the enum's first entry is on the same line as `{`, fall through
            # to the entry parse below — but for the common multi-line style
            # we just continue.
            if stripped.endswith("{"):
                continue
        if in_enum and "}" in stripped:
            in_enum = False
            continue
        # #define wins anywhere
        m = define_re.match(line)
        if m:
            name, expr = m.group(1), m.group(2).strip().rstrip(",")
            value = resolve_expr(expr)
            if value is not None:
                symbols[name] = value
            continue
        if in_enum:
            m = enum_entry_re.match(line)
            if m:
                name, expr = m.group(1), m.group(2)
                if name in ("enum", "struct"):
                    continue
                if expr is not None:
                    value = resolve_expr(expr.strip().rstrip(","))
                    if value is None:
                        continue  # unresolvable, skip but keep counter going
                    counter = value
                symbols[name] = counter
                counter += 1
    return {k: v for k, v in symbols.items() if k.startswith(prefix)}


def _name_to_id_normalized(table: dict[str, int], prefix: str, name: str) -> int | None:
    """Try common name → ID lookups. Showdown uses 'Life Orb' style, headers
    use ITEM_LIFE_ORB style. Normalize both."""
    norm = name.upper().replace(" ", "_").replace("-", "_").replace("'", "").replace(".", "")
    candidates = [
        f"{prefix}{norm}",
        f"{prefix}{norm.replace('_', '')}",
    ]
    for c in candidates:
        if c in table:
            return table[c]
    return None


# Lazy-loaded constant tables
_SPECIES: dict[str, int] | None = None
_ITEMS: dict[str, int] | None = None
_MOVES: dict[str, int] | None = None
_ABILITIES: dict[str, int] | None = None


def _tables():
    global _SPECIES, _ITEMS, _MOVES, _ABILITIES
    if _SPECIES is None:
        # Pokeemerald-expansion uses C enums for items / moves / species and
        # #defines for some legacy abilities. _load_enum handles both.
        _SPECIES = _load_enum(REPO_ROOT / "include/constants/species.h", "SPECIES_")
        _ITEMS = _load_enum(REPO_ROOT / "include/constants/items.h", "ITEM_")
        _MOVES = _load_enum(REPO_ROOT / "include/constants/moves.h", "MOVE_")
        _ABILITIES = _load_enum(REPO_ROOT / "include/constants/abilities.h", "ABILITY_")
    return _SPECIES, _ITEMS, _MOVES, _ABILITIES


def species_id(name: str) -> int:
    species, _, _, _ = _tables()
    found = _name_to_id_normalized(species, "SPECIES_", name)
    if found is None:
        raise KeyError(f"Unknown species: {name!r}")
    return found


def item_id(name: str) -> int:
    _, items, _, _ = _tables()
    if not name or name.lower() in ("none", "no item"):
        return 0
    found = _name_to_id_normalized(items, "ITEM_", name)
    if found is None:
        raise KeyError(f"Unknown item: {name!r}")
    return found


def move_id(name: str) -> int:
    _, _, moves, _ = _tables()
    if not name:
        return 0
    found = _name_to_id_normalized(moves, "MOVE_", name)
    if found is None:
        raise KeyError(f"Unknown move: {name!r}")
    return found


# ---------------------------------------------------------------------------
# Showdown parser
# ---------------------------------------------------------------------------

@dataclasses.dataclass
class Mon:
    species: int = 0
    held_item: int = 0
    ability_slot: int = 0   # 0 / 1 / 2 — we resolve by name → species ability table
    ability_name: str = ""  # raw, for later slot resolution against species data
    nickname: str = ""
    level: int = 50
    nature: int = 0
    gender: int = 0
    shiny: bool = False
    moves: list[int] = dataclasses.field(default_factory=lambda: [0, 0, 0, 0])
    evs: list[int] = dataclasses.field(default_factory=lambda: [0, 0, 0, 0, 0, 0])
    ivs: list[int] = dataclasses.field(default_factory=lambda: [31, 31, 31, 31, 31, 31])


def parse_showdown(text: str) -> list[Mon]:
    mons: list[Mon] = []
    current: Mon | None = None
    for raw_line in text.splitlines():
        line = raw_line.strip()
        if not line:
            if current is not None and current.species != 0:
                mons.append(current)
            current = None
            continue
        if current is None:
            current = Mon()
            # First non-empty line: "Nickname (Species) @ Item" OR "Species @ Item"
            mon_line = line
            if "@" in mon_line:
                left, item = mon_line.rsplit("@", 1)
                current.held_item = item_id(item.strip())
                mon_line = left.strip()
            # Strip trailing gender marker (M)/(F)
            m = re.match(r"^(.*)\s+\((M|F)\)\s*$", mon_line)
            if m:
                mon_line = m.group(1).strip()
                current.gender = 1 if m.group(2) == "M" else 2
            # Now mon_line is either "Species" or "Nickname (Species)"
            m = re.match(r"^(.+?)\s+\((.+?)\)\s*$", mon_line)
            if m:
                current.nickname = m.group(1).strip()
                current.species = species_id(m.group(2).strip())
            else:
                current.species = species_id(mon_line)
            continue
        # Subsequent lines: "Field: Value" or "- Move"
        if line.startswith("-"):
            move = line.lstrip("- ").strip()
            for i in range(4):
                if current.moves[i] == 0:
                    current.moves[i] = move_id(move)
                    break
            continue
        if ":" in line:
            key, value = line.split(":", 1)
            key, value = key.strip().lower(), value.strip()
            if key == "ability":
                current.ability_name = value
            elif key == "level":
                current.level = int(value)
            elif key in ("nature", "nature."):
                current.nature = NATURE_INDEX[value.lower().replace(" nature", "").strip()]
            elif key in ("shiny",):
                current.shiny = value.lower() in ("yes", "true", "1")
            elif key == "gender":
                current.gender = GENDER_MAP[value.lower()]
            elif key == "evs":
                _parse_stat_line(value, current.evs)
            elif key == "ivs":
                _parse_stat_line(value, current.ivs)
        elif line.endswith("Nature"):
            # "Adamant Nature" line format
            nature_name = line.replace("Nature", "").strip().lower()
            if nature_name in NATURE_INDEX:
                current.nature = NATURE_INDEX[nature_name]
    if current is not None and current.species != 0:
        mons.append(current)
    return mons


def _parse_stat_line(value: str, dest: list[int]):
    """Parse 'EVs: 252 HP / 252 Atk / 4 Spe' or 'IVs: 0 Atk / 31 ...'."""
    for chunk in value.split("/"):
        chunk = chunk.strip()
        m = re.match(r"^(\d+)\s+(\S+)", chunk)
        if not m:
            continue
        n, stat = int(m.group(1)), m.group(2).lower().strip(".")
        if stat in STAT_ALIASES:
            dest[STAT_ALIASES[stat]] = n


# ---------------------------------------------------------------------------
# Binary encoder
# ---------------------------------------------------------------------------

def encode_mon(mon: Mon) -> bytes:
    """Pack a Mon into the v2 bit-stream layout (see SPEC.md).

    Returns the byte string: [bit_stream_bytes...] + [checksum_byte].
    """
    if mon.species >= (1 << 11):
        raise ValueError(f"species {mon.species} doesn't fit in 11 bits (cap 2047)")
    if mon.held_item >= (1 << 10):
        raise ValueError(f"item {mon.held_item} doesn't fit in 10 bits (cap 1023)")

    w = BitWriter()
    w.write(FORMAT_VERSION, 4)
    w.write(mon.species, 11)
    w.write(mon.held_item, 10)
    w.write(mon.nature & 0x1F, 5)
    w.write(mon.ability_slot & 0x03, 2)
    w.write(1 if mon.shiny else 0, 1)
    w.write(max(1, min(100, mon.level)), 7)
    w.write(mon.gender & 0x03, 2)
    # Move count + moves
    move_count = sum(1 for m in mon.moves if m != 0)
    w.write(move_count, 3)
    written_moves = 0
    for m in mon.moves:
        if m == 0:
            continue
        if m >= (1 << 11):
            raise ValueError(f"move {m} doesn't fit in 11 bits (cap 2047)")
        w.write(m, 11)
        written_moves += 1
        if written_moves >= move_count:
            break
    # EV mask + non-zero EVs
    ev_mask = 0
    for i in range(6):
        if mon.evs[i] != 0:
            ev_mask |= 1 << i
    w.write(ev_mask, 6)
    for i in range(6):
        if ev_mask & (1 << i):
            w.write(min(252, mon.evs[i]), 8)
    # IV "has deviations" flag — saves 5 bits per mon for the common case
    # of all-31 IVs.
    iv_mask = 0
    for i in range(6):
        if mon.ivs[i] != 31:
            iv_mask |= 1 << i
    if iv_mask == 0:
        w.write(0, 1)
    else:
        w.write(1, 1)
        w.write(iv_mask, 6)
        for i in range(6):
            if iv_mask & (1 << i):
                w.write(min(31, mon.ivs[i]), 5)

    body = w.finish()
    # Checksum: XOR of every body byte (including the trailing zero-pad).
    chk = 0
    for b in body:
        chk ^= b
    return body + bytes([chk])


def encode_code(mon: Mon) -> str:
    payload = encode_mon(mon)
    b64 = base64.urlsafe_b64encode(payload).rstrip(b"=").decode("ascii")
    return "PB" + b64


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main(argv: list[str]) -> int:
    if len(argv) != 2:
        print("Usage: encode.py <showdown-export.txt>", file=sys.stderr)
        return 1
    text = pathlib.Path(argv[1]).read_text()
    mons = parse_showdown(text)
    if not mons:
        print("No Pokemon parsed.", file=sys.stderr)
        return 1
    for i, mon in enumerate(mons[:6], 1):
        code = encode_code(mon)
        print(f"Pokemon {i}: {code}   ({len(code)} chars)")
    if len(mons) > 6:
        print(f"Warning: {len(mons)} mons in export, only first 6 encoded.", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
