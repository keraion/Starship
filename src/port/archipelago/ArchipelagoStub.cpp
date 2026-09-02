// Compiled instead of the Archipelago sources when ENABLE_ARCHIPELAGO is OFF.
#include "ArchipelagoBridge.h"


static APSlotState sStubState = {};

extern "C" {

bool AP_IsEnabled(void) {
    return false;
}
bool AP_IsReady(void) {
    return false;
}
uint16_t AP_GetOption(uint16_t optionId) {
    (void) optionId;
    return 0;
}
uint8_t AP_GetItemCount(uint16_t itemId) {
    (void) itemId;
    return 0;
}
bool AP_HasItem(uint16_t itemId) {
    (void) itemId;
    return false;
}
bool AP_IsLocationChecked(uint16_t locationId) {
    (void) locationId;
    return false;
}
void AP_CheckLocation(uint16_t locationId) {
    (void) locationId;
}
void AP_GoalCompleted(void) {
}
APSlotState* AP_SaveState(void) {
    return &sStubState;
}
void AP_MarkSaveDirty(void) {
}
bool AP_TakeStateChanged(void) {
    return false;
}
bool AP_GetScoutedItem(uint16_t locationId, int64_t* outItem, int* outPlayer, unsigned* outFlags) {
    (void) locationId;
    (void) outItem;
    (void) outPlayer;
    (void) outFlags;
    return false;
}
const char* AP_GetItemName(uint16_t itemId) {
    return (itemId < AP_ITEM_MAX) ? gApItemNames[itemId] : "";
}
const char* AP_GetLocationName(uint16_t locationId) {
    return (locationId < AP_LOCATION_MAX) ? gApLocationNames[locationId] : "";
}
const char* AP_GetPlayerAlias(int player) {
    (void) player;
    return "";
}
void AP_Notify(const char* prefix, const char* message, const char* suffix) {
    (void) prefix;
    (void) message;
    (void) suffix;
}
bool AP_EepromRead(void* dst, size_t size) {
    (void) dst;
    (void) size;
    return false;
}
bool AP_EepromWrite(const void* src, size_t size) {
    (void) src;
    (void) size;
    return false;
}
void AP_OnGameTick(void* event) {
    (void) event;
}
}
