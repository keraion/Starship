#include "ArchipelagoBridge.h"
#include "Archipelago.h"
#include "port/notification/notification.h"

#include <string>

static APSlotState sInactiveState = {};

extern "C" {

bool AP_IsEnabled(void) {
    return Archipelago::Instance != nullptr && Archipelago::Instance->IsEnabled();
}

bool AP_IsReady(void) {
    return Archipelago::Instance != nullptr && Archipelago::Instance->IsReady();
}

uint16_t AP_GetOption(uint16_t optionId) {
    if (!AP_IsEnabled() || optionId >= AP_OPTION_MAX) {
        return 0;
    }
    return Archipelago::Instance->State().options[optionId];
}

uint8_t AP_GetItemCount(uint16_t itemId) {
    if (!AP_IsEnabled() || itemId >= AP_ITEM_MAX) {
        return 0;
    }
    return Archipelago::Instance->State().items[itemId];
}

bool AP_HasItem(uint16_t itemId) {
    return AP_GetItemCount(itemId) > 0;
}

bool AP_IsLocationChecked(uint16_t locationId) {
    if (!AP_IsEnabled() || locationId >= AP_LOCATION_MAX) {
        return false;
    }
    return (Archipelago::Instance->State().locations[locationId / 8] >> (locationId % 8)) & 1;
}

void AP_CheckLocation(uint16_t locationId) {
    if (AP_IsEnabled()) {
        Archipelago::Instance->CheckLocation(locationId);
    }
}

void AP_GoalCompleted(void) {
    if (AP_IsEnabled()) {
        Archipelago::Instance->GoalCompleted();
    }
}

uint32_t AP_GetSeedHash(void) {
    if (!AP_IsEnabled()) {
        return 0;
    }
    const APSlotFile::Key& key = Archipelago::Instance->Slot().GetKey();
    std::string ident = key.seed + "/" + std::to_string(key.team) + "/" + std::to_string(key.slot);
    uint32_t h = 2166136261u;
    for (unsigned char c : ident) {
        h = (h ^ c) * 16777619u;
    }
    return h != 0 ? h : 1u;
}

APSlotState* AP_SaveState(void) {
    if (!AP_IsEnabled()) {
        return &sInactiveState;
    }
    return &Archipelago::Instance->State();
}

void AP_MarkSaveDirty(void) {
    if (AP_IsEnabled()) {
        Archipelago::Instance->MarkDirty();
    }
}

bool AP_TakeStateChanged(void) {
    if (!AP_IsEnabled()) {
        return false;
    }
    return Archipelago::Instance->TakeStateChanged();
}

bool AP_GetScoutedItem(uint16_t locationId, int64_t* outItem, int* outPlayer, unsigned* outFlags) {
    if (!AP_IsEnabled()) {
        return false;
    }
    return Archipelago::Instance->GetScouted(locationId, outItem, outPlayer, outFlags);
}

const char* AP_GetItemName(uint16_t itemId) {
    return (itemId < AP_ITEM_MAX) ? gApItemNames[itemId] : "";
}

const char* AP_GetLocationName(uint16_t locationId) {
    return (locationId < AP_LOCATION_MAX) ? gApLocationNames[locationId] : "";
}

const char* AP_GetPlayerAlias(int player) {
    static std::string sAlias;
    sAlias = (Archipelago::Instance != nullptr) ? Archipelago::Instance->PlayerAlias(player) : "";
    return sAlias.c_str();
}

void AP_Notify(const char* prefix, const char* message, const char* suffix) {
    Notification::Emit({ .prefix = prefix ? prefix : "",
                         .prefixColor = ImVec4(0.6f, 0.6f, 1.0f, 1.0f),
                         .message = message ? message : "",
                         .messageColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                         .suffix = suffix ? suffix : "",
                         .suffixColor = ImVec4(0.8f, 0.8f, 0.8f, 1.0f),
                         .remainingTime = 6.0f });
}

bool AP_EepromRead(void* dst, size_t size) {
    if (!AP_IsEnabled()) {
        return false;
    }
    return Archipelago::Instance->EepromRead(dst, size);
}

bool AP_EepromWrite(const void* src, size_t size) {
    if (!AP_IsEnabled()) {
        return false;
    }
    return Archipelago::Instance->EepromWrite(src, size);
}

void AP_OnGameTick(void* event) {
    (void) event;
    if (Archipelago::Instance != nullptr) {
        Archipelago::Instance->OnGameTick();
    }
}
}
