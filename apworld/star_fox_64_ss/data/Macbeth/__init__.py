regions = {
  "Macbeth": {
    "meta": {
      "level": "LEVEL_MACBETH",
      "planet": "PLANET_MACBETH",
      "medal_option": "medal_macbeth",
      "level_item": "Macbeth",
    },
    "locations": {
      "Macbeth - Mission Complete": {
        "item": ["Nothing", "Macbeth - Blue Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Macbeth - Mission Accomplished": {
        "item": ["Area 6", "Macbeth - Red Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "accomplished",
      },
      "Macbeth - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
      "Macbeth - Before Hill With Rolling Rocks: Bomb": {
        "item": "Bomb",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0097,
      },
      "Macbeth - Above Rolling Rocks: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00C6,
      },
      "Macbeth - On Tracks Before Bridge: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00F9,
      },
      "Macbeth - On Tracks After Bridge: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0116,
      },
      "Macbeth - Checkpoint": {
        "item": "Macbeth - Checkpoint",
        "group": "Checkpoints",
        "logic": "true",
        "object_index": 0x0139,
      },
      "Macbeth - After First Building: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0152,
      },
      "Macbeth - Before Buildings Three And Four: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x015C,
      },
      "Macbeth - After Buildings Three And Four, Right: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0166,
      },
      "Macbeth - Before Building Five: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x016B,
      },
      "Macbeth - After Building Five: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x016D,
      },
      "Macbeth - Under Bridge After Checkpoint: Bomb": {
        "item": "Bomb",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x018A,
      },
      "Macbeth - First Building After Switcher: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x01D3,
      },
      "Macbeth - Second Building After Switcher: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x01DA,
      },
      "Macbeth - After Marker 100: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x01EA,
      },
      "Macbeth - Before Marker 300: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0280,
      },
    },
    "exits": {
      "Bolse": {
        "type": "Level",
        "path": "Macbeth - Blue Path",
        "logic": "LevelAccess == 'shuffle_paths' and MacbethBluePath",
      },
      "Area 6": {
        "type": "Level",
        "path": "Macbeth - Red Path",
        "logic": "LevelAccess == 'shuffle_paths' and MacbethRedPath",
      },
    },
  },
}
