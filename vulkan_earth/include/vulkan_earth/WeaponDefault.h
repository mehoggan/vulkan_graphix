#ifndef WEAPON_DEFAULT_H
#define WEAPON_DEFAULT_H

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