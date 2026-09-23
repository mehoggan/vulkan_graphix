#include "ItemExtraBattery.h"
#include "Item.h"
#include "MacroCrtdbg.h"
#include "Sound.h"

extern void playSFX(int sfx);

ItemExtraBattery::ItemExtraBattery() = default;
ItemExtraBattery::ItemExtraBattery(int id) {
    uniqueidentifier = id;
    package_num = 3;
    max_stack = 9;
    remaining = 3;
    image_file_name = "ItemExtraBattery.raw";
    description = "Extra Battery:     Recovers from the damage of EMP.";
    price = 50;
    special_num = 0;
}
ItemExtraBattery::~ItemExtraBattery() = default;

ItemExtraBattery* ItemExtraBattery::getItemInstance() {
    return new ItemExtraBattery(uniqueidentifier);
}

bool ItemExtraBattery::causeEffectToTank(Tank* tank) {
    tank->setDurationEMP(special_num);
    return false;
}

void ItemExtraBattery::playUseSFX() { playSFX(ITEM_USE3); }