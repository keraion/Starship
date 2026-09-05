#include "RestartPoint.h"
#include "port/mods/PortEnhancements.h"
#include "port/archipelago/ArchipelagoBridge.h"
#include "port/hooks/Events.h"

extern s32 sPauseScreenIwork[10];
extern s32 sPauseScreenTimer[10];
s32 HUD_PauseScreenInput(void);
void HUD_MsgWindowBg_Draw2(f32 x, f32 y, f32 xScale, f32 yScale);

// ---------------------------------------------------------------------------
// Loadout snapshots
//
// The engine already snapshots position and progress for checkpoints (gSavedPathProgress,
// gSavedObjectLoadIndex, gSavedHitCount, gSavedTeamShields in ItemCheckpoint_Update) but nothing about
// the craft: the death path deliberately resets bombs to 3 and lasers to single. These are the pieces
// it throws away, captured at the same two moments.

// Frames spent in PLAYERSTATE_NEXT before the fade to black begins. The death sequence uses 70 to let
// the explosion play out; a voluntary restart has nothing to watch, so it just needs long enough not to
// cut abruptly.
#define kRestartFadeDelay 10

typedef struct {
    bool valid;
    s32 lasers;
    s32 bombs;
    u8 goldRings;
    s32 rightWingHealth;
    s32 leftWingHealth;
    u8 rightWingState;
    u8 leftWingState;
} RestartLoadout;

static RestartLoadout sAtLevelStart;
static RestartLoadout sAtCheckpoint;
static RestartLoadout sPending;
static bool sRestorePending = false;

static s32 sLastLevel = -1;
static s32 sLastSavedObjectLoadIndex = -1;
static bool sLevelStartPending = false;
static bool sSubmenuActive = false; // "RESTART >" submenu is showing instead of the pause menu

static void Capture(RestartLoadout* out) {
    Player* player = &gPlayer[0];

    out->lasers = gLaserStrength[gPlayerNum];
    out->bombs = gBombCount[gPlayerNum];
    out->goldRings = gGoldRingCount[0];
    out->rightWingHealth = gRightWingHealth[0];
    out->leftWingHealth = gLeftWingHealth[0];
    out->rightWingState = player->arwing.rightWingState;
    out->leftWingState = player->arwing.leftWingState;
    out->valid = true;
}

static void Apply(const RestartLoadout* in) {
    Player* player = &gPlayer[0];

    if (!in->valid) {
        return;
    }
    gLaserStrength[gPlayerNum] = in->lasers;
    gBombCount[gPlayerNum] = in->bombs;
    gGoldRingCount[0] = in->goldRings;
    gSavedGoldRingCount[0] = in->goldRings;
    gRightWingHealth[0] = in->rightWingHealth;
    gLeftWingHealth[0] = in->leftWingHealth;
    player->arwing.rightWingState = in->rightWingState;
    player->arwing.leftWingState = in->leftWingState;
}

// ---------------------------------------------------------------------------

bool RestartPoint_Enabled(void) {
    return (CVarGetInteger("gEnhancements.CheckpointRestart", 0) != 0) && (gCurrentLevel != LEVEL_TRAINING) &&
           !gVersusMode;
}

bool RestartPoint_HasCheckpoint(void) {
    // gSavedObjectLoadIndex is zeroed at level start and set by ItemCheckpoint_Update when the ring is
    // taken, so it doubles as "a checkpoint exists to go back to".
    return sAtCheckpoint.valid && (gSavedObjectLoadIndex > 0);
}

// Restarting drops straight into the tail of the engine's death sequence: PLAYERSTATE_NEXT plus
// gFadeoutType 7 is what drives fox_play.c to fade out and re-enter the level through PLAY_INIT, which
// rebuilds it from whatever gSaved* state we leave below.
//
// Entering there rather than at PLAYERSTATE_DOWN skips the Arwing's death animation, which is the
// point: this is a voluntary restart, not a crash. It also means Cutscene_KillPlayer never runs, so
// nothing decrements gLifeCount and nothing needs to be handed back, and the player never passes
// through PLAYERSTATE_DOWN, so an Archipelago DeathLink is never broadcast for it.
void RestartPoint_Restart(RestartPointKind kind) {
    Player* player = &gPlayer[0];

    if (kind == RESTART_POINT_LEVEL_START) {
        // Drop the checkpoint so Play_Init rebuilds the level from the top.
        gSavedObjectLoadIndex = 0;
        gSavedPathProgress = 0.0f;
        gSavedHitCount = 0;
        gAllRangeCheckpoint = 0;
        sPending = sAtLevelStart;
    } else {
        sPending = sAtCheckpoint;
    }

    // Never give back less than the player is holding right now. Laser upgrades, bombs and gold rings
    // can be Archipelago items, and ApItems.c grants each one exactly once against a high-water mark in
    // APSlotState; dropping the count here would strand that mark and destroy the item permanently. It
    // also matches what a free restart should feel like: it can undo losses, never take winnings.
    {
        RestartLoadout now;

        Capture(&now);
        if (!sPending.valid) {
            sPending = now;
        } else {
            if (now.lasers > sPending.lasers) {
                sPending.lasers = now.lasers;
            }
            if (now.bombs > sPending.bombs) {
                sPending.bombs = now.bombs;
            }
            if (now.goldRings > sPending.goldRings) {
                sPending.goldRings = now.goldRings;
            }
        }
    }
    sRestorePending = sPending.valid;

    sPauseScreenIwork[0] = 10; // close the pause menu

    Audio_StopPlayerNoise(player->num);
    Audio_ClearVoice();
    gRadioState = 0;
    gShowBossHealth = false;
    player->state = PLAYERSTATE_NEXT;
    player->csTimer = kRestartFadeDelay;
    player->dmgEffectTimer = 0;
    gFadeoutType = 7;
}

// ---------------------------------------------------------------------------
// Snapshot upkeep

static void OnPlayUpdate(PlayUpdateEvent* event) {
    (void) event;

    // Play resumed, so the submenu is not on screen any more; do not leave it up for the next pause.
    if (gPlayState != PLAY_PAUSE) {
        sSubmenuActive = false;
    }

    if (gVersusMode || (gCurrentLevel == LEVEL_TRAINING)) {
        return;
    }

    if (gCurrentLevel != sLastLevel) {
        // Entering a level. The snapshot cannot be taken on this frame: the Arwing has not been set up
        // yet, so its wings still read WINGSTATE_NONE and the laser strength the player arrived with
        // has not been applied. Capturing here is what used to restart them wingless on a single laser.
        sLastLevel = gCurrentLevel;
        sAtLevelStart.valid = false;
        sAtCheckpoint.valid = false;
        sLevelStartPending = true;
        sRestorePending = false; // a restore for a level we have since left must not apply to this one
    } else if (gSavedObjectLoadIndex < sLastSavedObjectLoadIndex) {
        // We restarted the level: the checkpoint is gone, but the loadout we arrived with still stands.
        sAtCheckpoint.valid = false;
    } else if (gSavedObjectLoadIndex > sLastSavedObjectLoadIndex) {
        // ItemCheckpoint_Update just took a checkpoint; snapshot the craft alongside it.
        Capture(&sAtCheckpoint);
    }
    sLastSavedObjectLoadIndex = gSavedObjectLoadIndex;

    // First frame actually flying: this is the loadout a level-start restart goes back to.
    if (sLevelStartPending && (gPlayState == PLAY_UPDATE) && (gPlayer[0].state == PLAYERSTATE_ACTIVE)) {
        sLevelStartPending = false;
        Capture(&sAtLevelStart);
    }
}

static void OnPlayerUpdatePost(PlayerPostUpdateEvent* event) {
    Player* player = event->player;

    if ((player != &gPlayer[0]) || (gPlayState != PLAY_UPDATE)) {
        return;
    }

    if (!sRestorePending) {
        return;
    }
    // Wait for PLAYERSTATE_ACTIVE rather than the level intro: by then Play_Init, the fadeout handler's
    // "bombs = 3, lasers = single" reset and the Arwing's own setup have all run, so nothing downstream
    // can overwrite what we put back.
    if (player->state == PLAYERSTATE_ACTIVE) {
        Apply(&sPending);
        sRestorePending = false;
    }
}

// ---------------------------------------------------------------------------
// "RESTART >" submenu

typedef enum {
    SUB_LEVEL_START,
    SUB_CHECKPOINT,
    SUB_BACK,
    SUB_MAX,
} RestartSubEntry;

static s32 sSubmenuSel = 0;
static s32 sSubEntries[SUB_MAX];

static s32 BuildSubEntries(void) {
    s32 n = 0;

    sSubEntries[n++] = SUB_LEVEL_START;
    if (RestartPoint_HasCheckpoint()) {
        sSubEntries[n++] = SUB_CHECKPOINT;
    }
    sSubEntries[n++] = SUB_BACK;
    return n;
}

static const char* SubLabel(s32 kind) {
    switch (kind) {
        case SUB_LEVEL_START:
            return "FROM LEVEL START";
        case SUB_CHECKPOINT:
            return "FROM CHECKPOINT";
        case SUB_BACK:
        default:
            return "BACK";
    }
}

bool RestartPoint_SubmenuActive(void) {
    return sSubmenuActive;
}

void RestartPoint_SubmenuEnter(void) {
    sSubmenuActive = true;
    sSubmenuSel = 0;
}

void RestartPoint_SubmenuInput(void) {
    s32 count = BuildSubEntries();
    s32 dir;
    u16 press;

    if (sSubmenuSel >= count) {
        sSubmenuSel = count - 1;
    }

    dir = HUD_PauseScreenInput();
    if (dir != 0) {
        sPauseScreenTimer[0] = 0;
        if ((dir > 0) && (sSubmenuSel > 0)) {
            sSubmenuSel--;
            AUDIO_PLAY_SFX(NA_SE_CURSOR, gDefaultSfxSource, 4);
        } else if ((dir < 0) && (sSubmenuSel < count - 1)) {
            sSubmenuSel++;
            AUDIO_PLAY_SFX(NA_SE_CURSOR, gDefaultSfxSource, 4);
        }
    }

    press = gInputPress->button;
    if (press & B_BUTTON) {
        sSubmenuActive = false; // back to the pause menu, not out of the pause
        AUDIO_PLAY_SFX(NA_SE_CURSOR, gDefaultSfxSource, 4);
        return;
    }
    if (press & A_BUTTON) {
        switch (sSubEntries[sSubmenuSel]) {
            case SUB_LEVEL_START:
                sSubmenuActive = false;
                RestartPoint_Restart(RESTART_POINT_LEVEL_START);
                break;
            case SUB_CHECKPOINT:
                sSubmenuActive = false;
                RestartPoint_Restart(RESTART_POINT_CHECKPOINT);
                break;
            case SUB_BACK:
                sSubmenuActive = false;
                AUDIO_PLAY_SFX(NA_SE_CURSOR, gDefaultSfxSource, 4);
                break;
        }
    }
}

void RestartPoint_SubmenuDraw(f32 x, f32 y) {
    s32 count = BuildSubEntries();
    s32 pulse;
    s32 i;

    HUD_MsgWindowBg_Draw2(x - 10.0f, y - 4.0f, 4.7f, (count > 2) ? 3.9f : 2.8f);

    pulse = sPauseScreenTimer[0] % 20;
    if (pulse >= 10) {
        pulse = 20 - pulse;
    }
    pulse = pulse * 16 - 1;
    if (pulse < 0) {
        pulse = 0;
    }

    RCP_SetupDL(&gMasterDisp, SETUPDL_83_OPTIONAL);
    for (i = 0; i < count; i++) {
        char* label = (char*) SubLabel(sSubEntries[i]);

        if (i == sSubmenuSel) {
            gDPSetPrimColor(gMasterDisp++, 0, 0, 160, pulse, pulse, 255);
        } else {
            gDPSetPrimColor(gMasterDisp++, 0, 0, 64, 64, 64, 255);
        }
        Graphics_DisplaySmallText(160 - Graphics_GetSmallTextWidth(label) / 2, (s32) y + i * 12, 1.0f, 1.0f, label);
    }
}

// ---------------------------------------------------------------------------
// Vanilla pause menu replacement (the Archipelago one lives in ApPause.c and calls the same helpers)

typedef enum {
    VP_CONTINUE,
    VP_RESTART,
    VP_BACK_TO_MAP,
    VP_MAX,
} VanillaPauseEntry;

static s32 sEntryKind[VP_MAX];

static s32 BuildEntries(void) {
    s32 n = 0;

    sEntryKind[n++] = VP_CONTINUE;
    sEntryKind[n++] = VP_RESTART;
    sEntryKind[n++] = VP_BACK_TO_MAP;
    return n;
}

static const char* EntryLabel(s32 kind) {
    switch (kind) {
        case VP_RESTART:
            return "RESTART";
        case VP_BACK_TO_MAP:
            return "BACK TO MAP";
        case VP_CONTINUE:
        default:
            return "CONTINUE";
    }
}

// Leave the level for the map without registering a clear.
static void BackToMap(void) {
    gLeveLClearStatus[gCurrentLevel] = 0;
    gMissionStatus = MISSION_COMPLETE;

    Audio_StopPlayerNoise(gPlayer[0].num);
    Audio_ClearVoice();
    gRadioState = 0;
    Play_ClearObjectData();
    gShowBossHealth = false;

    gShowLevelClearStatusScreen = false;
    gLevelStartStatusScreenTimer = 0;
    gStarCount = 0;
    gPlayerGlareAlphas[0] = 0;
    gControllerLock = 3;

    gGameState = GSTATE_MAP;
    gNextGameStateTimer = 2;
    gMapState = 0;
    gLastGameState = GSTATE_PLAY;
    gDrawMode = DRAW_NONE;
    gPlayState = PLAY_UPDATE;
}

static bool ShouldHandleVanillaPause(void) {
    // Archipelago replaces the pause menu itself and calls these helpers from there.
    return RestartPoint_Enabled() && !AP_IsEnabled();
}

static void OnPauseInput(PauseMenuInputEvent* event) {
    s32 count;
    s32 sel;
    s32 dir;
    u16 press;

    if (!ShouldHandleVanillaPause()) {
        return;
    }
    event->event.cancelled = true;

    if (RestartPoint_SubmenuActive()) {
        RestartPoint_SubmenuInput();
        return;
    }

    count = BuildEntries();
    sel = sPauseScreenIwork[1];
    if (sel >= count) {
        sel = count - 1;
    }

    dir = HUD_PauseScreenInput();
    if (dir != 0) {
        sPauseScreenTimer[0] = 0;
        if ((dir > 0) && (sel > 0)) {
            sel--;
            AUDIO_PLAY_SFX(NA_SE_CURSOR, gDefaultSfxSource, 4);
        } else if ((dir < 0) && (sel < count - 1)) {
            sel++;
            AUDIO_PLAY_SFX(NA_SE_CURSOR, gDefaultSfxSource, 4);
        }
    }
    sPauseScreenIwork[1] = sel;

    press = gInputPress->button;
    if (press & B_BUTTON) {
        sPauseScreenIwork[0] = 10; // continue
        return;
    }
    if (press & A_BUTTON) {
        switch (sEntryKind[sel]) {
            case VP_CONTINUE:
                sPauseScreenIwork[0] = 10;
                break;
            case VP_RESTART:
                RestartPoint_SubmenuEnter();
                AUDIO_PLAY_SFX(NA_SE_CURSOR, gDefaultSfxSource, 4);
                break;
            case VP_BACK_TO_MAP:
                Audio_PlayPauseSfx(0);
                BackToMap();
                break;
        }
    }
}

static void OnPauseDraw(PauseMenuDrawEvent* event) {
    s32 count;
    s32 i;
    s32 pulse;

    if (!ShouldHandleVanillaPause()) {
        return;
    }
    event->event.cancelled = true;

    if (RestartPoint_SubmenuActive()) {
        RestartPoint_SubmenuDraw(event->x, event->y);
        return;
    }

    count = BuildEntries();
    HUD_MsgWindowBg_Draw2(event->x - 10.0f, event->y - 4.0f, 4.7f, (count > 2) ? 3.9f : 2.8f);

    pulse = sPauseScreenTimer[0] % 20;
    if (pulse >= 10) {
        pulse = 20 - pulse;
    }
    pulse = pulse * 16 - 1;
    if (pulse < 0) {
        pulse = 0;
    }

    RCP_SetupDL(&gMasterDisp, SETUPDL_83_OPTIONAL);
    for (i = 0; i < count; i++) {
        const char* label = EntryLabel(sEntryKind[i]);

        if (i == sPauseScreenIwork[1]) {
            gDPSetPrimColor(gMasterDisp++, 0, 0, 160, pulse, pulse, 255);
        } else {
            gDPSetPrimColor(gMasterDisp++, 0, 0, 64, 64, 64, 255);
        }
        Graphics_DisplaySmallText(160 - Graphics_GetSmallTextWidth((char*) label) / 2, (s32) event->y + i * 12, 1.0f,
                                  1.0f, (char*) label);
    }
}

void RestartPoint_Init(void) {
    REGISTER_LISTENER(PlayUpdateEvent, (EventCallback) OnPlayUpdate, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(PlayerPostUpdateEvent, (EventCallback) OnPlayerUpdatePost, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(PauseMenuInputEvent, (EventCallback) OnPauseInput, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(PauseMenuDrawEvent, (EventCallback) OnPauseDraw, EVENT_PRIORITY_NORMAL);
}
