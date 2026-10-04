#ifndef VULKAN_EARTH_WEAPONATOM_H
#define VULKAN_EARTH_WEAPONATOM_H

#include <cstdint>
#include "vulkan_earth/Weapon.h"

class WeaponAtom : public Weapon {
public:
    WeaponAtom();
    WeaponAtom(std::int32_t id);
    ~WeaponAtom() override;
    WeaponAtom* getWeaponInstance() override;
};

#endif