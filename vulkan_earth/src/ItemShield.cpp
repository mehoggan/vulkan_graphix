#include "ItemShield.h"
#include "Item.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ItemShield::ItemShield() = default;
ItemShield::ItemShield(int ID) {
    UNIQUEIDENTIFIER = ID;
    packageNum = 1;
    maxStack = 5;
    remaining = 1;
    imageFileName = "ItemShield.raw";
    description =
            "Shield:     Neutralize the damage taken for 5 times (uses 1 "
            "turn).";
    price = 150;
    specialNum = 5;
}
ItemShield::~ItemShield() = default;

ItemShield* ItemShield::getItemInstance() {
    return new ItemShield(UNIQUEIDENTIFIER);
}

bool ItemShield::causeEffectToTank(Tank* tank) {
    tank->setDurationShield(specialNum);
    return true;
}

void ItemShield::playUseSFX() { playSFX(ITEM_USE_SHIELD); }