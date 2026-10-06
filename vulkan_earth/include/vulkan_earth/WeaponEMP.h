#ifndef VULKAN_EARTH_WEAPONEMP_H
#define VULKAN_EARTH_WEAPONEMP_H

#include <cstdint>
#include "vulkan_earth/Weapon.h"

class WeaponEMP : public Weapon {
public:
  WeaponEMP();
  WeaponEMP(std::int32_t id);
  ~WeaponEMP() override;
  WeaponEMP* getWeaponInstance() override;
  void causeEffectToTank(float distance, Tank* tank) override;
  void playExplosionSFX() override;
};

#endif