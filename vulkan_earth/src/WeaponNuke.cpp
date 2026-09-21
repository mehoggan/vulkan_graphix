#include "WeaponNuke.h"
#include "Weapon.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponNuke::WeaponNuke() = default;
WeaponNuke::WeaponNuke(int id) {
    uniqueidentifier = id;
    max_stack = 1;
    package_num = 1;
    remaining = 1;
    scale = 70;
    image_file_name = "WeaponNuke.raw";
    description = "Nuke:     Do NOT use this weapon!!";
    price = 500;
    radius = 50;
    damage = 800;
    special_number = 0;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Silver};
    GLfloat temp_colors3[3] = {Red};
    GLfloat temp_colors4[3] = {Red};
    for (int i = 0; i < 3; i++) {
        explosion_color1[i] = temp_colors1[i];
        explosion_color2[i] = temp_colors2[i];
        explosion_color3[i] = temp_colors3[i];
        explosion_color4[i] = temp_colors4[i];
    }
}
WeaponNuke::~WeaponNuke() = default;

WeaponNuke* WeaponNuke::getWeaponInstance() {
    return new WeaponNuke(uniqueidentifier);
}

void WeaponNuke::playExplosionSFX() { playSFX(EXPLOSION3); }