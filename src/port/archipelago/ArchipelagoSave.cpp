#include "ArchipelagoSave.h"

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "libultraship/src/Context.h"

using json = nlohmann::json;

static std::string Sanitize(const std::string& in) {
    std::string out;
    out.reserve(in.size());
    for (char c : in) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-') {
            out.push_back(c);
        } else {
            out.push_back('_');
        }
    }
    if (out.empty()) {
        out = "seed";
    }
    return out;
}

static std::string HexEncode(const uint8_t* data, size_t size) {
    static const char* digits = "0123456789abcdef";
    std::string s;
    s.reserve(size * 2);
    for (size_t i = 0; i < size; i++) {
        s.push_back(digits[data[i] >> 4]);
        s.push_back(digits[data[i] & 0xF]);
    }
    return s;
}

static bool HexDecode(const std::string& s, uint8_t* out, size_t size) {
    if (s.size() != size * 2) {
        return false;
    }
    auto nib = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };
    for (size_t i = 0; i < size; i++) {
        int hi = nib(s[i * 2]);
        int lo = nib(s[i * 2 + 1]);
        if (hi < 0 || lo < 0) {
            return false;
        }
        out[i] = (uint8_t) ((hi << 4) | lo);
    }
    return true;
}

APSlotFile::APSlotFile() {
    ResetToDefaults();
}

std::string APSlotFile::Directory() {
    return Ship::Context::GetPathRelativeToAppDirectory("archipelago");
}

std::string APSlotFile::PathForKey(const Key& key) {
    return Directory() + "/" + Sanitize(key.seed) + "_" + std::to_string(key.team) + "_" + std::to_string(key.slot) +
           ".json";
}

void APSlotFile::ResetToDefaults() {
    std::memset(&state, 0, sizeof(state));
    for (auto& s : state.shields) {
        s = 255;
    }
    state.starWolfAlive = { 1, 1, 1, 1 };
    state.lives = -1;
    state.bombs = 3;
    state.lasers = 0;
    state.goldRings = 0;
    state.greatFoxIntact = 1;
    receivedItemCount = 0;
    goalSent = false;
    hasEeprom = false;
    std::memset(eeprom, 0, sizeof(eeprom));
}

bool APSlotFile::LoadOrCreate(const Key& key, const std::string& serverAddress) {
    mKey = key;
    mPath = PathForKey(key);
    server = serverAddress;
    worldVersion = AP_WORLD_VERSION_U32;
    ResetToDefaults();
    mLoaded = true;

    std::error_code ec;
    std::filesystem::create_directories(Directory(), ec);

    std::ifstream in(mPath);
    if (!in.good()) {
        SPDLOG_INFO("[AP] creating new slot file {}", mPath);
        return false;
    }

    json j = json::parse(in, nullptr, false);
    if (j.is_discarded() || !j.is_object()) {
        SPDLOG_ERROR("[AP] slot file {} is corrupt, starting fresh (backup at .bak)", mPath);
        std::filesystem::rename(mPath, mPath + ".bak", ec);
        return false;
    }

    int schema = j.value("schema", 0);
    if (schema != kSchemaVersion) {
        SPDLOG_ERROR("[AP] slot file {} has schema {} (expected {}), starting fresh (backup at .bak)", mPath, schema,
                     kSchemaVersion);
        std::filesystem::rename(mPath, mPath + ".bak", ec);
        return false;
    }

    try {
        if (j.contains("key")) {
            mKey.slotName = j["key"].value("slotName", key.slotName);
        }
        worldVersion = j.value("worldVersion", worldVersion);
        receivedItemCount = j.value("receivedItemCount", 0u);
        goalSent = j.value("goalSent", false);

        if (j.contains("options") && j["options"].is_array()) {
            size_t n = std::min<size_t>(j["options"].size(), AP_OPTION_MAX);
            for (size_t i = 0; i < n; i++) {
                state.options[i] = (uint16_t) j["options"][i].get<int>();
            }
        }
        if (j.contains("checkedLocations") && j["checkedLocations"].is_array()) {
            for (const auto& v : j["checkedLocations"]) {
                int id = v.get<int>();
                if (id >= 0 && id < AP_LOCATION_MAX) {
                    state.locations[id / 8] |= (uint8_t) (1u << (id % 8));
                }
            }
        }
        if (j.contains("items") && j["items"].is_array()) {
            size_t n = std::min<size_t>(j["items"].size(), AP_ITEM_MAX);
            for (size_t i = 0; i < n; i++) {
                state.items[i] = (uint8_t) j["items"][i].get<int>();
            }
        }
        if (j.contains("received")) {
            const auto& r = j["received"];
            state.received.lasers = (uint8_t) r.value("lasers", 0);
            state.received.silvers = (uint8_t) r.value("silvers", 0);
            state.received.golds = (uint8_t) r.value("golds", 0);
            state.received.stars = (uint8_t) r.value("stars", 0);
            state.received.bombs = (uint8_t) r.value("bombs", 0);
            state.received.lives = (uint8_t) r.value("lives", 0);
        }
        if (j.contains("state")) {
            const auto& s = j["state"];
            if (s.contains("planets") && s["planets"].is_array()) {
                size_t n = std::min<size_t>(s["planets"].size(), 16);
                for (size_t i = 0; i < n; i++) {
                    const auto& p = s["planets"][i];
                    state.planets[i].peppy = (uint8_t) p.value("peppy", 0);
                    state.planets[i].slippy = (uint8_t) p.value("slippy", 0);
                    state.planets[i].falco = (uint8_t) p.value("falco", 0);
                    state.planets[i].score = (uint16_t) p.value("score", 0);
                }
            }
            if (s.contains("shields") && s["shields"].is_array()) {
                size_t n = std::min<size_t>(s["shields"].size(), 3);
                for (size_t i = 0; i < n; i++) {
                    state.shields[i] = (int16_t) s["shields"][i].get<int>();
                }
            }
            if (s.contains("starWolfAlive")) {
                const auto& w = s["starWolfAlive"];
                state.starWolfAlive.wolf = (uint8_t) w.value("wolf", 1);
                state.starWolfAlive.leon = (uint8_t) w.value("leon", 1);
                state.starWolfAlive.pigma = (uint8_t) w.value("pigma", 1);
                state.starWolfAlive.andrew = (uint8_t) w.value("andrew", 1);
            }
            state.bombs = (uint8_t) s.value("bombs", 3);
            state.lasers = (uint8_t) s.value("lasers", 0);
            state.goldRings = (uint8_t) s.value("goldRings", 0);
            state.greatFoxIntact = (uint8_t) s.value("greatFoxIntact", 1);
            state.lives = (int8_t) s.value("lives", -1);
        }
        if (j.contains("eeprom") && j["eeprom"].is_string()) {
            hasEeprom = HexDecode(j["eeprom"].get<std::string>(), eeprom, kEepromSize);
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[AP] error reading slot file {}: {}", mPath, e.what());
        ResetToDefaults();
        std::filesystem::rename(mPath, mPath + ".bak", ec);
        return false;
    }

    SPDLOG_INFO("[AP] loaded slot file {}", mPath);
    return true;
}

bool APSlotFile::Save() const {
    if (!mLoaded) {
        return false;
    }
    json j;
    j["schema"] = kSchemaVersion;
    j["key"] = { { "seed", mKey.seed }, { "team", mKey.team }, { "slot", mKey.slot }, { "slotName", mKey.slotName } };
    j["worldVersion"] = worldVersion;
    j["server"] = server;
    j["receivedItemCount"] = receivedItemCount;
    j["goalSent"] = goalSent;

    json options = json::array();
    for (int i = 0; i < AP_OPTION_MAX; i++) {
        options.push_back(state.options[i]);
    }
    j["options"] = options;

    json checked = json::array();
    for (int id = 0; id < AP_LOCATION_MAX; id++) {
        if (state.locations[id / 8] & (1u << (id % 8))) {
            checked.push_back(id);
        }
    }
    j["checkedLocations"] = checked;

    json items = json::array();
    for (int i = 0; i < AP_ITEM_MAX; i++) {
        items.push_back(state.items[i]);
    }
    j["items"] = items;

    j["received"] = { { "lasers", state.received.lasers }, { "silvers", state.received.silvers },
                      { "golds", state.received.golds },   { "stars", state.received.stars },
                      { "bombs", state.received.bombs },   { "lives", state.received.lives } };

    json planets = json::array();
    for (int i = 0; i < 16; i++) {
        planets.push_back({ { "peppy", state.planets[i].peppy },
                            { "slippy", state.planets[i].slippy },
                            { "falco", state.planets[i].falco },
                            { "score", state.planets[i].score } });
    }
    j["state"] = { { "planets", planets },
                   { "shields", { state.shields[0], state.shields[1], state.shields[2] } },
                   { "starWolfAlive",
                     { { "wolf", state.starWolfAlive.wolf },
                       { "leon", state.starWolfAlive.leon },
                       { "pigma", state.starWolfAlive.pigma },
                       { "andrew", state.starWolfAlive.andrew } } },
                   { "bombs", state.bombs },
                   { "lasers", state.lasers },
                   { "goldRings", state.goldRings },
                   { "greatFoxIntact", state.greatFoxIntact },
                   { "lives", state.lives } };

    if (hasEeprom) {
        j["eeprom"] = HexEncode(eeprom, kEepromSize);
    } else {
        j["eeprom"] = nullptr;
    }

    std::error_code ec;
    std::filesystem::create_directories(Directory(), ec);
    const std::string tmp = mPath + ".tmp";
    {
        std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
        if (!out.good()) {
            SPDLOG_ERROR("[AP] cannot write {}", tmp);
            return false;
        }
        out << j.dump(2);
        if (!out.good()) {
            SPDLOG_ERROR("[AP] write failed for {}", tmp);
            return false;
        }
    }
    std::filesystem::rename(tmp, mPath, ec);
    if (ec) {
        // Windows may refuse to overwrite; fall back to remove + rename.
        std::filesystem::remove(mPath, ec);
        std::filesystem::rename(tmp, mPath, ec);
        if (ec) {
            SPDLOG_ERROR("[AP] cannot rename {} -> {}: {}", tmp, mPath, ec.message());
            return false;
        }
    }
    return true;
}
