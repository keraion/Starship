regions = {
  "Katina": {
    "meta": {
      "level": "LEVEL_KATINA",
      "planet": "PLANET_KATINA",
      "medal_option": "medal_katina",
      "level_item": "Katina",
    },
    "locations": {
      "Katina - Mission Complete": {
        "item": ["Nothing", "Katina - Blue Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Katina - Mission Accomplished": {
        "item": ["Nothing", "Katina - Yellow Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "accomplished",
      },
      "Katina - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
    },
    "exits": {
      "Sector X": {
        "type": "Level",
        "path": "Katina - Blue Path",
        "logic": "LevelAccess == 'shuffle_paths' and KatinaBluePath",
      },
      "Solar": {
        "type": "Level",
        "path": "Katina - Yellow Path",
        "logic": "LevelAccess == 'shuffle_paths' and KatinaYellowPath",
      },
    },
  },
}
