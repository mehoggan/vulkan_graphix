#include "vulkan_earth/ItemFloat.h"
#include <cstdint>
#include "vulkan_earth/Item.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

ItemFloat::ItemFloat() = default;
ItemFloat::ItemFloat(std::int32_t id) {
    uniqueidentifier = id;
    package_num = 2;
    max_stack = 10;
    remaining = 2;
    image_file_name = "ItemFloat.raw";
    description = "Float:     Allows the player to float (uses 1 turn).";
    price = 80;
    special_num = 5;
}
ItemFloat::~ItemFloat() = default;

ItemFloat* ItemFloat::getItemInstance() {
    return new ItemFloat(uniqueidentifier);
}

bool ItemFloat::causeEffectToTank(Tank* tank) {
    tank->setDurationFloat(special_num + 1);
    return true;
}

void ItemFloat::playUseSFX() { playSFX(ITEM_USE_FLOAT); }