import logging
from typing import Any

import Options
from worlds.AutoWorld import World, WebWorld
from Options import OptionGroup
from BaseClasses import Entrance, Location, Region, Tutorial

from . import options, locations, items, data
from .options import StarFox64SSOptions
from .locations import StarFox64SSLocation
from .items import StarFox64SSItem
from .rules import StarFox64SSRules
from .ids import option_name_to_id


class StarFox64SSWebWorld(WebWorld):
    rich_text_options_doc = True
    tutorials = [
        Tutorial(
            "Setup Guide",
            "A guide to playing Star Fox 64 with Archipelago through Starship.",
            "English",
            "setup_en.md",
            "setup/en",
            ["keraion"],
        )
    ]
    option_groups = [
        OptionGroup(
            "Common Options",
            [
                Options.ProgressionBalancing,
                Options.Accessibility,
                options.DeathLink,
                options.RingLink,
            ],
        ),
        OptionGroup(
            "Goal Options",
            [
                options.VictoryCondition,
                options.RequiredMedals,
            ],
        ),
        OptionGroup(
            "Shuffle Options",
            [
                options.LevelAccess,
                options.ShuffleStartingLevel,
                options.ShuffleMedals,
                options.ShuffleCheckpoints,
            ],
        ),
        OptionGroup(
            "Speedup Options",
            [
                options.AccomplishedSendsComplete,
            ],
        ),
        OptionGroup(
            "Vanity Options",
            [
                options.RadioRando,
                options.EngineGlow,
            ],
        ),
        OptionGroup(
            "Accessibility Options",
            [
                options.DefaultLives,
                options.MedalCorneria,
                options.MedalMeteo,
                options.MedalSectorY,
                options.MedalKatina,
                options.MedalFortuna,
                options.MedalAquas,
                options.MedalSolar,
                options.MedalSectorX,
                options.MedalZoness,
                options.MedalTitania,
                options.MedalSectorZ,
                options.MedalMacbeth,
                options.MedalArea6,
                options.MedalBolse,
                options.MedalVenom,
            ],
        ),
    ]


class StarFox64SSWorld(World):
    """
    Star Fox 64 is a 3D rail shooter game in which the player controls one of the vehicles piloted by Fox McCloud,
    usually an Arwing.
    """

    game = "Star Fox 64 (Starship)"
    options_dataclass = StarFox64SSOptions
    options: StarFox64SSOptions
    item_name_to_id = items.name_to_id
    location_name_to_id = locations.name_to_id
    item_name_groups = items.groups
    location_name_groups = locations.groups
    topology_present = True
    web = StarFox64SSWebWorld()
    filler_weights = {
        "Silver Ring": 50,
        "Silver Star": 25,
        "Laser Upgrade": 9.5,
        "Bomb": 9.5,
        "Gold Ring": 6,
    }

    def generate_early(self) -> None:
        if (
            not self.options.shuffle_medals
            and self.options.required_medals == 15
            and self.options.victory_condition == "andross_or_robot_andross"
        ):
            logging.warning(
                f"{self.game} player {self.player} ({self.player_name}): "
                "Wants all Medals to access Venom and wants Venom to have a Medal. Forcing required_medals to 14."
            )
            self.options.required_medals.value = 14

        self.swap_items: dict[str, str] = {}
        if self.options.shuffle_starting_level:
            valid_levels = data.item_groups["Levels"].copy()
            valid_levels.remove("Venom")
            item_name = self.random.choice(valid_levels)
            self.swap_items["Corneria"] = item_name
            self.swap_items[item_name] = "Corneria"

        # After the clamp above: the parser snapshots option values.
        self.rules = StarFox64SSRules(self)

    def create_item(self, item_name: str) -> StarFox64SSItem:
        return items.create_item(self, item_name)

    def create_victory_condition(self) -> None:
        condition = lambda state: False
        andross = "Defeated Andross"
        robot_andross = "Defeated Robot Andross"
        match self.options.victory_condition:
            case "andross_or_robot_andross":
                condition = lambda state: state.has_any(
                    [andross, robot_andross], self.player
                )
            case "andross_and_robot_andross":
                condition = lambda state: state.has_all(
                    [andross, robot_andross], self.player
                )
            case "andross":
                condition = lambda state: state.has(andross, self.player)
        self.multiworld.completion_condition[self.player] = condition

    def create_regions(self) -> None:
        ap_regions = {name: Region(name, self.player, self.multiworld) for name in data.regions}
        # (location or entrance, logic, where), applied in set_rules
        self.pending_rules: list[tuple[Location | Entrance, str, str]] = []
        self.pending_items: list[tuple[StarFox64SSLocation, str]] = []  # (location, item name), filled in create_items
        self.start_items: list[str] = []  # Menu "locations" are option-only grants, precollected in create_items
        for region_name, region in data.regions.items():
            ap_region = ap_regions[region_name]
            for location_name, location in region.get("locations", {}).items():
                item_name = items.pick_name(self, location["item"], location.get("group"))
                item_name = self.swap_items.get(item_name, item_name)
                if region_name == "Menu":
                    if self.rules.parse(location["logic"], f"{self.game}, Location: Menu -> {location_name}")(None):
                        self.start_items.append(item_name)
                    continue
                ap_location = StarFox64SSLocation(self.player, location_name, None, ap_region)
                if not items.is_event(self, data.items[item_name].get("type", item_name)):
                    ap_location.address = self.location_name_to_id[location_name]
                ap_region.locations.append(ap_location)
                self.pending_items.append((ap_location, item_name))
                self.pending_rules.append(
                    (ap_location, location["logic"], f"{self.game}, Location: {region_name} -> {location_name}")
                )
            for exit_name, _exit in region.get("exits", {}).items():
                entrance = ap_region.connect(ap_regions[exit_name])
                self.pending_rules.append(
                    (entrance, _exit["logic"], f"{self.game}, Exit: {region_name} -> {exit_name}")
                )
        self.multiworld.regions += ap_regions.values()

    def create_items(self) -> None:
        for item_name in self.start_items:
            self.push_precollected(self.create_item(item_name))
        for ap_location, item_name in self.pending_items:
            if item_name == "Nothing":
                item_name = self.get_filler_item_name()
            item = self.create_item(item_name)
            # create_regions picked the address from the same event check create_item uses.
            assert (item.code is None) == (ap_location.address is None), ap_location.name
            if item.code:
                self.multiworld.itempool.append(item)
            else:
                ap_location.place_locked_item(item)

    def set_rules(self) -> None:
        self.create_victory_condition()
        for spot, logic, where in self.pending_rules:
            spot.access_rule = self.rules.parse(logic, where)

    def get_filler_item_name(self) -> str:
        return self.random.choices(
            list(self.filler_weights.keys()), self.filler_weights.values()
        )[0]

    def fill_slot_data(self) -> dict[str, Any]:
        return {
            "options": self.options.as_dict(*option_name_to_id.keys()),
            "version": (self.world_version.major << 16) | (self.world_version.minor << 8) | self.world_version.build,
        }
