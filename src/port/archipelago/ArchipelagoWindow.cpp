#include "ArchipelagoWindow.h"
#include "Archipelago.h"
#include "ArchipelagoConsole.h"
#include "port/ui/UIWidgets.h"
#include "port/ui/ImguiUI.h"

#include <cstring>
#include <imgui.h>

namespace GameUI {
extern std::shared_ptr<ArchipelagoWindow> mArchipelagoWindow;
extern std::shared_ptr<ArchipelagoConsole::Window> mArchipelagoConsoleWindow;
} // namespace GameUI

static ImVec4 StatusColor(Archipelago::Conn conn, bool synced) {
    switch (conn) {
        case Archipelago::Conn::SlotConnected:
            return synced ? ImVec4(0.4f, 0.9f, 0.4f, 1.0f) : ImVec4(1.0f, 0.75f, 0.2f, 1.0f);
        case Archipelago::Conn::Connecting:
        case Archipelago::Conn::RoomInfo:
            return ImVec4(1.0f, 0.75f, 0.2f, 1.0f);
        case Archipelago::Conn::Refused:
        case Archipelago::Conn::VersionMismatch:
        case Archipelago::Conn::Failed:
            return ImVec4(1.0f, 0.35f, 0.35f, 1.0f);
        case Archipelago::Conn::Idle:
        default:
            return ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    }
}

static void CopyCVar(char* dst, size_t size, const char* cvar) {
    const char* v = CVarGetString(cvar, "");
    std::strncpy(dst, v, size - 1);
    dst[size - 1] = '\0';
}

void ArchipelagoWindow::DrawElement() {
    Archipelago* ap = Archipelago::Instance;
    ImGui::SetNextWindowSize(ImVec2(480, 420), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Archipelago", &mIsVisible)) {
        ImGui::End();
        return;
    }
    if (ap == nullptr) {
        ImGui::TextUnformatted("Archipelago is not initialized.");
        ImGui::End();
        return;
    }

    if (!mLoaded) {
        mLoaded = true;
        CopyCVar(mServer, sizeof(mServer), "gArchipelago.ServerAddress");
        if (mServer[0] == '\0') {
            std::strncpy(mServer, "archipelago.gg:38281", sizeof(mServer) - 1);
        }
        CopyCVar(mSlot, sizeof(mSlot), "gArchipelago.Slot");
        CopyCVar(mPassword, sizeof(mPassword), "gArchipelago.Password");
    }

    const Archipelago::Conn conn = ap->GetConn();
    const bool busy = conn == Archipelago::Conn::Connecting || conn == Archipelago::Conn::RoomInfo ||
                      conn == Archipelago::Conn::SlotConnected;

    ImGui::BeginDisabled(busy);
    if (ImGui::InputText("Server", mServer, sizeof(mServer))) {
        CVarSetString("gArchipelago.ServerAddress", mServer);
        CVarSave();
    }
    UIWidgets::Tooltip("host:port of the Archipelago room, e.g. archipelago.gg:38281");
    if (ImGui::InputText("Slot name", mSlot, sizeof(mSlot))) {
        CVarSetString("gArchipelago.Slot", mSlot);
        CVarSave();
    }
    ImGuiInputTextFlags pwFlags = mShowPassword ? 0 : ImGuiInputTextFlags_Password;
    if (ImGui::InputText("Password", mPassword, sizeof(mPassword), pwFlags)) {
        if (CVarGetInteger("gArchipelago.RememberPassword", 1) != 0) {
            CVarSetString("gArchipelago.Password", mPassword);
            CVarSave();
        } else {
            // Kept in memory only; Connect() reads the CVar, so mirror it without saving.
            CVarSetString("gArchipelago.Password", mPassword);
        }
    }
    ImGui::SameLine();
    ImGui::Checkbox("Show", &mShowPassword);
    ImGui::EndDisabled();

    if (UIWidgets::CVarCheckbox("Remember password", "gArchipelago.RememberPassword",
                                { .tooltip = "Stores the password in starship.cfg.json in plain text.",
                                  .defaultValue = true })) {
        if (CVarGetInteger("gArchipelago.RememberPassword", 1) == 0) {
            CVarSetString("gArchipelago.Password", "");
            CVarSave();
            CVarSetString("gArchipelago.Password", mPassword);
        }
    }
    UIWidgets::CVarCheckbox("Connect automatically on startup", "gArchipelago.AutoConnect",
                            { .tooltip = "Connects with the saved server/slot/password when Starship starts." });

    ImGui::Separator();

    const char* buttonLabel = "Connect";
    if (conn == Archipelago::Conn::Connecting || conn == Archipelago::Conn::RoomInfo) {
        buttonLabel = "Cancel";
    } else if (conn == Archipelago::Conn::SlotConnected) {
        buttonLabel = "Disconnect";
    }
    if (ImGui::Button(buttonLabel, ImVec2(120, 0))) {
        if (busy) {
            ap->Disconnect();
        } else {
            CVarSetString("gArchipelago.ServerAddress", mServer);
            CVarSetString("gArchipelago.Slot", mSlot);
            CVarSetString("gArchipelago.Password", mPassword);
            ap->Connect();
        }
    }
    ImGui::SameLine();
    ImGui::TextColored(StatusColor(conn, ap->IsReady()), "%s", ap->StatusText().c_str());

    if (ap->IsEnabled()) {
        ImGui::Separator();
        const auto& key = ap->Slot().GetKey();
        ImGui::Text("Seed: %s", key.seed.c_str());
        ImGui::Text("Slot: %d (%s), team %d", key.slot, key.slotName.c_str(), key.team);
        ImGui::Text("Items received: %zu", ap->ReceivedCount());
        ImGui::Text("Locations checked: %zu / %d", ap->CheckedCount(), AP_LOCATION_MAX - 1);
        ImGui::Text("Goal: %s", ap->GoalDone() ? "completed" : "not yet");
        ImGui::TextWrapped("Save: %s", ap->Slot().GetPath().c_str());
        if (ImGui::Button("End session", ImVec2(120, 0))) {
            ap->EndSession();
        }
        UIWidgets::Tooltip("Disconnects, saves the slot file and restarts the game on the vanilla save.");
    }

    ImGui::Separator();
    if (GameUI::mArchipelagoConsoleWindow != nullptr) {
        UIWidgets::WindowButton("Console", "gArchipelago.ConsoleOpen", GameUI::mArchipelagoConsoleWindow,
                                { .tooltip = "Server messages and chat" });
    }
    UIWidgets::CVarCheckbox("Notify on items", "gArchipelago.Notify.Items", { .defaultValue = true });
    UIWidgets::CVarCheckbox("Notify on connection changes", "gArchipelago.Notify.Connection",
                            { .defaultValue = true });
    UIWidgets::CVarCheckbox("Notify on chat", "gArchipelago.Notify.Chat");

    ImGui::End();
}

void DrawArchipelagoMenu() {
    if (UIWidgets::BeginMenu("Archipelago")) {
        if (GameUI::mArchipelagoWindow != nullptr) {
            UIWidgets::WindowButton("Connection", "gArchipelago.WindowOpen", GameUI::mArchipelagoWindow,
                                    { .tooltip = "Server / slot settings and connection status" });
        }
        if (GameUI::mArchipelagoConsoleWindow != nullptr) {
            UIWidgets::WindowButton("Console", "gArchipelago.ConsoleOpen", GameUI::mArchipelagoConsoleWindow,
                                    { .tooltip = "Server messages and chat" });
        }
        if (Archipelago::Instance != nullptr) {
            ImGui::TextColored(StatusColor(Archipelago::Instance->GetConn(), Archipelago::Instance->IsReady()), "%s",
                               Archipelago::Instance->StatusText().c_str());
        }
        ImGui::EndMenu();
    }
}
