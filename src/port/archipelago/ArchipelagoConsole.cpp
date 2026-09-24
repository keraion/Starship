#include "ArchipelagoConsole.h"
#include "Archipelago.h"
#include "port/ui/UIWidgets.h"

#include <spdlog/spdlog.h>

namespace ArchipelagoConsole {

static constexpr size_t kMaxLines = 200;
static std::deque<Line> sLines;

void LogLine(Line line) {
    std::string flat;
    for (const auto& s : line) {
        flat += s.text;
    }
    SPDLOG_INFO("[AP] {}", flat);
    sLines.push_back(std::move(line));
    while (sLines.size() > kMaxLines) {
        sLines.pop_front();
    }
}

void Log(const std::string& text, ImVec4 color) {
    LogLine({ { text, color } });
}

void LogError(const std::string& text) {
    LogLine({ { text, ImVec4(1.0f, 0.35f, 0.35f, 1.0f) } });
}

const std::deque<Line>& Lines() {
    return sLines;
}

void Clear() {
    sLines.clear();
}

void Window::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(560, 420), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Archipelago Console", &mIsVisible)) {
        ImGui::End();
        return;
    }

    const float footer = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("##aplog", ImVec2(0, -footer), true, ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 2.0f));
        for (const auto& line : sLines) {
            bool first = true;
            for (const auto& seg : line) {
                if (!first) {
                    ImGui::SameLine(0.0f, 0.0f);
                }
                first = false;
                ImGui::TextColored(seg.color, "%s", seg.text.c_str());
            }
            if (line.empty()) {
                ImGui::TextUnformatted("");
            }
        }
        ImGui::PopStyleVar();
        if (mScrollToBottom || ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 4.0f) {
            ImGui::SetScrollHereY(1.0f);
        }
        mScrollToBottom = false;
    }
    ImGui::EndChild();

    ImGui::SetNextItemWidth(-90.0f);
    bool send = ImGui::InputText("##apchat", mInput, sizeof(mInput), ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    send |= ImGui::Button("Send", ImVec2(80.0f, 0.0f));
    if (send && mInput[0] != '\0') {
        std::string text(mInput);
        mInput[0] = '\0';
        if (Archipelago::Instance != nullptr) {
            Archipelago::Instance->Say(text);
        }
        mScrollToBottom = true;
        ImGui::SetKeyboardFocusHere(-1);
    }

    ImGui::End();
}

} // namespace ArchipelagoConsole
