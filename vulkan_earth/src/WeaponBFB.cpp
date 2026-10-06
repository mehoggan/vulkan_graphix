#include "vulkan_earth/WeaponBFB.h"
#include <cstdint>
#include "vulkan_earth/MacroCrtdbg.h"

WeaponBFB::WeaponBFB() = default;
WeaponBFB::WeaponBFB(std::int32_t id) {
    m_uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::weapon(
      vulkan_graphix::GameCatalog::WeaponKind::BFB));
}
WeaponBFB::~WeaponBFB() = default;

WeaponBFB* WeaponBFB::getWeaponInstance() {
    return new WeaponBFB(m_uniqueidentifier);
}
