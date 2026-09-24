# Setup guide for Star Fox 64 (Starship)

## Prerequisites

* [Archipelago](https://github.com/ArchipelagoMW/Archipelago/releases) 0.6.4 or newer.
* The Archipelago build of Starship, the PC port of Star Fox 64. Download the zip for your system
  (`Starship_Archipelago_<version>_Windows.zip`, `_Linux.zip` or `_Mac.zip`) from the
  [releases page](https://github.com/keraion/Starship/releases). Upstream Starship builds do not include
  Archipelago support.
* A Star Fox 64 US v1.1 ROM (md5 `741a94eee093c4c8684e66b89f8685e8`). Starship extracts the game assets
  from it on first launch; no ROM patching is needed.

## Installing the world

1. From the same release, download `star_fox_64_ss.apworld`.
2. Double-click it to have Archipelago install it, or copy it into your Archipelago install's
   `custom_worlds/` folder.

Use the apworld from the same release as your Starship build: Starship refuses seeds generated with a
different world version.

This world registers as the game **Star Fox 64 (Starship)**, so it installs alongside the emulator
world for Star Fox 64 without replacing it. Seeds from one cannot be played with the other.

## Creating your options file

Download `Starship_<version>_Default_Template.yaml` from the release, or create one from the Archipelago
Launcher with **Generate Template Options** (it writes to Archipelago's `Players/Templates` folder). Edit the options, set your `name:`, and keep the line
`game: Star Fox 64 (Starship)`.

## Joining a multiworld

1. Generate or join a room as usual with your Star Fox 64 (Starship) options yaml file.
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
