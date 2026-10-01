#ifndef WEAPON_EMP_H
#define WEAPON_EMP_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "vulkan_earth/Weapon.h"

class WeaponEMP : public Weapon {
public:
    WeaponEMP();
    WeaponEMP(int id);
    ~WeaponEMP() override;
    WeaponEMP* getWeaponInstance() override;
    void causeEffectToTank(float distance, Tank* tank) override;
    void playExplosionSFX() override;
};

#endif