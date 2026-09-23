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
