#include "WeaponNuke.h"
#include "Weapon.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponNuke::WeaponNuke() = default;
WeaponNuke::WeaponNuke(int id) {
    UNIQUEIDENTIFIER = id;
    maxStack = 1;
    packageNum = 1;
    remaining = 1;
    scale = 70;
    imageFileName = "WeaponNuke.raw";
    description = "Nuke:     Do NOT use this weapon!!";
    price = 500;
    radius = 50;
    damage = 800;
    specialNumber = 0;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Silver};
    GLfloat temp_colors3[3] = {Red};
    GLfloat temp_colors4[3] = {Red};
    for (int i = 0; i < 3; i++) {
        explosionColor1[i] = temp_colors1[i];
        explosionColor2[i] = temp_colors2[i];
        explosionColor3[i] = temp_colors3[i];
        explosionColor4[i] = temp_colors4[i];
    }
}
WeaponNuke::~WeaponNuke() = default;

WeaponNuke* WeaponNuke::getWeaponInstance() {
    return new WeaponNuke(UNIQUEIDENTIFIER);
}

void WeaponNuke::playExplosionSFX() { playSFX(EXPLOSION3); }