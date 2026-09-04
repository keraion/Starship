regions = {
  "Sector Y": {
    "meta": {
      "level": "LEVEL_SECTOR_Y",
      "planet": "PLANET_SECTOR_Y",
      "medal_option": "medal_sector_y",
      "level_item": "Sector Y",
    },
    "locations": {
      "Sector Y - Mission Complete": {
        "item": ["Nothing", "Sector Y - Yellow Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Sector Y - Mission Accomplished": {
        "item": ["Aquas", "Sector Y - Red Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "accomplished",
      },
      "Sector Y - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
      "Sector Y - Above Friendly Ship Near Start: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0021,
      },
      "Sector Y - Before Checkpoint: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00DB,
      },
      "Sector Y - Checkpoint": {
        "item": "Sector Y - Checkpoint",
        "group": "Checkpoints",
        "logic": "true",
        "object_index": 0x00D7,
      },
      "Sector Y - Breaking Through The Enemy Fleet, Beginning: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x01B0,
      },
      "Sector Y - Breaking Through The Enemy Fleet, Middle: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x01CC,
      },
      "Sector Y - Breaking Through The Enemy Fleet, End: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x01E3,
      },
    },
    "exits": {
      "Katina": {
        "type": "Level",
        "path": "Sector Y - Yellow Path",
        "logic": "LevelAccess == 'shuffle_paths' and SectorYYellowPath",
      },
      "Aquas": {
        "type": "Level",
        "path": "Sector Y - Red Path",
        "logic": "LevelAccess == 'shuffle_paths' and SectorYRedPath",
      },
    },
  },
}
