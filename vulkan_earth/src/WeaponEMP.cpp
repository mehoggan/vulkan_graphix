#include "WeaponEMP.h"
#include "Weapon.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponEMP::WeaponEMP() = default;
WeaponEMP::WeaponEMP(int ID) {
    UNIQUEIDENTIFIER = ID;
    maxStack = 8;
    packageNum = 2;
    remaining = 2;
    scale = 40;
    imageFileName = "WeaponEMP.raw";
    description = "EMP:     Disrupt tanks in the target area for 5 turns.";
    price = 60;
    radius = 12;
    damage = 0;
    specialNumber = 5;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Silver};
    GLfloat temp_colors3[3] = {White};
    GLfloat temp_colors4[3] = {Silver};
    for (int i = 0; i < 3; i++) {
        explosionColor1[i] = temp_colors1[i];
        explosionColor2[i] = temp_colors2[i];
        explosionColor3[i] = temp_colors3[i];
        explosionColor4[i] = temp_colors4[i];
    }
}
WeaponEMP::~WeaponEMP() = default;

WeaponEMP* WeaponEMP::getWeaponInstance() {
    return new WeaponEMP(UNIQUEIDENTIFIER);
}
void WeaponEMP::causeEffectToTank(GLfloat distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationEMP(specialNumber);
    }
}

void WeaponEMP::playExplosionSFX() { playSFX(ELECTRIC_ZAP2); }