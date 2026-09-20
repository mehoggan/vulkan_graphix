#ifndef WEAPON_ACID_H
#define WEAPON_ACID_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Weapon.h"

class WeaponAcid : public Weapon {
public:
    WeaponAcid();
    WeaponAcid(int ID);
    ~WeaponAcid() override;
    WeaponAcid* getWeaponInstance() override;
    void causeEffectToTank(GLfloat distance, Tank* tank) override;
    void playExplosionSFX() override;
};

#endif