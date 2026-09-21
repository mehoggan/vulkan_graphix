#ifndef WEAPON_MFB_H
#define WEAPON_MFB_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Weapon.h"

class WeaponMFB : public Weapon {
public:
    WeaponMFB();
    WeaponMFB(int id);
    ~WeaponMFB() override;
    WeaponMFB* getWeaponInstance() override;
};

#endif