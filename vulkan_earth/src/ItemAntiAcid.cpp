#include "vulkan_earth/ItemAntiAcid.h"
#include "vulkan_earth/Item.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(int sfx);

ItemAntiAcid::ItemAntiAcid() = default;
ItemAntiAcid::ItemAntiAcid(int id) {
    uniqueidentifier = id;
    package_num = 3;
    max_stack = 12;
    remaining = 3;
    image_file_name = "ItemAntiAcid.raw";
    description = "Anti-Acid:     Cures acid status.";
    price = 70;
    special_num = 0;
}
ItemAntiAcid::~ItemAntiAcid() = default;

ItemAntiAcid* ItemAntiAcid::getItemInstance() {
    return new ItemAntiAcid(uniqueidentifier);
}

bool ItemAntiAcid::causeEffectToTank(Tank* tank) {
    tank->setDurationAcid(special_num);
    return false;
}

void ItemAntiAcid::playUseSFX() { playSFX(ITEM_USE3); }