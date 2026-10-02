#include "vulkan_earth/ItemExtraBattery.h"
#include <cstdint>
#include "vulkan_earth/Item.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

ItemExtraBattery::ItemExtraBattery() = default;
ItemExtraBattery::ItemExtraBattery(std::int32_t id) {
    uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::item(
            vulkan_graphix::GameCatalog::ItemKind::ExtraBattery));
}
ItemExtraBattery::~ItemExtraBattery() = default;

ItemExtraBattery* ItemExtraBattery::getItemInstance() {
    return new ItemExtraBattery(uniqueidentifier);
}

bool ItemExtraBattery::causeEffectToTank(Tank* tank) {
    tank->setDurationEMP(special_num);
    return false;
}

void ItemExtraBattery::playUseSFX() { playSFX(ITEM_USE3); }