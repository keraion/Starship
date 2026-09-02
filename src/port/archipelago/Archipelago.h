#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <imgui.h>

#include "ArchipelagoBridge.h"
#include "ArchipelagoSave.h"
#include "transport/APTransport.h"

// Archipelago session: owns the transport, the per-slot save file and the sync
// logic. Single-threaded: everything runs on the game thread from OnGameTick()
// (polling the socket) or from ImGui / console callbacks.
class Archipelago final : public IAPTransportListener {
  public:
    enum class Conn { Idle, Connecting, RoomInfo, SlotConnected, Refused, VersionMismatch, Failed };

    static Archipelago* Instance;
    static void Init();
    static void Shutdown();

    // UI / console entry points
    void Connect();
    void Disconnect();          // drop the connection but keep the session (slot file) active
    void EndSession();          // disconnect, flush, unload the slot and soft-reset to vanilla
    void Say(const std::string& text);
    void OnGameTick();

    // Status
    Conn GetConn() const { return mConn; }
    bool IsConnected() const { return mConn == Conn::SlotConnected; }
    bool IsEnabled() const { return mSessionActive; }
    bool IsReady() const { return mSessionActive && mConn == Conn::SlotConnected && mSynced; }
    std::string StatusText() const;
    const std::string& LastError() const { return mLastError; }
    const APSlotFile& Slot() const { return mSlot; }
    std::string PlayerAlias(int player) const;
    int OwnPlayer() const { return mOwnPlayer; }
    size_t CheckedCount() const;
    size_t ReceivedCount() const { return mReceived.size(); }
    bool GoalDone() const;

    // Bridge surface
    APSlotState& State() { return mSlot.state; }
    void CheckLocation(uint16_t id);
    void GoalCompleted();
    void MarkDirty() { mDirty = true; }
    void MarkStateChanged() { mStateChanged = true; }
    bool TakeStateChanged();
    bool GetScouted(uint16_t location, int64_t* item, int* player, unsigned* flags) const;
    bool EepromRead(void* dst, size_t size);
    bool EepromWrite(const void* src, size_t size);

    // Debug helpers (console)
    void DebugGiveItem(uint16_t item, int count);
    void FlushNow();

    // IAPTransportListener
    void OnSocketConnected() override;
    void OnSocketDisconnected() override;
    void OnSocketError(const std::string& msg) override;
    void OnRoomInfo() override;
    void OnSlotConnected(const std::string& slotDataJson) override;
    void OnSlotRefused(const std::vector<std::string>& reasons) override;
    void OnSlotDisconnected() override;
    void OnItemsReceived(const std::vector<APNetItem>& items) override;
    void OnLocationsChecked(const std::vector<int64_t>& locations) override;
    void OnLocationInfo(const std::vector<APNetItem>& scouted) override;
    void OnPrintJson(const APPrintJson& msg) override;
    void OnBounced(const std::string& json) override;

  private:
    Archipelago() = default;
    void RegisterConsoleCommands();
    void DestroyTransport();
    void RebuildItemCounts();
    void SendPendingLocations(bool force);
    void NotifyConnection(const std::string& text, const ImVec4& color, float seconds = 5.0f);
    void RequestSoftReset();
    std::string CertPath() const;
    std::vector<std::string> Tags() const;

    Conn mConn = Conn::Idle;
    std::unique_ptr<APTransport> mTransport;
    std::string mLastError;
    std::string mServer;
    int mRetries = 0;
    bool mEverConnected = false;
    bool mWasSlotConnected = false;

    bool mSessionActive = false;
    bool mSynced = false;
    int mOwnPlayer = 0;
    APSlotFile mSlot;

    std::vector<APNetItem> mReceived;
    std::unordered_set<int64_t> mServerChecked;
    std::unordered_set<int64_t> mServerLocations; // every location id the server knows for this slot
    std::unordered_set<int64_t> mSentThisSession;
    std::unordered_map<int64_t, APNetItem> mScouted;

    bool mDirty = false;
    bool mStateChanged = false;
    bool mSendNow = false;
    bool mPendingSoftReset = false;
    uint32_t mFramesSinceFlush = 0;
    uint32_t mFramesSinceSend = 0;
};
