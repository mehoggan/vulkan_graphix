#ifndef VULKAN_EARTH_WEAPONMFB_H
#define VULKAN_EARTH_WEAPONMFB_H

#include <cstdint>
#include "vulkan_earth/Weapon.h"

class WeaponMFB : public Weapon {
public:
  WeaponMFB();
  WeaponMFB(std::int32_t id);
  ~WeaponMFB() override;
  WeaponMFB* getWeaponInstance() override;
};

#endif