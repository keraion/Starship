regions = {
  "Area 6": {
    "meta": {
      "level": "LEVEL_AREA_6",
      "planet": "PLANET_AREA_6",
      "medal_option": "medal_area_6",
      "level_item": "Area 6",
    },
    "locations": {
      "Area 6 - Mission Complete": {
        "item": ["Nothing", "Area 6 - Red Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Area 6 - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
      "Area 6 - Below Early Defense Station: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00A3,
      },
      "Area 6 - Above Early Defense Station: Laser Upgrade": {
        "item": "Laser Upgrade",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00A4,
      },
      "Area 6 - Andross' Taunt: Gold Ring": {
        "item": "Laser Upgrade",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00E3,
      },
      "Area 6 - Checkpoint": {
        "item": "Area 6 - Checkpoint",
        "group": "Checkpoints",
        "logic": "true",
        "object_index": 0x00FA,
      },
      "Area 6 - Near Defense Station After Checkpoint: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0115,
      },
    },
    "exits": {
      "Venom 2": {
        "type": "Level",
        "path": "Area 6 - Red Path",
        "logic": "LevelAccess == 'shuffle_paths' and Area6RedPath and (Medal, RequiredMedals)",
      },
    },
  },
}
