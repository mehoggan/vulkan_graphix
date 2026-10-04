#include "vulkan_earth/WeaponNuke.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

WeaponNuke::WeaponNuke() = default;
WeaponNuke::WeaponNuke(std::int32_t id) {
    uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::weapon(
            vulkan_graphix::GameCatalog::WeaponKind::Nuke));
}
WeaponNuke::~WeaponNuke() = default;

WeaponNuke* WeaponNuke::getWeaponInstance() {
    return new WeaponNuke(uniqueidentifier);
}

void WeaponNuke::playExplosionSFX() { playSFX(EXPLOSION3); }