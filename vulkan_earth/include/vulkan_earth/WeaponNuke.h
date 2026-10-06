#ifndef VULKAN_EARTH_WEAPONNUKE_H
#define VULKAN_EARTH_WEAPONNUKE_H

#include <cstdint>
#include "vulkan_earth/Weapon.h"

class WeaponNuke : public Weapon {
public:
  WeaponNuke();
  WeaponNuke(std::int32_t id);
  ~WeaponNuke() override;
  WeaponNuke* getWeaponInstance() override;
  void playExplosionSFX() override;
};

#endif