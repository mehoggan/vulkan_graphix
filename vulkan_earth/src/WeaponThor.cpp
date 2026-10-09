#include "vulkan_earth/WeaponThor.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"

extern void playSFX(std::int32_t sfx);

WeaponThor::WeaponThor() = default;
WeaponThor::WeaponThor(std::int32_t id) {
  m_uniqueidentifier = id;
  loadSpec(
      vulkan_graphix::GameCatalog::weapon(
          vulkan_graphix::GameCatalog::WeaponKind::Thor));
}
WeaponThor::~WeaponThor() = default;

WeaponThor* WeaponThor::getWeaponInstance() {
  return new WeaponThor(m_uniqueidentifier);
}
void WeaponThor::causeEffectToTank(float distance, Tank* tank) {
  if (tank->getDurationShield() == 0) {
    tank->setDurationParalyze(m_special_number);
    tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
  }
}

void WeaponThor::playFireSFX() { playSFX(TANK_FIRE5); }

void WeaponThor::playExplosionSFX() { playSFX(EXPLOSION_THOR); }