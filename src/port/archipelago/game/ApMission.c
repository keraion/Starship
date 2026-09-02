#include "ApGame.h"

// ---------------------------------------------------------------------------
// Mission clear / medal / Venom / goal (map.c map_set_level_flags, main.c)

void ApMission_PersistStats(void) {
    APSlotState* st;
    s32 i;

    if (!AP_IsEnabled()) {
        return;
    }
    st = AP_SaveState();
    for (i = TEAM_ID_FALCO; i <= TEAM_ID_PEPPY; i++) {
        st->shields[i - 1] = (int16_t) gTeamShields[i];
    }
    st->lasers = (uint8_t) gLaserStrength[0];
    st->goldRings = gGoldRingCount[0];
    st->lives = (int8_t) MIN(gLifeCount[0], 99);
    st->bombs = (uint8_t) gBombCount[0];
    st->greatFoxIntact = gGreatFoxIntact;
    AP_MarkSaveDirty();
}

// Medals gating Venom: received Medal items when shuffled, otherwise the medal locations we checked ourselves.
s32 ApMission_MedalCount(void) {
    s32 count = 0;
    s32 i;

    if (!AP_IsEnabled()) {
        return 0;
    }
    if (AP_GetOption(AP_OPTION_SHUFFLE_MEDALS)) {
        return AP_GetItemCount(AP_ITEM_MEDAL);
    }
    for (i = 0; i < PLANET_MAX; i++) {
        if (AP_IsLocationChecked((uint16_t) ApTables_MedalLocationForPlanet(i))) {
            count++;
        }
    }
    return count;
}

static void RecordPlanet(PlanetId planet) {
    APSlotState* st = AP_SaveState();
    APPlanetState* p;

    if ((planet < 0) || (planet >= PLANET_MAX)) {
        return;
    }
    p = &st->planets[planet];
    if (gTeamShields[TEAM_ID_PEPPY] > 0) {
        p->peppy = 1;
    }
    if (gTeamShields[TEAM_ID_SLIPPY] > 0) {
        p->slippy = 1;
    }
    if (gTeamShields[TEAM_ID_FALCO] > 0) {
        p->falco = 1;
    }
    if (gHitCount > p->score) {
        p->score = (uint16_t) gHitCount;
    }
    if (planet == PLANET_FORTUNA) {
        st->starWolfAlive.wolf = gStarWolfTeamAlive[0] != 0;
        st->starWolfAlive.leon = gStarWolfTeamAlive[1] != 0;
        st->starWolfAlive.pigma = gStarWolfTeamAlive[2] != 0;
        st->starWolfAlive.andrew = gStarWolfTeamAlive[3] != 0;
    }
}

static void OnMissionClear(MissionClearEvent* event) {
    s16 loc;

    if (!AP_IsEnabled()) {
        return;
    }

    ApMission_PersistStats();
    RecordPlanet(event->planet);

    if (event->planet == PLANET_VENOM) {
        return; // handled by OnVenomClear
    }

    loc = ApTables_ClearLocation(event->planet, event->status);
    if (loc > 0) {
        AP_CheckLocation((uint16_t) loc);
    }
    if (AP_GetOption(AP_OPTION_ACCOMPLISHED_SENDS_COMPLETE) && (event->status == MISSION_ACCOMPLISHED)) {
        loc = ApTables_ClearLocation(event->planet, MISSION_COMPLETE);
        if (loc > 0) {
            AP_CheckLocation((uint16_t) loc);
        }
    }
    if (event->medal) {
        loc = ApTables_MedalLocationForLevel(event->level);
        if (loc > 0) {
            AP_CheckLocation((uint16_t) loc);
        }
    }
    AP_MarkSaveDirty();
}

// Bolse / Area 6 normally chain straight into Venom. Under Archipelago we go back to the map instead.
static void OnPreVenomTransition(PreVenomTransitionEvent* event) {
    if (!AP_IsEnabled()) {
        return;
    }
    event->event.cancelled = true;
    gNextGameState = GSTATE_MAP;
    gLastGameState = GSTATE_PLAY;
    gMissionStatus = MISSION_COMPLETE;
}

static void OnVenomClear(VenomClearEvent* event) {
    bool andross;
    bool robot;
    bool goal = false;

    if (!AP_IsEnabled()) {
        return;
    }

    AP_CheckLocation(event->hardClear ? AP_LOCATION_VENOM_DEFEAT_ANDROSS : AP_LOCATION_VENOM_DEFEAT_ROBOT_ANDROSS);
    if (gLeveLClearStatus[LEVEL_VENOM_ANDROSS] == 2) {
        AP_CheckLocation(AP_LOCATION_VENOM_MEDAL);
    }
    ApMission_PersistStats();
    RecordPlanet(PLANET_VENOM);

    andross = AP_IsLocationChecked(AP_LOCATION_VENOM_DEFEAT_ANDROSS);
    robot = AP_IsLocationChecked(AP_LOCATION_VENOM_DEFEAT_ROBOT_ANDROSS);
    switch (AP_GetOption(AP_OPTION_VICTORY_CONDITION)) {
        case AP_OPTION_VICTORY_CONDITION_ANDROSS:
            goal = andross;
            break;
        case AP_OPTION_VICTORY_CONDITION_ANDROSS_AND_ROBOT_ANDROSS:
            goal = andross && robot;
            break;
        case AP_OPTION_VICTORY_CONDITION_ANDROSS_OR_ROBOT_ANDROSS:
        default:
            goal = andross || robot;
            break;
    }

    if (goal) {
        AP_GoalCompleted();
        // let the vanilla ending play
    } else {
        event->event.cancelled = true;
        gNextGameState = GSTATE_MAP;
        gLastGameState = GSTATE_PLAY;
        gMissionStatus = MISSION_COMPLETE;
    }
}

static void OnMedalThreshold(MedalThresholdEvent* event) {
    s16 option;

    if (!AP_IsEnabled()) {
        return;
    }
    option = ApTables_MedalOption(event->level);
    if (option >= 0) {
        event->hitCount = AP_GetOption((uint16_t) option);
    }
}

void ApMission_Init(void) {
    REGISTER_LISTENER(MissionClearEvent, (EventCallback) OnMissionClear, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(PreVenomTransitionEvent, (EventCallback) OnPreVenomTransition, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(VenomClearEvent, (EventCallback) OnVenomClear, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(MedalThresholdEvent, (EventCallback) OnMedalThreshold, EVENT_PRIORITY_NORMAL);
}
