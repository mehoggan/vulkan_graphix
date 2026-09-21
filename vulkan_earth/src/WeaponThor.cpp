#include "WeaponThor.h"
#include "Weapon.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponThor::WeaponThor() = default;
WeaponThor::WeaponThor(int id) {
    uniqueidentifier = id;
    max_stack = 6;
    package_num = 1;
    remaining = 1;
    scale = 70;
    image_file_name = "WeaponThor.raw";
    description = "Thor:     Damage: 200, Paralyze targets for 1 turn";
    price = 80;
    radius = 6;
    damage = 200;
    special_number = 1;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {MediumSlateBlue};
    GLfloat temp_colors3[3] = {White};
    GLfloat temp_colors4[3] = {MediumBlue};
    for (int i = 0; i < 3; i++) {
        explosion_color1[i] = temp_colors1[i];
        explosion_color2[i] = temp_colors2[i];
        explosion_color3[i] = temp_colors3[i];
        explosion_color4[i] = temp_colors4[i];
    }
}
WeaponThor::~WeaponThor() = default;

WeaponThor* WeaponThor::getWeaponInstance() {
    return new WeaponThor(uniqueidentifier);
}
void WeaponThor::causeEffectToTank(GLfloat distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationParalyze(special_number);
        tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
    }
}

void WeaponThor::playFireSFX() { playSFX(TANK_FIRE5); }

void WeaponThor::playExplosionSFX() { playSFX(EXPLOSION_THOR); }