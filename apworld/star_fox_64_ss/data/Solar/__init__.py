regions = {
  "Solar": {
    "meta": {
      "level": "LEVEL_SOLAR",
      "planet": "PLANET_SOLAR",
      "medal_option": "medal_solar",
      "level_item": "Solar",
    },
    "locations": {
      "Solar - Mission Complete": {
        "item": ["Nothing", "Solar - Yellow Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Solar - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
      "Solar - Checkpoint": {
        "item": "Solar - Checkpoint",
        "group": "Checkpoints",
        "logic": "true",
        "object_index": 0x005C,
      },
    },
    "exits": {
      "Macbeth": {
        "type": "Level",
        "path": "Solar - Yellow Path",
        "logic": "LevelAccess == 'shuffle_paths' and SolarYellowPath",
      },
    },
  },
}
