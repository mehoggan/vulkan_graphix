#include "vulkan_earth/WeaponAcid.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/Weapon.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

WeaponAcid::WeaponAcid() = default;
WeaponAcid::WeaponAcid(std::int32_t id) {
    uniqueidentifier = id;
    max_stack = 8;
    package_num = 2;
    remaining = 2;
    scale = 60;
    image_file_name = "WeaponAcid.raw";
    description = "Acid:     Damage: 150, DOT: 10%% of total HP for 5 turns";
    price = 100;
    radius = 7;
    damage = 150;
    special_number = 5;

    float temp_colors1[3] = {White};
    float temp_colors2[3] = {LimeGreen};
    float temp_colors3[3] = {PaleGreen};
    float temp_colors4[3] = {SeaGreen};
    for (std::int32_t i = 0; i < 3; i++) {
        explosion_color1[i] = temp_colors1[i];
        explosion_color2[i] = temp_colors2[i];
        explosion_color3[i] = temp_colors3[i];
        explosion_color4[i] = temp_colors4[i];
    }
}
WeaponAcid::~WeaponAcid() = default;

WeaponAcid* WeaponAcid::getWeaponInstance() {
    return new WeaponAcid(uniqueidentifier);
}
void WeaponAcid::causeEffectToTank(float distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationAcid(special_number);
        tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
    }
}

void WeaponAcid::playExplosionSFX() { playSFX(EXPLOSION_ACID); }