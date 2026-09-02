#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ArchipelagoBridge.h"

// Per-slot persistence: <appdir>/archipelago/<seed>_<team>_<slot>.json
class APSlotFile {
  public:
    static constexpr int kSchemaVersion = 1;
    static constexpr size_t kEepromSize = 512;

    struct Key {
        std::string seed;
        int team = 0;
        int slot = 0;
        std::string slotName;
        bool operator==(const Key& o) const {
            return seed == o.seed && team == o.team && slot == o.slot;
        }
    };

    APSlotFile();

    // Loads the file for key (creating a fresh state if absent). Returns true if an existing file was loaded.
    bool LoadOrCreate(const Key& key, const std::string& server);
    bool Save() const;
    void ResetToDefaults();

    const Key& GetKey() const { return mKey; }
    const std::string& GetPath() const { return mPath; }
    bool IsLoaded() const { return mLoaded; }

    APSlotState state{};
    uint32_t receivedItemCount = 0;
    uint32_t worldVersion = 0;
    std::string server;
    bool goalSent = false;

    bool hasEeprom = false;
    uint8_t eeprom[kEepromSize]{};

    static std::string Directory();
    static std::string PathForKey(const Key& key);

  private:
    Key mKey;
    std::string mPath;
    bool mLoaded = false;
};
