#pragma once

#include "gfx.h"
#include "sf64object.h"
#include "port/hooks/impl/EventSystem.h"

DEFINE_EVENT(ItemDropEvent,
    Item* item;
);

// Fired after a static item from the level's object list was loaded (objectIndex = index into gLevelObjects).
DEFINE_EVENT(ItemStaticLoadEvent,
    Item* item;
    ObjectInit* objInit;
    s32 objectIndex;
);

// Fired after an actor (regular or event actor) was loaded from an ObjectInit. objectIndex is the index into
// gLevelObjects, or -1 when the ObjectInit does not belong to the level's object list.
DEFINE_EVENT(ActorStaticLoadEvent,
    Actor* actor;
    ObjectInit* objInit;
    s32 objectIndex;
);

// Fired after an actor dropped an item (func_enmy_800660F0), once the item's info has been set up.
DEFINE_EVENT(ActorItemDropEvent,
    Actor* actor;
    Item* item;
);
