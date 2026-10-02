#include "vulkan_earth/WeaponDefault.h"
#include <cstdint>
#include "vulkan_earth/Weapon.h"
#include "vulkan_earth/MacroCrtdbg.h"

WeaponDefault::WeaponDefault() = default;
WeaponDefault::WeaponDefault(std::int32_t id) {
    uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::weapon(
            vulkan_graphix::GameCatalog::WeaponKind::Default));
}
WeaponDefault::~WeaponDefault() = default;

WeaponDefault* WeaponDefault::getWeaponInstance() {
    return new WeaponDefault(uniqueidentifier);
}