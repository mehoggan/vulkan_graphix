#include "vulkan_earth/WeaponEMP.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"

extern void playSFX(std::int32_t sfx);

WeaponEMP::WeaponEMP() = default;
WeaponEMP::WeaponEMP(std::int32_t id) {
  m_uniqueidentifier = id;
  loadSpec(
      vulkan_graphix::GameCatalog::weapon(
          vulkan_graphix::GameCatalog::WeaponKind::EMP));
}
WeaponEMP::~WeaponEMP() = default;

WeaponEMP* WeaponEMP::getWeaponInstance() {
  return new WeaponEMP(m_uniqueidentifier);
}
void WeaponEMP::causeEffectToTank(float /*distance*/, Tank* tank) {
  if (tank->getDurationShield() == 0) {
    tank->setDurationEMP(m_special_number);
  }
}

void WeaponEMP::playExplosionSFX() { playSFX(ELECTRIC_ZAP2); }