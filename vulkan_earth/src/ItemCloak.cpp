#include "ItemCloak.h"
#include "Item.h"
#include "macro_crtdbg.h"

ItemCloak::ItemCloak() = default;
ItemCloak::ItemCloak(int id) {
    UNIQUEIDENTIFIER = id;
    packageNum = 2;
    maxStack = 10;
    remaining = 2;
    imageFileName = "ItemCloak.raw";
    description =
            "Cloak:     Makes the player's tank disappear (users 1 turn).";
    price = 100;
    specialNum = 5;
}
ItemCloak::~ItemCloak() = default;

ItemCloak* ItemCloak::getItemInstance() {
    return new ItemCloak(UNIQUEIDENTIFIER);
}

bool ItemCloak::causeEffectToTank(Tank* tank) {
    tank->setDurationCloak(specialNum + 1);
    return true;
}