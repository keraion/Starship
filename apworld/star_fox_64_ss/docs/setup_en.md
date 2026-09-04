# Setup guide for Star Fox 64 (Starship)

## Prerequisites

* [Starship](https://github.com/HarbourMasters/Starship), the PC port of Star Fox 64, built with
  Archipelago support (the default on Windows, Linux and macOS builds).
* A Star Fox 64 US v1.1 ROM (md5 `741a94eee093c4c8684e66b89f8685e8`). Starship extracts the game
  assets from it on first launch; no ROM patching is needed.
* `star_fox_64.apworld` placed in your Archipelago install's `custom_worlds/` folder (or double-click
  it to have Archipelago install it). The Starship release that matches this world version ships it.

## Joining a multiworld

1. Generate or join a room as usual with your Star Fox 64 options yaml.
2. Launch Starship and open the menu bar (`F1`), then **Archipelago > Connection**.
3. Enter the server (`host:port`, for example `archipelago.gg:38281`), your slot name and the room
   password if there is one, then press **Connect**.
4. Once the slot data has been applied the game restarts on a save dedicated to that seed and slot.
   The main menu shows `AP CONNECTED`; start **Main Game** to begin.

Server messages and chat are in **Archipelago > Console**. Item pickups, sends and connection
changes also appear as on-screen notifications.

## Notes

* Each seed and slot has its own save file under Starship's `archipelago/` folder, so the vanilla save is
  never touched while connected.
* If the room is on `archipelago.gg` (or any `wss://` host), keep the `networking/cacert.pem` file that
  ships next to the Starship executable.
* Reconnecting resends any checks made while offline.
