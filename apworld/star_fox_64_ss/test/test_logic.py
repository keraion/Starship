from . import StarFox64SSTestBase


class TestShuffleLevels(StarFox64SSTestBase):
    options = {
        "level_access": "shuffle_levels",
        "shuffle_starting_level": False,
        "shuffle_medals": True,
        "required_medals": 3,
        "victory_condition": "andross",
    }

    def test_level_needs_its_item(self) -> None:
        self.assertTrue(self.can_reach_region("Corneria"))
        self.assertFalse(self.can_reach_region("Meteo"))
        self.collect_by_name("Meteo")
        self.assertTrue(self.can_reach_region("Meteo"))

    def test_venom_2_needs_venom_and_medals(self) -> None:
        medals = [item for item in self.multiworld.itempool if item.name == "Medal"]
        self.collect_by_name("Venom")
        self.collect(medals[:2])
        self.assertFalse(self.can_reach_region("Venom 2"))
        self.collect(medals[2])
        self.assertTrue(self.can_reach_region("Venom 2"))


class TestShufflePaths(StarFox64SSTestBase):
    options = {
        "level_access": "shuffle_paths",
    }

    def test_path_needs_its_item(self) -> None:
        self.assertTrue(self.can_reach_region("Corneria"))
        self.assertFalse(self.can_reach_region("Meteo"))
        self.collect_by_name("Corneria - Blue Path")
        self.assertTrue(self.can_reach_region("Meteo"))

    def test_level_items_are_not_in_pool(self) -> None:
        pool = {item.name for item in self.multiworld.itempool}
        self.assertNotIn("Meteo", pool)
        self.assertIn("Corneria - Blue Path", pool)


class TestShuffleStartingLevel(StarFox64SSTestBase):
    options = {
        "level_access": "shuffle_levels",
        "shuffle_starting_level": True,
    }

    def test_starting_level_swaps_with_corneria(self) -> None:
        start = [item.name for item in self.multiworld.precollected_items[self.player]]
        self.assertEqual(len(start), 1)
        self.assertNotEqual(start[0], "Venom")
        self.assertTrue(self.can_reach_region(start[0]))  # level items are named after their region
        pool = {item.name for item in self.multiworld.itempool}
        self.assertNotIn(start[0], pool)
        if start[0] != "Corneria":  # the shuffle can land on Corneria itself
            self.assertIn("Corneria", pool)


class TestAllMedalsClamped(StarFox64SSTestBase):
    options = {
        "shuffle_medals": False,
        "required_medals": 15,
        "victory_condition": "andross_or_robot_andross",
    }

    def test_required_medals_clamped_to_14(self) -> None:
        self.assertEqual(self.world.options.required_medals.value, 14)
