#include "PortEnhancements.h"
#include "global.h"
#include "assets/ast_common.h"
#include "port/hooks/Events.h"
#include "port/archipelago/ArchipelagoBridge.h"

// ---------------------------------------------------------------------------
// Engine glow colour. A Starship enhancement (Enhancements > Cosmetics) stored in gCosmetics.EngineGlow;
// while an Archipelago session is active, the slot's engine_glow option overrides it unless the player
// turned that off (gCosmetics.AllowArchipelago). The choice list matches the apworld option one for one,
// so the yaml value can be used as-is.

typedef enum {
    ENGINE_GLOW_DEFAULT,
    ENGINE_GLOW_RAINBOW,
    ENGINE_GLOW_RED,
    ENGINE_GLOW_DEEP_PINK,
    ENGINE_GLOW_MAGENTA,
    ENGINE_GLOW_ELECTRIC_INDIGO,
    ENGINE_GLOW_BLUE,
    ENGINE_GLOW_DODGER_BLUE,
    ENGINE_GLOW_AQUA,
    ENGINE_GLOW_SPRING_GREEN,
    ENGINE_GLOW_LIME,
    ENGINE_GLOW_CHARTREUSE,
    ENGINE_GLOW_YELLOW,
    ENGINE_GLOW_DARK_ORANGE,
    ENGINE_GLOW_MAX,
} EngineGlowChoice;

// CSS colours of the same names (the apworld option is named after them).
static const u8 sEngineGlowRgb[ENGINE_GLOW_MAX][3] = {
    { 0, 0, 0 },       // default (unused)
    { 0, 0, 0 },       // rainbow (computed)
    { 255, 0, 0 },     // red
    { 255, 20, 147 },  // deep pink
    { 255, 0, 255 },   // magenta
    { 111, 0, 255 },   // electric indigo
    { 0, 0, 255 },     // blue
    { 30, 144, 255 },  // dodger blue
    { 0, 255, 255 },   // aqua
    { 0, 255, 127 },   // spring green
    { 0, 255, 0 },     // lime
    { 127, 255, 0 },   // chartreuse
    { 255, 255, 0 },   // yellow
    { 255, 140, 0 },   // dark orange
};

static s32 EffectiveEngineGlow(void) {
    s32 choice = CVarGetInteger("gCosmetics.EngineGlow", ENGINE_GLOW_DEFAULT);

    if (AP_IsEnabled() && CVarGetInteger("gCosmetics.AllowArchipelago", 1)) {
        s32 fromYaml = AP_GetOption(AP_OPTION_ENGINE_GLOW);
        if (fromYaml != ENGINE_GLOW_DEFAULT) {
            choice = fromYaml;
        }
    }
    if ((choice < ENGINE_GLOW_DEFAULT) || (choice >= ENGINE_GLOW_MAX)) {
        choice = ENGINE_GLOW_DEFAULT;
    }
    return choice;
}

// Fully saturated hue (0..1) to RGB.
static void HueToRgb(f32 hue, u8* r, u8* g, u8* b) {
    f32 h6 = (hue - (s32) hue) * 6.0f;
    s32 sector = (s32) h6;
    f32 frac = h6 - sector;
    u8 rise = (u8) (frac * 255.0f);
    u8 fall = (u8) ((1.0f - frac) * 255.0f);

    switch (sector) {
        case 0: *r = 255; *g = rise; *b = 0; break;
        case 1: *r = fall; *g = 255; *b = 0; break;
        case 2: *r = 0; *g = 255; *b = rise; break;
        case 3: *r = 0; *g = fall; *b = 255; break;
        case 4: *r = rise; *g = 0; *b = 255; break;
        default: *r = 255; *g = 0; *b = fall; break;
    }
}

static void OnPlayerEngineGlow(PlayerEngineGlowEvent* event) {
    s32 choice = EffectiveEngineGlow();
    u8 r;
    u8 g;
    u8 b;

    if (choice == ENGINE_GLOW_DEFAULT) {
        return; // vanilla colour for the level type
    }
    if (choice == ENGINE_GLOW_RAINBOW) {
        HueToRgb((gGameFrameCount % 200) / 200.0f, &r, &g, &b); // one cycle every 200 frames, like the ROM hack
    } else {
        r = sEngineGlowRgb[choice][0];
        g = sEngineGlowRgb[choice][1];
        b = sEngineGlowRgb[choice][2];
    }

    event->event.cancelled = true;
    // Same setup as Display_DrawEngineGlow, with our env colour.
    RCP_SetupDL(&gMasterDisp, SETUPDL_67);
    gDPSetPrimColor(gMasterDisp++, 0x00, 0x00, 255, 255, 255, 255);
    gDPSetEnvColor(gMasterDisp++, r, g, b, 255);
    gSPDisplayList(gMasterDisp++, aOrbDL);
}

void Cosmetics_Init(void) {
    REGISTER_LISTENER(PlayerEngineGlowEvent, (EventCallback) OnPlayerEngineGlow, EVENT_PRIORITY_NORMAL);
}
