#pragma once

// Thin C++17-safe facade over apclientpp. This header must not include
// apclient.hpp (websocketpp does not build under C++20); the implementation in
// APTransport.cpp is compiled as its own C++17 static library target.

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct APNetItem {
    int64_t item = 0;
    int64_t location = 0;
    int player = 0;
    unsigned flags = 0;
    int index = -1;
};

struct APTextNode {
    std::string type;
    std::string color;
    std::string text; // resolved text (item/location/player names already looked up)
    int player = 0;
    unsigned flags = 0;
    unsigned hintStatus = 0;
};

struct APPrintJson {
    std::string type;
    std::vector<APTextNode> nodes;
    std::string rendered;
    bool hasReceiving = false;
    int receiving = 0;
    bool hasItem = false;
    APNetItem item;
};

enum class APConnState { Disconnected, SocketConnecting, SocketConnected, RoomInfo, SlotConnected };

enum APItemFlags : unsigned { AP_FLAG_NONE = 0, AP_FLAG_ADVANCEMENT = 1, AP_FLAG_NEVER_EXCLUDE = 2, AP_FLAG_TRAP = 4 };

class IAPTransportListener {
  public:
    virtual ~IAPTransportListener() = default;
    virtual void OnSocketConnected() = 0;
    virtual void OnSocketDisconnected() = 0;
    virtual void OnSocketError(const std::string& msg) = 0;
    virtual void OnRoomInfo() = 0;
    virtual void OnSlotConnected(const std::string& slotDataJson) = 0;
    virtual void OnSlotRefused(const std::vector<std::string>& reasons) = 0;
    virtual void OnSlotDisconnected() = 0;
    virtual void OnItemsReceived(const std::vector<APNetItem>& items) = 0;
    virtual void OnLocationsChecked(const std::vector<int64_t>& locations) = 0;
    virtual void OnLocationInfo(const std::vector<APNetItem>& scouted) = 0;
    virtual void OnPrintJson(const APPrintJson& msg) = 0;
    virtual void OnBounced(const std::string& json) = 0; // DeathLink / RingLink extension point
};

class APTransport {
  public:
    // uri may be "host:port", "ws://host:port" or "wss://host:port".
    // certPath: PEM CA bundle for wss (may be empty on Linux to use system defaults).
    // cacheDir: fallback directory for the data-package cache.
    APTransport(const std::string& uuid, const std::string& game, const std::string& uri, const std::string& certPath,
                const std::string& cacheDir, IAPTransportListener* listener);
    ~APTransport();

    APTransport(const APTransport&) = delete;
    APTransport& operator=(const APTransport&) = delete;

    void Poll();
    APConnState GetState() const;

    bool ConnectSlot(const std::string& slot, const std::string& password, int itemsHandling,
                     const std::vector<std::string>& tags);
    bool ConnectUpdate(const std::vector<std::string>& tags);
    bool LocationChecks(const std::vector<int64_t>& locations);
    bool LocationScouts(const std::vector<int64_t>& locations);
    bool SendGoal();
    bool Say(const std::string& text);
    bool Bounce(const std::string& dataJson, const std::vector<std::string>& tags);

    std::string GetSeed() const;
    std::string GetSlotName() const;
    int GetTeam() const;
    int GetSlotNumber() const;
    std::string GetPlayerAlias(int player) const;
    std::string GetPlayerGame(int player) const;
    std::string GetItemName(int64_t id, const std::string& game) const;
    std::string GetLocationName(int64_t id, const std::string& game) const;
    std::vector<int64_t> GetCheckedLocations() const;
    std::vector<int64_t> GetMissingLocations() const;

    static std::string GetUuid(const std::string& uuidFilePath);

  private:
    struct Impl;
    std::unique_ptr<Impl> mImpl;
};
