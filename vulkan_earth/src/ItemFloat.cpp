#include "ItemFloat.h"
#include "Item.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemFloat::ItemFloat() = default;
ItemFloat::ItemFloat(int ID) {
    UNIQUEIDENTIFIER = ID;
    packageNum = 2;
    maxStack = 10;
    remaining = 2;
    imageFileName = "ItemFloat.raw";
    description = "Float:     Allows the player to float (uses 1 turn).";
    price = 80;
    specialNum = 5;
}
ItemFloat::~ItemFloat() = default;

ItemFloat* ItemFloat::getItemInstance() {
    return new ItemFloat(UNIQUEIDENTIFIER);
}

bool ItemFloat::causeEffectToTank(Tank* tank) {
    tank->setDurationFloat(specialNum + 1);
    return true;
}

void ItemFloat::playUseSFX() { playSFX(ITEM_USE_FLOAT); }