// Reads "options: <27 ints>" / "items: <61 ints>" pairs from stdin, prints reachable regions + in-logic locations.
#include <stdio.h>
#include <string.h>
#include "ApGame.h"
#include "ApLogic.h"
static uint8_t sItems[AP_ITEM_MAX];
static uint16_t sOptions[AP_OPTION_MAX];
uint8_t AP_GetItemCount(uint16_t item) { return item < AP_ITEM_MAX ? sItems[item] : 0; }
uint16_t AP_GetOption(uint16_t option) { return option < AP_OPTION_MAX ? sOptions[option] : 0; }
int ApMission_MedalCount(void) { return sItems[AP_ITEM_MEDAL]; } // the test feeds medals as items
int main(void) {
    int v;
    while (scanf("%d", &v) == 1) { // first value is a marker: 1 = case follows
        for (int i = 0; i < AP_OPTION_MAX; i++) { if (scanf("%d", &v) != 1) return 1; sOptions[i] = (uint16_t) v; }
        for (int i = 0; i < AP_ITEM_MAX; i++) { if (scanf("%d", &v) != 1) return 1; sItems[i] = (uint8_t) v; }
        ApLogic_Update();
        printf("R");
        for (int r = 0; r < AP_REGION_MAX; r++) printf(" %d", ApLogic_RegionReachable(r) ? 1 : 0);
        printf(" L");
        for (int l = 1; l < AP_LOCATION_MAX; l++) printf(" %d", ApLogic_LocationInLogic((uint16_t) l) ? 1 : 0);
        printf("\n");
    }
    return 0;
}
