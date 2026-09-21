#include "ItemBigRepair.h"
#include "Item.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemBigRepair::ItemBigRepair() = default;
ItemBigRepair::ItemBigRepair(int id) {
    uniqueidentifier = id;
    package_num = 3;
    max_stack = 9;
    remaining = 3;
    image_file_name = "ItemBigRepair.raw";
    description = "Big Repair:     Heals 700 damage (uses 1 turn).";
    price = 100;
    special_num = 700;
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