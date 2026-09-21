#include "ItemSmallRepair.h"
#include "Item.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemSmallRepair::ItemSmallRepair() = default;
ItemSmallRepair::ItemSmallRepair(int id) {
    UNIQUEIDENTIFIER = id;
    packageNum = 3;
    maxStack = 15;
    remaining = 3;
    imageFileName = "ItemSmallRepair.raw";
    description = "Small Repair:     Heals 200 damage.";
    price = 50;
    specialNum = 200;
}
ItemSmallRepair::~ItemSmallRepair() = default;

ItemSmallRepair* ItemSmallRepair::getItemInstance() {
    return new ItemSmallRepair(UNIQUEIDENTIFIER);
}

bool ItemSmallRepair::causeEffectToTank(Tank* tank) {
    tank->setHP(tank->getHP() + specialNum);
    if (tank->getHP() > tank->getArmor() * 100) {
        tank->setHP(tank->getArmor() * 100);
    }
    return false;
}

void ItemSmallRepair::playUseSFX() { playSFX(ITEM_USE3); }