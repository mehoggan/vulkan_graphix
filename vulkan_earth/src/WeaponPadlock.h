#ifndef WEAPON_PADLOCK_H
#define WEAPON_PADLOCK_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Weapon.h"

class WeaponPadlock : public Weapon {
public:
    WeaponPadlock();
    WeaponPadlock(int id);
    ~WeaponPadlock() override;
    WeaponPadlock* getWeaponInstance() override;
    void causeEffectToTank(GLfloat distance, Tank* tank) override;
};

#endif