regions = {
  "Corneria": {
    "meta": {
      "level": "LEVEL_CORNERIA",
      "planet": "PLANET_CORNERIA",
      "medal_option": "medal_corneria",
      "level_item": "Corneria",
    },
    "locations": {
      "Corneria - Mission Complete": {
        "item": ["Meteo", "Corneria - Blue Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Corneria - Mission Accomplished": {
        "item": ["Sector Y", "Corneria - Red Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "accomplished",
      },
      "Corneria - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
      "Corneria - Under Arch: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x005E,
      },
      "Corneria - Behind Doors: Bomb": {
        "item": "Bomb",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x006C,
      },
      "Corneria - Center Of Corneria City: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00A6,
      },
      "Corneria - Near First Ground Robot: Laser Upgrade": {
        "item": "Laser Upgrade",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00DC,
      },
      "Corneria - Under Highway Arch: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00F0,
      },
      "Corneria - Behind Second Ground Robot: Bomb": {
        "item": "Bomb",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x011E,
      },
      "Corneria - Behind Doors Near Checkpoint: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x014B,
      },
      "Corneria - Checkpoint": {
        "item": "Corneria - Checkpoint",
        "group": "Checkpoints",
        "logic": "true",
        "object_index": 0x0171,
      },
      "Corneria - After Falco's G-Diffuser Issue: Laser Upgrade": {
        "item": "Laser Upgrade",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x020A,
      },
      "Corneria - After Falco's G-Diffuser Issue: Bomb": {
        "item": "Bomb",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x020C,
      },
      "Corneria - Water Section Last Arch: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x025D,
      },
      "Corneria - Left Of Waterfall: Laser Upgrade": {
        "item": "Laser Upgrade",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0290,
      },
      "Corneria - Before Mission Complete Boss, Lower: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x02A4,
      },
      "Corneria - Before Mission Complete Boss, Upper: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x02A5,
      },
    },
    "exits": {
      "Meteo": {
        "type": "Level",
        "path": "Corneria - Blue Path",
        "logic": "LevelAccess == 'shuffle_paths' and CorneriaBluePath",
      },
      "Sector Y": {
        "type": "Level",
        "path": "Corneria - Red Path",
        "logic": "LevelAccess == 'shuffle_paths' and CorneriaRedPath",
      },
    },
  },
}
