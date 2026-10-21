# AI Tiers

Battle Theater re-levels each trainer's battle AI by their role in the games. This page lists exactly what each tier does and which tier every roster trainer is set to. It is generated from the tier table in `src/debug.c` (`sSimAiTierById`), so it always matches the game.

## The setting

**AI: Canon Tiers / Equalized** — saved between sessions. It appears in three places:

- **Run Simulation** menu, under Level Cap
- **E4 Challenge** and **Boss Battles**, on the level-cap step
- **Tournaments**, on the Random Spectate / Spectate / Pilot step

- **Canon Tiers** (default): every trainer uses the tier listed below.
- **Equalized**: every trainer on both sides plays at Champion tier, so matches come down to the teams.

## What each tier does

The tiers are built from behaviors that already exist in the pokeemerald-expansion battle engine. Each tier adds to the one before it.

| Tier | Engine behaviors | What you'll see |
|---|---|---|
| **Gym Leader** | Check Bad Move, Try To Faint, Check Viability | Avoids moves that would fail or do nothing (no Thunder Wave on a Ground type, no Electric move into Volt Absorb, no status on a target that's already statused), takes KOs when it can, and picks moves for the situation: hazards, healing, screens and status at sensible times. Doesn't switch strategically. |
| **Elite Four** | Gym Leader + Smart Switching (with Smart Mon Choices) | Switches out of losing matchups — when it can't hurt the target, is about to be KO'd, or Toxic is building — and sends in the teammate that matches up best instead of the next one in the party. |
| **Champion** | Elite Four + HP Aware, Smart Tera | Plays differently based on its own and its target's HP: doesn't set up or heal at full HP, saves sacrifice moves like Explosion and Destiny Bond for when it's low, doesn't waste status on a nearly fainted target. Terastallizes when it actually wins a matchup or a KO. |

A trainer's personality flags are kept at every tier — Ace Pokémon, setup on the first turn, stall, and so on. **Omniscient AI is never used**: it lets the AI see the opponent's hidden set, which produces odd-looking plays.

Trainers marked **Own AI (unchanged)** keep the AI from their trainer data.

## Who gets which tier

- **Gym Leaders** → Gym Leader. **Elite Four** → Elite Four. **Champions** → Champion.
- **Rivals** → Elite Four, except **Nemona** and **Hop** → Champion.
- **Alola:** trial captains → Gym Leader; kahunas → Elite Four; title-defense challengers → Champion. A trainer with two roles gets the higher one (Acerola → Elite Four; Sophocles and Hapu → Champion).
- **Villain bosses**, **Frontier Brains / Subway Bosses / Battle Tree**, **Orre heads** (Evice, Gonzap, Greevil) → Champion. Other Orre admins → Elite Four.
- **SV** Academy teachers, Blueberry Elite Four, Carmine, Kieran, Penny, Cyrano → Elite Four.
- **Legends: Arceus** and **Legends: Z-A** casts → Champion.
- **Prof. Oak**, **Mustard**, **Peony**, **Klara**, **Avery**, **anime teams**, **VGC teams** and **custom slots** → Champion.

Totals across the roster below: 119 Gym Leader, 149 Elite Four, 179 Champion. Custom slots 1–8 are all Champion.

## Full roster

### RGBY (1996-98, Gen 1 — RBY E4, Champion Blue variants, Oak Glitch boss)

| Trainer | Tier |
|---|---|
| Lorelei Rby | Elite Four |
| Bruno Rby | Elite Four |
| Agatha Rby | Elite Four |
| Lance Rby | Elite Four |
| Blue Rgb Charizard | Champion |
| Blue Rgb Blastoise | Champion |
| Blue Rgb Venusaur | Champion |
| Blue Yellow Vaporeon | Champion |
| Blue Yellow Jolteon | Champion |
| Blue Yellow Flareon | Champion |
| Oak Glitch Venusaur | Champion |
| Oak Glitch Charizard | Champion |
| Oak Glitch Blastoise | Champion |

### GSC (1999-2000, Gen 2 — Kanto gyms, Johto E4, Lance, Silver, Red)

| Trainer | Tier |
|---|---|
| Brock Gsc | Gym Leader |
| Misty Gsc | Gym Leader |
| Erika Gsc | Gym Leader |
| Surge Gsc | Gym Leader |
| Janine Gsc | Gym Leader |
| Sabrina Gsc | Gym Leader |
| Blaine Gsc | Gym Leader |
| Blue Gsc | Champion |
| Will Gsc | Elite Four |
| Koga Gsc | Elite Four |
| Bruno Gsc | Elite Four |
| Karen Gsc | Elite Four |
| Lance Gsc | Champion |
| Silver Gsc Typhlosion | Elite Four |
| Silver Gsc Feraligatr | Elite Four |
| Silver Gsc Meganium | Elite Four |
| Red Gsc | Champion |

### FRLG (2004, Gen 3 Kanto remake — original Lv 63-75 ports)

| Trainer | Tier |
|---|---|
| Elite Four Lorelei Kanto | Elite Four |
| Elite Four Bruno Kanto | Elite Four |
| Elite Four Agatha Kanto | Elite Four |
| Elite Four Lance Kanto | Elite Four |
| Blue Squirtle | Champion |
| Blue Bulbasaur | Champion |
| Blue Charmander | Champion |

### Emerald (2004, Gen 3 Hoenn) + Hoenn Battle Frontier brains

| Trainer | Tier |
|---|---|
| Roxanne 5 | Gym Leader |
| Brawly 5 | Gym Leader |
| Wattson 5 | Gym Leader |
| Flannery 5 | Gym Leader |
| Norman 5 | Gym Leader |
| Winona 5 | Gym Leader |
| Tate And Liza 5 | Gym Leader |
| Juan 5 | Gym Leader |
| Sidney | Elite Four |
| Phoebe | Elite Four |
| Glacia | Elite Four |
| Drake | Elite Four |
| Wallace | Champion |
| Steven | Champion |
| Wally Vr 5 | Elite Four |
| Sidney Rs | Elite Four |
| Phoebe Rs | Elite Four |
| Glacia Rs | Elite Four |
| Drake Rs | Elite Four |
| Steven Rs | Champion |
| Anabel | Champion |
| Tucker | Champion |
| Spenser | Champion |
| Greta | Champion |
| Noland | Champion |
| Lucy | Champion |
| Brandon | Champion |

### Orre (2003-05, Gen 3 spin-offs: Colosseum + XD Gale of Darkness)

| Trainer | Tier |
|---|---|
| Miror B | Elite Four |
| Dakim | Elite Four |
| Venus | Elite Four |
| Ein | Elite Four |
| Justy | Elite Four |
| Nascour | Elite Four |
| Evice | Champion |
| Lovrina | Elite Four |
| Snattle | Elite Four |
| Gorigan | Elite Four |
| Gonzap | Champion |
| Gonzap Colo | Champion |
| Miror B Colo | Elite Four |
| Ardos | Elite Four |
| Eldes | Elite Four |
| Fein | Elite Four |
| Chobin | Elite Four |
| Greevil | Champion |

### Platinum (2008, Gen 4 Sinnoh) + Gen 4 Battle Frontier brains

| Trainer | Tier |
|---|---|
| Aaron Dp | Elite Four |
| Bertha Dp | Elite Four |
| Flint Dp | Elite Four |
| Lucian Dp | Elite Four |
| Cynthia Dp | Champion |
| Barry Turtwig | Elite Four |
| Barry Chimchar | Elite Four |
| Barry Piplup | Elite Four |
| Roark Pt | Gym Leader |
| Gardenia Pt | Gym Leader |
| Fantina Pt | Gym Leader |
| Maylene Pt | Gym Leader |
| Wake Pt | Gym Leader |
| Byron Pt | Gym Leader |
| Candice Pt | Gym Leader |
| Volkner Pt | Gym Leader |
| Cheryl Pt | Elite Four |
| Mira Pt | Elite Four |
| Riley Pt | Elite Four |
| Marley Pt | Elite Four |
| Buck Pt | Elite Four |
| Aaron Pt | Elite Four |
| Bertha Pt | Elite Four |
| Flint Pt | Elite Four |
| Lucian Pt | Elite Four |
| Cynthia Pt | Champion |
| Palmer | Champion |
| Dahlia | Champion |
| Darach | Champion |
| Argenta | Champion |
| Thorton | Champion |

### HGSS (2009, Gen 4 Johto remake — shares Frontier with Platinum)

| Trainer | Tier |
|---|---|
| Red | Champion |
| Blue Hgss | Champion |
| Silver Cynd Hgss | Elite Four |
| Silver Toto Hgss | Elite Four |
| Silver Chik Hgss | Elite Four |
| Falkner Hgss | Gym Leader |
| Bugsy Hgss | Gym Leader |
| Whitney Hgss | Gym Leader |
| Morty Hgss | Gym Leader |
| Chuck Hgss | Gym Leader |
| Jasmine Hgss | Gym Leader |
| Pryce Hgss | Gym Leader |
| Clair Hgss | Gym Leader |
| Leader Brock Rematch | Gym Leader |
| Leader Misty Rematch | Gym Leader |
| Leader Lt Surge Rematch | Gym Leader |
| Leader Erika Rematch | Gym Leader |
| Leader Koga Rematch | Gym Leader |
| Leader Sabrina Rematch | Gym Leader |
| Leader Blaine Rematch | Gym Leader |
| Will Hgss | Elite Four |
| Koga Hgss | Elite Four |
| Bruno Hgss | Elite Four |
| Karen Hgss | Elite Four |
| Lance Hgss | Champion |

### BW (2010-11, Gen 5 Unova) + Subway Bosses + Black Tower boss

| Trainer | Tier |
|---|---|
| N Zekrom Bw | Champion |
| N Reshiram Bw | Champion |
| N B2W2 | Champion |
| Alder Bw | Champion |
| Iris Pwt | Champion |
| Cheren Emboar Bw | Elite Four |
| Cheren Samurott Bw | Elite Four |
| Cheren Serperior Bw | Elite Four |
| Bianca Samurott Bw | Elite Four |
| Bianca Serperior Bw | Elite Four |
| Bianca Emboar Bw | Elite Four |
| Hugh Emboar Bw | Elite Four |
| Hugh Samurott Bw | Elite Four |
| Hugh Serperior Bw | Elite Four |
| Shauntal Bw | Elite Four |
| Marshal Bw | Elite Four |
| Grimsley Bw | Elite Four |
| Caitlin Bw | Elite Four |
| Shauntal Bw1 | Elite Four |
| Marshal Bw1 | Elite Four |
| Grimsley Bw1 | Elite Four |
| Caitlin Bw1 | Elite Four |
| Cynthia Bw | Champion |
| Ghetsis Bw | Champion |
| Colress Bw | Champion |
| Benga | Champion |
| Ingo | Champion |
| Emmet | Champion |

### PWT (B2W2 Pokemon World Tournament) — a Gen 5 feature, grouped with BW

| Trainer | Tier |
|---|---|
| Leader Brock Pwt | Gym Leader |
| Leader Misty Pwt | Gym Leader |
| Leader Lt Surge Pwt | Gym Leader |
| Leader Erika Pwt | Gym Leader |
| Leader Janine Pwt | Gym Leader |
| Leader Sabrina Pwt | Gym Leader |
| Leader Blaine Pwt | Gym Leader |
| Leader Giovanni Pwt | Champion |
| Roxanne Pwt | Gym Leader |
| Brawly Pwt | Gym Leader |
| Wattson Pwt | Gym Leader |
| Flannery Pwt | Gym Leader |
| Norman Pwt | Gym Leader |
| Winona Pwt | Gym Leader |
| Tate Pwt | Gym Leader |
| Liza Pwt | Gym Leader |
| Juan Pwt | Gym Leader |
| Clair Pwt | Gym Leader |
| Pryce Pwt | Gym Leader |
| Jasmine Pwt | Gym Leader |
| Chuck Pwt | Gym Leader |
| Morty Pwt | Gym Leader |
| Whitney Pwt | Gym Leader |
| Bugsy Pwt | Gym Leader |
| Falkner Pwt | Gym Leader |
| Volkner Pwt | Gym Leader |
| Roark Pwt | Gym Leader |
| Gardenia Pwt | Gym Leader |
| Byron Pwt | Gym Leader |
| Wake Pwt | Gym Leader |
| Candice Pwt | Gym Leader |
| Fantina Pwt | Gym Leader |
| Maylene Pwt | Gym Leader |
| Lenora Pwt | Gym Leader |
| Burgh Pwt | Gym Leader |
| Elesa Pwt | Gym Leader |
| Clay Pwt | Gym Leader |
| Skyla Pwt | Gym Leader |
| Brycen Pwt | Gym Leader |
| Drayden Pwt | Gym Leader |
| Marlon Pwt | Gym Leader |
| Cilan Pwt | Gym Leader |
| Chili Pwt | Gym Leader |
| Cress Pwt | Gym Leader |
| Cheren Pwt | Gym Leader |
| Roxie Pwt | Gym Leader |
| Red Pwt | Champion |
| Blue Pwt | Champion |
| Lance Pwt | Champion |
| Steven Pwt | Champion |
| Wallace Pwt | Champion |
| Cynthia Pwt | Champion |
| Alder Pwt | Champion |
| Bianca Pwt | Elite Four |

### XY (2013, Gen 6 Kalos)

| Trainer | Tier |
|---|---|
| Diantha Xy | Champion |
| Serena Xy | Elite Four |
| Calem Xy | Elite Four |
| Malva | Elite Four |
| Siebold | Elite Four |
| Wikstrom | Elite Four |
| Drasna | Elite Four |
| Lysandre | Champion |
| Tierno | Elite Four |
| Shauna | Elite Four |
| Trevor | Elite Four |

### ORAS (2014, Gen 6 Hoenn remake)

| Trainer | Tier |
|---|---|
| Sidney Oras | Elite Four |
| Phoebe Oras | Elite Four |
| Glacia Oras | Elite Four |
| Drake Oras | Elite Four |
| Brendan Oras | Elite Four |
| May Oras | Elite Four |
| Steven Oras | Champion |
| Wally Oras | Elite Four |
| Wallace Oras | Champion |
| Zinnia | Champion |
| Ilima | Gym Leader |
| Lana | Gym Leader |
| Kiawe | Gym Leader |
| Mallow | Gym Leader |
| Sophocles | Champion |
| Acerola | Elite Four |
| Mina | Gym Leader |
| Hala | Elite Four |
| Olivia Alola | Elite Four |
| Nanu | Elite Four |
| Hapu | Champion |
| Molayne | Elite Four |
| Kahili | Elite Four |
| Hala Sm E4 | Elite Four |
| Olivia Sm E4 | Elite Four |
| Acerola Sm E4 | Elite Four |
| Kahili Sm E4 | Elite Four |
| Kukui | Champion |
| Kukui Primarina | Champion |
| Kukui Incineroar | Champion |
| Hau | Champion |
| Hau Decidueye | Champion |
| Hau Incineroar | Champion |
| Gladion Venusaur | Champion |
| Gladion Charizard | Champion |
| Gladion Blastoise | Champion |
| Faba Sm | Champion |
| Dexio Sm | Elite Four |
| Plumeria Sm | Champion |
| Ryuki Sm | Champion |
| Guzma Sm | Champion |
| Lusamine Sm | Champion |
| Tristan Sm | Champion |
| Blue Usum | Champion |
| Red Usum | Champion |
| Anabel Usum | Champion |
| Red Bt | Champion |
| Blue Bt | Champion |
| Anabel Bt | Champion |
| Wally Bt | Champion |
| Cynthia Bt | Champion |
| Colress Bt | Champion |
| Dexio Bt | Champion |
| Sina Bt | Champion |
| Grimsley Bt | Champion |
| Guzma Bt | Champion |
| Plumeria Bt | Champion |
| Kiawe Bt | Champion |
| Kukui Bt | Champion |
| Mallow Bt | Champion |
| Archie Rr | Champion |
| Maxie Rr | Champion |
| Cyrus Rr Us | Champion |
| Cyrus Rr Um | Champion |
| Lysandre Us Rr | Champion |
| Lysandre Um Rr | Champion |
| Ghetsis Us Rr | Champion |
| Ghetsis Um Rr | Champion |
| Giovanni Us Rr | Champion |
| Giovanni Um Rr | Champion |

### LGPE (2018, Gen 7 Kanto remake)

| Trainer | Tier |
|---|---|
| Green Lgpe | Elite Four |
| Lorelei Lgpe | Elite Four |
| Agatha Lgpe | Elite Four |
| Lance Lgpe | Elite Four |
| Red Lgpe | Champion |
| Blue Lgpe | Champion |
| Bruno Lgpe | Elite Four |
| Trace Lgpe Pikachu | Champion |
| Trace Lgpe Eevee | Champion |
| Brock Lgpe | Gym Leader |
| Misty Lgpe | Gym Leader |
| Surge Lgpe | Gym Leader |
| Erika Lgpe | Gym Leader |
| Koga Lgpe | Gym Leader |
| Sabrina Lgpe | Gym Leader |
| Blaine Lgpe | Gym Leader |
| Giovanni Lgpe | Champion |

### SwSh (2019-20, Gen 8 Galar + Isle of Armor + Crown Tundra)

| Trainer | Tier |
|---|---|
| Leon Grookey | Champion |
| Leon Scorbunny | Champion |
| Leon Sobble | Champion |
| Hop Gr Sw | Champion |
| Hop Gr Sh | Champion |
| Hop Sc Sw | Champion |
| Hop Sc Sh | Champion |
| Hop So Sw | Champion |
| Hop So Sh | Champion |
| Mustard Single | Champion |
| Mustard Rapid | Champion |
| Marnie | Elite Four |
| Bede | Elite Four |
| Milo Gym | Gym Leader |
| Nessa | Gym Leader |
| Kabu | Gym Leader |
| Bea | Gym Leader |
| Allister | Gym Leader |
| Gordie | Gym Leader |
| Melony | Gym Leader |
| Piers | Gym Leader |
| Raihan | Gym Leader |
| Klara | Champion |
| Avery | Champion |
| Peony | Champion |

### BDSP (2021, Gen 8 Sinnoh remake)

| Trainer | Tier |
|---|---|
| Barry Bdsp Turtwig | Elite Four |
| Barry Bdsp Chimchar | Elite Four |
| Barry Bdsp Piplup | Elite Four |
| Roark Bdsp | Gym Leader |
| Gardenia Bdsp | Gym Leader |
| Maylene Bdsp | Gym Leader |
| Wake Bdsp | Gym Leader |
| Fantina Bdsp | Gym Leader |
| Byron Bdsp | Gym Leader |
| Candice Bdsp | Gym Leader |
| Volkner Bdsp | Gym Leader |
| Aaron Bdsp | Elite Four |
| Bertha Bdsp | Elite Four |
| Flint Bdsp | Elite Four |
| Lucian Bdsp | Elite Four |
| Cynthia Bdsp | Champion |
| Lucas Bdsp Turtwig | Elite Four |
| Lucas Bdsp Chimchar | Elite Four |
| Lucas Bdsp Piplup | Elite Four |
| Dawn Bdsp Turtwig | Elite Four |
| Dawn Bdsp Chimchar | Elite Four |
| Dawn Bdsp Piplup | Elite Four |

### Legends Arceus (2022, Hisui)

| Trainer | Tier |
|---|---|
| Volo La | Champion |
| Adaman La | Champion |
| Irida La | Champion |
| Ingo La | Champion |
| Akari La | Champion |
| Kamado La | Champion |
| Zisu La | Champion |
| Beni La | Champion |
| Rei La | Champion |

### SV (2022-23, Gen 9 Paldea + Teal Mask + Indigo Disk)

| Trainer | Tier |
|---|---|
| Nemona Quaquaval | Champion |
| Nemona Meowscarada | Champion |
| Nemona Skeledirge | Champion |
| Geeta | Champion |
| Rika | Elite Four |
| Poppy | Elite Four |
| Larry E4 | Elite Four |
| Hassel | Elite Four |
| Penny | Elite Four |
| Sada | Champion |
| Turo | Champion |
| Katy | Gym Leader |
| Brassius | Gym Leader |
| Iono | Gym Leader |
| Kofu | Gym Leader |
| Ryme | Gym Leader |
| Tulip | Gym Leader |
| Grusha | Gym Leader |
| Jacq | Elite Four |
| Raifort | Elite Four |
| Tyme | Elite Four |
| Salvatore | Elite Four |
| Dendra | Elite Four |
| Saguaro | Elite Four |
| Carmine | Elite Four |
| Crispin | Elite Four |
| Amarys | Elite Four |
| Lacey | Elite Four |
| Drayton | Elite Four |
| Kieran | Elite Four |
| Clavell Skeledirge | Elite Four |
| Clavell Quaquaval | Elite Four |
| Clavell Meowscarada | Elite Four |
| Miriam | Elite Four |
| Arven | Elite Four |
| Cyrano | Elite Four |

### Legends ZA (2025, Kalos Lumiose Royale)

| Trainer | Tier |
|---|---|
| Urbain | Champion |
| Taunie | Champion |
| Vinnie | Champion |
| Canari | Champion |
| Ivor | Champion |
| Corbeau | Champion |
| L Za | Champion |
| Naveen | Champion |
| Tarragon | Champion |
| Grisham | Champion |
| Jacinthe | Champion |
| Rintaro | Champion |
| Xavi | Champion |
| Yvon | Champion |
| Griselle | Champion |
| Gwynn | Champion |
| Lidia | Champion |
| Lebanne | Champion |
| Mani | Champion |
| Emma | Champion |
| Philippe | Champion |
| Josee | Champion |
| Andy | Champion |
| Korrina Za | Champion |

### Anime section

| Trainer | Tier |
|---|---|
| Ash World Champion | Champion |
| Alain | Champion |
| Ash Alola | Champion |
| Ash Kalos | Champion |
| Ash Sinnoh | Champion |
| Ash Hoenn | Champion |
| Ash Unova | Champion |
| Ash Indigo | Champion |
| Ash All | Champion |
| Paul Anime | Champion |
| Gary | Champion |

### VGC 2012 World Championships

| Trainer | Tier |
|---|---|
| Wolfe Vgc2012 | Champion |
| Ray Vgc2012 | Champion |
