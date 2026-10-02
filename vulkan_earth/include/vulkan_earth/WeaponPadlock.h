#ifndef WEAPON_PADLOCK_H
#define WEAPON_PADLOCK_H

#include <cstdint>
#include "vulkan_earth/Weapon.h"

class WeaponPadlock : public Weapon {
public:
    WeaponPadlock();
    WeaponPadlock(std::int32_t id);
    ~WeaponPadlock() override;
    WeaponPadlock* getWeaponInstance() override;
    void causeEffectToTank(float distance, Tank* tank) override;
};

#endif