#include "WeaponRevive.h"
#include "Weapon.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponRevive::WeaponRevive() = default;
WeaponRevive::WeaponRevive(int ID) {
    UNIQUEIDENTIFIER = ID;
    maxStack = 6;
    packageNum = 1;
    remaining = 1;
    scale = 60;
    imageFileName = "WeaponRevive.raw";
    description = "Revive:     Revive/repair tanks in the target area";
    price = 50;
    radius = 6;
    damage = 0;
    specialNumber = 400;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Silver};
    GLfloat temp_colors3[3] = {White};
    GLfloat temp_colors4[3] = {LightGray};
    for (int i = 0; i < 3; i++) {
        explosionColor1[i] = temp_colors1[i];
        explosionColor2[i] = temp_colors2[i];
        explosionColor3[i] = temp_colors3[i];
        explosionColor4[i] = temp_colors4[i];
    }
}
WeaponRevive::~WeaponRevive() = default;

WeaponRevive* WeaponRevive::getWeaponInstance() {
    return new WeaponRevive(UNIQUEIDENTIFIER);
}
void WeaponRevive::causeEffectToTank(GLfloat distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setHP(tank->getHP() + specialNumber);
        if (tank->getHP() > tank->getArmor() * 100) {
            tank->setHP(tank->getArmor() * 100);
            tank->tankRevive();
        }
    }
}

void WeaponRevive::playExplosionSFX() { playSFX(EXPLOSION_REVIVE); }