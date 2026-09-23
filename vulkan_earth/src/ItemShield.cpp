#include "ItemShield.h"
#include "Item.h"
#include "MacroCrtdbg.h"
#include "Sound.h"

extern void playSFX(int sfx);

ItemShield::ItemShield() = default;
ItemShield::ItemShield(int id) {
    uniqueidentifier = id;
    package_num = 1;
    max_stack = 5;
    remaining = 1;
    image_file_name = "ItemShield.raw";
    description =
            "Shield:     Neutralize the damage taken for 5 times (uses 1 "
            "turn).";
    price = 150;
    special_num = 5;
}
ItemShield::~ItemShield() = default;

ItemShield* ItemShield::getItemInstance() {
    return new ItemShield(uniqueidentifier);
}

bool ItemShield::causeEffectToTank(Tank* tank) {
    tank->setDurationShield(special_num);
    return true;
}

void ItemShield::playUseSFX() { playSFX(ITEM_USE_SHIELD); }