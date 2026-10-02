#ifndef WEAPON_ACID_H
#define WEAPON_ACID_H

#include <cstdint>
#include "vulkan_earth/Weapon.h"

class WeaponAcid : public Weapon {
public:
    WeaponAcid();
    WeaponAcid(std::int32_t id);
    ~WeaponAcid() override;
    WeaponAcid* getWeaponInstance() override;
    void causeEffectToTank(float distance, Tank* tank) override;
    void playExplosionSFX() override;
};

#endif