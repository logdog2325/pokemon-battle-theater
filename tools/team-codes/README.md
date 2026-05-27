# tools/team-codes/

Showdown text → Battle Theater team codes. Used by the v1.2 in-ROM "Import
from code…" feature.

## End-user file

- **`encoder.html`** — single-file offline web encoder. Open in any browser,
  paste a Showdown export, copy the 6 codes into the ROM. Self-contained
  (~75 KB) with all species/item/move/ability lookup tables baked in.

## Developer files

- **`SPEC.md`** — binary format spec (v1)
- **`encode.py`** — Python reference encoder (also the source of truth for
  the parser logic that gets ported to JS)
- **`decode.py`** — Python reference decoder. Pipe `encode.py` output into
  it to verify round-trip parity (`python3 encode.py team.txt | python3 decode.py -`)
- **`build_encoder.py`** — regenerates `encoder.html` from the template
  inside `build_encoder.py` itself + the lookup tables parsed out of the
  pokeemerald-expansion headers via `encode.py`'s parser. **Re-run after
  pulling new game data so the encoder knows about new species / moves.**
- **`test_js_encoder.js`** — Node smoke test that extracts the JS encoder
  out of `encoder.html` and runs it against `sample-team.txt`. Output must
  match `encode.py` byte-for-byte. Pipe into `decode.py` to verify
  round-trip too.
- **`sample-team.txt`** — Cynthia BDSP team in Showdown format, used by
  both the Python encoder regression and the JS smoke test.

## Workflow when game data changes

```sh
# After pulling new species/items/moves/abilities into the headers:
python3 build_encoder.py        # regenerates encoder.html
node test_js_encoder.js > new.codes
python3 encode.py sample-team.txt | sed 's/   ([0-9]* chars)//' > py.codes
diff new.codes py.codes         # must be empty
node test_js_encoder.js | python3 decode.py -   # round-trip sanity check
```

## Why per-mon codes instead of one-per-team

User preference — short individual codes are easier to type and recover from
than a single long code. See `SPEC.md` for the trade-off matrix.
