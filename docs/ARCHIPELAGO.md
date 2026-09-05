# Archipelago (multiworld randomizer) support

Starship can act as a native client for the [Archipelago](https://archipelago.gg)
**Star Fox 64 (Starship)** world. No emulator, ROM patch or Python client is needed: Starship
connects to the multiworld server itself.

## Requirements

- A build with `ENABLE_ARCHIPELAGO=ON` (the default on Windows, Linux and macOS;
  not available on Switch / Wii U).
- A multiworld generated with this repository's `star_fox_64_ss.apworld`
  (world version 0.1.0, see `apworld/star_fox_64_ss/version.py`; the fork restarted its numbering at 0.1.0 so seeds are never confused with the emulator world). The world is
  vendored in `apworld/star_fox_64_ss` and maintained there as its own fork of
  Auztin's AP-Star-Fox-64 world. It registers as the game `Star Fox 64 (Starship)`
  (that is the `game:` line in player yamls) so it can be installed next to the
  emulator world and never shares a room slot with it; `apworld/README.md` explains packaging and how
  the game-side tables are generated from it. Starship refuses seeds generated
  with a different world version.
- `networking/cacert.pem` next to the executable (copied automatically by the
  build) for `wss://` rooms such as `archipelago.gg`.

## Connecting

1. Open the menu bar (F1) and pick **Archipelago > Connection**.
2. Enter the server (`host:port`, for example `archipelago.gg:38281`), your slot
   name and the room password (if any).
3. Press **Connect**. Once the slot data has been applied the game restarts on
   the per-seed save. The main menu shows `AP CONNECTED`; starting *Main Game*
   or *Training* is blocked while disconnected (hold Z + R to bypass).

The first connection attempt gives up after 30 seconds; `wss://` is tried first
and plain `ws://` second, so a local room without TLS takes a moment longer.

Each seed/slot has its own save file under `<app dir>/archipelago/`
(`<seed>_<team>_<slot>.json`). It stores the received items, checked locations,
options and the persisted game state (lives, lasers, bombs, wingman shields,
Star Wolf status) as well as the vanilla EEPROM, so the regular `default.sav`
is never touched while a session is active. **End session** in the connection
window returns to the vanilla save.

The **Archipelago** menu also opens two more windows:

- **Console**: server messages, chat and server commands (`!hint`, `!release`, ...).
- **Tracker**: every unchecked location the seed's logic considers reachable
  with the items you have, grouped by region, with totals. **Show checked
  locations** adds the ones already sent. The logic is the world's own rules
  compiled to C, so it matches what the generator used.

## Gameplay changes while connected

- **Map**: no forced progression. After a level you stay on the map. Use the
  D-pad: **Right** moves toward Venom along an owned path, **Left** goes back,
  **Up/Down** switch between the paths leaving the previous planet. **A**
  starts the selected level (only if you have access), **START** opens the heal
  menu (spend one Arwing to give a wingman 128 shields), **B** returns to the
  main menu.
- **Map tracker**: coloured lines are paths you can take, grey lines are locked.
  A star on a planet means you can play it; a medal glyph appears once the
  medal location is checked. The medal icon in the top right shows the medal
  count toward Venom (`x OF y`), and `CHECKS: n` under it counts the selected
  planet's unchecked locations that are in logic right now.
- **Pause menu**: **Continue**, **Retry Course**, **Respawn** (lose an Arwing
  and restart at the last checkpoint) and **Back to Map** (leave the level
  without a clear; nothing is sent). Training only has Continue and Quit. The
  seed's hit requirement for the level's medal is shown next to the menu.
- **Locations**: Mission Complete / Accomplished / Warp, medals (thresholds
  come from the seed's options), freestanding rings / bombs / laser upgrades,
  the items specific enemies drop when shot (about a third of the item
  locations, for example the Silver Ring in the middle of Corneria City), and
  checkpoint rings when shuffled. An unchecked item location appears as a
  spinning gold ring with a star inside, whether it is placed in the level or
  dropped by an enemy; collecting it sends the check. Checked ones are the
  vanilla pickups again.
- **Items**: level / path unlocks and checkpoints are applied on the map,
  consumables (laser upgrades, rings, stars, bombs, extra Arwings) are granted
  while flying. Received items are announced in the corner of the screen.
- **Venom**: Bolse and Area 6 return to the map instead of chaining into Venom;
  Venom is selected from the map like any other planet (Area 6 route enters
  Venom 2) once you hold the required number of medals. The ending plays only
  when the seed's victory condition is met.

## Not supported yet

DeathLink and RingLink, radio message shuffle, the engine glow and AP logo
models of the ROM hack, and the world's trap / Extra Arwing / cameo options.
Cosmetic shuffles belong in Starship's own enhancements rather than the client.

## Console commands (developer console)

```
ap connect | disconnect | end | status
ap say <text>
ap check <locationId>      # debug: send a location check
ap give <itemId> [count]   # debug: add to the local item count
```

## Building

The client uses [apclientpp](https://github.com/black-sliver/apclientpp) with
wswrap / websocketpp / standalone asio (git submodules under `subprojects/`)
and links OpenSSL + zlib. The apclient translation unit is compiled as its own
C++17 static library (`StarshipAPTransport`) because websocketpp does not build
under GCC's C++20 mode.

```
git submodule update --init --recursive
cmake -B build -DENABLE_ARCHIPELAGO=ON
cmake --build build
```

With `ENABLE_ARCHIPELAGO=OFF` the game-side code still compiles against a stub
bridge, so the decompiled hooks stay identical in both configurations.

### Generated code

The world data is the source of truth. `python3 tools/ap_gen_ids.py` produces:

- `src/port/archipelago/ArchipelagoIds.h`: item / location / option IDs, names
  and option choice enums.
- `src/port/archipelago/game/ApTables.c`: object indices per level, mission
  clear and medal locations, planet / level / path item tables.
- `src/port/archipelago/game/ApLogic.{h,c}`: the access rules compiled to C
  plus the region graph, used by the tracker.

CI runs `tools/ap_gen_ids.py --check` and packages the world with
`tools/ap_package_apworld.py`. `tools/ap_logic_test/difftest.py` evaluates the
compiled logic against the Python rules on random option / item sets (needs an
Archipelago checkout, see `apworld/README.md`).

### Testing on Windows from WSL

`docs/WSL_WINDOWS_TESTING.md` describes the mirror-and-build loop used to run
the native Windows build while developing in WSL, and what to copy so the
Windows executable joins the same seed.

## Code map

| Path | Purpose |
|---|---|
| `src/port/archipelago/transport/` | C++17 wrapper around apclientpp (`APTransport`) |
| `src/port/archipelago/Archipelago.*` | session state machine, sync, item/location bookkeeping |
| `src/port/archipelago/ArchipelagoSave.*` | per-slot JSON save file |
| `src/port/archipelago/ArchipelagoBridge.h`, `ArchipelagoStub.cpp` | C API used by the game code (`AP_*`), and its no-op version for `ENABLE_ARCHIPELAGO=OFF` |
| `src/port/archipelago/ArchipelagoWindow.*`, `ArchipelagoConsole.*`, `ArchipelagoTracker.*` | ImGui windows: connection, console, tracker |
| `src/port/archipelago/game/ApItems.c` | item locations: static pickups and enemy drops become AP orbs, consumable grants |
| `src/port/archipelago/game/ApMission.c` | mission clear / medal / Venom locations, goal, run stats |
| `src/port/archipelago/game/ApMap.c` | map navigation, path and planet tracker, heal menu, HUD |
| `src/port/archipelago/game/ApMenu.c`, `ApPause.c` | main menu gating and the pause menu |
| `src/port/archipelago/game/ApTables.c`, `ApLogic.*` | generated tables and compiled logic |
| `src/port/hooks/list/MapEvent.h`, `ItemEvent.h`, `EngineEvent.h` | events fired from the decompiled map, menu, item and pause code |
| `apworld/star_fox_64_ss/` | the Archipelago world itself |
| `tools/ap_gen_ids.py`, `ap_package_apworld.py`, `ap_logic_test/` | generator, packager, logic diff test |
