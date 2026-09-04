#include "ArchipelagoTracker.h"
#include "Archipelago.h"
#include "game/ApLogic.h"

#include <imgui.h>
#include <vector>

void ArchipelagoTrackerWindow::DrawElement() {
    Archipelago* ap = Archipelago::Instance;
    ImGui::SetNextWindowSize(ImVec2(420, 520), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Archipelago Tracker", &mIsVisible)) {
        ImGui::End();
        return;
    }
    if (ap == nullptr || !ap->IsEnabled()) {
        ImGui::TextUnformatted("No Archipelago session.");
        ImGui::End();
        return;
    }

    ApLogic_Update();

    int total = 0;
    int checkedTotal = 0;
    for (uint16_t loc = 1; loc < AP_LOCATION_MAX; loc++) {
        if (ApLogic_LocationRegion(loc) >= 0 && AP_IsLocationChecked(loc)) {
            checkedTotal++;
        }
    }

    static bool showChecked = false;
    ImGui::Checkbox("Show checked locations", &showChecked);
    ImGui::Separator();

    for (int region = 0; region < AP_REGION_MAX; region++) {
        std::vector<uint16_t> inLogic;
        std::vector<uint16_t> checked;
        for (uint16_t loc = 1; loc < AP_LOCATION_MAX; loc++) {
            if (ApLogic_LocationRegion(loc) != region) {
                continue;
            }
            if (AP_IsLocationChecked(loc)) {
                checked.push_back(loc);
            } else if (ApLogic_LocationInLogic(loc)) {
                inLogic.push_back(loc);
            }
        }
        if (inLogic.empty() && (!showChecked || checked.empty())) {
            continue;
        }
        total += (int) inLogic.size();
        ImGui::PushID(region);
        if (ImGui::TreeNodeEx(gApRegionNames[region], ImGuiTreeNodeFlags_DefaultOpen, "%s (%zu)",
                              gApRegionNames[region], inLogic.size())) {
            for (uint16_t loc : inLogic) {
                ImGui::BulletText("%s", gApLocationNames[loc]);
            }
            if (showChecked) {
                for (uint16_t loc : checked) {
                    ImGui::TextDisabled("  %s (checked)", gApLocationNames[loc]);
                }
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    ImGui::Separator();
    ImGui::Text("%d in logic, %d checked", total, checkedTotal);
    ImGui::End();
}
