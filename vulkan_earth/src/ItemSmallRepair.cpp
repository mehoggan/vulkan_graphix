#include "ItemSmallRepair.h"
#include "Item.h"
#include "MacroCrtdbg.h"
#include "Sound.h"

extern void playSFX(int sfx);

ItemSmallRepair::ItemSmallRepair() = default;
ItemSmallRepair::ItemSmallRepair(int id) {
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