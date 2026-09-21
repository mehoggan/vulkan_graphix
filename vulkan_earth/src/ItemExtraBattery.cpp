#include "ItemExtraBattery.h"
#include "Item.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemExtraBattery::ItemExtraBattery() = default;
ItemExtraBattery::ItemExtraBattery(int id) {
    UNIQUEIDENTIFIER = id;
    packageNum = 3;
    maxStack = 9;
    remaining = 3;
    imageFileName = "ItemExtraBattery.raw";
    description = "Extra Battery:     Recovers from the damage of EMP.";
    price = 50;
    specialNum = 0;
}
ItemExtraBattery::~ItemExtraBattery() = default;

ItemExtraBattery* ItemExtraBattery::getItemInstance() {
    return new ItemExtraBattery(UNIQUEIDENTIFIER);
}

bool ItemExtraBattery::causeEffectToTank(Tank* tank) {
    tank->setDurationEMP(specialNum);
    return false;
}

void ItemExtraBattery::playUseSFX() { playSFX(ITEM_USE3); }