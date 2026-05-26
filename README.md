# pokebattlesim

An AI-vs-AI Pokémon battle simulator built on
[pokeemerald-expansion 1.15.2](https://github.com/rh-hideout/pokeemerald-expansion).
Pit any two trainers from the mainline games (Gens 1–9) against each other,
or pilot one side via Pilot Mode.

**Releases:** see the [Releases page](https://github.com/logdog2325/pokebattlesim/releases)
for the `.bps` patch and end-user docs.

---

## What this is

- ~120 curated rosters spanning Kanto / Johto / Hoenn / Sinnoh / Unova /
  Kalos / Alola / Galar / Hisui, plus the PWT and Ash's anime World
  Champion team
- 9-trainer Legends Arceus section (Volo, Adaman, Irida, Ingo, Akari,
  Kamado, Zisu, Beni, Rei)
- Custom Trainer slots with full PKHeX-style editor (sprite / name /
  species / item / ability / EVs / IVs / moves / nature / gender / shiny)
- Tournament mode (8-trainer single-elim brackets across all regions)
- Best-of-N matches with adaptive picks
- VGC mode (forced doubles, Lv 50 cap, 4-pick-of-6)
- Pilot Mode (control the player AI's loaner mons yourself)
- Custom Battle Theater background by LiYun
- AI tweaks for Z-Move / Mega / Dynamax / Gmax bias
- Custom singles AI controller (fixes vanilla post-KO crash)
- Affection and Terastallization disabled

## Building from source

You'll need [devkitARM](https://devkitpro.org/) and the standard
pokeemerald-expansion toolchain. Then:

```sh
git clone https://github.com/logdog2325/pokebattlesim.git
cd pokebattlesim
make -j4   # produces pokeemerald.gba
```

To produce a `RELEASE_BUILD` (Logan + Taylor stripped from the picker),
uncomment `#define RELEASE_BUILD 1` at the top of `src/debug.c` and rebuild.

## Distributing a patch

The release ROM cannot be shared directly (Nintendo copyright). Use
[Flips](https://github.com/Alcaro/Flips) to generate a `.bps` patch from
your modded ROM against vanilla Emerald (USA, MD5
`605b89b67018abcea91e693a4dd25be3`), and distribute just the `.bps`.

```sh
flips --create --bps vanilla-emerald.gba pokeemerald.gba pokebattlesim.bps
```

## Upstream

This is a fork of
[rh-hideout/pokeemerald-expansion](https://github.com/rh-hideout/pokeemerald-expansion).
The pokeemerald-expansion README is preserved at
[`UPSTREAM_README.md`](./UPSTREAM_README.md) for engine docs / contribution
flow / debug menu reference.

## Credits

- pokeemerald-expansion engine + decompilation project
- LiYun — Battle Theater background art
- Bulbapedia / Smogon archives — canonical trainer rosters

Not affiliated with Nintendo, Game Freak, or The Pokémon Company.
Pokémon and all related marks are trademarks of their respective owners.
This is a fan-made, non-commercial modification.
