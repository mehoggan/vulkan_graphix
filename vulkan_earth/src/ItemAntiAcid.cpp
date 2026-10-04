#include "vulkan_earth/ItemAntiAcid.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

ItemAntiAcid::ItemAntiAcid() = default;
ItemAntiAcid::ItemAntiAcid(std::int32_t id) {
    uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::item(
            vulkan_graphix::GameCatalog::ItemKind::AntiAcid));
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