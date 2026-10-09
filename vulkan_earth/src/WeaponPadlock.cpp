#include "vulkan_earth/WeaponPadlock.h"
#include <cstdint>

WeaponPadlock::WeaponPadlock() = default;
WeaponPadlock::WeaponPadlock(std::int32_t id) {
  m_uniqueidentifier = id;
  loadSpec(
      vulkan_graphix::GameCatalog::weapon(
          vulkan_graphix::GameCatalog::WeaponKind::Padlock));
}
WeaponPadlock::~WeaponPadlock() = default;

WeaponPadlock* WeaponPadlock::getWeaponInstance() {
  return new WeaponPadlock(m_uniqueidentifier);
}
void WeaponPadlock::causeEffectToTank(float distance, Tank* tank) {
  if (tank->getDurationShield() == 0) {
    tank->setDurationPadlock(m_special_number);
    tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
  }
}