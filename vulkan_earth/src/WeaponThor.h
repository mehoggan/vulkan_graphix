#ifndef WEAPON_THOR_H
#define WEAPON_THOR_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Weapon.h"

class WeaponThor : public Weapon {
public:
    WeaponThor();
    WeaponThor(int id);
    ~WeaponThor() override;
    WeaponThor* getWeaponInstance() override;
    void causeEffectToTank(GLfloat distance, Tank* tank) override;
    void playFireSFX() override;
    void playExplosionSFX() override;
};

#endif