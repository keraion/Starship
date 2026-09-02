// The ONLY translation unit that includes apclientpp / wswrap / websocketpp / asio.
// Built as the StarshipAPTransport static library with CXX_STANDARD 17.

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_RANDOM_

#include <apuuid.hpp>
#include <apclient.hpp>
#include <defaultdatapackagestore.hpp>

#include "APTransport.h"

struct APTransport::Impl {
    std::unique_ptr<DefaultDataPackageStore> store;
    std::unique_ptr<APClient> client;
    IAPTransportListener* listener = nullptr;

    APNetItem Convert(const APClient::NetworkItem& in) const {
        APNetItem out;
        out.item = in.item;
        out.location = in.location;
        out.player = in.player;
        out.flags = in.flags;
        out.index = in.index;
        return out;
    }

    std::vector<APNetItem> Convert(const std::list<APClient::NetworkItem>& in) const {
        std::vector<APNetItem> out;
        out.reserve(in.size());
        for (const auto& i : in) {
            out.push_back(Convert(i));
        }
        return out;
    }
};

APTransport::APTransport(const std::string& uuid, const std::string& game, const std::string& uri,
                         const std::string& certPath, const std::string& cacheDir, IAPTransportListener* listener)
    : mImpl(new Impl()) {
    mImpl->listener = listener;
    mImpl->store.reset(new DefaultDataPackageStore(cacheDir));
    mImpl->client.reset(new APClient(uuid, game, uri, certPath, mImpl->store.get()));

    APClient* c = mImpl->client.get();
    Impl* impl = mImpl.get();

    c->set_socket_connected_handler([impl]() { impl->listener->OnSocketConnected(); });
    c->set_socket_disconnected_handler([impl]() { impl->listener->OnSocketDisconnected(); });
    c->set_socket_error_handler([impl](const std::string& msg) { impl->listener->OnSocketError(msg); });
    c->set_room_info_handler([impl]() { impl->listener->OnRoomInfo(); });
    c->set_slot_connected_handler([impl](const nlohmann::json& data) { impl->listener->OnSlotConnected(data.dump()); });
    c->set_slot_refused_handler([impl](const std::list<std::string>& reasons) {
        impl->listener->OnSlotRefused(std::vector<std::string>(reasons.begin(), reasons.end()));
    });
    c->set_slot_disconnected_handler([impl]() { impl->listener->OnSlotDisconnected(); });
    c->set_items_received_handler([impl](const std::list<APClient::NetworkItem>& items) {
        impl->listener->OnItemsReceived(impl->Convert(items));
    });
    c->set_location_checked_handler([impl](const std::list<int64_t>& locations) {
        impl->listener->OnLocationsChecked(std::vector<int64_t>(locations.begin(), locations.end()));
    });
    c->set_location_info_handler([impl](const std::list<APClient::NetworkItem>& items) {
        impl->listener->OnLocationInfo(impl->Convert(items));
    });
    c->set_print_json_handler([impl](const APClient::PrintJSONArgs& args) {
        APClient* client = impl->client.get();
        APPrintJson msg;
        msg.type = args.type;
        msg.rendered = client->render_json(args.data, APClient::RenderFormat::TEXT);
        msg.nodes.reserve(args.data.size());
        for (const auto& n : args.data) {
            APTextNode node;
            node.type = n.type;
            node.color = n.color;
            node.player = n.player;
            node.flags = n.flags;
            node.hintStatus = n.hintStatus;
            if (n.type == "player_id") {
                node.text = client->get_player_alias(std::stoi(n.text));
            } else if (n.type == "item_id") {
                node.text = client->get_item_name(std::stoll(n.text), client->get_player_game(n.player));
            } else if (n.type == "location_id") {
                node.text = client->get_location_name(std::stoll(n.text), client->get_player_game(n.player));
            } else {
                node.text = n.text;
            }
            msg.nodes.push_back(std::move(node));
        }
        if (args.receiving != nullptr) {
            msg.hasReceiving = true;
            msg.receiving = *args.receiving;
        }
        if (args.item != nullptr) {
            msg.hasItem = true;
            msg.item = impl->Convert(*args.item);
        }
        impl->listener->OnPrintJson(msg);
    });
    c->set_bounced_handler([impl](const nlohmann::json& data) { impl->listener->OnBounced(data.dump()); });
}

APTransport::~APTransport() {
    // Destroy the client before the store it references.
    mImpl->client.reset();
    mImpl->store.reset();
}

void APTransport::Poll() {
    mImpl->client->poll();
}

APConnState APTransport::GetState() const {
    switch (mImpl->client->get_state()) {
        case APClient::State::SOCKET_CONNECTING:
            return APConnState::SocketConnecting;
        case APClient::State::SOCKET_CONNECTED:
            return APConnState::SocketConnected;
        case APClient::State::ROOM_INFO:
            return APConnState::RoomInfo;
        case APClient::State::SLOT_CONNECTED:
            return APConnState::SlotConnected;
        case APClient::State::DISCONNECTED:
        default:
            return APConnState::Disconnected;
    }
}

bool APTransport::ConnectSlot(const std::string& slot, const std::string& password, int itemsHandling,
                              const std::vector<std::string>& tags) {
    std::list<std::string> t(tags.begin(), tags.end());
    return mImpl->client->ConnectSlot(slot, password, itemsHandling, t, APClient::Version{ 0, 6, 3 });
}

bool APTransport::ConnectUpdate(const std::vector<std::string>& tags) {
    std::list<std::string> t(tags.begin(), tags.end());
    return mImpl->client->ConnectUpdate(false, 0, true, t);
}

bool APTransport::LocationChecks(const std::vector<int64_t>& locations) {
    return mImpl->client->LocationChecks(std::list<int64_t>(locations.begin(), locations.end()));
}

bool APTransport::LocationScouts(const std::vector<int64_t>& locations) {
    return mImpl->client->LocationScouts(std::list<int64_t>(locations.begin(), locations.end()), 0);
}

bool APTransport::SendGoal() {
    return mImpl->client->StatusUpdate(APClient::ClientStatus::GOAL);
}

bool APTransport::Say(const std::string& text) {
    return mImpl->client->Say(text);
}

bool APTransport::Bounce(const std::string& dataJson, const std::vector<std::string>& tags) {
    nlohmann::json data = nlohmann::json::parse(dataJson, nullptr, false);
    if (data.is_discarded()) {
        return false;
    }
    return mImpl->client->Bounce(data, {}, {}, std::list<std::string>(tags.begin(), tags.end()));
}

std::string APTransport::GetSeed() const {
    return mImpl->client->get_seed();
}

std::string APTransport::GetSlotName() const {
    return mImpl->client->get_slot();
}

int APTransport::GetTeam() const {
    return mImpl->client->get_team_number();
}

int APTransport::GetSlotNumber() const {
    return mImpl->client->get_player_number();
}

std::string APTransport::GetPlayerAlias(int player) const {
    return mImpl->client->get_player_alias(player);
}

std::string APTransport::GetPlayerGame(int player) const {
    return mImpl->client->get_player_game(player);
}

std::string APTransport::GetItemName(int64_t id, const std::string& game) const {
    return mImpl->client->get_item_name(id, game);
}

std::string APTransport::GetLocationName(int64_t id, const std::string& game) const {
    return mImpl->client->get_location_name(id, game);
}

std::vector<int64_t> APTransport::GetCheckedLocations() const {
    const auto s = mImpl->client->get_checked_locations();
    return std::vector<int64_t>(s.begin(), s.end());
}

std::vector<int64_t> APTransport::GetMissingLocations() const {
    const auto s = mImpl->client->get_missing_locations();
    return std::vector<int64_t>(s.begin(), s.end());
}

std::string APTransport::GetUuid(const std::string& uuidFilePath) {
    return ap_get_uuid(uuidFilePath);
}
