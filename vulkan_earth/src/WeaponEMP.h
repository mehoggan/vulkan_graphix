#ifndef WEAPON_EMP_H
#define WEAPON_EMP_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Weapon.h"

class WeaponEMP : public Weapon {
public:
    WeaponEMP();
    WeaponEMP(int ID);
    ~WeaponEMP() override;
    WeaponEMP* getWeaponInstance() override;
    void causeEffectToTank(GLfloat distance, Tank* tank) override;
    void playExplosionSFX() override;
};

#endif