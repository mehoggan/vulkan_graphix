#include "ItemBigRepair.h"
#include "Item.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemBigRepair::ItemBigRepair() = default;
ItemBigRepair::ItemBigRepair(int ID) {
    this->UNIQUEIDENTIFIER = ID;
    this->packageNum = 3;
    this->maxStack = 9;
    this->remaining = 3;
    this->imageFileName = "ItemBigRepair.raw";
    this->description = "Big Repair:     Heals 700 damage (uses 1 turn).";
    this->price = 100;
    this->specialNum = 700;
}
ItemBigRepair::~ItemBigRepair() = default;

ItemBigRepair* ItemBigRepair::getItemInstance() {
    return new ItemBigRepair(this->UNIQUEIDENTIFIER);
}

bool ItemBigRepair::causeEffectToTank(Tank* tank) {
    tank->setHP(tank->getHP() + specialNum);
    if (tank->getHP() > tank->getArmor() * 100) {
        tank->setHP(tank->getArmor() * 100);
    }
    return true;
}

void ItemBigRepair::playUseSFX() { playSFX(ITEM_USE3); }