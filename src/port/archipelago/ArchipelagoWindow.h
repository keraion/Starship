#pragma once

#include <libultraship/libultraship.h>

class ArchipelagoWindow : public Ship::GuiWindow {
  public:
    using Ship::GuiWindow::GuiWindow;

    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};

  private:
    bool mLoaded = false;
    bool mShowPassword = false;
    char mServer[256] = {};
    char mSlot[128] = {};
    char mPassword[128] = {};
};

// Menubar entry ("Archipelago" menu). Call from GameMenuBar::DrawElement().
void DrawArchipelagoMenu();
