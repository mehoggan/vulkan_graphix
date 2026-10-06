#include "vulkan_earth/ItemCloak.h"
#include <cstdint>
#include "vulkan_earth/MacroCrtdbg.h"

ItemCloak::ItemCloak() = default;
ItemCloak::ItemCloak(std::int32_t id) {
    m_uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::item(
      vulkan_graphix::GameCatalog::ItemKind::Cloak));
}
ItemCloak::~ItemCloak() = default;

ItemCloak* ItemCloak::getItemInstance() {
    return new ItemCloak(m_uniqueidentifier);
}

bool ItemCloak::causeEffectToTank(Tank* tank) {
    tank->setDurationCloak(m_special_num + 1);
    return true;
}