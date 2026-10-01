#ifndef WEAPON_DEFAULT_H
#define WEAPON_DEFAULT_H

#include <GL/glew.h>
#include <GL/freeglut.h>
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