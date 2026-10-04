#include "vulkan_earth/WeaponEMP.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

WeaponEMP::WeaponEMP() = default;
WeaponEMP::WeaponEMP(std::int32_t id) {
    uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::weapon(
            vulkan_graphix::GameCatalog::WeaponKind::EMP));
}
WeaponEMP::~WeaponEMP() = default;

WeaponEMP* WeaponEMP::getWeaponInstance() {
    return new WeaponEMP(uniqueidentifier);
}
void WeaponEMP::causeEffectToTank(float /*distance*/, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationEMP(special_number);
    }
}

void WeaponEMP::playExplosionSFX() { playSFX(ELECTRIC_ZAP2); }