#ifndef VULKAN_EARTH_WEAPONPADLOCK_H
#define VULKAN_EARTH_WEAPONPADLOCK_H

#include <cstdint>
#include "vulkan_earth/Weapon.h"

class WeaponPadlock : public Weapon {
public:
  WeaponPadlock();
  WeaponPadlock(std::int32_t id);
  ~WeaponPadlock() override;
  WeaponPadlock* getWeaponInstance() override;
  void causeEffectToTank(float distance, Tank* tank) override;
};

#endif