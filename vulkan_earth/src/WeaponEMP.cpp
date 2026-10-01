#include "vulkan_earth/WeaponEMP.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/Weapon.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

WeaponEMP::WeaponEMP() = default;
WeaponEMP::WeaponEMP(std::int32_t id) {
    uniqueidentifier = id;
    max_stack = 8;
    package_num = 2;
    remaining = 2;
    scale = 40;
    image_file_name = "WeaponEMP.raw";
    description = "EMP:     Disrupt tanks in the target area for 5 turns.";
    price = 60;
    radius = 12;
    damage = 0;
    special_number = 5;

    float temp_colors1[3] = {White};
    float temp_colors2[3] = {Silver};
    float temp_colors3[3] = {White};
    float temp_colors4[3] = {Silver};
    for (std::int32_t i = 0; i < 3; i++) {
        explosion_color1[i] = temp_colors1[i];
        explosion_color2[i] = temp_colors2[i];
        explosion_color3[i] = temp_colors3[i];
        explosion_color4[i] = temp_colors4[i];
    }
}
WeaponEMP::~WeaponEMP() = default;

WeaponEMP* WeaponEMP::getWeaponInstance() {
    return new WeaponEMP(uniqueidentifier);
}
void WeaponEMP::causeEffectToTank(float distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationEMP(special_number);
    }
}

void WeaponEMP::playExplosionSFX() { playSFX(ELECTRIC_ZAP2); }