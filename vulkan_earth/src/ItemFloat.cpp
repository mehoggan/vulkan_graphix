#include "vulkan_earth/ItemFloat.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

ItemFloat::ItemFloat() = default;
ItemFloat::ItemFloat(std::int32_t id) {
    uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::item(
            vulkan_graphix::GameCatalog::ItemKind::Float));
}
ItemFloat::~ItemFloat() = default;

ItemFloat* ItemFloat::getItemInstance() {
    return new ItemFloat(uniqueidentifier);
}

bool ItemFloat::causeEffectToTank(Tank* tank) {
    tank->setDurationFloat(special_num + 1);
    return true;
}

void ItemFloat::playUseSFX() { playSFX(ITEM_USE_FLOAT); }