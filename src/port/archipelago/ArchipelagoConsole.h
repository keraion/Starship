#pragma once

#include <deque>
#include <string>
#include <vector>
#include <imgui.h>
#include <libultraship/libultraship.h>

namespace ArchipelagoConsole {

struct Segment {
    std::string text;
    ImVec4 color;
};

using Line = std::vector<Segment>;

void Log(const std::string& text, ImVec4 color = ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
void LogLine(Line line);
void LogError(const std::string& text);
const std::deque<Line>& Lines();
void Clear();

class Window : public Ship::GuiWindow {
  public:
    using Ship::GuiWindow::GuiWindow;

    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};

  private:
    char mInput[512] = {};
    bool mScrollToBottom = false;
};

} // namespace ArchipelagoConsole
