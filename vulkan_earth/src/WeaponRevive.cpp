#include "WeaponRevive.h"
#include "Weapon.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponRevive::WeaponRevive() = default;
WeaponRevive::WeaponRevive(int id) {
    uniqueidentifier = id;
    max_stack = 6;
    package_num = 1;
    remaining = 1;
    scale = 60;
    image_file_name = "WeaponRevive.raw";
    description = "Revive:     Revive/repair tanks in the target area";
    price = 50;
    radius = 6;
    damage = 0;
    special_number = 400;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Silver};
    GLfloat temp_colors3[3] = {White};
    GLfloat temp_colors4[3] = {LightGray};
    for (int i = 0; i < 3; i++) {
        explosion_color1[i] = temp_colors1[i];
        explosion_color2[i] = temp_colors2[i];
        explosion_color3[i] = temp_colors3[i];
        explosion_color4[i] = temp_colors4[i];
    }
}
WeaponRevive::~WeaponRevive() = default;

WeaponRevive* WeaponRevive::getWeaponInstance() {
    return new WeaponRevive(uniqueidentifier);
}
void WeaponRevive::causeEffectToTank(GLfloat distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setHP(tank->getHP() + special_number);
        if (tank->getHP() > tank->getArmor() * 100) {
            tank->setHP(tank->getArmor() * 100);
            tank->tankRevive();
        }
    }
}

void WeaponRevive::playExplosionSFX() { playSFX(EXPLOSION_REVIVE); }