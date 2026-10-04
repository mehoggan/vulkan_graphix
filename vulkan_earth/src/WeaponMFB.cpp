#include "vulkan_earth/WeaponMFB.h"
#include <cstdint>
#include "vulkan_earth/MacroCrtdbg.h"

WeaponMFB::WeaponMFB() = default;
WeaponMFB::WeaponMFB(std::int32_t id) {
    uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::weapon(
            vulkan_graphix::GameCatalog::WeaponKind::MFB));
}
WeaponMFB::~WeaponMFB() = default;

WeaponMFB* WeaponMFB::getWeaponInstance() {
    return new WeaponMFB(uniqueidentifier);
}
