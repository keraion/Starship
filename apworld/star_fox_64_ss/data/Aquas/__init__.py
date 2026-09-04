regions = {
  "Aquas": {
    "meta": {
      "level": "LEVEL_AQUAS",
      "planet": "PLANET_AQUAS",
      "medal_option": "medal_aquas",
      "level_item": "Aquas",
    },
    "locations": {
      "Aquas - Mission Complete": {
        "item": ["Zoness", "Aquas - Red Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Aquas - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
      "Aquas - Checkpoint": {
        "item": "Aquas - Checkpoint",
        "group": "Checkpoints",
        "logic": "true",
        "object_index": 0x00BD,
      },
    },
    "exits": {
      "Zoness": {
        "type": "Level",
        "path": "Aquas - Red Path",
        "logic": "LevelAccess == 'shuffle_paths' and AquasRedPath",
      },
    },
  },
}
