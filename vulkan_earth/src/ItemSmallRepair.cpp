#include "vulkan_earth/ItemSmallRepair.h"
#include <cstdint>
#include "vulkan_earth/Item.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

ItemSmallRepair::ItemSmallRepair() = default;
ItemSmallRepair::ItemSmallRepair(std::int32_t id) {
    uniqueidentifier = id;
    package_num = 3;
    max_stack = 15;
    remaining = 3;
    image_file_name = "ItemSmallRepair.raw";
    description = "Small Repair:     Heals 200 damage.";
    price = 50;
    special_num = 200;
}
ItemSmallRepair::~ItemSmallRepair() = default;

ItemSmallRepair* ItemSmallRepair::getItemInstance() {
    return new ItemSmallRepair(uniqueidentifier);
}

bool ItemSmallRepair::causeEffectToTank(Tank* tank) {
    tank->setHP(tank->getHP() + special_num);
    if (tank->getHP() > tank->getArmor() * 100) {
        tank->setHP(tank->getArmor() * 100);
    }
    return false;
}

void ItemSmallRepair::playUseSFX() { playSFX(ITEM_USE3); }