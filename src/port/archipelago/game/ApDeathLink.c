#include "ApGame.h"

// ---------------------------------------------------------------------------
// DeathLink: broadcast our deaths to the room and apply the ones that come back.
//
// Follows the ROM hack (n64/src/main.c): one queue of deaths we owe, fed both by the network and by the
// pause menu's Respawn. Entering PLAYERSTATE_DOWN consumes a queued death instead of broadcasting it,
// which is what keeps a received death from echoing back out and keeps Respawn off the wire.
//
// The kill itself is the same one the pause menu used to do inline (empty shields plus a damage timer,
// which Player_UpdateOnRails / Player_Update360 turn into Player_Down on the next update, for every
// craft form), reapplied until it lands.

static s32 sPrevPlayerState = PLAYERSTATE_STANDBY;
static s32 sOwedDeaths = 0;

// Pause menu Respawn: a death we asked for ourselves, so it must not go out over the wire.
void ApDeathLink_QueueLocalDeath(void) {
    sOwedDeaths++;
}

// Player 0 in a normal level. Training and Versus have no Archipelago run to speak of.
static bool InLevel(Player* player) {
    return AP_IsEnabled() && (gGameState == GSTATE_PLAY) && (gPlayState == PLAY_UPDATE) && (player == &gPlayer[0]) &&
           (gCurrentLevel != LEVEL_TRAINING) && !gVersusMode;
}

static void OnPlayerUpdatePost(PlayerPostUpdateEvent* event) {
    Player* player = event->player;
    s32 state;

    if (!InLevel(player)) {
        return;
    }
    state = player->state;

    // Keep asking until it takes: the kill only lands on a player who is actually flying.
    if ((sOwedDeaths > 0) && (state == PLAYERSTATE_ACTIVE)) {
        player->shields = 0;
        player->radioDamageTimer = 2;
    }

    if ((state == PLAYERSTATE_DOWN) && (sPrevPlayerState != PLAYERSTATE_DOWN)) {
        if (sOwedDeaths > 0) {
            sOwedDeaths--; // asked for by another world or by Respawn: do not send it back out
        } else if (AP_DeathLinkEnabled()) {
            AP_DeathLinkSend(NULL);
        }
    }
    sPrevPlayerState = state;
}

static void OnGameTick(GamePreUpdateEvent* event) {
    (void) event;

    if (!AP_IsEnabled()) {
        sOwedDeaths = 0;
        sPrevPlayerState = PLAYERSTATE_STANDBY;
        return;
    }
    // Deaths that arrive outside a level are kept, not dropped: they land once the player is flying again.
    sOwedDeaths += AP_DeathLinkTakePending();
    if (gGameState != GSTATE_PLAY) {
        sPrevPlayerState = PLAYERSTATE_STANDBY;
    }
}

void ApDeathLink_Init(void) {
    REGISTER_LISTENER(PlayerPostUpdateEvent, (EventCallback) OnPlayerUpdatePost, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(GamePreUpdateEvent, (EventCallback) OnGameTick, EVENT_PRIORITY_NORMAL);
}
