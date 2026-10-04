#ifndef VULKAN_EARTH_WEAPONDEFAULT_H
#define VULKAN_EARTH_WEAPONDEFAULT_H

#include <cstdint>
#include "vulkan_earth/Weapon.h"

class WeaponDefault : public Weapon {
public:
    WeaponDefault();
    WeaponDefault(std::int32_t id);
    ~WeaponDefault() override;
    WeaponDefault* getWeaponInstance() override;
};

#endif