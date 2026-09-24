#pragma once

#include "global.h"

// "Checkpoint Restart" enhancement (gEnhancements.CheckpointRestart).
//
// Adds restart entries to the pause menu that put you back at the start of the level or at the last
// checkpoint without costing an Arwing and without counting as a death. The loadout you had at that
// point (lasers, bombs, gold rings, wing health) is restored with you, instead of the vanilla reset
// to single lasers and three bombs.
//
// The restart drives the same transition the engine's own death path uses (Cutscene_KillPlayer:
// PLAYERSTATE_NEXT + gFadeoutType 7 -> PLAY_INIT) but skips the gLifeCount decrement. It never enters
// PLAYERSTATE_DOWN, so an Archipelago DeathLink is never broadcast for it.
//
// The restart logic lives here so the vanilla pause menu (below) and the Archipelago one
// (src/port/archipelago/game/ApPause.c) can share it.

typedef enum {
    RESTART_POINT_LEVEL_START,
    RESTART_POINT_CHECKPOINT,
} RestartPointKind;

bool RestartPoint_Enabled(void);        // CVar on, and we are in a level where restarting makes sense
bool RestartPoint_HasCheckpoint(void);  // a checkpoint ring has been taken this run
void RestartPoint_Restart(RestartPointKind kind);

// "RESTART >" submenu, shared by the vanilla pause menu and the Archipelago one: a pause menu shows a
// single RESTART entry and hands input and drawing over here while the submenu is up.
bool RestartPoint_SubmenuActive(void);
void RestartPoint_SubmenuEnter(void);
void RestartPoint_SubmenuInput(void);
void RestartPoint_SubmenuDraw(f32 x, f32 y);

void RestartPoint_Init(void);
