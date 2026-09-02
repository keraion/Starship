#pragma once

// C-callable contract between the Archipelago client (C++) and the game code.
// Every function is safe to call at any time; when no session is active (or
// the build has ENABLE_ARCHIPELAGO off) they are no-ops / return defaults.

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "ArchipelagoIds.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct APPlanetState {
    uint8_t peppy;
    uint8_t slippy;
    uint8_t falco;
    uint16_t score;
} APPlanetState;

typedef struct APReceivedMarks {
    uint8_t lasers;
    uint8_t silvers;
    uint8_t golds;
    uint8_t stars;
    uint8_t bombs;
    uint8_t lives;
} APReceivedMarks;

typedef struct APStarWolfAlive {
    uint8_t wolf;
    uint8_t leon;
    uint8_t pigma;
    uint8_t andrew;
} APStarWolfAlive;

// Persisted per-slot state (mirrors the ROM hack's save_custom_data_t).
typedef struct APSlotState {
    uint16_t options[AP_OPTION_MAX];
    uint8_t locations[AP_LOCATION_MAX_BYTES]; // bit per location id; bit 0 = goal completed
    uint8_t items[AP_ITEM_MAX];               // counts, rebuilt from the server's ReceivedItems list
    APReceivedMarks received;                 // high-water marks for granted consumables (game-owned)
    APPlanetState planets[16];                // indexed by PlanetId
    int16_t shields[3];                       // falco, slippy, peppy
    APStarWolfAlive starWolfAlive;
    uint8_t bombs;
    uint8_t lasers;
    uint8_t goldRings;
    uint8_t greatFoxIntact;
    int8_t lives; // -1 = use options[AP_OPTION_DEFAULT_LIVES]
} APSlotState;

bool AP_IsEnabled(void); // a slot is loaded (session active), even if temporarily disconnected
bool AP_IsReady(void);   // connected, slot_data applied and save synced: the game may start

uint16_t AP_GetOption(uint16_t optionId);
uint8_t AP_GetItemCount(uint16_t itemId);
bool AP_HasItem(uint16_t itemId);
bool AP_IsLocationChecked(uint16_t locationId);
void AP_CheckLocation(uint16_t locationId); // sets the bit, marks dirty, queues the send
void AP_GoalCompleted(void);

APSlotState* AP_SaveState(void); // never NULL
void AP_MarkSaveDirty(void);
bool AP_TakeStateChanged(void); // true once after items/locations/options changed ("map needs recompute")
bool AP_GetScoutedItem(uint16_t locationId, int64_t* outItem, int* outPlayer, unsigned* outFlags);

const char* AP_GetItemName(uint16_t itemId);
const char* AP_GetLocationName(uint16_t locationId);
const char* AP_GetPlayerAlias(int player); // "" when disconnected; result valid until the next call
void AP_Notify(const char* prefix, const char* message, const char* suffix);

// Port-internal (called from src/sys/sys_joybus.c and PortEnhancements.c).
bool AP_EepromRead(void* dst, size_t size);
bool AP_EepromWrite(const void* src, size_t size);
void AP_OnGameTick(void* event); // EventCallback for GamePreUpdateEvent

#ifdef __cplusplus
}
#endif
