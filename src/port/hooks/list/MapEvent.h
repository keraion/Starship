#pragma once

#include "global.h"
#include "fox_map.h"
#include "port/hooks/impl/EventSystem.h"

// Map (Lylat system screen) and menu events, added for the Archipelago integration.

DEFINE_EVENT(MapSetupPreEvent, s32 lastGameState;);
DEFINE_EVENT(MapSetupPostEvent, s32 lastGameState;);
DEFINE_EVENT(MapIdleUpdateEvent);
DEFINE_EVENT(MapMenuInputEvent);
DEFINE_EVENT(MapMenuDrawEvent);
DEFINE_EVENT(MapHudDrawEvent);
DEFINE_EVENT(MainMenuSelectEvent, s32 cursor;);

// Fired from Map_Setup_Play once a level result has been recorded (gHitCount still holds the level score).
DEFINE_EVENT(MissionClearEvent, LevelId level; PlanetId planet; s32 status; bool medal;);
// Fired when Bolse / Area 6 would jump straight to Venom (HUD_Bolse_Area6_SaveData). Cancel to skip the vanilla body.
DEFINE_EVENT(PreVenomTransitionEvent, LevelId level;);
// Fired when Andross is defeated, before the ending is queued. Cancel to skip the ending.
DEFINE_EVENT(VenomClearEvent, bool hardClear;);
// Fired from Play_CheckMedalStatus; listeners may overwrite hitCount.
DEFINE_EVENT(MedalThresholdEvent, LevelId level; u16 hitCount;);
