#include "WeaponAcid.h"
#include "Weapon.h"
#include "MacroCrtdbg.h"
#include "Sound.h"

extern void playSFX(int sfx);

WeaponAcid::WeaponAcid() = default;
WeaponAcid::WeaponAcid(int id) {
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

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {LimeGreen};
    GLfloat temp_colors3[3] = {PaleGreen};
    GLfloat temp_colors4[3] = {SeaGreen};
    for (int i = 0; i < 3; i++) {
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
void WeaponAcid::causeEffectToTank(GLfloat distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationAcid(special_number);
        tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
    }
}

void WeaponAcid::playExplosionSFX() { playSFX(EXPLOSION_ACID); }