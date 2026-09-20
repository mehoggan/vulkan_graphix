#ifndef WEAPON_NUKE_H
#define WEAPON_NUKE_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Weapon.h"

class WeaponNuke : public Weapon {
public:
    WeaponNuke();
    WeaponNuke(int ID);
    ~WeaponNuke() override;
    WeaponNuke* getWeaponInstance() override;
    void playExplosionSFX() override;
};

#endif