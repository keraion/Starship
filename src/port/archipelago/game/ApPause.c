#include "ApGame.h"
#include <stdio.h>

// ---------------------------------------------------------------------------
// Pause menu: Continue / Retry Course / Respawn / Back to Map (hud.c of the ROM hack)

extern s32 sPauseScreenIwork[10];
extern s32 sPauseScreenTimer[10];
s32 HUD_PauseScreenInput(void);
void HUD_MsgWindowBg_Draw2(f32 x, f32 y, f32 xScale, f32 yScale);

bool gApPauseIgnoreRewards = false;

typedef enum {
    AP_PAUSE_CONTINUE,
    AP_PAUSE_RETRY,
    AP_PAUSE_RESPAWN,
    AP_PAUSE_BACK_TO_MAP,
    AP_PAUSE_COUNT,
} ApPauseEntry;

static s32 EntryCount(void) {
    return (gCurrentLevel == LEVEL_TRAINING) ? 2 : AP_PAUSE_COUNT;
}

// Kill the player where they are; the vanilla death handling respawns at the last checkpoint.
static void Respawn(void) {
    Player* player = &gPlayer[0];

    if (player->state == PLAYERSTATE_ACTIVE) {
        player->shields = 0;
        player->radioDamageTimer = 2;
    }
}

// Leave the level for the map without registering a clear (same transition as the debug "jump to map" cheat).
static void BackToMap(void) {
    gApPauseIgnoreRewards = true;
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

static void OnPauseInput(PauseMenuInputEvent* event) {
    s32 count;
    s32 sel;
    s32 dir;
    u16 press;

    if (!AP_IsEnabled()) {
        return;
    }
    event->event.cancelled = true;

    count = EntryCount();
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
        switch (sel) {
            case AP_PAUSE_CONTINUE:
                sPauseScreenIwork[0] = 10;
                break;
            case AP_PAUSE_RETRY:
                sPauseScreenIwork[0] = 2; // vanilla retry / quit training sequence
                break;
            case AP_PAUSE_RESPAWN:
                sPauseScreenIwork[0] = 10;
                Respawn();
                break;
            case AP_PAUSE_BACK_TO_MAP:
                Audio_PlayPauseSfx(0);
                BackToMap();
                break;
        }
    }
}

static void OnPauseDraw(PauseMenuDrawEvent* event) {
    static char* sLabels[AP_PAUSE_COUNT] = { "CONTINUE", "RETRY COURSE", "RESPAWN", "BACK TO MAP" };
    s32 count;
    s32 i;
    s32 pulse;
    s16 medalOption;
    char buf[32];

    if (!AP_IsEnabled()) {
        return;
    }
    event->event.cancelled = true;

    count = EntryCount();
    HUD_MsgWindowBg_Draw2(event->x - 10.0f, event->y - 4.0f, 4.7f, (count > 2) ? 3.9f : 2.8f);

    pulse = sPauseScreenTimer[0] % 20;
    if (pulse >= 10) {
        pulse = 20 - pulse;
    }
    pulse = pulse * 16 - 1;
    if (pulse < 0) {
        pulse = 0;
    }

    // The vanilla option textures are centred on x = 160; do the same with the small font.
    RCP_SetupDL(&gMasterDisp, SETUPDL_83_OPTIONAL);
    for (i = 0; i < count; i++) {
        char* label = sLabels[i];
        if ((i == AP_PAUSE_RETRY) && (gCurrentLevel == LEVEL_TRAINING)) {
            label = "QUIT TRAINING";
        }
        if (i == sPauseScreenIwork[1]) {
            gDPSetPrimColor(gMasterDisp++, 0, 0, 160, pulse, pulse, 255);
        } else {
            gDPSetPrimColor(gMasterDisp++, 0, 0, 64, 64, 64, 255);
        }
        Graphics_DisplaySmallText(160 - Graphics_GetSmallTextWidth(label) / 2, (s32) event->y + i * 12, 1.0f, 1.0f,
                                  label);
    }

    // Hit requirement for this level's medal (from the seed's options): two short lines to the left of the
    // menu box (the bottom of the screen belongs to the team status strip).
    medalOption = ApTables_MedalOption(gCurrentLevel);
    if (medalOption >= 0) {
        sprintf(buf, "%d HITS", AP_GetOption((uint16_t) medalOption));
        gDPSetPrimColor(gMasterDisp++, 0, 0, 255, 255, 0, 255);
        Graphics_DisplaySmallText(20, (s32) event->y, 1.0f, 1.0f, "MEDAL");
        Graphics_DisplaySmallText(20, (s32) event->y + 10, 1.0f, 1.0f, buf);
    }
}

void ApPause_Init(void) {
    REGISTER_LISTENER(PauseMenuInputEvent, (EventCallback) OnPauseInput, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(PauseMenuDrawEvent, (EventCallback) OnPauseDraw, EVENT_PRIORITY_NORMAL);
}
