#!/usr/bin/env python3
"""
Build tools/team-codes/encoder.html — a single self-contained offline HTML
page that converts a Showdown text export into 6 Pokemon Battle Theater
team codes.

Embeds the species/item/move/ability lookup tables inline (parsed from
include/constants/*.h via the same logic encode.py uses) so the page works
fully offline. Re-run this script after pulling new game data.

Usage:
    python3 build_encoder.py
"""

from __future__ import annotations
import json
import pathlib
import re

# Re-use encode.py's table parser so the HTML and the Python reference impl
# stay in lockstep.
from encode import _tables, FORMAT_VERSION, NATURES

THIS_DIR = pathlib.Path(__file__).resolve().parent


def _normalize(name: str) -> str:
    """Strip everything non-alphanumeric and lowercase. Matches how the JS
    encoder normalizes Showdown input ('Life Orb' -> 'lifeorb')."""
    return re.sub(r"[^a-z0-9]", "", name.lower())


def _build_lookup(table: dict[str, int], prefix: str) -> dict[str, int]:
    """SPECIES_LIFE_ORB -> 'lifeorb' -> id. Drops the prefix and normalizes."""
    out: dict[str, int] = {}
    for name, value in table.items():
        if not name.startswith(prefix):
            continue
        short = name.removeprefix(prefix)
        # Drop GameFreak-style suffixes like _MEGA, _GMAX from the lookup key
        # (Showdown's text format doesn't have underscores).
        out[_normalize(short)] = value
    return out


def main() -> int:
    species, items, moves, abilities = _tables()
    tables = {
        "species":   _build_lookup(species,   "SPECIES_"),
        "items":     _build_lookup(items,     "ITEM_"),
        "moves":     _build_lookup(moves,     "MOVE_"),
        "abilities": _build_lookup(abilities, "ABILITY_"),
    }
    natures_lookup = {n.lower(): i for i, n in enumerate(NATURES)}

    # JSON-serialize, then squeeze to keep the HTML compact. Sorted keys help
    # diff-readability when checking the file into git.
    tables_json = json.dumps(tables, sort_keys=True, separators=(",", ":"))
    natures_json = json.dumps(natures_lookup, sort_keys=True, separators=(",", ":"))

    html = HTML_TEMPLATE.replace("__TABLES_JSON__", tables_json) \
                        .replace("__NATURES_JSON__", natures_json) \
                        .replace("__FORMAT_VERSION__", str(FORMAT_VERSION))
    out_path = THIS_DIR / "encoder.html"
    out_path.write_text(html)
    size_kb = out_path.stat().st_size / 1024
    print(f"Wrote {out_path} ({size_kb:.1f} KB)")
    print(f"  species: {len(tables['species'])} entries")
    print(f"  items:   {len(tables['items'])} entries")
    print(f"  moves:   {len(tables['moves'])} entries")
    print(f"  abilities: {len(tables['abilities'])} entries")
    return 0


HTML_TEMPLATE = r"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>Pokemon Battle Theater — Team Code Encoder</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  :root {
    --bg: #1d2640;
    --panel: #2a3553;
    --accent: #f6c026;
    --text: #ecf0f1;
    --muted: #95a5a6;
    --error: #e74c3c;
    --ok: #27ae60;
    --code-bg: #15192a;
  }
  * { box-sizing: border-box; }
  body {
    margin: 0; padding: 24px;
    background: var(--bg); color: var(--text);
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", system-ui, sans-serif;
    line-height: 1.5;
  }
  h1 { margin: 0 0 4px 0; font-size: 22px; color: var(--accent); }
  .subtitle { margin: 0 0 24px 0; color: var(--muted); font-size: 14px; }
  .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 24px; max-width: 1200px; margin: 0 auto; }
  @media (max-width: 800px) { .grid { grid-template-columns: 1fr; } }
  .panel {
    background: var(--panel);
    border-radius: 8px;
    padding: 16px;
  }
  .panel h2 { margin: 0 0 8px 0; font-size: 14px; font-weight: 600; color: var(--muted); text-transform: uppercase; letter-spacing: 0.05em; }
  textarea {
    width: 100%; min-height: 380px;
    padding: 12px;
    border: 1px solid #3d4a72;
    border-radius: 6px;
    background: var(--code-bg);
    color: var(--text);
    font-family: ui-monospace, "SF Mono", Menlo, Consolas, monospace;
    font-size: 13px;
    line-height: 1.45;
    resize: vertical;
  }
  .codes { display: flex; flex-direction: column; gap: 10px; }
  .code-row {
    display: flex; align-items: center; gap: 10px;
    background: var(--code-bg);
    border: 1px solid #3d4a72;
    border-radius: 6px;
    padding: 10px;
  }
  .code-row.err { border-color: var(--error); }
  .code-label {
    font-size: 12px; color: var(--muted);
    min-width: 90px;
    font-weight: 600;
  }
  .code-value {
    flex: 1;
    font-family: ui-monospace, "SF Mono", Menlo, Consolas, monospace;
    font-size: 13px;
    word-break: break-all;
    color: var(--text);
  }
  .code-row.err .code-value { color: var(--error); }
  .code-meta { font-size: 11px; color: var(--muted); white-space: nowrap; }
  .copy-btn {
    background: var(--accent); color: #000;
    border: none; border-radius: 4px;
    padding: 6px 12px; font-size: 12px; font-weight: 600;
    cursor: pointer;
    min-width: 70px;
  }
  .copy-btn:disabled { opacity: 0.4; cursor: not-allowed; background: var(--muted); }
  .copy-btn.copied { background: var(--ok); color: #fff; }
  .footer {
    margin-top: 32px; padding-top: 16px;
    border-top: 1px solid #3d4a72;
    max-width: 1200px; margin-left: auto; margin-right: auto;
    color: var(--muted); font-size: 13px;
  }
  .footer code { background: var(--code-bg); padding: 2px 6px; border-radius: 3px; color: var(--text); }
  .empty { color: var(--muted); font-style: italic; }
</style>
</head>
<body>

<h1>Pokemon Battle Theater — Team Code Encoder</h1>
<p class="subtitle">
  Paste a <a href="https://www.smogon.com/forums/threads/3656476/" style="color: var(--accent)">Pokémon Showdown</a> team export on the left. Six codes appear on the right — one per Pokémon. Type each code into the ROM's <b>"Import from code…"</b> screen on the custom-trainer slot you want to fill.
</p>

<div class="grid">
  <div class="panel">
    <h2>Showdown export</h2>
    <textarea id="input" placeholder="Lucario @ Life Orb&#10;Ability: Inner Focus&#10;Level: 50&#10;EVs: 252 Atk / 4 SpD / 252 Spe&#10;Adamant Nature&#10;- Close Combat&#10;- Extreme Speed&#10;- Bullet Punch&#10;- Crunch&#10;&#10;Garchomp @ Yache Berry&#10;..."></textarea>
  </div>

  <div class="panel">
    <h2>Team codes</h2>
    <div class="codes" id="codes">
      <div class="code-row"><div class="code-value empty">Paste a team on the left to generate codes here.</div></div>
    </div>
  </div>
</div>

<div class="footer">
  <p><b>Notes</b></p>
  <ul>
    <li>Each code starts with <code>PB</code> so you can verify the right thing pasted. Codes are URL-safe base64 with no padding.</li>
    <li>The ROM imports one Pokémon at a time — type the first code into slot Pokémon 1, the second into Pokémon 2, and so on.</li>
    <li>Trainer name and sprite are <b>not</b> part of the codes. Set those on the slot itself (or use <i>Copy preset team…</i>).</li>
    <li>Format version: <code>__FORMAT_VERSION__</code>. ROMs older than v1.2 won't recognize these codes.</li>
  </ul>
</div>

<script>
// ============================================================================
// Pokemon Battle Theater — JS team-code encoder
// Mirrors tools/team-codes/encode.py / SPEC.md.
// ============================================================================

const FORMAT_VERSION = __FORMAT_VERSION__;
const TABLES = __TABLES_JSON__;
const NATURES = __NATURES_JSON__;
const STAT_ORDER = ["hp", "atk", "def", "spa", "spd", "spe"];
const STAT_ALIASES = {
  hp: 0,
  atk: 1, attack: 1,
  def: 2, defense: 2,
  spa: 3, spatk: 3, specialattack: 3,
  spd: 4, spdef: 4, specialdefense: 4,
  spe: 5, speed: 5
};
const GENDER_MAP = { any: 0, "": 0, male: 1, m: 1, female: 2, f: 2 };

function normalize(s) {
  return s.toLowerCase().replace(/[^a-z0-9]/g, "");
}

function lookup(table, name, kind) {
  if (!name) return 0;
  const key = normalize(name);
  if (key in table) return table[key];
  throw new Error(`Unknown ${kind}: "${name}"`);
}

// Parse a Showdown text export into an array of Mon objects.
function parseShowdown(text) {
  const mons = [];
  let current = null;
  const lines = text.split(/\r?\n/);

  function flushCurrent() {
    if (current && current.species) mons.push(current);
    current = null;
  }

  for (const raw of lines) {
    const line = raw.trim();
    if (!line) { flushCurrent(); continue; }

    if (!current) {
      current = {
        species: 0, heldItem: 0, abilitySlot: 0, abilityName: "",
        nickname: "", level: 50, nature: 0, gender: 0, shiny: false,
        moves: [0, 0, 0, 0],
        evs: [0, 0, 0, 0, 0, 0],
        ivs: [31, 31, 31, 31, 31, 31],
      };
      // "Name (Species) @ Item" or "Species @ Item"
      let monLine = line;
      const atIdx = monLine.lastIndexOf("@");
      if (atIdx !== -1) {
        const item = monLine.slice(atIdx + 1).trim();
        current.heldItem = lookup(TABLES.items, item, "item");
        monLine = monLine.slice(0, atIdx).trim();
      }
      // Trailing gender marker
      const genderMatch = monLine.match(/^(.*)\s+\((M|F)\)\s*$/);
      if (genderMatch) {
        monLine = genderMatch[1].trim();
        current.gender = genderMatch[2] === "M" ? 1 : 2;
      }
      // Species or Nickname (Species)
      const nickMatch = monLine.match(/^(.+?)\s+\((.+?)\)\s*$/);
      if (nickMatch) {
        current.nickname = nickMatch[1].trim();
        current.species = lookup(TABLES.species, nickMatch[2].trim(), "species");
      } else {
        current.species = lookup(TABLES.species, monLine, "species");
      }
      continue;
    }

    if (line.startsWith("-")) {
      const move = line.replace(/^-\s*/, "").trim();
      for (let i = 0; i < 4; i++) {
        if (current.moves[i] === 0) {
          current.moves[i] = lookup(TABLES.moves, move, "move");
          break;
        }
      }
      continue;
    }

    const colonIdx = line.indexOf(":");
    if (colonIdx !== -1) {
      const key = line.slice(0, colonIdx).trim().toLowerCase();
      const value = line.slice(colonIdx + 1).trim();
      switch (key) {
        case "ability":
          current.abilityName = value;
          // We don't resolve ability slot here — Showdown lists by name, but
          // the ROM stores 0/1/2. Default to 0 and let user adjust if needed.
          // A v1.3 enhancement could lookup species ability table.
          break;
        case "level": current.level = parseInt(value, 10); break;
        case "nature":
        case "nature.": {
          const n = value.toLowerCase().replace(/\s+nature$/i, "").trim();
          if (n in NATURES) current.nature = NATURES[n];
          break;
        }
        case "shiny": current.shiny = /^(yes|true|1)$/i.test(value); break;
        case "gender": current.gender = GENDER_MAP[value.toLowerCase()] ?? 0; break;
        case "evs": parseStatLine(value, current.evs); break;
        case "ivs": parseStatLine(value, current.ivs); break;
      }
    } else if (/\bNature\b/i.test(line)) {
      // "Adamant Nature" alternative format
      const n = line.replace(/Nature/i, "").trim().toLowerCase();
      if (n in NATURES) current.nature = NATURES[n];
    }
  }
  flushCurrent();
  return mons;
}

function parseStatLine(value, dest) {
  for (const chunk of value.split("/")) {
    const m = chunk.trim().match(/^(\d+)\s+(\S+)/);
    if (!m) continue;
    const n = parseInt(m[1], 10);
    const stat = m[2].toLowerCase().replace(/\.$/, "");
    if (stat in STAT_ALIASES) dest[STAT_ALIASES[stat]] = n;
  }
}

// Pack a Mon into bytes per SPEC.md v1 format.
function encodeMon(mon) {
  const bytes = [];
  bytes.push(FORMAT_VERSION);
  bytes.push(mon.species & 0xFF, (mon.species >> 8) & 0xFF);
  bytes.push(mon.heldItem & 0xFF, (mon.heldItem >> 8) & 0xFF);
  const packed = (mon.nature & 0x1F)
               | ((mon.abilitySlot & 0x03) << 5)
               | ((mon.shiny ? 1 : 0) << 7);
  bytes.push(packed);
  bytes.push(Math.max(1, Math.min(100, mon.level)));
  bytes.push(mon.gender & 0x03);
  for (let i = 0; i < 4; i++) {
    bytes.push(mon.moves[i] & 0xFF, (mon.moves[i] >> 8) & 0xFF);
  }
  let evMask = 0;
  for (let i = 0; i < 6; i++) if (mon.evs[i] !== 0) evMask |= (1 << i);
  bytes.push(evMask);
  for (let i = 0; i < 6; i++) {
    if (evMask & (1 << i)) bytes.push(Math.min(252, mon.evs[i]));
  }
  let ivMask = 0;
  for (let i = 0; i < 6; i++) if (mon.ivs[i] !== 31) ivMask |= (1 << i);
  bytes.push(ivMask);
  for (let i = 0; i < 6; i++) {
    if (ivMask & (1 << i)) bytes.push(Math.min(31, mon.ivs[i]));
  }
  let chk = 0;
  for (const b of bytes) chk ^= b;
  bytes.push(chk);
  return bytes;
}

// URL-safe base64, no padding.
function base64UrlEncode(bytes) {
  let bin = "";
  for (const b of bytes) bin += String.fromCharCode(b);
  return btoa(bin).replace(/\+/g, "-").replace(/\//g, "_").replace(/=+$/, "");
}

function encodeCode(mon) {
  return "PB" + base64UrlEncode(encodeMon(mon));
}

// ----- UI -----

const inputEl = document.getElementById("input");
const codesEl = document.getElementById("codes");

function render() {
  const text = inputEl.value;
  if (!text.trim()) {
    codesEl.innerHTML = '<div class="code-row"><div class="code-value empty">Paste a team on the left to generate codes here.</div></div>';
    return;
  }
  let mons = [];
  let parseErr = null;
  try {
    mons = parseShowdown(text);
  } catch (e) {
    parseErr = e;
  }

  const rows = [];
  for (let i = 0; i < 6; i++) {
    if (i < mons.length) {
      try {
        const code = encodeCode(mons[i]);
        rows.push(rowHtml(i + 1, code, null));
      } catch (e) {
        rows.push(rowHtml(i + 1, e.message, "encode error"));
      }
    } else {
      rows.push(emptyRowHtml(i + 1));
    }
  }
  if (parseErr) {
    rows.unshift(rowHtml("Parse", parseErr.message, "error"));
  }
  codesEl.innerHTML = rows.join("");
  attachCopyHandlers();
}

function rowHtml(label, code, errKind) {
  const isErr = !!errKind;
  const cls = isErr ? "code-row err" : "code-row";
  const meta = isErr ? errKind : `${code.length} chars`;
  const btn = isErr
    ? `<button class="copy-btn" disabled>copy</button>`
    : `<button class="copy-btn" data-code="${escapeAttr(code)}">copy</button>`;
  return `
    <div class="${cls}">
      <div class="code-label">Pokémon ${label}</div>
      <div class="code-value">${escapeHtml(code)}</div>
      <div class="code-meta">${escapeHtml(meta)}</div>
      ${btn}
    </div>`;
}

function emptyRowHtml(n) {
  return `
    <div class="code-row">
      <div class="code-label">Pokémon ${n}</div>
      <div class="code-value empty">(empty slot)</div>
      <div class="code-meta"></div>
      <button class="copy-btn" disabled>copy</button>
    </div>`;
}

function attachCopyHandlers() {
  for (const btn of document.querySelectorAll(".copy-btn[data-code]")) {
    btn.addEventListener("click", async () => {
      try {
        await navigator.clipboard.writeText(btn.dataset.code);
        const orig = btn.textContent;
        btn.textContent = "copied!";
        btn.classList.add("copied");
        setTimeout(() => { btn.textContent = orig; btn.classList.remove("copied"); }, 1200);
      } catch (e) {
        // Older browsers: fall back to legacy execCommand path
        const tmp = document.createElement("textarea");
        tmp.value = btn.dataset.code;
        document.body.appendChild(tmp);
        tmp.select();
        document.execCommand("copy");
        document.body.removeChild(tmp);
      }
    });
  }
}

function escapeHtml(s) {
  return String(s).replace(/[&<>"']/g, (c) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;"
  })[c]);
}
function escapeAttr(s) { return escapeHtml(s); }

inputEl.addEventListener("input", render);
render();
</script>

</body>
</html>
"""


if __name__ == "__main__":
    raise SystemExit(main())
