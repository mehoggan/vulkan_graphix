#include "WeaponTeleport.h"
#include "Weapon.h"
#include "MacroCrtdbg.h"
#include "Sound.h"

extern void playSFX(int sfx);

WeaponTeleport::WeaponTeleport() = default;
WeaponTeleport::WeaponTeleport(int id) {
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

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Silver};
    GLfloat temp_colors3[3] = {Silver};
    GLfloat temp_colors4[3] = {Quartz};
    for (int i = 0; i < 3; i++) {
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
void WeaponTeleport::causeEffectToTank(GLfloat distance, Tank* tank) {}

void WeaponTeleport::playExplosionSFX() { playSFX(SHIELD); }