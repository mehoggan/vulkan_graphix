#include "ItemCloak.h"
#include "Item.h"
#include "macro_crtdbg.h"

ItemCloak::ItemCloak() = default;
ItemCloak::ItemCloak(int id) {
    uniqueidentifier = id;
    package_num = 2;
    max_stack = 10;
    remaining = 2;
    image_file_name = "ItemCloak.raw";
    description =
            "Cloak:     Makes the player's tank disappear (users 1 turn).";
    price = 100;
    special_num = 5;
}
ItemCloak::~ItemCloak() = default;

ItemCloak* ItemCloak::getItemInstance() {
    return new ItemCloak(uniqueidentifier);
}

bool ItemCloak::causeEffectToTank(Tank* tank) {
    tank->setDurationCloak(special_num + 1);
    return true;
}