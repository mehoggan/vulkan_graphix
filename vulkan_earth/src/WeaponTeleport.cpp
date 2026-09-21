#include "WeaponTeleport.h"
#include "Weapon.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponTeleport::WeaponTeleport() = default;
WeaponTeleport::WeaponTeleport(int id) {
    UNIQUEIDENTIFIER = id;
    maxStack = 15;
    packageNum = 3;
    remaining = 3;
    scale = 60;
    imageFileName = "WeaponTeleport.raw";
    description = "Teleport:     Teleport to where the projectile lands on.";
    price = 50;
    radius = 0;
    damage = 0;
    specialNumber = 0;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Silver};
    GLfloat temp_colors3[3] = {Silver};
    GLfloat temp_colors4[3] = {Quartz};
    for (int i = 0; i < 3; i++) {
        explosionColor1[i] = temp_colors1[i];
        explosionColor2[i] = temp_colors2[i];
        explosionColor3[i] = temp_colors3[i];
        explosionColor4[i] = temp_colors4[i];
    }
}
WeaponTeleport::~WeaponTeleport() = default;

WeaponTeleport* WeaponTeleport::getWeaponInstance() {
    return new WeaponTeleport(UNIQUEIDENTIFIER);
}
void WeaponTeleport::causeEffectToTank(GLfloat distance, Tank* tank) {}

void WeaponTeleport::playExplosionSFX() { playSFX(SHIELD); }