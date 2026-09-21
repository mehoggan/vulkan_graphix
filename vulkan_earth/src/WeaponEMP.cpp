#include "WeaponEMP.h"
#include "Weapon.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponEMP::WeaponEMP() = default;
WeaponEMP::WeaponEMP(int id) {
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

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Silver};
    GLfloat temp_colors3[3] = {White};
    GLfloat temp_colors4[3] = {Silver};
    for (int i = 0; i < 3; i++) {
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
void WeaponEMP::causeEffectToTank(GLfloat distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationEMP(special_number);
    }
}

void WeaponEMP::playExplosionSFX() { playSFX(ELECTRIC_ZAP2); }