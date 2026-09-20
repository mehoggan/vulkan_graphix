#include "WeaponThor.h"
#include "Weapon.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponThor::WeaponThor() = default;
WeaponThor::WeaponThor(int ID) {
    this->UNIQUEIDENTIFIER = ID;
    this->maxStack = 6;
    this->packageNum = 1;
    this->remaining = 1;
    this->scale = 70;
    this->imageFileName = "WeaponThor.raw";
    this->description = "Thor:     Damage: 200, Paralyze targets for 1 turn";
    this->price = 80;
    this->radius = 6;
    this->damage = 200;
    this->specialNumber = 1;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {MediumSlateBlue};
    GLfloat temp_colors3[3] = {White};
    GLfloat temp_colors4[3] = {MediumBlue};
    for (int i = 0; i < 3; i++) {
        explosionColor1[i] = temp_colors1[i];
        explosionColor2[i] = temp_colors2[i];
        explosionColor3[i] = temp_colors3[i];
        explosionColor4[i] = temp_colors4[i];
    }
}
WeaponThor::~WeaponThor() = default;

WeaponThor* WeaponThor::getWeaponInstance() {
    return new WeaponThor(this->UNIQUEIDENTIFIER);
}
void WeaponThor::causeEffectToTank(GLfloat distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationParalyze(specialNumber);
        tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
    }
}

void WeaponThor::playFireSFX() { playSFX(TANK_FIRE5); }

void WeaponThor::playExplosionSFX() { playSFX(EXPLOSION_THOR); }