#include "vulkan_earth/WeaponPadlock.h"
#include <cstdint>
#include "vulkan_earth/Weapon.h"
#include "vulkan_earth/MacroCrtdbg.h"

WeaponPadlock::WeaponPadlock() = default;
WeaponPadlock::WeaponPadlock(std::int32_t id) {
    uniqueidentifier = id;
    max_stack = 8;
    package_num = 2;
    remaining = 2;
    scale = 60;
    image_file_name = "WeaponPadlock.raw";
    description =
            "Padlock:     Damage: 50, Locks target's inventory for 4 turns";
    price = 40;
    radius = 7;
    damage = 50;
    special_number = 4;

    float temp_colors1[3] = {DimGray};
    float temp_colors2[3] = {Violet};
    float temp_colors3[3] = {DarkSlateBlue};
    float temp_colors4[3] = {DarkPurple};
    for (std::int32_t i = 0; i < 3; i++) {
        explosion_color1[i] = temp_colors1[i];
        explosion_color2[i] = temp_colors2[i];
        explosion_color3[i] = temp_colors3[i];
        explosion_color4[i] = temp_colors4[i];
    }
}
WeaponPadlock::~WeaponPadlock() = default;

WeaponPadlock* WeaponPadlock::getWeaponInstance() {
    return new WeaponPadlock(uniqueidentifier);
}
void WeaponPadlock::causeEffectToTank(float distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationPadlock(special_number);
        tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
    }
}