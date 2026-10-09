#include "vulkan_earth/WeaponDefault.h"
#include <cstdint>

WeaponDefault::WeaponDefault() = default;
WeaponDefault::WeaponDefault(std::int32_t id) {
  m_uniqueidentifier = id;
  loadSpec(
      vulkan_graphix::GameCatalog::weapon(
          vulkan_graphix::GameCatalog::WeaponKind::Default));
}
WeaponDefault::~WeaponDefault() = default;

WeaponDefault* WeaponDefault::getWeaponInstance() {
  return new WeaponDefault(m_uniqueidentifier);
}