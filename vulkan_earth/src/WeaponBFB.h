#ifndef WEAPON_BFB_H
#define WEAPON_BFB_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Weapon.h"

class WeaponBFB : public Weapon {
public:
    WeaponBFB();
    WeaponBFB(int id);
    ~WeaponBFB() override;
    WeaponBFB* getWeaponInstance() override;
};

#endif