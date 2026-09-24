#pragma once

#include <libultraship/libultraship.h>

// Lists the unchecked locations that the seed's logic considers reachable, grouped by region.
class ArchipelagoTrackerWindow : public Ship::GuiWindow {
  public:
    using Ship::GuiWindow::GuiWindow;

    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};
};
