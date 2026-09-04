regions = {
  "Bolse": {
    "meta": {
      "level": "LEVEL_BOLSE",
      "planet": "PLANET_BOLSE",
      "medal_option": "medal_bolse",
      "level_item": "Bolse",
    },
    "locations": {
      "Bolse - Mission Complete": {
        "item": ["Venom", "Bolse - Blue Path"],
        "group": "Mission Finished",
        "logic": "true",
        "status": "complete",
      },
      "Bolse - Medal": {
        "item": "Medal",
        "group": "Medals",
        "logic": "true",
      },
    },
    "exits": {
      "Venom 1": {
        "type": "Level",
        "path": "Bolse - Blue Path",
        "logic": """
          LevelAccess == 'shuffle_paths'
          and BolseBluePath and (VictoryCondition != "andross_or_robot_andross" or (Medal, RequiredMedals))
        """,
      },
    },
  },
}
