regions = {
  "Zoness": {
    "meta": {
      "level": "LEVEL_ZONESS",
      "planet": "PLANET_ZONESS",
      "medal_option": "medal_zoness",
      "level_item": "Zoness",
    },
    "locations": {
      "Zoness - Mission Complete": {
        "item": ["Nothing", "Zoness - Yellow Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Zoness - Mission Accomplished": {
        "item": ["Nothing", "Zoness - Red Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "accomplished",
      },
      "Zoness - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
      "Zoness - Before Katt Appears, Top: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0076,
      },
      "Zoness - Before Katt Appears, Middle: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0077,
      },
      "Zoness - Before Katt Appears, Bottom: Bomb": {
        "item": "Bomb",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0073,
      },
      "Zoness - Checkpoint": {
        "item": "Zoness - Checkpoint",
        "group": "Checkpoints",
        "logic": "true",
        "object_index": 0x010F,
      },
      "Zoness - Aim For The Rudder, First Gate: Laser Upgrade": {
        "item": "Laser Upgrade",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0121,
      },
      "Zoness - Aim For The Rudder, Second Gate, Left: Bomb": {
        "item": "Bomb",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x012D,
      },
      "Zoness - Aim For The Rudder, Third Gate: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0133,
      },
    },
    "exits": {
      "Macbeth": {
        "type": "Level",
        "path": "Zoness - Yellow Path",
        "logic": "LevelAccess == 'shuffle_paths' and ZonessYellowPath",
      },
      "Sector Z": {
        "type": "Level",
        "path": "Zoness - Red Path",
        "logic": "LevelAccess == 'shuffle_paths' and ZonessRedPath",
      },
    },
  },
}
