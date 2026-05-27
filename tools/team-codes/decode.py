#!/usr/bin/env python3
"""
Pokemon Battle Theater — team code decoder (Python reference impl).

Reverse of encode.py — takes a `PB...` code and reconstructs the Mon struct
so we can verify round-trip fidelity. The GBA-side decoder in
src/sim_team_decoder.c implements the same logic.

Usage:
    python3 decode.py <code>
    python3 decode.py < codes.txt   # one code per line
"""

from __future__ import annotations
import base64
import dataclasses
import sys

# Re-use the same name-table lookups from encode.py.
from encode import (
    FORMAT_VERSION, NATURES, STAT_ORDER, Mon, _tables, _load_defines,
    REPO_ROOT,
)


class DecodeError(ValueError):
    pass


def decode_payload(code: str) -> bytes:
    if not code.startswith("PB"):
        raise DecodeError("Code is missing the 'PB' prefix.")
    b64 = code[2:]
    # Restore base64 padding (urlsafe_b64decode requires `=` padding)
    padding = "=" * ((4 - len(b64) % 4) % 4)
    try:
        return base64.urlsafe_b64decode(b64 + padding)
    except Exception as e:
        raise DecodeError(f"Base64 decode failed: {e}") from e


def decode_mon(code: str) -> Mon:
    payload = decode_payload(code)
    if len(payload) < 19:
        raise DecodeError(f"Payload too short ({len(payload)} bytes; need ≥19).")
    # Verify checksum
    chk = 0
    for b in payload[:-1]:
        chk ^= b
    if chk != payload[-1]:
        raise DecodeError(f"Checksum mismatch (computed {chk:#x}, stored {payload[-1]:#x}).")

    pos = 0
    version = payload[pos]; pos += 1
    if version > FORMAT_VERSION:
        raise DecodeError(f"Unsupported format version {version} (this decoder knows ≤{FORMAT_VERSION}).")

    mon = Mon()
    mon.species = int.from_bytes(payload[pos:pos+2], "little"); pos += 2
    mon.held_item = int.from_bytes(payload[pos:pos+2], "little"); pos += 2
    packed = payload[pos]; pos += 1
    mon.nature = packed & 0x1F
    mon.ability_slot = (packed >> 5) & 0x03
    mon.shiny = bool((packed >> 7) & 0x01)
    mon.level = payload[pos]; pos += 1
    mon.gender = payload[pos] & 0x03; pos += 1
    for i in range(4):
        mon.moves[i] = int.from_bytes(payload[pos:pos+2], "little"); pos += 2
    ev_mask = payload[pos]; pos += 1
    for i in range(6):
        if ev_mask & (1 << i):
            mon.evs[i] = payload[pos]; pos += 1
        else:
            mon.evs[i] = 0
    iv_mask = payload[pos]; pos += 1
    for i in range(6):
        if iv_mask & (1 << i):
            mon.ivs[i] = payload[pos]; pos += 1
        else:
            mon.ivs[i] = 31
    return mon


def _reverse_lookup(table: dict[str, int], prefix: str, value: int) -> str:
    for name, v in table.items():
        if v == value:
            return name.removeprefix(prefix).replace("_", " ").title()
    return f"?id={value}"


def pretty_print(mon: Mon) -> None:
    species, items, moves, abilities = _tables()
    print(f"  Species:   {_reverse_lookup(species, 'SPECIES_', mon.species)} (#{mon.species})")
    print(f"  Held Item: {_reverse_lookup(items, 'ITEM_', mon.held_item) if mon.held_item else 'None'}")
    print(f"  Ability:   slot {mon.ability_slot}")
    print(f"  Level:     {mon.level}")
    print(f"  Nature:    {NATURES[mon.nature]}")
    print(f"  Gender:    {['Any', 'Male', 'Female'][mon.gender]}")
    print(f"  Shiny:     {'Yes' if mon.shiny else 'No'}")
    for i, m in enumerate(mon.moves, 1):
        if m:
            print(f"  Move {i}:    {_reverse_lookup(moves, 'MOVE_', m)}")
    ev_parts = [f"{v} {s}" for v, s in zip(mon.evs, STAT_ORDER) if v]
    if ev_parts:
        print(f"  EVs:       {' / '.join(ev_parts)}")
    iv_parts = [f"{v} {s}" for v, s in zip(mon.ivs, STAT_ORDER) if v != 31]
    if iv_parts:
        print(f"  IVs:       {' / '.join(iv_parts)} (others 31)")


def main(argv: list[str]) -> int:
    if len(argv) >= 2 and argv[1] != "-":
        codes = [argv[1]]
    else:
        codes = [line.strip() for line in sys.stdin if line.strip()]
    for i, code in enumerate(codes, 1):
        # Strip "Pokemon N: " prefix if present (so you can pipe encode.py output)
        if ":" in code and code.split(":")[0].lower().startswith("pokemon"):
            code = code.split(":", 1)[1].strip().split()[0]
        try:
            mon = decode_mon(code)
            print(f"Code #{i}: {code}  ({len(code)} chars)")
            pretty_print(mon)
            print()
        except DecodeError as e:
            print(f"Code #{i}: {code}\n  ERROR: {e}\n", file=sys.stderr)
            return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
