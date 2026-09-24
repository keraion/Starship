import importlib

from . import Game
from .Game import items

regions = {}

for module_name in Game.region_modules:
    module = importlib.import_module(f".{module_name}", __name__)
    for region_name, region in module.regions.items():
        if region_name in regions:
            raise Exception(f"Duplicated Region from {module_name}: '{region_name}'")
        regions[region_name] = region

# Groups come from the data itself: an item's "type" and a location's "group" (lists, in data order).
item_groups = {}
for item_name, item in items.items():
    if "type" in item:
        item_groups.setdefault(item["type"], []).append(item_name)

location_groups = {}
for region in regions.values():
    for location_name, location in region.get("locations", {}).items():
        if "group" in location:
            location_groups.setdefault(location["group"], []).append(location_name)
