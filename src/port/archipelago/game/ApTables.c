#include "ApGame.h"

// Tables ported verbatim from the AP-Star-Fox-64 ROM hack (n64/src/object.c, map.c, main.c).

s16 ApTables_GetStaticLocation(LevelId level, s32 index) {
    switch (level) {
        case LEVEL_CORNERIA:
            switch (index) {
                case 0x005E:
                    return AP_LOCATION_CORNERIA_UNDER_ARCH_GOLD_RING;
                case 0x006C:
                    return AP_LOCATION_CORNERIA_BEHIND_DOORS_BOMB;
                case 0x00A6:
                    return AP_LOCATION_CORNERIA_CENTER_OF_CORNERIA_CITY_SILVER_RING;
                case 0x00DC:
                    return AP_LOCATION_CORNERIA_NEAR_FIRST_GROUND_ROBOT_LASER_UPGRADE;
                case 0x00F0:
                    return AP_LOCATION_CORNERIA_UNDER_HIGHWAY_ARCH_GOLD_RING;
                case 0x011E:
                    return AP_LOCATION_CORNERIA_BEHIND_SECOND_GROUND_ROBOT_BOMB;
                case 0x014B:
                    return AP_LOCATION_CORNERIA_BEHIND_DOORS_NEAR_CHECKPOINT_GOLD_RING;
                case 0x0171:
                    return AP_LOCATION_CORNERIA_CHECKPOINT;
                case 0x020A:
                    return AP_LOCATION_CORNERIA_AFTER_FALCOS_G_DIFFUSER_ISSUE_LASER_UPGRADE;
                case 0x020C:
                    return AP_LOCATION_CORNERIA_AFTER_FALCOS_G_DIFFUSER_ISSUE_BOMB;
                case 0x025D:
                    return AP_LOCATION_CORNERIA_WATER_SECTION_LAST_ARCH_GOLD_RING;
                case 0x0290:
                    return AP_LOCATION_CORNERIA_LEFT_OF_WATERFALL_LASER_UPGRADE;
                case 0x02A4:
                    return AP_LOCATION_CORNERIA_BEFORE_MISSION_COMPLETE_BOSS_LOWER_GOLD_RING;
                case 0x02A5:
                    return AP_LOCATION_CORNERIA_BEFORE_MISSION_COMPLETE_BOSS_UPPER_GOLD_RING;
            }
            break;
        case LEVEL_METEO:
            switch (index) {
                case 0x0072:
                    return AP_LOCATION_METEO_AFTER_STARTING_ASTEROIDS_GOLD_RING;
                case 0x00BB:
                    return AP_LOCATION_METEO_END_OF_FIRST_TUNNEL_BOTTOM_BOMB;
                case 0x00BD:
                    return AP_LOCATION_METEO_END_OF_FIRST_TUNNEL_TOP_SILVER_RING;
                case 0x00BE:
                    return AP_LOCATION_METEO_END_OF_FIRST_TUNNEL_MIDDLE_GOLD_RING;
                case 0x00F2:
                    return AP_LOCATION_METEO_BETWEEN_TWO_BIG_ASTEROIDS_BOMB;
                case 0x01DF:
                    return AP_LOCATION_METEO_CHECKPOINT;
                case 0x028E:
                    return AP_LOCATION_METEO_NEAR_WARP_RINGS_GOLD_RING;
                case 0x033E:
                    return AP_LOCATION_METEO_JUST_BEFORE_BOSS_GOLD_RING;
            }
            break;
        case LEVEL_SECTOR_X:
            switch (index) {
                case 0x00E9:
                    return AP_LOCATION_SECTOR_X_IN_DEBRIS_FIELD_LEFT_GOLD_RING;
                case 0x0113:
                    return AP_LOCATION_SECTOR_X_CHECKPOINT;
                case 0x0146:
                    return AP_LOCATION_SECTOR_X_BEHIND_ENEMY_AFTER_PEPPY_GETS_CHASED_RIGHT_GOLD_RING;
                case 0x0196:
                    return AP_LOCATION_SECTOR_X_LEFT_PATH_JUST_AFTER_FORK_THROUGH_BOTTOM_SLOT_LASER_UPGRADE;
                case 0x01D2:
                    return AP_LOCATION_SECTOR_X_LEFT_PATH_CLOSING_DOOR_SECTION_GOLD_RING;
                case 0x0254:
                    return AP_LOCATION_SECTOR_X_LEFT_PATH_AFTER_CLOSING_DOOR_SECTION_BOMB;
                case 0x02B7:
                    return AP_LOCATION_SECTOR_X_LEFT_PATH_BEHIND_FIRST_WARP_GATE_GOLD_RING;
                case 0x02C4:
                    return AP_LOCATION_SECTOR_X_LEFT_PATH_AFTER_FIRST_WARP_GATE_UP_HIGH_BOMB;
                case 0x0392:
                    return AP_LOCATION_SECTOR_X_LEFT_PATH_BEFORE_FINAL_WARP_GATE_GOLD_RING;
            }
            break;
        case LEVEL_TITANIA:
            switch (index) {
                case 0x0018:
                    return AP_LOCATION_TITANIA_SECOND_FALLING_TOWER_FROM_START_BOMB;
                case 0x0028:
                    return AP_LOCATION_TITANIA_THROUGH_TWO_LEANING_TOWERS_GOLD_RING;
                case 0x004F:
                    return AP_LOCATION_TITANIA_AFTER_THREE_BONE_STRUCTURES_BOMB;
                case 0x0093:
                    return AP_LOCATION_TITANIA_ABOVE_SKULL_SILVER_RING;
                case 0x00C5:
                    return AP_LOCATION_TITANIA_ON_BRIDGE_GOLD_RING;
                case 0x00C6:
                    return AP_LOCATION_TITANIA_RIGHT_OF_BRIDGE_GOLD_RING;
                case 0x010A:
                    return AP_LOCATION_TITANIA_UPPER_LEFT_PATH_BOMB;
                case 0x011E:
                    return AP_LOCATION_TITANIA_SUSPENDED_ABOVE_TWO_BRIDGES_SILVER_RING;
                case 0x015E:
                    return AP_LOCATION_TITANIA_CHECKPOINT;
                case 0x0190:
                    return AP_LOCATION_TITANIA_FALLING_TOWERS_SECTION_FIRST_BOMB;
                case 0x019C:
                    return AP_LOCATION_TITANIA_FALLING_TOWERS_SECTION_GOLD_RING;
                case 0x01A5:
                    return AP_LOCATION_TITANIA_FALLING_TOWERS_SECTION_SECOND_BOMB;
                case 0x01BE:
                    return AP_LOCATION_TITANIA_FALLING_TOWERS_SECTION_THIRD_BOMB;
                case 0x020C:
                    return AP_LOCATION_TITANIA_BEHIND_ARCHES_ON_THE_RIGHT_GOLD_RING;
                case 0x0221:
                    return AP_LOCATION_TITANIA_UPPER_RIGHT_HILL_GOLD_RING;
                case 0x0231:
                    return AP_LOCATION_TITANIA_FIRST_FALLING_TOWER_BEFORE_BOSS_SILVER_RING;
                case 0x0236:
                    return AP_LOCATION_TITANIA_FIRST_FALLING_TOWER_BEFORE_BOSS_GOLD_RING;
                case 0x023A:
                    return AP_LOCATION_TITANIA_SECOND_FALLING_TOWER_BEFORE_BOSS_SILVER_RING;
                case 0x023F:
                    return AP_LOCATION_TITANIA_SECOND_FALLING_TOWER_BEFORE_BOSS_GOLD_RING;
            }
            break;
        case LEVEL_SECTOR_Y:
            switch (index) {
                case 0x0021:
                    return AP_LOCATION_SECTOR_Y_ABOVE_FRIENDLY_SHIP_NEAR_START_GOLD_RING;
                case 0x00DB:
                    return AP_LOCATION_SECTOR_Y_BEFORE_CHECKPOINT_GOLD_RING;
                case 0x00D7:
                    return AP_LOCATION_SECTOR_Y_CHECKPOINT;
                case 0x01B0:
                    return AP_LOCATION_SECTOR_Y_BREAKING_THROUGH_THE_ENEMY_FLEET_BEGINNING_GOLD_RING;
                case 0x01CC:
                    return AP_LOCATION_SECTOR_Y_BREAKING_THROUGH_THE_ENEMY_FLEET_MIDDLE_GOLD_RING;
                case 0x01E3:
                    return AP_LOCATION_SECTOR_Y_BREAKING_THROUGH_THE_ENEMY_FLEET_END_GOLD_RING;
            }
            break;
        case LEVEL_AQUAS:
            switch (index) {
                case 0x00BD:
                    return AP_LOCATION_AQUAS_CHECKPOINT;
            }
            break;
        case LEVEL_ZONESS:
            switch (index) {
                case 0x0073:
                    return AP_LOCATION_ZONESS_BEFORE_KATT_APPEARS_BOTTOM_BOMB;
                case 0x0076:
                    return AP_LOCATION_ZONESS_BEFORE_KATT_APPEARS_TOP_GOLD_RING;
                case 0x0077:
                    return AP_LOCATION_ZONESS_BEFORE_KATT_APPEARS_MIDDLE_SILVER_RING;
                case 0x010F:
                    return AP_LOCATION_ZONESS_CHECKPOINT;
                case 0x0121:
                    return AP_LOCATION_ZONESS_AIM_FOR_THE_RUDDER_FIRST_GATE_LASER_UPGRADE;
                case 0x012D:
                    return AP_LOCATION_ZONESS_AIM_FOR_THE_RUDDER_SECOND_GATE_LEFT_BOMB;
                case 0x0133:
                    return AP_LOCATION_ZONESS_AIM_FOR_THE_RUDDER_THIRD_GATE_SILVER_RING;
            }
            break;
        case LEVEL_AREA_6:
            switch (index) {
                case 0x00A3:
                    return AP_LOCATION_AREA_6_BELOW_EARLY_DEFENSE_STATION_GOLD_RING;
                case 0x00A4:
                    return AP_LOCATION_AREA_6_ABOVE_EARLY_DEFENSE_STATION_LASER_UPGRADE;
                case 0x00E3:
                    return AP_LOCATION_AREA_6_ANDROSS_TAUNT_GOLD_RING;
                case 0x00FA:
                    return AP_LOCATION_AREA_6_CHECKPOINT;
                case 0x0115:
                    return AP_LOCATION_AREA_6_NEAR_DEFENSE_STATION_AFTER_CHECKPOINT_GOLD_RING;
            }
            break;
        case LEVEL_SOLAR:
            switch (index) {
                case 0x005C:
                    return AP_LOCATION_SOLAR_CHECKPOINT;
            }
            break;
        case LEVEL_MACBETH:
            switch (index) {
                case 0x0097:
                    return AP_LOCATION_MACBETH_BEFORE_HILL_WITH_ROLLING_ROCKS_BOMB;
                case 0x00C6:
                    return AP_LOCATION_MACBETH_ABOVE_ROLLING_ROCKS_GOLD_RING;
                case 0x00F9:
                    return AP_LOCATION_MACBETH_ON_TRACKS_BEFORE_BRIDGE_GOLD_RING;
                case 0x0116:
                    return AP_LOCATION_MACBETH_ON_TRACKS_AFTER_BRIDGE_GOLD_RING;
                case 0x0139:
                    return AP_LOCATION_MACBETH_CHECKPOINT;
                case 0x0152:
                    return AP_LOCATION_MACBETH_AFTER_FIRST_BUILDING_SILVER_RING;
                case 0x015C:
                    return AP_LOCATION_MACBETH_BEFORE_BUILDINGS_THREE_AND_FOUR_SILVER_RING;
                case 0x0166:
                    return AP_LOCATION_MACBETH_AFTER_BUILDINGS_THREE_AND_FOUR_RIGHT_SILVER_RING;
                case 0x016B:
                    return AP_LOCATION_MACBETH_BEFORE_BUILDING_FIVE_SILVER_RING;
                case 0x016D:
                    return AP_LOCATION_MACBETH_AFTER_BUILDING_FIVE_GOLD_RING;
                case 0x018A:
                    return AP_LOCATION_MACBETH_UNDER_BRIDGE_AFTER_CHECKPOINT_BOMB;
                case 0x01D3:
                    return AP_LOCATION_MACBETH_FIRST_BUILDING_AFTER_SWITCHER_GOLD_RING;
                case 0x01DA:
                    return AP_LOCATION_MACBETH_SECOND_BUILDING_AFTER_SWITCHER_SILVER_RING;
                case 0x01EA:
                    return AP_LOCATION_MACBETH_AFTER_MARKER_100_SILVER_RING;
                case 0x0280:
                    return AP_LOCATION_MACBETH_BEFORE_MARKER_300_SILVER_RING;
            }
            break;
        case LEVEL_VENOM_1:
            switch (index) {
                case 0x0087:
                    return AP_LOCATION_VENOM_1_NEAR_START_BOTTOM_GOLD_RING;
                case 0x0088:
                    return AP_LOCATION_VENOM_1_NEAR_START_TOP_LASER_UPGRADE;
                case 0x02A8:
                    return AP_LOCATION_VENOM_1_MAZE_R_R_R_GOLD_RING;
                case 0x02C1:
                    return AP_LOCATION_VENOM_1_MAZE_L_L_GOLD_RING;
                case 0x02DA:
                    return AP_LOCATION_VENOM_1_MAZE_R_R_L_L_GOLD_RING;
                case 0x039F:
                    return AP_LOCATION_VENOM_1_END_OF_MAZE_R_R_R_R_GOLD_RING;
                case 0x03B3:
                    return AP_LOCATION_VENOM_1_END_OF_MAZE_L_L_L_GOLD_RING;
                case 0x0405:
                    return AP_LOCATION_VENOM_1_END_OF_MAZE_R_R_L_L_GOLD_RING;
                case 0x048C:
                    return AP_LOCATION_VENOM_1_CHECKPOINT;
            }
            break;
        default:
            break;
    }
    return AP_LOCATION_NONE;
}

// [PlanetId][MissionStatus] (map.c map_get_clear_location)
static const s16 sClearLocations[PLANET_MAX][3] = {
    /* METEO    */ { AP_LOCATION_METEO_MISSION_COMPLETE, AP_LOCATION_NONE, AP_LOCATION_METEO_WARP },
    /* AREA_6   */ { AP_LOCATION_AREA_6_MISSION_COMPLETE, AP_LOCATION_NONE, AP_LOCATION_NONE },
    /* BOLSE    */ { AP_LOCATION_BOLSE_MISSION_COMPLETE, AP_LOCATION_NONE, AP_LOCATION_NONE },
    /* SECTOR_Z */ { AP_LOCATION_SECTOR_Z_MISSION_COMPLETE, AP_LOCATION_SECTOR_Z_MISSION_ACCOMPLISHED, AP_LOCATION_NONE },
    /* SECTOR_X */ { AP_LOCATION_SECTOR_X_MISSION_COMPLETE, AP_LOCATION_SECTOR_X_MISSION_ACCOMPLISHED, AP_LOCATION_SECTOR_X_WARP },
    /* SECTOR_Y */ { AP_LOCATION_SECTOR_Y_MISSION_COMPLETE, AP_LOCATION_SECTOR_Y_MISSION_ACCOMPLISHED, AP_LOCATION_NONE },
    /* KATINA   */ { AP_LOCATION_KATINA_MISSION_COMPLETE, AP_LOCATION_KATINA_MISSION_ACCOMPLISHED, AP_LOCATION_NONE },
    /* MACBETH  */ { AP_LOCATION_MACBETH_MISSION_COMPLETE, AP_LOCATION_MACBETH_MISSION_ACCOMPLISHED, AP_LOCATION_NONE },
    /* ZONESS   */ { AP_LOCATION_ZONESS_MISSION_COMPLETE, AP_LOCATION_ZONESS_MISSION_ACCOMPLISHED, AP_LOCATION_NONE },
    /* CORNERIA */ { AP_LOCATION_CORNERIA_MISSION_COMPLETE, AP_LOCATION_CORNERIA_MISSION_ACCOMPLISHED, AP_LOCATION_NONE },
    /* TITANIA  */ { AP_LOCATION_TITANIA_MISSION_COMPLETE, AP_LOCATION_NONE, AP_LOCATION_NONE },
    /* AQUAS    */ { AP_LOCATION_AQUAS_MISSION_COMPLETE, AP_LOCATION_NONE, AP_LOCATION_NONE },
    /* FORTUNA  */ { AP_LOCATION_FORTUNA_MISSION_COMPLETE, AP_LOCATION_FORTUNA_MISSION_ACCOMPLISHED, AP_LOCATION_NONE },
    /* VENOM    */ { AP_LOCATION_NONE, AP_LOCATION_NONE, AP_LOCATION_NONE },
    /* SOLAR    */ { AP_LOCATION_SOLAR_MISSION_COMPLETE, AP_LOCATION_NONE, AP_LOCATION_NONE },
};

s16 ApTables_ClearLocation(PlanetId planet, s32 missionStatus) {
    if ((planet < 0) || (planet >= PLANET_MAX) || (missionStatus < 0) || (missionStatus > 2)) {
        return AP_LOCATION_NONE;
    }
    return sClearLocations[planet][missionStatus];
}

// Indexed by PlanetId (map.c map_paths medal_locations / save.c save_sync_medals)
static const s16 sMedalLocationsByPlanet[PLANET_MAX] = {
    AP_LOCATION_METEO_MEDAL,    AP_LOCATION_AREA_6_MEDAL,   AP_LOCATION_BOLSE_MEDAL,   AP_LOCATION_SECTOR_Z_MEDAL,
    AP_LOCATION_SECTOR_X_MEDAL, AP_LOCATION_SECTOR_Y_MEDAL, AP_LOCATION_KATINA_MEDAL,  AP_LOCATION_MACBETH_MEDAL,
    AP_LOCATION_ZONESS_MEDAL,   AP_LOCATION_CORNERIA_MEDAL, AP_LOCATION_TITANIA_MEDAL, AP_LOCATION_AQUAS_MEDAL,
    AP_LOCATION_FORTUNA_MEDAL,  AP_LOCATION_VENOM_MEDAL,    AP_LOCATION_SOLAR_MEDAL,
};

s16 ApTables_MedalLocationForPlanet(PlanetId planet) {
    if ((planet < 0) || (planet >= PLANET_MAX)) {
        return AP_LOCATION_NONE;
    }
    return sMedalLocationsByPlanet[planet];
}

s16 ApTables_MedalLocationForLevel(LevelId level) {
    switch (level) {
        case LEVEL_AQUAS:
            return AP_LOCATION_AQUAS_MEDAL;
        case LEVEL_AREA_6:
            return AP_LOCATION_AREA_6_MEDAL;
        case LEVEL_BOLSE:
            return AP_LOCATION_BOLSE_MEDAL;
        case LEVEL_CORNERIA:
            return AP_LOCATION_CORNERIA_MEDAL;
        case LEVEL_FORTUNA:
            return AP_LOCATION_FORTUNA_MEDAL;
        case LEVEL_KATINA:
            return AP_LOCATION_KATINA_MEDAL;
        case LEVEL_MACBETH:
            return AP_LOCATION_MACBETH_MEDAL;
        case LEVEL_METEO:
            return AP_LOCATION_METEO_MEDAL;
        case LEVEL_SECTOR_X:
            return AP_LOCATION_SECTOR_X_MEDAL;
        case LEVEL_SECTOR_Y:
            return AP_LOCATION_SECTOR_Y_MEDAL;
        case LEVEL_SECTOR_Z:
            return AP_LOCATION_SECTOR_Z_MEDAL;
        case LEVEL_SOLAR:
            return AP_LOCATION_SOLAR_MEDAL;
        case LEVEL_TITANIA:
            return AP_LOCATION_TITANIA_MEDAL;
        case LEVEL_VENOM_1:
        case LEVEL_VENOM_2:
        case LEVEL_VENOM_ANDROSS:
            return AP_LOCATION_VENOM_MEDAL;
        case LEVEL_ZONESS:
            return AP_LOCATION_ZONESS_MEDAL;
        default:
            return AP_LOCATION_NONE;
    }
}

// Indexed by LevelId (main.c main_check_medal); -1 = keep the vanilla threshold
static const s16 sMedalOptionsByLevel[20] = {
    /* CORNERIA      */ AP_OPTION_MEDAL_CORNERIA,
    /* METEO         */ AP_OPTION_MEDAL_METEO,
    /* SECTOR_X      */ AP_OPTION_MEDAL_SECTOR_X,
    /* AREA_6        */ AP_OPTION_MEDAL_AREA_6,
    /* UNK_4         */ -1,
    /* SECTOR_Y      */ AP_OPTION_MEDAL_SECTOR_Y,
    /* VENOM_1       */ AP_OPTION_MEDAL_VENOM,
    /* SOLAR         */ AP_OPTION_MEDAL_SOLAR,
    /* ZONESS        */ AP_OPTION_MEDAL_ZONESS,
    /* VENOM_ANDROSS */ AP_OPTION_MEDAL_VENOM,
    /* TRAINING      */ -1,
    /* MACBETH       */ AP_OPTION_MEDAL_MACBETH,
    /* TITANIA       */ AP_OPTION_MEDAL_TITANIA,
    /* AQUAS         */ AP_OPTION_MEDAL_AQUAS,
    /* FORTUNA       */ AP_OPTION_MEDAL_FORTUNA,
    /* UNK_15        */ -1,
    /* KATINA        */ AP_OPTION_MEDAL_KATINA,
    /* BOLSE         */ AP_OPTION_MEDAL_BOLSE,
    /* SECTOR_Z      */ AP_OPTION_MEDAL_SECTOR_Z,
    /* VENOM_2       */ AP_OPTION_MEDAL_VENOM,
};

s16 ApTables_MedalOption(LevelId level) {
    if ((level < 0) || (level >= 20)) {
        return -1;
    }
    return sMedalOptionsByLevel[level];
}

static const LevelId sPlanetToLevel[PLANET_MAX] = {
    LEVEL_METEO,  LEVEL_AREA_6,   LEVEL_BOLSE,   LEVEL_SECTOR_Z, LEVEL_SECTOR_X,      LEVEL_SECTOR_Y, LEVEL_KATINA, LEVEL_MACBETH,
    LEVEL_ZONESS, LEVEL_CORNERIA, LEVEL_TITANIA, LEVEL_AQUAS,    LEVEL_FORTUNA, LEVEL_VENOM_ANDROSS, LEVEL_SOLAR,
};

LevelId ApTables_PlanetToLevel(PlanetId planet) {
    if ((planet < 0) || (planet >= PLANET_MAX)) {
        return LEVEL_UNK_M1;
    }
    return sPlanetToLevel[planet];
}

s16 ApTables_LevelItem(LevelId level) {
    switch (level) {
        case LEVEL_CORNERIA:
            return AP_ITEM_CORNERIA;
        case LEVEL_METEO:
            return AP_ITEM_METEO;
        case LEVEL_SECTOR_X:
            return AP_ITEM_SECTOR_X;
        case LEVEL_AREA_6:
            return AP_ITEM_AREA_6;
        case LEVEL_SECTOR_Y:
            return AP_ITEM_SECTOR_Y;
        case LEVEL_VENOM_1:
        case LEVEL_VENOM_2:
        case LEVEL_VENOM_ANDROSS:
            return AP_ITEM_VENOM;
        case LEVEL_SOLAR:
            return AP_ITEM_SOLAR;
        case LEVEL_ZONESS:
            return AP_ITEM_ZONESS;
        case LEVEL_MACBETH:
            return AP_ITEM_MACBETH;
        case LEVEL_TITANIA:
            return AP_ITEM_TITANIA;
        case LEVEL_AQUAS:
            return AP_ITEM_AQUAS;
        case LEVEL_FORTUNA:
            return AP_ITEM_FORTUNA;
        case LEVEL_KATINA:
            return AP_ITEM_KATINA;
        case LEVEL_BOLSE:
            return AP_ITEM_BOLSE;
        case LEVEL_SECTOR_Z:
            return AP_ITEM_SECTOR_Z;
        default:
            return AP_ITEM_NONE;
    }
}

// Same order as sPaths[24] in fox_map.c (map.c map_path_item)
const s16 gApTablesPathItem[24] = {
    AP_ITEM_CORNERIA_BLUE_PATH,  AP_ITEM_METEO_BLUE_PATH,     AP_ITEM_FORTUNA_BLUE_PATH,  AP_ITEM_SECTOR_X_BLUE_PATH,
    AP_ITEM_TITANIA_BLUE_PATH,   AP_ITEM_KATINA_BLUE_PATH,    AP_ITEM_MACBETH_BLUE_PATH,  AP_ITEM_SECTOR_Z_BLUE_PATH,
    AP_ITEM_BOLSE_BLUE_PATH,     AP_ITEM_SECTOR_Y_YELLOW_PATH, AP_ITEM_KATINA_YELLOW_PATH, AP_ITEM_SOLAR_YELLOW_PATH,
    AP_ITEM_SECTOR_X_YELLOW_PATH, AP_ITEM_FORTUNA_YELLOW_PATH, AP_ITEM_ZONESS_YELLOW_PATH, AP_ITEM_CORNERIA_RED_PATH,
    AP_ITEM_SECTOR_Y_RED_PATH,   AP_ITEM_AQUAS_RED_PATH,      AP_ITEM_ZONESS_RED_PATH,    AP_ITEM_SECTOR_Z_RED_PATH,
    AP_ITEM_MACBETH_RED_PATH,    AP_ITEM_AREA_6_RED_PATH,     AP_ITEM_METEO_WARP_PATH,    AP_ITEM_SECTOR_X_WARP_PATH,
};

bool ApTables_IsCheckpointLocation(s16 loc) {
    return (loc > 0) && (loc < AP_LOCATION_MAX) && (gApLocationGroup[loc] == AP_LOCATION_GROUP_CHECKPOINTS);
}

s16 ApTables_CheckpointItem(s16 loc) {
    if ((loc <= 0) || (loc >= AP_LOCATION_MAX)) {
        return AP_ITEM_NONE;
    }
    return gApLocationCheckpointItem[loc];
}
