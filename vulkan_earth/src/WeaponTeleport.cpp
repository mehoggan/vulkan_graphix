#include "vulkan_earth/WeaponTeleport.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

WeaponTeleport::WeaponTeleport() = default;
WeaponTeleport::WeaponTeleport(std::int32_t id) {
  m_uniqueidentifier = id;
  loadSpec(vulkan_graphix::GameCatalog::weapon(
      vulkan_graphix::GameCatalog::WeaponKind::Teleport));
}
WeaponTeleport::~WeaponTeleport() = default;

WeaponTeleport* WeaponTeleport::getWeaponInstance() {
  return new WeaponTeleport(m_uniqueidentifier);
}
void WeaponTeleport::causeEffectToTank(float distance, Tank* tank) {}

void WeaponTeleport::playExplosionSFX() { playSFX(SHIELD); }
