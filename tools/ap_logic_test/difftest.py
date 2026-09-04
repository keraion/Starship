"""Differential test: the world's Python rules (rules.py, run under Archipelago) vs the compiled C logic.

Usage (from an Archipelago checkout's venv, ARCHIPELAGO_DIR pointing at the checkout):
    ARCHIPELAGO_DIR=~/repos/Archipelago-0.6.7 python3 tools/ap_logic_test/difftest.py

It repackages apworld/star_fox_64_ss into the checkout's custom_worlds/, builds a small C harness
around the generated src/port/archipelago/game/ApLogic.c with stubbed AP_* accessors, and compares
region reachability and per-location in-logic results for random option/item sets.
"""
import os, random, subprocess, sys, importlib, shutil, tempfile
AP = os.environ.get("ARCHIPELAGO_DIR", os.path.expanduser("~/repos/Archipelago-0.6.7"))
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", ".."))

# 1. repackage the world so Archipelago loads the current data
subprocess.run([sys.executable, os.path.join(REPO, "tools", "ap_package_apworld.py"),
                os.path.join(AP, "custom_worlds", "star_fox_64_ss.apworld")], check=True)
# 2. build the harness (ApLogic.c compiled against the stub ApGame.h in this directory)
BUILD = tempfile.mkdtemp(prefix="ap_logic_test_")
for f in ("ApLogic.c", "ApLogic.h"):
    shutil.copy(os.path.join(REPO, "src", "port", "archipelago", "game", f), BUILD)
shutil.copy(os.path.join(HERE, "ApGame.h"), BUILD)
HARNESS = os.path.join(BUILD, "logictest")
subprocess.run(["cc", "-std=c11", "-I", BUILD, "-I", os.path.join(REPO, "src"),
                "-I", os.path.join(REPO, "src", "port", "archipelago"), "-o", HARNESS,
                os.path.join(HERE, "main.c"), os.path.join(BUILD, "ApLogic.c")], check=True)
sys.path.insert(0, AP)
os.chdir(AP)
import Options  # loads custom_worlds/star_fox_64_ss.apworld (repackaged from the repo just before this run)
from worlds.star_fox_64_ss import data, ids
from worlds.star_fox_64_ss.rules import StarFox64SSRules
from worlds.star_fox_64_ss.options import StarFox64SSOptions

option_names = list(ids.option_name_to_id.keys())
item_names = [n for n, i in ids.item_name_to_id.items() if i > 0]
loc_ids = ids.location_name_to_id
region_names = list(data.regions.keys())

class FakeState:
    def __init__(self, counts): self.c = counts
    def has(self, item, player, count=1): return self.c.get(item, 0) >= count
    def has_all(self, items, player): return all(self.has(i, player) for i in items)
    def has_any(self, items, player): return any(self.has(i, player) for i in items)
    def has_all_counts(self, d, player): return all(self.has(i, player, n) for i, n in d.items())
    def has_any_count(self, d, player): return any(self.has(i, player, n) for i, n in d.items())
    def count(self, item, player): return self.c.get(item, 0)

class FakeWorld:
    player = 1
    def __init__(self, values):
        opts = {}
        for name, cls in StarFox64SSOptions.type_hints.items():
            opts[name] = cls(values[name]) if name in values else cls(cls.default)
        self.options = StarFox64SSOptions(**opts)

def python_eval(values, counts):
    world = FakeWorld(values)
    parser = StarFox64SSRules(world)
    state = FakeState(counts)
    rules = {}
    for rn, r in data.regions.items():
        for en, ex in r.get("exits", {}).items():
            rules[("exit", rn, en)] = parser.parse(ex["logic"], f"{rn}->{en}")
        for ln, loc in r.get("locations", {}).items():
            rules[("loc", rn, ln)] = parser.parse(loc["logic"], f"{rn}:{ln}")
    reach = {"Menu"}
    changed = True
    while changed:
        changed = False
        for (kind, rn, en), fn in rules.items():
            if kind == "exit" and rn in reach and en not in reach and fn(state):
                reach.add(en); changed = True
    regions = [1 if rn in reach else 0 for rn in region_names]
    locs = []
    for lid in range(1, max(loc_ids.values()) + 1):
        name = next((n for n, i in loc_ids.items() if i == lid), None)
        ok = 0
        if name:
            for (kind, rn, ln), fn in rules.items():
                if kind == "loc" and ln == name and rn != "Menu":
                    ok = 1 if (rn in reach and fn(state)) else 0
        locs.append(ok)
    return regions, locs

rng = random.Random(64)
cases = []
for _ in range(400):
    values = {"level_access": rng.choice([0, 1]), "victory_condition": rng.choice([0, 1, 2]),
              "required_medals": rng.choice([0, 1, 3, 8, 15]), "shuffle_medals": rng.choice([0, 1])}
    counts = {n: 1 for n in item_names if rng.random() < rng.choice([0.2, 0.5, 0.9])}
    counts["Medal"] = rng.choice([0, 0, 1, 2, 5, 15])
    cases.append((values, counts))

# the C harness feeds AP_ITEM_MAX(61) ints: index by item id, "None" (-1) is excluded so ids 1..60 (+ slot 0)
item_by_id = {i: n for n, i in ids.item_name_to_id.items() if i >= 0}
def c_input(values, counts):
    line = ["1"] + [str(values.get(n, 0)) for n in option_names]
    line += [str(counts.get(item_by_id.get(i), 0)) for i in range(61)]
    return " ".join(line)
out = subprocess.run([HARNESS], input="\n".join(c_input(v, c) for v, c in cases) + "\n", capture_output=True, text=True, check=True).stdout
c_results = []
for l in out.strip().splitlines():
    r, ls = l[1:].split(" L")
    c_results.append(([int(x) for x in r.split()], [int(x) for x in ls.split()]))

mismatch = 0
for (values, counts), (cr, cl) in zip(cases, c_results):
    pr, pl = python_eval(values, counts)
    if pr != cr or pl != cl:
        mismatch += 1
        if mismatch <= 3:
            print("MISMATCH", values, {k: v for k, v in counts.items() if v})
            print("  py regions", pr, "\n  c  regions", cr)
            diffs = [i + 1 for i, (a, b) in enumerate(zip(pl, cl)) if a != b]
            print("  location diffs (ids):", diffs[:20])
print(f"{len(cases)} cases, {mismatch} mismatches")
sys.exit(1 if mismatch else 0)
