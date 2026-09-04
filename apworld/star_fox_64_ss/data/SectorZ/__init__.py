regions = {
  "Sector Z": {
    "meta": {
      "level": "LEVEL_SECTOR_Z",
      "planet": "PLANET_SECTOR_Z",
      "medal_option": "medal_sector_z",
      "level_item": "Sector Z",
    },
    "locations": {
      "Sector Z - Mission Complete": {
        "item": ["Nothing", "Sector Z - Blue Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Sector Z - Mission Accomplished": {
        "item": ["Nothing", "Sector Z - Red Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "accomplished",
      },
      "Sector Z - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
    },
    "exits": {
      "Bolse": {
        "type": "Level",
        "path": "Sector Z - Blue Path",
        "logic": "LevelAccess == 'shuffle_paths' and SectorZBluePath",
      },
      "Area 6": {
        "type": "Level",
        "path": "Sector Z - Red Path",
        "logic": "LevelAccess == 'shuffle_paths' and SectorZRedPath",
      },
    },
  },
}
