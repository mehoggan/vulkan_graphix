#include "vulkan_earth/ItemBigRepair.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

ItemBigRepair::ItemBigRepair() = default;
ItemBigRepair::ItemBigRepair(std::int32_t id) {
    uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::item(
            vulkan_graphix::GameCatalog::ItemKind::BigRepair));
}
ItemBigRepair::~ItemBigRepair() = default;

ItemBigRepair* ItemBigRepair::getItemInstance() {
    return new ItemBigRepair(uniqueidentifier);
}

bool ItemBigRepair::causeEffectToTank(Tank* tank) {
    tank->setHP(tank->getHP() + special_num);
    if (tank->getHP() > tank->getArmor() * 100) {
        tank->setHP(tank->getArmor() * 100);
    }
    return true;
}

void ItemBigRepair::playUseSFX() { playSFX(ITEM_USE3); }