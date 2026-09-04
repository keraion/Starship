#!/usr/bin/env python3
"""Package apworld/star_fox_64_ss into a star_fox_64_ss.apworld zip.

Usage: tools/ap_package_apworld.py [OUTPUT]   (default: ./star_fox_64_ss.apworld)
"""
import importlib.util
import json
import os
import sys
import zipfile

HERE = os.path.dirname(os.path.abspath(__file__))
WORLD = os.path.normpath(os.path.join(HERE, "..", "apworld", "star_fox_64_ss"))
NAME = "star_fox_64_ss"
GAME = "Star Fox 64"
MINIMUM_AP_VERSION = "0.6.3"


def world_version():
    """version.py is plain Python (no Archipelago imports), so it can be loaded directly."""
    sys.dont_write_bytecode = True
    spec = importlib.util.spec_from_file_location("sf64_version", os.path.join(WORLD, "version.py"))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod.version.as_string()


def manifest():
    """archipelago.json as read by worlds/Files.py APWorldContainer (Archipelago >= 0.6.x; required from 0.7)."""
    return {
        "game": GAME,
        "world_version": world_version(),
        "minimum_ap_version": MINIMUM_AP_VERSION,
        "compatible_version": 7,
        "version": 7,
    }


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else NAME + ".apworld"
    if not os.path.isfile(os.path.join(WORLD, "__init__.py")):
        raise SystemExit("world package not found at %s" % WORLD)
    count = 0
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as z:
        for root, dirs, files in os.walk(WORLD):
            dirs[:] = sorted(d for d in dirs if d != "__pycache__")
            for f in sorted(files):
                if f.endswith((".pyc", ".pyo")):
                    continue
                full = os.path.join(root, f)
                rel = os.path.join(NAME, os.path.relpath(full, WORLD))
                z.write(full, rel)
                count += 1
        z.writestr(os.path.join(NAME, "archipelago.json"), json.dumps(manifest(), indent=2) + "\n")
        count += 1
    print("wrote %s (%d files)" % (out, count))


if __name__ == "__main__":
    main()
