from BaseClasses import Location
from . import data
from .ids import location_name_to_id

name_to_id = {}
groups = {name: set(members) for name, members in data.location_groups.items()}

for name, value in location_name_to_id.items():
    if value > 0:
        name_to_id[name] = value

class StarFox64SSLocation(Location):
    game = "Star Fox 64 (Starship)"
