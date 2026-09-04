// Test stub standing in for the game-side ApGame.h: only what ApLogic.c needs.
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "port/archipelago/ArchipelagoIds.h"
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
uint8_t AP_GetItemCount(uint16_t item);
uint16_t AP_GetOption(uint16_t option);
int ApMission_MedalCount(void);
typedef enum { PLANET_NONE = -1, PLANET_METEO, PLANET_AREA_6, PLANET_BOLSE, PLANET_SECTOR_Z, PLANET_SECTOR_X, PLANET_SECTOR_Y,
    PLANET_KATINA, PLANET_MACBETH, PLANET_ZONESS, PLANET_CORNERIA, PLANET_TITANIA, PLANET_AQUAS, PLANET_FORTUNA, PLANET_VENOM,
    PLANET_SOLAR, PLANET_MAX } PlanetId;
