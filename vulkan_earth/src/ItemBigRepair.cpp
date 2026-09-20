#include "ItemBigRepair.h"
#include "Item.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemBigRepair::ItemBigRepair() = default;
ItemBigRepair::ItemBigRepair(int ID) {
    UNIQUEIDENTIFIER = ID;
    packageNum = 3;
    maxStack = 9;
    remaining = 3;
    imageFileName = "ItemBigRepair.raw";
    description = "Big Repair:     Heals 700 damage (uses 1 turn).";
    price = 100;
    specialNum = 700;
}
ItemBigRepair::~ItemBigRepair() = default;

ItemBigRepair* ItemBigRepair::getItemInstance() {
    return new ItemBigRepair(UNIQUEIDENTIFIER);
}

bool ItemBigRepair::causeEffectToTank(Tank* tank) {
    tank->setHP(tank->getHP() + specialNum);
    if (tank->getHP() > tank->getArmor() * 100) {
        tank->setHP(tank->getArmor() * 100);
    }
    return true;
}

void ItemBigRepair::playUseSFX() { playSFX(ITEM_USE3); }