#ifndef WEAPON_TELEPORT_H
#define WEAPON_TELEPORT_H

#include <cstdint>
#include "vulkan_earth/Weapon.h"

class WeaponTeleport : public Weapon {
public:
    WeaponTeleport();
    WeaponTeleport(std::int32_t id);
    ~WeaponTeleport() override;
    WeaponTeleport* getWeaponInstance() override;
    void causeEffectToTank(float distance, Tank* tank) override;
    void playExplosionSFX() override;
};

#endif