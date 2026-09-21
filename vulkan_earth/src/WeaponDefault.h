#ifndef WEAPON_DEFAULT_H
#define WEAPON_DEFAULT_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Weapon.h"

class WeaponDefault : public Weapon {
public:
    WeaponDefault();
    WeaponDefault(int id);
    ~WeaponDefault() override;
    WeaponDefault* getWeaponInstance() override;
};

#endif