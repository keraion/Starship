#include "ApGame.h"
#include "assets/ast_common.h"

// ---------------------------------------------------------------------------
// Static item tagging: unchecked AP locations are shown as an "AP orb" (a
// silver ring hitbox with a custom draw) and collecting one sends the check.

static s16 sApItemLocation[ARRAY_COUNT(gItems)]; // AP location id per live item slot, 0 = untagged

static s32 ItemSlot(Item* item) {
    s32 slot = (s32) (item - gItems);
    if ((slot < 0) || (slot >= ARRAY_COUNT(gItems))) {
        return -1;
    }
    return slot;
}

static void ClearTag(Item* item) {
    s32 slot = ItemSlot(item);
    if (slot >= 0) {
        sApItemLocation[slot] = 0;
    }
}

// Every Item_Load (static or enemy drop) fires ItemDropEvent first: reset the slot's tag.
static void OnItemDrop(ItemDropEvent* event) {
    ClearTag(event->item);
}

// Some locations are items dropped by a scripted enemy (e.g. Corneria's city-center Silver Ring); the world
// data keys them by the dropping actor's index in the level object list. Remember that index per actor slot.
static s16 sApActorObjectIndex[ARRAY_COUNT(gActors)];
static s16 sApActorObjectId[ARRAY_COUNT(gActors)]; // objInit->id, used to detect a reused slot

static s32 ActorSlot(Actor* actor) {
    s32 slot = (s32) (actor - gActors);
    if ((slot < 0) || (slot >= ARRAY_COUNT(gActors))) {
        return -1;
    }
    return slot;
}

static void OnActorStaticLoad(ActorStaticLoadEvent* event) {
    s32 slot = ActorSlot(event->actor);

    if (slot < 0) {
        return;
    }
    sApActorObjectIndex[slot] = (event->objectIndex >= 0) ? (s16) event->objectIndex : -1;
    sApActorObjectId[slot] = event->objInit->id;
}

static bool TagItem(Item* item, s16 loc) {
    s32 slot = ItemSlot(item);

    if ((slot < 0) || (loc <= 0) || AP_IsLocationChecked(loc)) {
        return false;
    }
    // Replace the pickup with an AP orb. The silver ring info gives us the hitbox; update/draw are overridden below.
    item->obj.id = OBJ_ITEM_SILVER_RING;
    Object_SetInfo(&item->info, item->obj.id);
    sApItemLocation[slot] = loc;
    return true;
}

static void OnActorItemDrop(ActorItemDropEvent* event) {
    Actor* actor = event->actor;
    s32 slot = ActorSlot(actor);
    s16 expectedId;

    if ((slot < 0) || !AP_IsEnabled() || (gLevelPhase != 0) || gVersusMode || (sApActorObjectIndex[slot] < 0)) {
        return;
    }
    // A slot can be reused by an actor spawned without an ObjectInit; only trust the index if the ids still agree.
    expectedId = (actor->obj.id == OBJ_ACTOR_EVENT) ? (s16) (actor->aiType + ACTOR_EVENT_ID) : actor->obj.id;
    if (sApActorObjectId[slot] != expectedId) {
        return;
    }
    TagItem(event->item, ApTables_GetStaticLocation(gCurrentLevel, sApActorObjectIndex[slot]));
}

static void OnItemStaticLoad(ItemStaticLoadEvent* event) {
    Item* item = event->item;
    s32 slot = ItemSlot(item);
    s16 loc;
    bool isCheckpoint;

    if (slot < 0) {
        return;
    }
    sApItemLocation[slot] = 0;

    if (!AP_IsEnabled() || (gLevelPhase != 0) || gVersusMode) {
        return;
    }

    loc = ApTables_GetStaticLocation(gCurrentLevel, event->objectIndex);
    if (loc <= 0) {
        return;
    }

    isCheckpoint = ApTables_IsCheckpointLocation(loc);
    if (isCheckpoint && !AP_GetOption(AP_OPTION_SHUFFLE_CHECKPOINTS)) {
        return; // vanilla checkpoint ring
    }

    if (TagItem(item, loc)) {
        // unchecked location: now an AP orb
    } else if (isCheckpoint && !AP_HasItem(ApTables_CheckpointItem(loc))) {
        // Location already sent but the checkpoint item hasn't been received: no ring.
        item->obj.status = OBJ_FREE;
    }
}

static void OnItemUpdate(ObjectUpdateEvent* event) {
    Item* item;
    s32 slot;
    s16 loc;

    if (event->type != OBJECT_TYPE_ITEM) {
        return;
    }
    item = (Item*) event->object;
    slot = ItemSlot(item);
    if (slot < 0) {
        return;
    }
    loc = sApItemLocation[slot];
    if (loc == 0) {
        return;
    }

    event->event.cancelled = true;
    Item_Move(item);

    switch (item->state) {
        case 0:
            Math_SmoothStepToF(&item->width, 0.4f, 1.0f, 0.05f, 0.0f);
            Item_CheckBounds(item);
            Item_SpinPickup(item);
            if (item->collected) {
                item->state = 1;
                item->timer_48 = 50;
                AP_CheckLocation((uint16_t) loc);
                Object_PlayerSfx(gPlayer[item->playerNum].sfxSource, NA_SE_GET_EMBLEM, item->playerNum);
            }
            break;

        default:
            // Vanilla pull-in animation; kills the object when timer_48 reaches 0.
            ItemSupplyRing_Update(item);
            break;
    }

    if (item->obj.status == OBJ_FREE) {
        sApItemLocation[slot] = 0;
    }
}

static void OnItemDraw(ObjectDrawPostSetupEvent* event) {
    Item* item;
    s32 slot;

    if (event->type != OBJECT_TYPE_ITEM) {
        return;
    }
    item = (Item*) event->object;
    slot = ItemSlot(item);
    if ((slot < 0) || (sApItemLocation[slot] == 0)) {
        return;
    }

    event->event.cancelled = true;

    // Placeholder "AP orb": a gold ring with a spinning silver star inside.
    RCP_SetupDL(&gMasterDisp, SETUPDL_29);
    gSPTexture(gMasterDisp++, 1900, 1700, 0, G_TX_RENDERTILE, G_ON);
    gSPSetGeometryMode(gMasterDisp++, G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);

    Matrix_Push(&gGfxMatrix);
    Graphics_SetScaleMtx(item->width);
    gSPDisplayList(gMasterDisp++, D_1016870);
    Matrix_Pop(&gGfxMatrix);

    Matrix_Push(&gGfxMatrix);
    Matrix_RotateY(gGfxMatrix, gGameFrameCount * 6.0f * M_DTOR, MTXF_APPLY);
    Matrix_RotateX(gGfxMatrix, gGameFrameCount * 3.0f * M_DTOR, MTXF_APPLY);
    Graphics_SetScaleMtx(item->width * 0.55f);
    gSPDisplayList(gMasterDisp++, D_1019CA0);
    Matrix_Pop(&gGfxMatrix);

    gSPClearGeometryMode(gMasterDisp++, G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);
}

// ---------------------------------------------------------------------------
// Received consumables (main.c main_loop)

void ApItems_GrantLives(void) {
    APSlotState* st;

    if (!AP_IsEnabled()) {
        return;
    }
    st = AP_SaveState();
    if (st->items[AP_ITEM_EXTRA_ARWING] > st->received.lives) {
        gLifeCount[0] += st->items[AP_ITEM_EXTRA_ARWING] - st->received.lives;
        if (gLifeCount[0] > 99) {
            gLifeCount[0] = 99;
        }
        st->received.lives = st->items[AP_ITEM_EXTRA_ARWING];
        AP_MarkSaveDirty();
    }
}

static void OnPlayerUpdatePost(PlayerPostUpdateEvent* event) {
    Player* player = event->player;
    APSlotState* st;
    bool dirty = false;

    if (!AP_IsEnabled()) {
        return;
    }
    if ((gGameState != GSTATE_PLAY) || (gPlayState != PLAY_UPDATE) || (player != &gPlayer[0]) ||
        (player->state != PLAYERSTATE_ACTIVE) || (gCurrentLevel == LEVEL_TRAINING) || gVersusMode) {
        return;
    }
    st = AP_SaveState();

    // Laser upgrades: repair a broken wing first, otherwise upgrade. Banked while already at hyper lasers.
    if ((gLaserStrength[0] < LASERS_HYPER) && (st->items[AP_ITEM_LASER_UPGRADE] > st->received.lasers)) {
        if ((player->arwing.rightWingState <= WINGSTATE_BROKEN) || (player->arwing.leftWingState <= WINGSTATE_BROKEN)) {
            func_enmy_80067A40();
        } else {
            gLaserStrength[0]++;
            Object_PlayerSfx(player->sfxSource, NA_SE_TWIN_LASER_GET, 0);
            if (gExpertMode) {
                gRightWingHealth[0] = gLeftWingHealth[0] = 10;
            } else {
                gRightWingHealth[0] = gLeftWingHealth[0] = 60;
            }
        }
        st->received.lasers++;
        dirty = true;
    }

    if (st->items[AP_ITEM_SILVER_RING] > st->received.silvers) {
        player->heal += (st->items[AP_ITEM_SILVER_RING] - st->received.silvers) * 32;
        Object_PlayerSfx(player->sfxSource, NA_SE_SHIELD_RING, 0);
        st->received.silvers = st->items[AP_ITEM_SILVER_RING];
        dirty = true;
    }

    if ((gGoldRingCount[0] < 3) && (st->items[AP_ITEM_GOLD_RING] > st->received.golds)) {
        player->heal += 32;
        gGoldRingCount[0]++;
        st->received.golds++;
        Object_PlayerSfx(player->sfxSource, (gGoldRingCount[0] == 3) ? NA_SE_SHIELD_UPGRADE : NA_SE_GOLD_RING, 0);
        dirty = true;
    }

    if (st->items[AP_ITEM_SILVER_STAR] > st->received.stars) {
        player->heal += (st->items[AP_ITEM_SILVER_STAR] - st->received.stars) * 128;
        Object_PlayerSfx(player->sfxSource, NA_SE_SHIELD_RING_M, 0);
        st->received.stars = st->items[AP_ITEM_SILVER_STAR];
        dirty = true;
    }

    if ((gBombCount[0] < 9) && (st->items[AP_ITEM_BOMB] > st->received.bombs)) {
        gBombCount[0]++;
        st->received.bombs++;
        Object_PlayerSfx(player->sfxSource, NA_SE_BOMB_GET, 0);
        dirty = true;
    }

    if (st->items[AP_ITEM_EXTRA_ARWING] > st->received.lives) {
        gLifeCount[0] += st->items[AP_ITEM_EXTRA_ARWING] - st->received.lives;
        if (gLifeCount[0] > 99) {
            gLifeCount[0] = 99;
        }
        Object_PlayerSfx(player->sfxSource, NA_SE_ONE_UP, 0);
        st->received.lives = st->items[AP_ITEM_EXTRA_ARWING];
        dirty = true;
    }

    if (dirty) {
        AP_MarkSaveDirty();
    }
}

void ApItems_Init(void) {
    memset(sApActorObjectIndex, 0xFF, sizeof(sApActorObjectIndex)); // -1: no object index known
    REGISTER_LISTENER(ItemDropEvent, (EventCallback) OnItemDrop, EVENT_PRIORITY_LOW);
    REGISTER_LISTENER(ItemStaticLoadEvent, (EventCallback) OnItemStaticLoad, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(ActorStaticLoadEvent, (EventCallback) OnActorStaticLoad, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(ActorItemDropEvent, (EventCallback) OnActorItemDrop, EVENT_PRIORITY_NORMAL);
    REGISTER_LISTENER(ObjectUpdateEvent, (EventCallback) OnItemUpdate, EVENT_PRIORITY_LOW);
    REGISTER_LISTENER(ObjectDrawPostSetupEvent, (EventCallback) OnItemDraw, EVENT_PRIORITY_LOW);
    REGISTER_LISTENER(PlayerPostUpdateEvent, (EventCallback) OnPlayerUpdatePost, EVENT_PRIORITY_NORMAL);
}
