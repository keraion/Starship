regions = {
  "Meteo": {
    "meta": {
      "level": "LEVEL_METEO",
      "planet": "PLANET_METEO",
      "medal_option": "medal_meteo",
      "level_item": "Meteo",
    },
    "locations": {
      "Meteo - Mission Complete": {
        "item": ["Fortuna", "Meteo - Blue Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Meteo - Warp": {
        "item": ["Katina", "Meteo - Warp Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "warp",
      },
      "Meteo - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
      "Meteo - After Starting Asteroids: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x0072,
      },
      "Meteo - End Of First Tunnel, Top: Silver Ring": {
        "item": "Silver Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00BD,
      },
      "Meteo - End Of First Tunnel, Middle: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00BE,
      },
      "Meteo - End Of First Tunnel, Bottom: Bomb": {
        "item": "Bomb",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00BB,
      },
      "Meteo - Between Two Big Asteroids: Bomb": {
        "item": "Bomb",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x00F2,
      },
      "Meteo - Checkpoint": {
        "item": "Meteo - Checkpoint",
        "group": "Checkpoints",
        "logic": "true",
        "object_index": 0x01DF,
      },
      "Meteo - Near Warp Rings: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x028E,
      },
      "Meteo - Just Before Boss: Gold Ring": {
        "item": "Gold Ring",
        "group": "Freestanding Items",
        "logic": "true",
        "object_index": 0x033E,
      },
    },
    "exits": {
      "Fortuna": {
        "type": "Level",
        "path": "Meteo - Blue Path",
        "logic": "LevelAccess == 'shuffle_paths' and MeteoBluePath",
      },
      "Katina": {
        "type": "Level",
        "path": "Meteo - Warp Path",
        "logic": "LevelAccess == 'shuffle_paths' and MeteoWarpPath",
      },
    },
  },
}
