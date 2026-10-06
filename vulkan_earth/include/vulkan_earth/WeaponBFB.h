#ifndef VULKAN_EARTH_WEAPONBFB_H
#define VULKAN_EARTH_WEAPONBFB_H

#include <cstdint>
#include "vulkan_earth/Weapon.h"

class WeaponBFB : public Weapon {
public:
  WeaponBFB();
  WeaponBFB(std::int32_t id);
  ~WeaponBFB() override;
  WeaponBFB* getWeaponInstance() override;
};

#endif