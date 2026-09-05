#pragma once

#include "global.h"
#include "port/hooks/impl/EventSystem.h"

DEFINE_EVENT(DisplayPreUpdateEvent);
DEFINE_EVENT(DisplayPostUpdateEvent);

DEFINE_EVENT(GamePreUpdateEvent);
DEFINE_EVENT(GamePostUpdateEvent);

DEFINE_EVENT(PlayUpdateEvent);

DEFINE_EVENT(PlayerPreUpdateEvent, Player* player;);
DEFINE_EVENT(PlayerPostUpdateEvent, Player* player;);

DEFINE_EVENT(DrawRadarHUDEvent);
DEFINE_EVENT(DrawRadarMarkArwingEvent, s32 colorIdx;);
DEFINE_EVENT(DrawRadarMarkWolfenEvent);
DEFINE_EVENT(DrawBoostGaugeHUDEvent);
DEFINE_EVENT(DrawBombCounterHUDEvent);
DEFINE_EVENT(DrawIncomingMsgHUDEvent);
DEFINE_EVENT(PreSetupRadioMsgEvent, s32* radioRedBox;);

// Fired at the start of Radio_PlayMessage with the caller's message pointer (asset path) and portrait
// character; listeners may replace `msg` (Archipelago radio shuffle).
DEFINE_EVENT(RadioMessageEvent,
    u16* msg;
    s32 character;
);
DEFINE_EVENT(DrawGoldRingsHUDEvent);
DEFINE_EVENT(DrawLivesCounterHUDEvent);
DEFINE_EVENT(DrawTrainingRingPassCountHUDEvent);
DEFINE_EVENT(DrawEdgeArrowsHUDEvent);
DEFINE_EVENT(DrawBossHealthHUDEvent);

DEFINE_EVENT(DrawGlobalHUDPreEvent);
DEFINE_EVENT(DrawGlobalHUDPostEvent);

// Pause menu (HUD_PauseScreen_Update). Cancel to replace the vanilla input handling / option drawing.
DEFINE_EVENT(PauseMenuInputEvent);
DEFINE_EVENT(PauseMenuDrawEvent, f32 x; f32 y;);
