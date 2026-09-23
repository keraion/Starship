#!/usr/bin/env python3
"""Package apworld/star_fox_64_ss into a star_fox_64_ss.apworld zip.

Same result as the Archipelago launcher's "Build APWorlds", without needing an Archipelago install.

Usage: tools/ap_package_apworld.py [OUTPUT]   (default: ./star_fox_64_ss.apworld)
"""
import json
import sys
import zipfile
from pathlib import Path

WORLD = Path(__file__).resolve().parent.parent / "apworld" / "star_fox_64_ss"
APCONTAINER_VERSION = 7  # worlds/Files.py container_version; required in a packaged manifest


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else f"{WORLD.name}.apworld"
    manifest_path = WORLD / "archipelago.json"
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    manifest |= {"version": APCONTAINER_VERSION, "compatible_version": APCONTAINER_VERSION}
    files = sorted(p for p in WORLD.rglob("*")
                   if p.is_file() and p != manifest_path and "__pycache__" not in p.parts)
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as z:
        for p in files:
            z.write(p, p.relative_to(WORLD.parent).as_posix())
        z.writestr(f"{WORLD.name}/archipelago.json", json.dumps(manifest, indent=2) + "\n")
    print(f"wrote {out} ({len(files) + 1} files)")


if __name__ == "__main__":
    main()
