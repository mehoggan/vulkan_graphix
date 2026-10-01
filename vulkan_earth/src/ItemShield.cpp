#include "vulkan_earth/ItemShield.h"
#include <cstdint>
#include "vulkan_earth/Item.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

ItemShield::ItemShield() = default;
ItemShield::ItemShield(std::int32_t id) {
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