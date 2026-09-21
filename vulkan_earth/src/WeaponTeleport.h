#ifndef WEAPON_TELEPORT_H
#define WEAPON_TELEPORT_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Weapon.h"

class WeaponTeleport : public Weapon {
public:
    WeaponTeleport();
    WeaponTeleport(int id);
    ~WeaponTeleport() override;
    WeaponTeleport* getWeaponInstance() override;
    void causeEffectToTank(GLfloat distance, Tank* tank) override;
    void playExplosionSFX() override;
};

#endif