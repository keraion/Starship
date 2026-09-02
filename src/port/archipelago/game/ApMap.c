#include "ApGame.h"
#include "assets/ast_map.h"
#include <stdio.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Map rewrite: level/path gating, in-game tracker, free navigation and the
// heal-teammates menu (map.c, hud.c of the ROM hack).

extern Planet sPlanets[PLANET_MAX];
extern PlanetPath sPaths[24];
extern PlanetId sCurrentPlanetId;
extern PlanetId sPrevPlanetId;
extern PlanetId sNextPlanetId;
extern MissionStatus sPrevMissionStatus;
extern s32 sMapState;
extern s32 sMapSubState;
extern s32 D_menu_801CD94C;
extern s32 D_menu_801CD900[15];
extern s32 D_menu_801CD970;
extern bool D_menu_801CD974;
extern s32 D_menu_801CD97C;
extern bool D_menu_801CD980;
extern s32 D_menu_801CD98C;
extern s32 D_menu_801CD968;
extern s32 D_menu_801CEFC4;
extern s32 D_menu_801CEFC8;
extern s32 D_menu_801CEFD0;
extern s32 D_menu_801CEFD4;
extern s32 D_menu_801CEFDC;
extern f32 D_menu_801CEFE0;
extern f32 D_menu_801CEFE8[3];
extern f32 sMapCamEyeX;
extern f32 sMapCamEyeY;
extern f32 sMapCamEyeZ;
extern f32 sMapCamAtX;
extern f32 sMapCamAtY;
extern f32 sMapCamAtZ;
extern f32 D_menu_801CDA0C;
extern f32 D_menu_801CDA10;
extern f32 D_menu_801CDA14;
extern f32 D_menu_801CDA18;
extern f32 D_menu_801CDA1C;

void Map_PositionCursor(void);
void Map_SetState_ZoomPlanet(void);
void Map_CurrentLevel_Setup(void);
s32 Map_GetPathId(PlanetId start, PlanetId end);
bool Map_Input_CursorY(void);
void Map_LevelStart_AudioSpecSetup(LevelId level);
void Map_PlayLevel(void);
void Map_SetCamRot(f32 camAtX, f32 camAtY, f32 camAtZ, f32* camEyeX, f32* camEyeY, f32* camEyeZ, f32 srcZ, f32 xRot,
                   f32 yRot);

// Path line alphas used by the tracker
#define AP_PATH_ALPHA_AVAILABLE 255
#define AP_PATH_ALPHA_LOCKED 96
#define AP_PATH_ALPHA_UNREACHABLE 64

static bool sAccess[PLANET_MAX];
static bool sTablesPatched = false;

// ---------------------------------------------------------------------------
// gating

static bool IsPathsMode(void) {
    return AP_GetOption(AP_OPTION_LEVEL_ACCESS) == AP_OPTION_LEVEL_ACCESS_SHUFFLE_PATHS;
}

static bool HasLevelItem(LevelId level) {
    s16 item = ApTables_LevelItem(level);
    if (item <= 0) {
        return true;
    }
    return AP_HasItem((uint16_t) item);
}

static bool HasPath(PlanetId from, PlanetId to) {
    s32 pathId;

    if ((to == PLANET_VENOM) && (ApMission_MedalCount() < AP_GetOption(AP_OPTION_REQUIRED_MEDALS))) {
        switch (AP_GetOption(AP_OPTION_VICTORY_CONDITION)) {
            case AP_OPTION_VICTORY_CONDITION_ANDROSS:
            case AP_OPTION_VICTORY_CONDITION_ANDROSS_AND_ROBOT_ANDROSS:
                if (from == PLANET_AREA_6) {
                    return false;
                }
                break;
            default:
                return false;
        }
    }
    if (!IsPathsMode()) {
        return true;
    }
    pathId = Map_GetPathId(from, to);
    if ((pathId < 0) || (pathId >= 24)) {
        return false;
    }
    return AP_HasItem((uint16_t) gApTablesPathItem[pathId]);
}

static void WalkPaths(PlanetId from, bool* access) {
    PlanetId dests[3];
    s32 i;

    if ((from < 0) || (from >= PLANET_MAX) || access[from]) {
        return;
    }
    access[from] = true;
    dests[0] = sPlanets[from].dest1;
    dests[1] = sPlanets[from].dest2;
    dests[2] = sPlanets[from].warp;
    for (i = 0; i < 3; i++) {
        if ((dests[i] != PLANET_NONE) && HasPath(from, dests[i])) {
            WalkPaths(dests[i], access);
        }
    }
}

static void ComputeAccess(void) {
    s32 i;

    memset(sAccess, 0, sizeof(sAccess));
    if (IsPathsMode()) {
        WalkPaths(PLANET_CORNERIA, sAccess);
    } else {
        for (i = 0; i < PLANET_MAX; i++) {
            sAccess[i] = HasLevelItem(ApTables_PlanetToLevel(i));
        }
    }
}

bool ApMap_CanStart(LevelId level) {
    if (!AP_IsEnabled()) {
        return true;
    }
    return IsPathsMode() || HasLevelItem(level);
}

// ---------------------------------------------------------------------------
// tables

static void PatchTables(bool enable) {
    static PlanetId sOrigBolseDest1;
    static PlanetId sOrigArea6Dest1;
    static PathType sOrigPathTypes[24];
    s32 i;

    if (enable && !sTablesPatched) {
        sOrigBolseDest1 = sPlanets[PLANET_BOLSE].dest1;
        sOrigArea6Dest1 = sPlanets[PLANET_AREA_6].dest1;
        for (i = 0; i < 24; i++) {
            sOrigPathTypes[i] = sPaths[i].type;
        }
        sTablesPatched = true;
    }
    if (enable) {
        // Venom is reachable from the map (vanilla chains Bolse / Area 6 straight into it).
        sPlanets[PLANET_BOLSE].dest1 = PLANET_VENOM;
        sPlanets[PLANET_AREA_6].dest1 = PLANET_VENOM;
        // Warp paths draw as regular lines.
        for (i = 0; i < 24; i++) {
            if (sPaths[i].type == PL_WARP_YLW) {
                sPaths[i].type = PL_PATH_YLW;
            } else if (sPaths[i].type == PL_WARP_RED) {
                sPaths[i].type = PL_PATH_RED;
            }
        }
    } else if (sTablesPatched) {
        sPlanets[PLANET_BOLSE].dest1 = sOrigBolseDest1;
        sPlanets[PLANET_AREA_6].dest1 = sOrigArea6Dest1;
        for (i = 0; i < 24; i++) {
            sPaths[i].type = sOrigPathTypes[i];
        }
        sTablesPatched = false;
    }
}

// ---------------------------------------------------------------------------
// tracker (map.c map_paths)

static void RefreshTracker(void) {
    s32 i;
    s32 j;
    bool pathsMode = IsPathsMode();
    PlanetId last;
    APSlotState* st = AP_SaveState();

    ComputeAccess();

    for (i = 0; i < 24; i++) {
        PlanetPath* path = &sPaths[i];
        bool hasLocation = false;
        bool hasPath;
        PlanetId start = path->start;

        if ((start < 0) || (start >= PLANET_MAX)) {
            continue;
        }

        if (sAccess[start]) {
            s16 loc = AP_LOCATION_NONE;
            if (sPlanets[start].dest1 == path->end) {
                loc = ApTables_ClearLocation(start, MISSION_COMPLETE);
            } else if (sPlanets[start].dest2 == path->end) {
                loc = ApTables_ClearLocation(start, MISSION_ACCOMPLISHED);
            } else if (sPlanets[start].warp == path->end) {
                loc = ApTables_ClearLocation(start, MISSION_WARP);
            }
            if (loc > 0) {
                hasLocation = AP_IsLocationChecked((uint16_t) loc);
            }
        }

        hasPath = pathsMode ? HasPath(start, path->end) : sAccess[start];

        if (hasPath) {
            path->alpha = AP_PATH_ALPHA_AVAILABLE;
        } else if (sAccess[start]) {
            path->alpha = AP_PATH_ALPHA_LOCKED;
        } else {
            path->alpha = pathsMode ? 0 : AP_PATH_ALPHA_UNREACHABLE;
        }
        gPlanetPathStatus[i] = hasPath ? 3 : 4; // 3 = coloured line, 4 = grey line
        gTexturedLines[i].zScale = 0.0f;

        // No Arwing glyphs on the paths: each one is a full model (expensive on slow GL paths) and the
        // animated variant disables frame interpolation for the rest of the frame. Path colour and the
        // planet star/medal glyphs carry the tracker information instead.
        path->unk_14 = 0;
        (void) hasLocation;
    }

    // Back-fill the route panel / status data from the slot so the map doubles as a tracker.
    last = PLANET_CORNERIA;
    gTotalHits = 0;
    memset(gLeveLClearStatus, 0, sizeof(gLeveLClearStatus));
    for (i = 0; i < 7; i++) {
        PlanetId next = gMissionPlanet[i];
        bool medal = false;
        s32 status = 0;

        if ((next <= PLANET_NONE) || (next >= PLANET_MAX)) {
            continue;
        }
        gMissionHitCount[i] = st->planets[next].score;
        medal = AP_IsLocationChecked((uint16_t) ApTables_MedalLocationForPlanet(next));
        gMissionMedal[i] = medal;
        status = 0;
        if (st->planets[next].peppy) {
            status |= 0x00FF0000;
        }
        if (st->planets[next].slippy) {
            status |= 0x0000FF00;
        }
        if (st->planets[next].falco) {
            status |= 0x000000FF;
        }
        gMissionTeamStatus[i] = status;

        if (i < gMissionNumber) {
            gTotalHits += gMissionHitCount[i];
            if (pathsMode || sAccess[next]) {
                LevelId level = ApTables_PlanetToLevel(next);
                if ((level >= 0) && (level < ARRAY_COUNT(gLeveLClearStatus))) {
                    gLeveLClearStatus[level] = medal ? 2 : 1;
                }
            }
        }

        if ((i > 0) && (i <= gMissionNumber)) {
            for (j = 0; j < 24; j++) {
                if ((sPaths[j].start == last) && (sPaths[j].end == next)) {
                    sPaths[j].alpha = 255;
                    last = next;
                    break;
                }
            }
        }
    }
}

s32 ApMap_PlanetStatus(PlanetId planet) {
    if (!AP_IsEnabled() || (planet < 0) || (planet >= PLANET_MAX)) {
        return PLANET_UNCLEARED;
    }
    if (AP_IsLocationChecked((uint16_t) ApTables_MedalLocationForPlanet(planet))) {
        return PLANET_MEDAL;
    }
    // The "cleared" star doubles as the accessibility marker.
    return sAccess[planet] ? PLANET_CLEARED : PLANET_UNCLEARED;
}

// ---------------------------------------------------------------------------
// restore / persist (map.c map_init, map_load_scene_data, map_check_mission)

static void RestoreFromSlot(void) {
    APSlotState* st = AP_SaveState();
    s32 i;

    for (i = TEAM_ID_FALCO; i < TEAM_ID_MAX; i++) {
        s32 shields = (i <= TEAM_ID_PEPPY) ? st->shields[i - 1] : 255;
        gTeamShields[i] = shields;
        gSavedTeamShields[i] = shields;
        gPrevPlanetTeamShields[i] = shields;
        gPrevPlanetSavedTeamShields[i] = shields;
    }
    for (i = 0; i < 6; i++) {
        gStarWolfTeamAlive[i] = 1;
        gSavedStarWolfTeamAlive[i] = 1;
    }
    gLaserStrength[0] = st->lasers;
    gGoldRingCount[0] = gSavedGoldRingCount[0] = st->goldRings;
    gTotalHits = 0;
    gLifeCount[0] = (st->lives < 0) ? (s16) AP_GetOption(AP_OPTION_DEFAULT_LIVES) : st->lives;
    gBombCount[0] = st->bombs;
    gGreatFoxIntact = st->greatFoxIntact;
}

static void ResetConsumablesAfterGameOver(void) {
    APSlotState* st = AP_SaveState();
    s32 i;

    for (i = 0; i < 3; i++) {
        st->shields[i] = 255;
    }
    st->lasers = LASERS_SINGLE;
    st->goldRings = 0;
    st->lives = -1;
    st->bombs = 3;
    AP_MarkSaveDirty();
}

void ApMap_OnLevelStart(void) {
    APSlotState* st;
    s32 i;

    if (!AP_IsEnabled()) {
        return;
    }
    st = AP_SaveState();
    gGoldRingCount[0] = gSavedGoldRingCount[0] = st->goldRings;

    switch (gCurrentLevel) {
        case LEVEL_CORNERIA:
            for (i = 0; i < TEAM_ID_MAX; i++) {
                gTeamShields[i] = 255;
                gSavedTeamShields[i] = 255;
            }
            break;

        case LEVEL_BOLSE:
            for (i = 0; i < 4; i++) {
                gStarWolfTeamAlive[i] = 1;
            }
            for (i = 0; i < ARRAY_COUNT(gMissionPlanet); i++) {
                if (gMissionPlanet[i] == PLANET_FORTUNA) {
                    gStarWolfTeamAlive[0] = st->starWolfAlive.wolf;
                    gStarWolfTeamAlive[1] = st->starWolfAlive.leon;
                    gStarWolfTeamAlive[2] = st->starWolfAlive.pigma;
                    gStarWolfTeamAlive[3] = st->starWolfAlive.andrew;
                    break;
                }
            }
            for (i = 0; i < 4; i++) {
                gSavedStarWolfTeamAlive[i] = gStarWolfTeamAlive[i];
            }
            break;

        default:
            break;
    }
}

// ---------------------------------------------------------------------------
// map setup hooks

static void OnMapSetupPre(MapSetupPreEvent* event) {
    (void) event;
}

static void OnMapSetupPost(MapSetupPostEvent* event) {
    s32 i;

    PatchTables(AP_IsEnabled());
    if (!AP_IsEnabled()) {
        return;
    }
    if (event->lastGameState == GSTATE_GAME_OVER) {
        ResetConsumablesAfterGameOver();
    }
    RestoreFromSlot();

    for (i = 0; i < 24; i++) {
        gPlanetPathStatus[i] = 0;
        sPaths[i].unk_14 = 0;
    }
    RefreshTracker();
    AP_TakeStateChanged();
}

// Called at the end of Map_Setup_Play: instead of zooming along the forced next path, stay on the cleared planet.
void ApMap_OnSetupPlayEnd(void) {
    s32 i;

    if (!AP_IsEnabled()) {
        return;
    }
    sNextPlanetId = sCurrentPlanetId;
    D_menu_801CD98C = 0;
    D_menu_801CD980 = false;
    for (i = 0; i < PLANET_MAX; i++) {
        D_menu_801CD900[i] = 255;
        if ((i == PLANET_SECTOR_Z) || (i == PLANET_SECTOR_X) || (i == PLANET_SECTOR_Y)) {
            sPlanets[i].alpha = 144;
        } else {
            sPlanets[i].alpha = 255;
        }
    }
    D_menu_801CD970 = 255;
    D_menu_801CD974 = true;
    D_menu_801CD97C = 1;
    D_menu_801CEFC8 = 0;
    gFillScreenAlpha = 0;

    // Map_Level_CamSetup pointed the camera at the cleared planet for the (skipped) path zoom;
    // put it back on the whole Lylat system like the idle map after the Lylat card (Map_Setup values).
    sMapCamAtX = -150.55215f;
    sMapCamAtY = -489.68f;
    sMapCamAtZ = 0.0f;
    D_menu_801CDA0C = -42.750008f;
    D_menu_801CDA10 = -9.849999f;
    D_menu_801CDA14 = 3375.0f;
    D_menu_801CDA18 = 0.0f;
    D_menu_801CDA1C = 0.0f;
    Map_SetCamRot(sMapCamAtX, sMapCamAtY, sMapCamAtZ, &sMapCamEyeX, &sMapCamEyeY, &sMapCamEyeZ, D_menu_801CDA14,
                  D_menu_801CDA0C + D_menu_801CDA18, D_menu_801CDA10 + D_menu_801CDA1C);

    sMapSubState = 0;
    sMapState = MAP_IDLE;
}

// Venom has no briefing data; start the level directly once the zoom state has been set up.
bool ApMap_StartVenomFromZoom(void) {
    if (!AP_IsEnabled() || (sCurrentPlanetId != PLANET_VENOM) || (sMapSubState == 0)) {
        return false;
    }
    ApMap_OnLevelStart();
    Map_LevelStart_AudioSpecSetup(gCurrentLevel);
    sLevelStartState = 0;
    D_menu_801CD968 = 0;
    Map_PlayLevel();
    return true;
}

// ---------------------------------------------------------------------------
// navigation (map.c map_idle)

static void SelectPlanet(PlanetId planet) {
    sCurrentPlanetId = planet;
    sNextPlanetId = planet;
    Map_CurrentLevel_Setup();
    Map_PositionCursor();
    RefreshTracker();
}

static void NavigateForward(void) {
    PlanetId dests[3];
    s32 i;

    if (gMissionNumber >= 6) {
        AUDIO_PLAY_SFX(NA_SE_ERROR, gDefaultSfxSource, 4);
        return;
    }
    dests[0] = sPlanets[sCurrentPlanetId].dest1;
    dests[1] = sPlanets[sCurrentPlanetId].dest2;
    dests[2] = sPlanets[sCurrentPlanetId].warp;
    for (i = 0; i < 3; i++) {
        if ((dests[i] == PLANET_NONE) || !HasPath(sCurrentPlanetId, dests[i])) {
            continue;
        }
        sPrevPlanetId = sCurrentPlanetId;
        gMissionNumber++;
        gMissionPlanet[gMissionNumber] = dests[i];
        AUDIO_PLAY_SFX(NA_SE_CURSOR, gDefaultSfxSource, 4);
        SelectPlanet(dests[i]);
        return;
    }
    AUDIO_PLAY_SFX(NA_SE_ERROR, gDefaultSfxSource, 4);
}

static void NavigateBack(void) {
    if (gMissionNumber <= 0) {
        AUDIO_PLAY_SFX(NA_SE_ERROR, gDefaultSfxSource, 4);
        return;
    }
    if (gMissionNumber < 6) {
        gMissionPlanet[gMissionNumber] = PLANET_NONE;
    }
    gMissionNumber--;
    sPrevPlanetId = (gMissionNumber > 0) ? gMissionPlanet[gMissionNumber - 1] : PLANET_NONE;
    AUDIO_PLAY_SFX(NA_SE_CURSOR, gDefaultSfxSource, 4);
    SelectPlanet(gMissionPlanet[gMissionNumber]);
}

static void NavigateSibling(bool up) {
    PlanetId dests[3];
    PlanetId dest = PLANET_NONE;
    s32 i;

    if ((gMissionNumber <= 0) || (gMissionNumber >= 6) || (sPrevPlanetId < 0) || (sPrevPlanetId >= PLANET_MAX)) {
        return;
    }
    if (up) {
        dests[0] = sPlanets[sPrevPlanetId].warp;
        dests[1] = sPlanets[sPrevPlanetId].dest2;
        dests[2] = sPlanets[sPrevPlanetId].dest1;
    } else {
        dests[0] = sPlanets[sPrevPlanetId].dest1;
        dests[1] = sPlanets[sPrevPlanetId].dest2;
        dests[2] = sPlanets[sPrevPlanetId].warp;
    }
    for (i = 0; i < 3; i++) {
        if (dests[i] == sCurrentPlanetId) {
            break;
        }
        if ((dests[i] == PLANET_NONE) || !HasPath(sPrevPlanetId, dests[i])) {
            continue;
        }
        dest = dests[i];
    }
    if (dest != PLANET_NONE) {
        gMissionPlanet[gMissionNumber] = dest;
        AUDIO_PLAY_SFX(NA_SE_CURSOR, gDefaultSfxSource, 4);
        SelectPlanet(dest);
    }
}

static void OpenMapMenu(void) {
    Audio_PlayMapMenuSfx(1);
    D_menu_801CEFC4 = 1;
    D_menu_801CEFD4 = 0;
    D_menu_801CEFDC = 0;
    D_menu_801CEFE0 = 32.0f;
    D_menu_801CEFE8[0] = D_menu_801CEFE8[1] = D_menu_801CEFE8[2] = 255.0f;
    sMapState = MAP_PATH_CHANGE;
    D_menu_801CD94C = 0;
}

static void OnMapIdleUpdate(MapIdleUpdateEvent* event) {
    u16 press;
    s32 i;

    if (!AP_IsEnabled()) {
        return;
    }
    if (AP_TakeStateChanged()) {
        RefreshTracker();
    }
    ApItems_GrantLives();

    press = gControllerPress[gMainController].button;

    if (press & A_BUTTON) {
        event->event.cancelled = true;
        if (!ApMap_CanStart(gCurrentLevel)) {
            AUDIO_PLAY_SFX(NA_SE_ERROR, gDefaultSfxSource, 4);
            return;
        }
        for (i = 0; i < TEAM_ID_MAX; i++) {
            D_ctx_80177C58[i] = gTeamShields[i];
        }
        Map_SetState_ZoomPlanet();
        return;
    }

    if (press & START_BUTTON) {
        event->event.cancelled = true;
        OpenMapMenu();
        return;
    }

    if (press & R_JPAD) {
        NavigateForward();
    } else if (press & L_JPAD) {
        NavigateBack();
    } else if (press & U_JPAD) {
        NavigateSibling(true);
    } else if (press & D_JPAD) {
        NavigateSibling(false);
    }

    if (press & B_BUTTON) {
        // Back to the main menu (same transition the training level uses).
        event->event.cancelled = true;
        Audio_KillSfxById(NA_SE_MAP_MOVE);
        Audio_KillSfxById(NA_SE_MAP_ROLL);
        gGameState = GSTATE_MENU;
        gNextGameStateTimer = 2;
        gOptionMenuStatus = OPTION_WAIT;
        gDrawMode = DRAW_NONE;
        gLastGameState = GSTATE_NONE;
        gStarCount = 0;
    }
}

// ---------------------------------------------------------------------------
// heal-teammates menu (replaces Change Course / Retry; hud.c hud_map_menu_*)

static const TeamId sHealTeamIds[3] = { TEAM_ID_PEPPY, TEAM_ID_SLIPPY, TEAM_ID_FALCO };
static char* sHealTeamNames[3] = { "PEPPY", "SLIPPY", "FALCO" };

static void OnMapMenuInput(MapMenuInputEvent* event) {
    u16 press;
    f32 temp;

    if (!AP_IsEnabled()) {
        return;
    }
    event->event.cancelled = true;

    if (Map_Input_CursorY()) {
        D_menu_801CEFE8[D_menu_801CEFDC] = 255.0f;
        if (D_menu_801CEFD4 < 0) {
            D_menu_801CEFDC++;
            if (D_menu_801CEFDC >= 3) {
                D_menu_801CEFDC = 0;
            }
        } else {
            D_menu_801CEFDC--;
            if (D_menu_801CEFDC < 0) {
                D_menu_801CEFDC = 2;
            }
        }
    }

    temp = Math_SmoothStepToF(&D_menu_801CEFE8[D_menu_801CEFDC], D_menu_801CEFE0, 0.5f, 100.0f, 1.0f);
    if (temp == 0.0f) {
        D_menu_801CEFE0 = (D_menu_801CEFE0 == 32.0f) ? 255.0f : 32.0f;
    }

    press = gControllerPress[gMainController].button;
    if (press & (B_BUTTON | START_BUTTON)) {
        Audio_PlayMapMenuSfx(0);
        D_menu_801CEFC4 = 0;
        sMapState = MAP_IDLE;
        return;
    }
    if (press & A_BUTTON) {
        TeamId id = sHealTeamIds[D_menu_801CEFDC];
        s32* shields = &gTeamShields[id];

        if ((gLifeCount[0] > 0) && (*shields < 255)) {
            APSlotState* st = AP_SaveState();

            AUDIO_PLAY_SFX(NA_SE_DECIDE, gDefaultSfxSource, 4);
            if (*shields < 0) {
                *shields = 0;
            }
            *shields += 128;
            if (*shields > 255) {
                *shields = 255;
            }
            gSavedTeamShields[id] = *shields;
            gPrevPlanetTeamShields[id] = *shields;
            gPrevPlanetSavedTeamShields[id] = *shields;
            D_ctx_80177C58[id] = *shields;
            st->shields[id - 1] = (int16_t) *shields;
            gLifeCount[0]--;
            st->lives = (int8_t) gLifeCount[0];
            AP_MarkSaveDirty();
        } else {
            AUDIO_PLAY_SFX(NA_SE_ERROR, gDefaultSfxSource, 4);
        }
    }
}

static void OnMapMenuDraw(MapMenuDrawEvent* event) {
    f32 x = 96.0f;
    f32 y = 88.0f;
    s32 i;
    char buf[32];

    if (!AP_IsEnabled()) {
        return;
    }
    event->event.cancelled = true;

    RCP_SetupDL(&gMasterDisp, SETUPDL_76);
    gDPSetPrimColor(gMasterDisp++, 0, 0, 60, 60, 255, 170);
    Lib_TextureRect_IA8(&gMasterDisp, (u8*) aMapOptionBgTex, 24, 17, x, y, 5.4f, 4.3f);

    RCP_SetupDL(&gMasterDisp, SETUPDL_83_OPTIONAL);
    gDPSetPrimColor(gMasterDisp++, 0, 0, 255, 255, 0, 255);
    Graphics_DisplaySmallText((s32) x + 8, (s32) y + 6, 1.0f, 1.0f, "-1 ARWING TO HEAL");

    for (i = 0; i < 3; i++) {
        s32 shields = gTeamShields[sHealTeamIds[i]];
        s32 pct = (shields > 0) ? (shields * 100) / 255 : 0;

        if (i == D_menu_801CEFDC) {
            gDPSetPrimColor(gMasterDisp++, 0, 0, 255, (s32) D_menu_801CEFE8[i], (s32) D_menu_801CEFE8[i], 255);
        } else {
            gDPSetPrimColor(gMasterDisp++, 0, 0, 160, 160, 160, 255);
        }
        sprintf(buf, "%s: %d", sHealTeamNames[i], pct);
        Graphics_DisplaySmallText((s32) x + 8, (s32) y + 22 + i * 12, 1.0f, 1.0f, buf);
    }
}

// ---------------------------------------------------------------------------
// HUD: medal counter (map.c map_draw_medals)

static void OnMapHudDraw(MapHudDrawEvent* event) {
    s32 have;
    s32 required;
    char buf[32];

    (void) event;
    if (!AP_IsEnabled()) {
        return;
    }
    if ((sMapState != MAP_IDLE) && (sMapState != MAP_PATH_CHANGE)) {
        return;
    }
    required = AP_GetOption(AP_OPTION_REQUIRED_MEDALS);
    if (!AP_GetOption(AP_OPTION_SHUFFLE_MEDALS) && (required == 0)) {
        return;
    }
    have = ApMission_MedalCount();
    if (required > 0) {
        sprintf(buf, "MEDALS %d OF %d", have, required);
    } else {
        sprintf(buf, "MEDALS %d", have);
    }
    RCP_SetupDL(&gMasterDisp, SETUPDL_83_OPTIONAL);
    gDPSetPrimColor(gMasterDisp++, 0, 0, 255, 255, 0, 255);
    Graphics_DisplaySmallText(24, 36, 1.0f, 1.0f, buf);
}

void ApMap_Init(void) {
    REGISTER_LISTENER(MapSetupPreEvent, (EventCallback) OnMapSetupPre, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(MapSetupPostEvent, (EventCallback) OnMapSetupPost, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(MapIdleUpdateEvent, (EventCallback) OnMapIdleUpdate, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(MapMenuInputEvent, (EventCallback) OnMapMenuInput, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(MapMenuDrawEvent, (EventCallback) OnMapMenuDraw, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(MapHudDrawEvent, (EventCallback) OnMapHudDraw, EVENT_PRIORITY_NORMAL);
}
