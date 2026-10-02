#include "vulkan_earth/ItemShield.h"
#include <cstdint>
#include "vulkan_earth/Item.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

ItemShield::ItemShield() = default;
ItemShield::ItemShield(std::int32_t id) {
    uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::item(
            vulkan_graphix::GameCatalog::ItemKind::Shield));
}
ItemShield::~ItemShield() = default;

ItemShield* ItemShield::getItemInstance() {
    return new ItemShield(uniqueidentifier);
}

bool ItemShield::causeEffectToTank(Tank* tank) {
    tank->setDurationShield(special_num);
    return true;
}

void ItemShield::playUseSFX() { playSFX(ITEM_USE_SHIELD); }