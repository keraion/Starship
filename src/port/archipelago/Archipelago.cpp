#include "Archipelago.h"
#include "ArchipelagoConsole.h"
#include "port/notification/notification.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <libultraship/libultraship.h>
#include "libultraship/src/Context.h"


extern "C" {
#include "sys.h"
#include "sf64thread.h"
}

using json = nlohmann::json;

Archipelago* Archipelago::Instance = nullptr;

static const ImVec4 kColorInfo = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
static const ImVec4 kColorOk = ImVec4(0.4f, 0.9f, 0.4f, 1.0f);
static const ImVec4 kColorWarn = ImVec4(1.0f, 0.75f, 0.2f, 1.0f);
static const ImVec4 kColorErr = ImVec4(1.0f, 0.35f, 0.35f, 1.0f);
static const ImVec4 kColorSelf = ImVec4(0.9f, 0.4f, 0.9f, 1.0f);
static const ImVec4 kColorPlayer = ImVec4(0.95f, 0.85f, 0.3f, 1.0f);
static const ImVec4 kColorLocation = ImVec4(0.4f, 0.85f, 0.5f, 1.0f);
static const ImVec4 kColorProgression = ImVec4(0.75f, 0.55f, 0.95f, 1.0f);
static const ImVec4 kColorUseful = ImVec4(0.45f, 0.6f, 0.95f, 1.0f);
static const ImVec4 kColorTrap = ImVec4(0.95f, 0.5f, 0.45f, 1.0f);
static const ImVec4 kColorFiller = ImVec4(0.4f, 0.85f, 0.9f, 1.0f);

static constexpr int kMaxRetries = 3;
static constexpr uint32_t kFlushInterval = 120;
static constexpr uint32_t kSendInterval = 120;

static inline bool BitGet(const uint8_t* bits, int id) {
    return (bits[id / 8] >> (id % 8)) & 1;
}
static inline void BitSet(uint8_t* bits, int id) {
    bits[id / 8] |= (uint8_t) (1u << (id % 8));
}

// ---------------------------------------------------------------------------
// lifecycle

void Archipelago::Init() {
    if (Instance != nullptr) {
        return;
    }
    Instance = new Archipelago();
    CVarRegisterString("gArchipelago.ServerAddress", "archipelago.gg:38281");
    CVarRegisterString("gArchipelago.Slot", "");
    CVarRegisterString("gArchipelago.Password", "");
    CVarRegisterInteger("gArchipelago.RememberPassword", 1);
    CVarRegisterInteger("gArchipelago.AutoConnect", 0);
    CVarRegisterInteger("gArchipelago.Notify.Items", 1);
    CVarRegisterInteger("gArchipelago.Notify.Chat", 0);
    CVarRegisterInteger("gArchipelago.Notify.Connection", 1);
    Instance->RegisterConsoleCommands();

    std::error_code ec;
    std::filesystem::create_directories(APSlotFile::Directory(), ec);

    if (CVarGetInteger("gArchipelago.AutoConnect", 0) != 0 && CVarGetString("gArchipelago.Slot", "")[0] != '\0') {
        Instance->Connect();
    }
}

void Archipelago::Shutdown() {
    if (Instance == nullptr) {
        return;
    }
    Instance->FlushNow();
    Instance->DestroyTransport();
    delete Instance;
    Instance = nullptr;
}

std::string Archipelago::CertPath() const {
    std::vector<std::string> candidates = {
        Ship::Context::GetPathRelativeToAppDirectory("networking/cacert.pem"),
        Ship::Context::LocateFileAcrossAppDirs("networking/cacert.pem"),
        "networking/cacert.pem",
#if defined(__linux__)
        "/etc/ssl/certs/ca-certificates.crt",
        "/etc/pki/tls/certs/ca-bundle.crt",
        "/etc/ssl/ca-bundle.pem",
        "/etc/pki/tls/cacert.pem",
#endif
    };
    for (const auto& c : candidates) {
        std::error_code ec;
        if (!c.empty() && std::filesystem::is_regular_file(c, ec)) {
            return c;
        }
    }
    return "";
}

std::vector<std::string> Archipelago::Tags() const {
    // Extension point: add "DeathLink" / "RingLink" here when implemented.
    return {};
}

void Archipelago::Connect() {
    if (mTransport != nullptr) {
        return;
    }
    mServer = CVarGetString("gArchipelago.ServerAddress", "archipelago.gg:38281");
    const std::string slot = CVarGetString("gArchipelago.Slot", "");
    if (mServer.empty() || slot.empty()) {
        mLastError = "Server address and slot name are required";
        mConn = Conn::Failed;
        ArchipelagoConsole::LogError(mLastError);
        return;
    }

    const std::string uuidPath = APSlotFile::Directory() + "/uuid";
    const std::string uuid = APTransport::GetUuid(uuidPath);
    const std::string cert = CertPath();
    if (cert.empty()) {
        ArchipelagoConsole::Log("No CA bundle found (networking/cacert.pem); wss:// connections may fail", kColorWarn);
    }

    mRetries = 0;
    mEverConnected = false;
    mWasSlotConnected = false;
    mSynced = false;
    mLastError.clear();
    mSentThisSession.clear();
    mConn = Conn::Connecting;
    ArchipelagoConsole::Log("Connecting to " + mServer + " as \"" + slot + "\"...", kColorInfo);

    try {
        mTransport = std::make_unique<APTransport>(uuid, AP_GAME_NAME, mServer, cert, APSlotFile::Directory(), this);
    } catch (const std::exception& e) {
        mLastError = std::string("Failed to create client: ") + e.what();
        mConn = Conn::Failed;
        ArchipelagoConsole::LogError(mLastError);
        NotifyConnection(mLastError, kColorErr, 7.0f);
    }
}

void Archipelago::DestroyTransport() {
    mTransport.reset();
}

void Archipelago::Disconnect() {
    if (mTransport == nullptr && mConn == Conn::Idle) {
        return;
    }
    FlushNow();
    DestroyTransport();
    mSynced = false;
    mConn = Conn::Idle;
    ArchipelagoConsole::Log("Disconnected", kColorWarn);
}

void Archipelago::EndSession() {
    Disconnect();
    if (mSessionActive) {
        mSessionActive = false;
        mReceived.clear();
        mServerChecked.clear();
        mScouted.clear();
        mSlot = APSlotFile();
        ArchipelagoConsole::Log("Session ended; returning to vanilla save", kColorWarn);
        RequestSoftReset();
    }
}

void Archipelago::RequestSoftReset() {
    mPendingSoftReset = true;
}

void Archipelago::Say(const std::string& text) {
    if (mTransport == nullptr || mConn != Conn::SlotConnected) {
        ArchipelagoConsole::LogError("Not connected");
        return;
    }
    mTransport->Say(text);
}

// ---------------------------------------------------------------------------
// per-frame

void Archipelago::OnGameTick() {
    if (mTransport != nullptr) {
        mTransport->Poll();
        if (mConn == Conn::Failed || mConn == Conn::Refused || mConn == Conn::VersionMismatch) {
            DestroyTransport();
        }
    }

    if (mPendingSoftReset) {
        mPendingSoftReset = false;
        gNextGameState = GSTATE_BOOT;
    }

    mFramesSinceFlush++;
    mFramesSinceSend++;

    if (IsReady()) {
        if (mSendNow || mFramesSinceSend >= kSendInterval) {
            SendPendingLocations(false);
        }
        if (BitGet(mSlot.state.locations, AP_LOCATION_GOAL_COMPLETED) && !mSlot.goalSent) {
            if (mTransport->SendGoal()) {
                mSlot.goalSent = true;
                mDirty = true;
                ArchipelagoConsole::Log("Goal completed! Sent to the server.", kColorOk);
                Notification::Emit({ .message = "Goal completed!", .messageColor = kColorOk, .remainingTime = 7.0f });
            }
        }
    }

    if (mDirty && mSessionActive && mFramesSinceFlush >= kFlushInterval) {
        FlushNow();
    }
}

void Archipelago::FlushNow() {
    if (mSessionActive && mSlot.IsLoaded()) {
        mSlot.Save();
    }
    mDirty = false;
    mFramesSinceFlush = 0;
}

void Archipelago::SendPendingLocations(bool force) {
    if (!IsReady()) {
        return;
    }
    std::vector<int64_t> pending;
    for (int id = 1; id < AP_LOCATION_MAX; id++) {
        if (!BitGet(mSlot.state.locations, id)) {
            continue;
        }
        if (mServerChecked.count(id) != 0) {
            continue;
        }
        if (mServerLocations.count(id) == 0) {
            continue; // event-only location for this slot (never registered with the server)
        }
        if (!force && mSentThisSession.count(id) != 0) {
            continue;
        }
        pending.push_back(id);
    }
    mSendNow = false;
    mFramesSinceSend = 0;
    if (pending.empty()) {
        return;
    }
    if (mTransport->LocationChecks(pending)) {
        for (auto id : pending) {
            mSentThisSession.insert(id);
        }
    }
}

// ---------------------------------------------------------------------------
// bridge surface

void Archipelago::CheckLocation(uint16_t id) {
    if (!mSessionActive || id >= AP_LOCATION_MAX) {
        return;
    }
    if (BitGet(mSlot.state.locations, id)) {
        return;
    }
    BitSet(mSlot.state.locations, id);
    mDirty = true;
    mStateChanged = true;
    mSendNow = true;
    if (id != AP_LOCATION_GOAL_COMPLETED) {
        ArchipelagoConsole::LogLine({ { "Checked ", kColorInfo }, { gApLocationNames[id], kColorLocation } });
    }
}

void Archipelago::GoalCompleted() {
    if (!mSessionActive) {
        return;
    }
    if (!BitGet(mSlot.state.locations, AP_LOCATION_GOAL_COMPLETED)) {
        BitSet(mSlot.state.locations, AP_LOCATION_GOAL_COMPLETED);
        mDirty = true;
        mStateChanged = true;
    }
}

bool Archipelago::TakeStateChanged() {
    bool v = mStateChanged;
    mStateChanged = false;
    return v;
}

bool Archipelago::GetScouted(uint16_t location, int64_t* item, int* player, unsigned* flags) const {
    auto it = mScouted.find(location);
    if (it == mScouted.end()) {
        return false;
    }
    if (item) *item = it->second.item;
    if (player) *player = it->second.player;
    if (flags) *flags = it->second.flags;
    return true;
}

bool Archipelago::EepromRead(void* dst, size_t size) {
    if (!mSessionActive) {
        return false;
    }
    size_t n = (std::min)(size, APSlotFile::kEepromSize);
    if (!mSlot.hasEeprom) {
        std::memset(dst, 0, size);
        return false;
    }
    std::memcpy(dst, mSlot.eeprom, n);
    return true;
}

bool Archipelago::EepromWrite(const void* src, size_t size) {
    if (!mSessionActive) {
        return false;
    }
    size_t n = (std::min)(size, APSlotFile::kEepromSize);
    std::memcpy(mSlot.eeprom, src, n);
    mSlot.hasEeprom = true;
    mDirty = true;
    FlushNow();
    return true;
}

std::string Archipelago::PlayerAlias(int player) const {
    if (mTransport == nullptr) {
        return "";
    }
    return mTransport->GetPlayerAlias(player);
}

size_t Archipelago::CheckedCount() const {
    size_t n = 0;
    for (int id = 1; id < AP_LOCATION_MAX; id++) {
        n += BitGet(mSlot.state.locations, id) ? 1 : 0;
    }
    return n;
}

bool Archipelago::GoalDone() const {
    return mSessionActive && BitGet(mSlot.state.locations, AP_LOCATION_GOAL_COMPLETED);
}

std::string Archipelago::StatusText() const {
    switch (mConn) {
        case Conn::Idle:
            return mSessionActive ? "Disconnected (session active)" : "Not connected";
        case Conn::Connecting:
            return mEverConnected ? "Reconnecting..." : "Connecting...";
        case Conn::RoomInfo:
            return "Authenticating...";
        case Conn::SlotConnected:
            return mSynced ? "Connected" : "Syncing...";
        case Conn::Refused:
            return "Refused: " + mLastError;
        case Conn::VersionMismatch:
            return "Version mismatch: " + mLastError;
        case Conn::Failed:
            return "Failed: " + mLastError;
    }
    return "";
}

void Archipelago::NotifyConnection(const std::string& text, const ImVec4& color, float seconds) {
    if (CVarGetInteger("gArchipelago.Notify.Connection", 1) == 0) {
        return;
    }
    Notification::Emit({ .prefix = "Archipelago:", .prefixColor = ImVec4(0.6f, 0.6f, 1.0f, 1.0f),
                         .message = text, .messageColor = color, .remainingTime = seconds });
}

void Archipelago::DebugGiveItem(uint16_t item, int count) {
    if (!mSessionActive || item >= AP_ITEM_MAX) {
        return;
    }
    int v = (int) mSlot.state.items[item] + count;
    mSlot.state.items[item] = (uint8_t) std::clamp(v, 0, 255);
    mDirty = true;
    mStateChanged = true;
}

// ---------------------------------------------------------------------------
// transport callbacks

void Archipelago::OnSocketConnected() {
    mEverConnected = true;
    mRetries = 0;
    ArchipelagoConsole::Log("Socket connected to " + mServer, kColorInfo);
}

void Archipelago::OnSocketDisconnected() {
    mSynced = false;
    mSentThisSession.clear();
    if (mWasSlotConnected) {
        mWasSlotConnected = false;
        mConn = Conn::Connecting;
        ArchipelagoConsole::Log("Connection lost, reconnecting...", kColorWarn);
        NotifyConnection("Connection lost, reconnecting...", kColorWarn);
    } else if (mConn == Conn::SlotConnected || mConn == Conn::RoomInfo) {
        mConn = Conn::Connecting;
    }
}

void Archipelago::OnSocketError(const std::string& msg) {
    if (mEverConnected) {
        // apclientpp keeps retrying inside poll(); just log it.
        ArchipelagoConsole::Log("Socket error: " + msg, kColorWarn);
        return;
    }
    mRetries++;
    ArchipelagoConsole::Log("Connection attempt " + std::to_string(mRetries) + " failed: " + msg, kColorWarn);
    if (mRetries >= kMaxRetries) {
        mLastError = "Could not connect to " + mServer + " (" + msg + ")";
        mConn = Conn::Failed;
        ArchipelagoConsole::LogError(mLastError);
        NotifyConnection("Could not connect to " + mServer, kColorErr, 7.0f);
    }
}

void Archipelago::OnRoomInfo() {
    mConn = Conn::RoomInfo;
    const std::string slot = CVarGetString("gArchipelago.Slot", "");
    const std::string password = CVarGetString("gArchipelago.Password", "");
    ArchipelagoConsole::Log("Room info received, connecting slot \"" + slot + "\"...", kColorInfo);
    mTransport->ConnectSlot(slot, password, 0b111, Tags());
}

void Archipelago::OnSlotRefused(const std::vector<std::string>& reasons) {
    std::string joined;
    for (const auto& r : reasons) {
        if (!joined.empty()) {
            joined += ", ";
        }
        joined += r;
    }
    mLastError = joined.empty() ? "unknown reason" : joined;
    mConn = Conn::Refused;
    ArchipelagoConsole::LogError("Connection refused: " + mLastError);
    NotifyConnection("Connection refused: " + mLastError, kColorErr, 7.0f);
}

void Archipelago::OnSlotDisconnected() {
    mSynced = false;
    if (mConn == Conn::SlotConnected) {
        mWasSlotConnected = false;
        mConn = Conn::Connecting;
        ArchipelagoConsole::Log("Slot disconnected, reconnecting...", kColorWarn);
    }
}

void Archipelago::OnSlotConnected(const std::string& slotDataJson) {
    json slotData = json::parse(slotDataJson, nullptr, false);
    uint32_t version = 0;
    if (slotData.is_object() && slotData.contains("version") && slotData["version"].is_number()) {
        version = slotData["version"].get<uint32_t>();
    }
    if (version != AP_WORLD_VERSION_U32) {
        char buf[128];
        snprintf(buf, sizeof(buf), "seed uses apworld %u.%u.%u, this build supports %s", (version >> 16) & 0xFFFF,
                 (version >> 8) & 0xFF, version & 0xFF, AP_WORLD_VERSION_STRING);
        mLastError = buf;
        mConn = Conn::VersionMismatch;
        ArchipelagoConsole::LogError("Version mismatch: " + mLastError);
        NotifyConnection("Version mismatch: " + mLastError, kColorErr, 10.0f);
        return;
    }

    APSlotFile::Key key;
    key.seed = mTransport->GetSeed();
    key.team = mTransport->GetTeam();
    key.slot = mTransport->GetSlotNumber();
    key.slotName = mTransport->GetSlotName();
    mOwnPlayer = key.slot;

    if (mSessionActive && !(mSlot.GetKey() == key)) {
        ArchipelagoConsole::Log("Switching to a different seed/slot; saving previous session", kColorWarn);
        FlushNow();
        mSessionActive = false;
    }
    if (!mSessionActive) {
        mReceived.clear();
        mScouted.clear();
        mSlot.LoadOrCreate(key, mServer);
        mSessionActive = true;
        RequestSoftReset();
    }
    mSlot.server = mServer;

    // Options
    if (slotData.contains("options") && slotData["options"].is_object()) {
        const auto& opts = slotData["options"];
        for (int i = 0; i < AP_OPTION_MAX; i++) {
            const char* name = gApOptionNames[i];
            if (opts.contains(name) && opts[name].is_number()) {
                mSlot.state.options[i] = (uint16_t) opts[name].get<int>();
            } else {
                SPDLOG_WARN("[AP] slot_data missing option {}", name);
            }
        }
    } else {
        ArchipelagoConsole::LogError("slot_data has no options object");
    }

    // Server-side checked locations. Only ids the server reports (missing + checked) exist for this
    // slot; event locations (e.g. unshuffled medals, "Starting Level") are unknown to it and must never
    // be sent, or the server drops the connection.
    mServerChecked.clear();
    mServerLocations.clear();
    for (auto id : mTransport->GetCheckedLocations()) {
        mServerChecked.insert(id);
        mServerLocations.insert(id);
        if (id >= 0 && id < AP_LOCATION_MAX) {
            BitSet(mSlot.state.locations, (int) id);
        }
    }
    std::vector<int64_t> missing = mTransport->GetMissingLocations();
    for (auto id : missing) {
        mServerLocations.insert(id);
    }

    // Scout the remaining locations for tracker/hint purposes.
    if (!missing.empty()) {
        mTransport->LocationScouts(missing);
    }

    mConn = Conn::SlotConnected;
    mWasSlotConnected = true;
    mSynced = true;
    mSlot.goalSent = mSlot.goalSent && GoalDone();
    mStateChanged = true;
    mDirty = true;
    FlushNow();
    SendPendingLocations(true);

    const std::string alias = mTransport->GetPlayerAlias(mOwnPlayer);
    ArchipelagoConsole::Log("Connected as " + alias + " (seed " + key.seed + ", slot " + std::to_string(key.slot) + ")",
                            kColorOk);
    ArchipelagoConsole::Log("Slot file: " + mSlot.GetPath(), kColorInfo);
    NotifyConnection("Connected as " + alias, kColorOk);
}

void Archipelago::RebuildItemCounts() {
    std::memset(mSlot.state.items, 0, sizeof(mSlot.state.items));
    for (const auto& it : mReceived) {
        if (it.item > 0 && it.item < AP_ITEM_MAX) {
            if (mSlot.state.items[it.item] < 255) {
                mSlot.state.items[it.item]++;
            }
        }
    }
}

void Archipelago::OnItemsReceived(const std::vector<APNetItem>& items) {
    if (!mSessionActive) {
        return;
    }
    if (!items.empty() && items.front().index == 0) {
        mReceived.clear();
    }
    for (const auto& it : items) {
        mReceived.push_back(it);
    }
    RebuildItemCounts();

    uint32_t maxIndex = mSlot.receivedItemCount;
    for (const auto& it : items) {
        if (it.index < 0 || (uint32_t) it.index < mSlot.receivedItemCount) {
            continue;
        }
        maxIndex = (std::max)(maxIndex, (uint32_t) it.index + 1);
        const char* name = (it.item > 0 && it.item < AP_ITEM_MAX) ? gApItemNames[it.item] : "Unknown item";
        std::string from = mTransport ? mTransport->GetPlayerAlias(it.player) : "";
        ArchipelagoConsole::LogLine({ { "Received ", kColorInfo },
                                      { name, (it.flags & AP_FLAG_ADVANCEMENT) ? kColorProgression : kColorFiller },
                                      { " from ", kColorInfo },
                                      { from, it.player == mOwnPlayer ? kColorSelf : kColorPlayer } });
        if (CVarGetInteger("gArchipelago.Notify.Items", 1) != 0) {
            Notification::Emit({ .prefix = "Received",
                                 .prefixColor = ImVec4(0.6f, 0.6f, 1.0f, 1.0f),
                                 .message = name,
                                 .messageColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                                 .suffix = from.empty() ? "" : ("from " + from),
                                 .suffixColor = ImVec4(0.8f, 0.8f, 0.8f, 1.0f),
                                 .remainingTime = 6.0f });
        }
    }
    mSlot.receivedItemCount = maxIndex;
    mStateChanged = true;
    mDirty = true;
}

void Archipelago::OnLocationsChecked(const std::vector<int64_t>& locations) {
    for (auto id : locations) {
        mServerChecked.insert(id);
        if (id >= 0 && id < AP_LOCATION_MAX && !BitGet(mSlot.state.locations, (int) id)) {
            BitSet(mSlot.state.locations, (int) id);
            mStateChanged = true;
            mDirty = true;
        }
    }
}

void Archipelago::OnLocationInfo(const std::vector<APNetItem>& scouted) {
    for (const auto& it : scouted) {
        mScouted[it.location] = it;
    }
}

void Archipelago::OnPrintJson(const APPrintJson& msg) {
    ArchipelagoConsole::Line line;
    for (const auto& n : msg.nodes) {
        ImVec4 color = kColorInfo;
        if (n.type == "player_id") {
            color = (n.player == mOwnPlayer) ? kColorSelf : kColorPlayer;
        } else if (n.type == "item_id") {
            if (n.flags & AP_FLAG_ADVANCEMENT) color = kColorProgression;
            else if (n.flags & AP_FLAG_TRAP) color = kColorTrap;
            else if (n.flags & AP_FLAG_NEVER_EXCLUDE) color = kColorUseful;
            else color = kColorFiller;
        } else if (n.type == "location_id") {
            color = kColorLocation;
        } else if (n.type == "color") {
            if (n.color == "red") color = kColorErr;
            else if (n.color == "green") color = kColorOk;
            else if (n.color == "yellow") color = kColorWarn;
        }
        line.push_back({ n.text, color });
    }
    ArchipelagoConsole::LogLine(std::move(line));

    if (msg.type == "ItemSend" && msg.hasItem && msg.hasReceiving && msg.item.player == mOwnPlayer &&
        msg.receiving != mOwnPlayer && CVarGetInteger("gArchipelago.Notify.Items", 1) != 0) {
        std::string itemName;
        std::string to;
        if (mTransport) {
            itemName = mTransport->GetItemName(msg.item.item, mTransport->GetPlayerGame(msg.receiving));
            to = mTransport->GetPlayerAlias(msg.receiving);
        }
        Notification::Emit({ .prefix = "Sent",
                             .prefixColor = ImVec4(0.6f, 1.0f, 0.6f, 1.0f),
                             .message = itemName,
                             .messageColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                             .suffix = "to " + to,
                             .suffixColor = ImVec4(0.8f, 0.8f, 0.8f, 1.0f),
                             .remainingTime = 6.0f });
    } else if (msg.type == "Chat" && CVarGetInteger("gArchipelago.Notify.Chat", 0) != 0) {
        Notification::Emit({ .message = msg.rendered, .remainingTime = 6.0f });
    }
}

void Archipelago::OnBounced(const std::string& jsonText) {
    // Extension point for DeathLink / RingLink.
    (void) jsonText;
}

// ---------------------------------------------------------------------------
// console command

void Archipelago::RegisterConsoleCommands() {
    auto console = Ship::Context::GetInstance()->GetConsole();
    if (console == nullptr) {
        return;
    }
    Ship::CommandEntry entry;
    entry.Description = "Archipelago: ap connect|disconnect|end|status|say <text>|check <locationId>|give <itemId> [n]";
    entry.Arguments = { { "subcommand", Ship::ArgumentType::TEXT }, { "argument", Ship::ArgumentType::TEXT, true },
                        { "argument2", Ship::ArgumentType::TEXT, true } };
    entry.Handler = [](std::shared_ptr<Ship::Console> c, std::vector<std::string> args, std::string* output) -> int32_t {
        Archipelago* ap = Archipelago::Instance;
        if (ap == nullptr || args.size() < 2) {
            *output = "usage: ap connect|disconnect|end|status|say <text>|check <id>|give <item> [n]";
            return 1;
        }
        const std::string& sub = args[1];
        if (sub == "connect") {
            ap->Connect();
            *output = ap->StatusText();
        } else if (sub == "disconnect") {
            ap->Disconnect();
            *output = "disconnected";
        } else if (sub == "end") {
            ap->EndSession();
            *output = "session ended";
        } else if (sub == "status") {
            *output = ap->StatusText();
            if (ap->IsEnabled()) {
                *output += " | seed " + ap->Slot().GetKey().seed + " slot " + std::to_string(ap->Slot().GetKey().slot) +
                           " | items " + std::to_string(ap->ReceivedCount()) + " | checks " +
                           std::to_string(ap->CheckedCount()) + "/" + std::to_string(AP_LOCATION_MAX - 1) +
                           " | file " + ap->Slot().GetPath();
            }
        } else if (sub == "say") {
            std::string text;
            for (size_t i = 2; i < args.size(); i++) {
                if (!text.empty()) text += " ";
                text += args[i];
            }
            ap->Say(text);
            *output = "sent";
        } else if (sub == "check" && args.size() >= 3) {
            int id = std::atoi(args[2].c_str());
            if (id < 0 || id >= AP_LOCATION_MAX) {
                *output = "invalid location id";
                return 1;
            }
            if (id == AP_LOCATION_GOAL_COMPLETED) ap->GoalCompleted();
            else ap->CheckLocation((uint16_t) id);
            *output = std::string("checked ") + gApLocationNames[id];
        } else if (sub == "give" && args.size() >= 3) {
            int id = std::atoi(args[2].c_str());
            int n = args.size() >= 4 ? std::atoi(args[3].c_str()) : 1;
            if (id <= 0 || id >= AP_ITEM_MAX) {
                *output = "invalid item id";
                return 1;
            }
            ap->DebugGiveItem((uint16_t) id, n);
            *output = std::string("local item count for ") + gApItemNames[id] + " is now " +
                      std::to_string(ap->State().items[id]);
        } else {
            *output = "unknown subcommand";
            return 1;
        }
        return 0;
    };
    console->AddCommand("ap", entry);
}
