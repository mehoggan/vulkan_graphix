#include "vulkan_earth/WeaponTeleport.h"
#include <cstdint>
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/Weapon.h"
#include "vulkan_earth/MacroCrtdbg.h"

extern void playSFX(std::int32_t sfx);

WeaponTeleport::WeaponTeleport() = default;
WeaponTeleport::WeaponTeleport(std::int32_t id) {
    uniqueidentifier = id;
    max_stack = 15;
    package_num = 3;
    remaining = 3;
    scale = 60;
    image_file_name = "WeaponTeleport.raw";
    description = "Teleport:     Teleport to where the projectile lands on.";
    price = 50;
    radius = 0;
    damage = 0;
    special_number = 0;

    float temp_colors1[3] = {White};
    float temp_colors2[3] = {Silver};
    float temp_colors3[3] = {Silver};
    float temp_colors4[3] = {Quartz};
    for (std::int32_t i = 0; i < 3; i++) {
        explosion_color1[i] = temp_colors1[i];
        explosion_color2[i] = temp_colors2[i];
        explosion_color3[i] = temp_colors3[i];
        explosion_color4[i] = temp_colors4[i];
    }
}
WeaponTeleport::~WeaponTeleport() = default;

WeaponTeleport* WeaponTeleport::getWeaponInstance() {
    return new WeaponTeleport(uniqueidentifier);
}
void WeaponTeleport::causeEffectToTank(float distance, Tank* tank) {}

void WeaponTeleport::playExplosionSFX() { playSFX(SHIELD); }