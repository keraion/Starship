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
