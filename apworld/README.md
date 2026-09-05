# Star Fox 64 apworld

`star_fox_64_ss/` is the Archipelago world for Star Fox 64. It started as the `ap/`
directory of [Auztin/AP-Star-Fox-64](https://github.com/Auztin/AP-Star-Fox-64)
(commit `58d691c`, upstream world version 0.4.1, MIT licensed; see
`star_fox_64_ss/LICENSE`) and is maintained here from now on as its own fork: this
copy is the source of truth for Starship, and it is versioned together with the
client that speaks its item, location and option IDs. The fork registers as the game
`Star Fox 64 (Starship)` and restarted its numbering at 0.1.0
(`star_fox_64_ss/version.py`), so it installs next to the emulator world and its
seeds are never confused with it. The upstream emulator client (`client.py`) and its
setup guide were dropped; the docs here describe Starship. Compatibility with the
upstream emulator world is not a goal; bump `version.py` whenever the IDs or slot
data change so mismatched seeds are refused rather than misread.

`template.yaml` is the player options template that matches this world version.

## Packaging

```
python3 tools/ap_package_apworld.py            # writes star_fox_64_ss.apworld
```

Copy the resulting `star_fox_64_ss.apworld` into your Archipelago install's
`custom_worlds/` directory. CI also uploads it as an artifact.

## Keeping the client in sync

The world data is the single source of truth for the game-side tables. Each level
region carries a `meta` block (`level`, `planet`, `medal_option`, `level_item`),
Mission Finished locations carry a `status` (`complete` / `accomplished` / `warp`),
freestanding items and checkpoints carry the `object_index` into the level's
object list, and level exits carry the `path` item they need. Python ignores these
keys; `tools/ap_gen_ids.py` turns them (plus `ids.py`, `version.py` and the
option classes in `options.py`) into `src/port/archipelago/ArchipelagoIds.h`,
`src/port/archipelago/game/ApTables.c`, and `src/port/archipelago/game/ApLogic.{h,c}`.
The last pair is the logic strings compiled to C: one predicate per rule, the
region graph, and reachability queries used by Starship's in-game tracker. The
rule language is the one `rules.py` accepts (item names, `(Item, count)` with an
integer or option as the count, option comparisons against choice names,
`and`/`or`/`not`, `true`/`false`). After changing the data, run:

```
python3 tools/ap_gen_ids.py
```

`tools/ap_gen_ids.py --check` fails if any generated file is out of date (CI runs it).

`tools/ap_logic_test/difftest.py` checks the compiled logic against the Python rules
by evaluating both on random option/item sets; it needs an Archipelago checkout
(`ARCHIPELAGO_DIR`, run it with that checkout's Python).
