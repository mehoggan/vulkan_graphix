#ifndef WEAPON_REVIVE_H
#define WEAPON_REVIVE_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>
#include "vulkan_earth/Weapon.h"

class WeaponRevive : public Weapon {
public:
    WeaponRevive();
    WeaponRevive(std::int32_t id);
    ~WeaponRevive() override;
    WeaponRevive* getWeaponInstance() override;
    void causeEffectToTank(float distance, Tank* tank) override;
    void playExplosionSFX() override;
};

#endif