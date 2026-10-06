#include "vulkan_earth/ItemSmallRepair.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

ItemSmallRepair::ItemSmallRepair() = default;
ItemSmallRepair::ItemSmallRepair(std::int32_t id) {
    m_uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::item(
      vulkan_graphix::GameCatalog::ItemKind::SmallRepair));
}
ItemSmallRepair::~ItemSmallRepair() = default;

ItemSmallRepair* ItemSmallRepair::getItemInstance() {
    return new ItemSmallRepair(m_uniqueidentifier);
}

bool ItemSmallRepair::causeEffectToTank(Tank* tank) {
    tank->setHP(tank->getHP() + m_special_num);
    if (tank->getHP() > tank->getArmor() * 100) {
        tank->setHP(tank->getArmor() * 100);
    }
    return false;
}

void ItemSmallRepair::playUseSFX() { playSFX(ITEM_USE3); }