#include "vulkan_earth/WeaponMFB.h"
#include <cstdint>

WeaponMFB::WeaponMFB() = default;
WeaponMFB::WeaponMFB(std::int32_t id) {
  m_uniqueidentifier = id;
  loadSpec(
      vulkan_graphix::GameCatalog::weapon(
          vulkan_graphix::GameCatalog::WeaponKind::MFB));
}
WeaponMFB::~WeaponMFB() = default;

WeaponMFB* WeaponMFB::getWeaponInstance() {
  return new WeaponMFB(m_uniqueidentifier);
}
