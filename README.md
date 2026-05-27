# Pokemon Battle Theater

<p align="center">
  <img src="docs/hero_title.png" alt="Pokemon Battle Theater — AI vs AI Simulator" width="600">
</p>

An AI vs AI Pokemon battle simulator built on
[pokeemerald-expansion 1.15.2](https://github.com/rh-hideout/pokeemerald-expansion).
Pit any two trainers from the mainline games (Gens 1-9) against each other,
or pilot one side via Pilot Mode.

**Download the latest patch:** [v1.4 — custom trainer save persistence](https://github.com/logdog2325/pokemon-battle-theater/releases/latest).
All releases live on the [Releases page](https://github.com/logdog2325/pokemon-battle-theater/releases).

---

## In-game

| Roster picker | Custom Trainer editor |
|---|---|
| ![Pick from ~120 trainers across every generation — Ash (Anime) Lv 90 shown](docs/roster_picker.png) | ![PKHeX-style per-mon editor — Arcanine with Flame Plate, Intimidate, Extreme Speed, Double-Edge, Flare Blitz, Take Down, EVs / IVs visible](docs/build_trainer_editor.png) |
| ~120 trainers across every generation. L/R jumps between regions. | Full per-mon editor saved to your file — sprite / name / species / item / ability / EVs / IVs / 4 moves / nature / gender / shiny. |

| Mega Evolution mid-trigger | Doubles + Mega in action |
|---|---|
| ![Giovanni's Mewtwonite Y reacting to his Mega Ring mid-animation](docs/battle_mewtwo_mega.png) | ![Mega Lucario evolved on the player side in a doubles match against Mewtwo + Nidoqueen](docs/battle_mega_lucario.png) |
| Z-Move / Mega / Dynamax / Gigantamax all wired with AI bias tuning. | Doubles, multi-battles, tournament brackets, best-of-N — every format works AI-vs-AI. |

---

## What this is

- ~120 curated rosters spanning Kanto / Johto / Hoenn / Sinnoh / Unova /
  Kalos / Alola / Galar / Hisui, plus the PWT and Ash's anime World
  Champion team
- **Hold L or R in the trainer picker to jump to the previous / next
  region** — fastest way to skip past the ~250 trainer entries instead of
  scrolling one at a time
- 9-trainer Legends Arceus section (Volo, Adaman, Irida, Ingo, Akari,
  Kamado, Zisu, Beni, Rei)
- 6 Custom Trainer slots with full PKHeX-style editor (sprite / name /
  species / item / ability / EVs / IVs / moves / nature / gender / shiny)
- Tournament mode (8-trainer single-elim brackets across all regions)
- Best-of-N matches with adaptive picks
- VGC mode (forced doubles, Lv 50 cap, 4-pick-of-6)
- Pilot Mode (control the player AI's loaner mons yourself)
- Custom Battle Theater background by LiYun
- AI tweaks for Z-Move / Mega / Dynamax / Gmax bias
- Custom singles AI controller (fixes vanilla post-KO crash)
- Affection and Terastallization disabled

## Known bugs (v1.0)

Cosmetic issues that don't affect battle behavior — being fixed for v1.1:

- **Player trainer name** sometimes renders with glitched / corrupted
  characters in battle dialogue.
- **Trainer picker menu** occasionally shows funky text on certain
  entries when scrolling. Closing and reopening the picker normalizes it.

## Roadmap

- **v1.1** — fix the trainer-name + picker-text glitches
- **v1.2** — add Gen 9 (Paldea) trainers: Nemona, Geeta, Penny,
  Larry, Iono, Grusha, Brassius, etc.
- **v1.3** — re-enable Terastallization with proper AI bias tuning
  (currently disabled because pre-gen-9 trainers don't have Tera types
  declared, which made matchups chaotic)

## Building from source

You'll need [devkitARM](https://devkitpro.org/) and the standard
pokeemerald-expansion toolchain. Then:

```sh
git clone https://github.com/logdog2325/pokemon-battle-theater.git
cd pokemon-battle-theater
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
flips --create --bps vanilla-emerald.gba pokeemerald.gba pokemon-battle-theater.bps
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
- u/Healthy_Bug7977 and u/LordePachi — suggested the Marvel Snap / Hearthstone–style
  deck-code import that became v1.3's Showdown team-code feature

Not affiliated with Nintendo, Game Freak, or The Pokémon Company.
Pokémon and all related marks are trademarks of their respective owners.
This is a fan-made, non-commercial modification.
