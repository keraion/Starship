regions = {
  "Titania": {
    "meta": {
      "level": "LEVEL_TITANIA",
      "planet": "PLANET_TITANIA",
      "medal_option": "medal_titania",
      "level_item": "Titania",
    },
    "locations": {
      "Titania - Mission Complete": {
        "item": ["Bolse", "Titania - Blue Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Titania - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
      "Titania - Second Falling Tower From Start: Bomb": {
        "item": "Bomb",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0018,
      },
      "Titania - Through Two Leaning Towers: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0028,
      },
      "Titania - After Three Bone Structures: Bomb": {
        "item": "Bomb",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x004F,
      },
      "Titania - Above Skull: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0093,
      },
      "Titania - On Bridge: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00C5,
      },
      "Titania - Right Of Bridge: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00C6,
      },
      "Titania - Upper Left Path: Bomb": {
        "item": "Bomb",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x010A,
      },
      "Titania - Suspended Above Two Bridges: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x011E,
      },
      "Titania - Checkpoint": {
        "item": "Titania - Checkpoint",
        "group": "Checkpoints",
        "logic": "true",
        "object_index": 0x015E,
      },
      "Titania - Falling Towers Section: First Bomb": {
        "item": "Bomb",
        "logic": "true",
        "object_index": 0x0190,
      },
      "Titania - Falling Towers Section: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x019C,
      },
      "Titania - Falling Towers Section: Second Bomb": {
        "item": "Bomb",
        "logic": "true",
        "object_index": 0x01A5,
      },
      "Titania - Falling Towers Section: Third Bomb": {
        "item": "Bomb",
        "logic": "true",
        "object_index": 0x01BE,
      },
      "Titania - Behind Arches On The Right: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x020C,
      },
      "Titania - Upper Right Hill: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0221,
      },
      "Titania - First Falling Tower Before Boss: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0231,
      },
      "Titania - First Falling Tower Before Boss: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0236,
      },
      "Titania - Second Falling Tower Before Boss: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x023A,
      },
      "Titania - Second Falling Tower Before Boss: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x023F,
      },
    },
    "exits": {
      "Bolse": {
        "type": "Level",
        "path": "Titania - Blue Path",
        "logic": "LevelAccess == 'shuffle_paths' and TitaniaBluePath",
      },
    },
  },
}
