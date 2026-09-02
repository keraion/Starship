#include "ApGame.h"

// Main menu: block starting the game until the Archipelago session is ready (Z+R held bypasses).
static void OnMainMenuSelect(MainMenuSelectEvent* event) {
    u16 hold;

    if (!AP_IsEnabled()) {
        return;
    }
    if ((event->cursor != OPTION_MAP) && (event->cursor != OPTION_TRAINING)) {
        return;
    }
    if (AP_IsReady()) {
        return;
    }
    hold = gControllerHold[gMainController].button;
    if ((hold & Z_TRIG) && (hold & R_TRIG)) {
        return;
    }
    AUDIO_PLAY_SFX(NA_SE_ERROR, gDefaultSfxSource, 4);
    event->event.cancelled = true;
}

void ApMenu_DrawStatus(void) {
    if (!AP_IsEnabled()) {
        return;
    }
    RCP_SetupDL(&gMasterDisp, SETUPDL_83_OPTIONAL);
    if (AP_IsReady()) {
        gDPSetPrimColor(gMasterDisp++, 0, 0, 96, 255, 96, 255);
        Graphics_DisplaySmallText(20, 20, 1.0f, 1.0f, "AP CONNECTED");
    } else {
        gDPSetPrimColor(gMasterDisp++, 0, 0, 255, 96, 96, 255);
        Graphics_DisplaySmallText(20, 20, 1.0f, 1.0f, "AP DISCONNECTED");
    }
}

void ApMenu_Init(void) {
    REGISTER_LISTENER(MainMenuSelectEvent, (EventCallback) OnMainMenuSelect, EVENT_PRIORITY_NORMAL);
}

void ApGame_Init(void) {
    ApItems_Init();
    ApMission_Init();
    ApMap_Init();
    ApMenu_Init();
    ApPause_Init();
}
