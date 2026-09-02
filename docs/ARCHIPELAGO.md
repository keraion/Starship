# Archipelago (multiworld randomizer) support

Starship can act as a native client for the [Archipelago](https://archipelago.gg)
**Star Fox 64** world (`AP-Star-Fox-64`). No emulator, ROM patch or Python
client is needed: Starship connects to the multiworld server itself.

## Requirements

- A build with `ENABLE_ARCHIPELAGO=ON` (the default on Windows, Linux and macOS;
  not available on Switch / Wii U).
- A multiworld generated with the `star_fox_64.apworld` (version 0.4.1). The same
  seed works on emulator and on Starship.
- `networking/cacert.pem` next to the executable (copied automatically by the
  build) for `wss://` rooms such as `archipelago.gg`.

## Connecting

1. Open the menu bar (F1) and pick **Archipelago > Connection**.
2. Enter the server (`host:port`, for example `archipelago.gg:38281`), your slot
   name and the room password (if any).
3. Press **Connect**. Once the slot data has been applied the game restarts on
   the per-seed save. The main menu shows `AP CONNECTED`; starting *Main Game*
   or *Training* is blocked while disconnected (hold Z + R to bypass).

Each seed/slot has its own save file under `<app dir>/archipelago/`
(`<seed>_<team>_<slot>.json`). It stores the received items, checked locations,
options and the persisted game state (lives, lasers, bombs, wingman shields,
Star Wolf status) as well as the vanilla EEPROM, so the regular `default.sav`
is never touched while a session is active. **End session** in the connection
window returns to the vanilla save.

The **Archipelago > Console** window shows server messages and lets you chat
or use server commands (`!hint`, `!release`, ...).

## Gameplay changes while connected

- **Map**: no forced progression. After a level you stay on the map. Use the
  D-pad: **Right** moves toward Venom along an owned path, **Left** goes back,
  **Up/Down** switch between the paths leaving the previous planet. **A**
  starts the selected level (only if you have access), **START** opens the heal
  menu (spend one Arwing to give a wingman 128 shields), **B** returns to the
  main menu.
- **Tracker**: coloured lines are paths you can take, grey lines are locked;
  an animated Arwing on a path means that clear is still unchecked, a parked
  Arwing means it was sent. A medal glyph appears once the medal location is
  checked. The medal counter (`MEDALS x OF y`) shows progress toward Venom.
- **Locations**: Mission Complete / Accomplished / Warp, medals (thresholds come
  from the seed's options), freestanding rings/bombs/laser upgrades and
  checkpoint rings (when shuffled). Unchecked freestanding items appear as a
  spinning gold ring with a star inside; collecting one sends the check.
- **Items**: level / path unlocks and checkpoints are applied on the map,
  consumables (laser upgrades, rings, stars, bombs, extra Arwings) are granted
  while flying.
- **Venom**: Bolse and Area 6 return to the map instead of chaining into Venom;
  Venom is selected from the map like any other planet (Area 6 route enters
  Venom 2). The ending plays only when the seed's victory condition is met.

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

Item, location and option IDs are generated from the apworld:
`python3 tools/ap_gen_ids.py --apworld ../AP-Star-Fox-64` (use `--check` in CI).

## Code map

| Path | Purpose |
|---|---|
| `src/port/archipelago/transport/` | C++17 wrapper around apclientpp (`APTransport`) |
| `src/port/archipelago/Archipelago.*` | session state machine, sync, item/location bookkeeping |
| `src/port/archipelago/ArchipelagoSave.*` | per-slot JSON save file |
| `src/port/archipelago/ArchipelagoBridge.h` | C API used by the game code (`AP_*`) |
| `src/port/archipelago/ArchipelagoWindow.*`, `ArchipelagoConsole.*` | ImGui UI |
| `src/port/archipelago/game/` | game-side listeners (items, missions, map, menu) |
| `src/port/hooks/list/MapEvent.h` | new events fired from the decompiled map / menu code |
