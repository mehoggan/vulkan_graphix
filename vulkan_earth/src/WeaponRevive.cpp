#include "vulkan_earth/WeaponRevive.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

WeaponRevive::WeaponRevive() = default;
WeaponRevive::WeaponRevive(std::int32_t id) {
    m_uniqueidentifier = id;
    loadSpec(vulkan_graphix::GameCatalog::weapon(
      vulkan_graphix::GameCatalog::WeaponKind::Revive));
}
WeaponRevive::~WeaponRevive() = default;

WeaponRevive* WeaponRevive::getWeaponInstance() {
    return new WeaponRevive(m_uniqueidentifier);
}
void WeaponRevive::causeEffectToTank(float /*distance*/, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setHP(tank->getHP() + m_special_number);
        if (tank->getHP() > tank->getArmor() * 100) {
            tank->setHP(tank->getArmor() * 100);
            tank->tankRevive();
        }
    }
}

void WeaponRevive::playExplosionSFX() { playSFX(EXPLOSION_REVIVE); }