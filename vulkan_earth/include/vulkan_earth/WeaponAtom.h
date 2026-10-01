#ifndef WEAPON_ATOM_H
#define WEAPON_ATOM_H

#include <GL/glew.h>
#include <GL/freeglut.h>
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