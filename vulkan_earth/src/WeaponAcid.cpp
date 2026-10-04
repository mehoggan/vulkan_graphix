#include "vulkan_earth/WeaponAcid.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

WeaponAcid::WeaponAcid() = default;
WeaponAcid::WeaponAcid(std::int32_t id) {
    m_uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::weapon(
            vulkan_graphix::GameCatalog::WeaponKind::Acid));
}
WeaponAcid::~WeaponAcid() = default;

WeaponAcid* WeaponAcid::getWeaponInstance() {
    return new WeaponAcid(m_uniqueidentifier);
}
void WeaponAcid::causeEffectToTank(float distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationAcid(m_special_number);
        tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
    }
}

void WeaponAcid::playExplosionSFX() { playSFX(EXPLOSION_ACID); }