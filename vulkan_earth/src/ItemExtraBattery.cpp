#include "vulkan_earth/ItemExtraBattery.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

ItemExtraBattery::ItemExtraBattery() = default;
ItemExtraBattery::ItemExtraBattery(std::int32_t id) {
    m_uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::item(
            vulkan_graphix::GameCatalog::ItemKind::ExtraBattery));
}
ItemExtraBattery::~ItemExtraBattery() = default;

ItemExtraBattery* ItemExtraBattery::getItemInstance() {
    return new ItemExtraBattery(m_uniqueidentifier);
}

bool ItemExtraBattery::causeEffectToTank(Tank* tank) {
    tank->setDurationEMP(m_special_num);
    return false;
}

void ItemExtraBattery::playUseSFX() { playSFX(ITEM_USE3); }