regions = {
  "Sector X": {
    "meta": {
      "level": "LEVEL_SECTOR_X",
      "planet": "PLANET_SECTOR_X",
      "medal_option": "medal_sector_x",
      "level_item": "Sector X",
    },
    "locations": {
      "Sector X - Mission Complete": {
        "item": ["Titania", "Sector X - Blue Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Sector X - Mission Accomplished": {
        "item": ["Macbeth", "Sector X - Yellow Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "accomplished",
      },
      "Sector X - Warp": {
        "item": ["Sector Z", "Sector X - Warp Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "warp",
      },
      "Sector X - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
      "Sector X - In Debris Field, Left: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00E9,
      },
      "Sector X - Checkpoint": {
        "item": "Sector X - Checkpoint",
        "group": "Checkpoints",
        "logic": "true",
        "object_index": 0x0113,
      },
      "Sector X - Behind Enemy After Peppy Gets Chased, Right: Gold Ring": {
        "item": "Gold Ring" ,
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0146,
      },
      "Sector X - Left Path - Just After Fork, Through Bottom Slot: Laser Upgrade": {
        "item": "Laser Upgrade" ,
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0196,
      },
      "Sector X - Left Path - Closing Door Section: Gold Ring": {
        "item": "Gold Ring" ,
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x01D2,
      },
      "Sector X - Left Path - After Closing Door Section: Bomb": {
        "item": "Bomb" ,
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0254,
      },
      "Sector X - Left Path - Behind First Warp Gate: Gold Ring": {
        "item": "Gold Ring" ,
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x02B7,
      },
      "Sector X - Left Path - After First Warp Gate, Up High: Bomb": {
        "item": "Bomb" ,
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x02C4,
      },
      "Sector X - Left Path - Before Final Warp Gate: Gold Ring": {
        "item": "Gold Ring" ,
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0392,
      },
    },
    "exits": {
      "Titania": {
        "type": "Level",
        "path": "Sector X - Blue Path",
        "logic": "LevelAccess == 'shuffle_paths' and SectorXBluePath",
      },
      "Macbeth": {
        "type": "Level",
        "path": "Sector X - Yellow Path",
        "logic": "LevelAccess == 'shuffle_paths' and SectorXYellowPath",
      },
      "Sector Z": {
        "type": "Level",
        "path": "Sector X - Warp Path",
        "logic": "LevelAccess == 'shuffle_paths' and SectorXWarpPath",
      },
    },
  },
}
