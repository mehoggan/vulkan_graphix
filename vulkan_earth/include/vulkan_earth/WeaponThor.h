#ifndef VULKAN_EARTH_WEAPONTHOR_H
#define VULKAN_EARTH_WEAPONTHOR_H

#include <cstdint>
#include "vulkan_earth/Weapon.h"

class WeaponThor : public Weapon {
public:
  WeaponThor();
  WeaponThor(std::int32_t id);
  ~WeaponThor() override;
  WeaponThor* getWeaponInstance() override;
  void causeEffectToTank(float distance, Tank* tank) override;
  void playFireSFX() override;
  void playExplosionSFX() override;
};

#endif