#include "vulkan_earth/WeaponAtom.h"
#include <cstdint>
#include "vulkan_earth/MacroCrtdbg.h"

WeaponAtom::WeaponAtom() = default;
WeaponAtom::WeaponAtom(std::int32_t id) {
    uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::weapon(
            vulkan_graphix::GameCatalog::WeaponKind::Atom));
}
WeaponAtom::~WeaponAtom() = default;

WeaponAtom* WeaponAtom::getWeaponInstance() {
    return new WeaponAtom(uniqueidentifier);
}
