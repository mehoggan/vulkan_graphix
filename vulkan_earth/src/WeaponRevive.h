#ifndef WEAPON_REVIVE_H
#define WEAPON_REVIVE_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Weapon.h"

class WeaponRevive : public Weapon {
public:
    WeaponRevive();
    WeaponRevive(int ID);
    ~WeaponRevive() override;
    WeaponRevive* getWeaponInstance() override;
    void causeEffectToTank(GLfloat distance, Tank* tank) override;
    void playExplosionSFX() override;
};

#endif