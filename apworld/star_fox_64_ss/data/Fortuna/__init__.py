regions = {
  "Fortuna": {
    "meta": {
      "level": "LEVEL_FORTUNA",
      "planet": "PLANET_FORTUNA",
      "medal_option": "medal_fortuna",
      "level_item": "Fortuna",
    },
    "locations": {
      "Fortuna - Mission Complete": {
        "item": ["Sector X", "Fortuna - Blue Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Fortuna - Mission Accomplished": {
        "item": ["Solar", "Fortuna - Yellow Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "accomplished",
      },
      "Fortuna - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
    },
    "exits": {
      "Sector X": {
        "type": "Level",
        "path": "Fortuna - Blue Path",
        "logic": "LevelAccess == 'shuffle_paths' and FortunaBluePath",
      },
      "Solar": {
        "type": "Level",
        "path": "Fortuna - Yellow Path",
        "logic": "LevelAccess == 'shuffle_paths' and FortunaYellowPath",
      },
    },
  },
}
