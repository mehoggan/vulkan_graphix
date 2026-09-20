#include "WeaponAcid.h"
#include "Weapon.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponAcid::WeaponAcid() = default;
WeaponAcid::WeaponAcid(int ID) {
    this->UNIQUEIDENTIFIER = ID;
    this->maxStack = 8;
    this->packageNum = 2;
    this->remaining = 2;
    this->scale = 60;
    this->imageFileName = "WeaponAcid.raw";
    this->description =
            "Acid:     Damage: 150, DOT: 10%% of total HP for 5 turns";
    this->price = 100;
    this->radius = 7;
    this->damage = 150;
    this->specialNumber = 5;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {LimeGreen};
    GLfloat temp_colors3[3] = {PaleGreen};
    GLfloat temp_colors4[3] = {SeaGreen};
    for (int i = 0; i < 3; i++) {
        explosionColor1[i] = temp_colors1[i];
        explosionColor2[i] = temp_colors2[i];
        explosionColor3[i] = temp_colors3[i];
        explosionColor4[i] = temp_colors4[i];
    }
}
WeaponAcid::~WeaponAcid() = default;

WeaponAcid* WeaponAcid::getWeaponInstance() {
    return new WeaponAcid(this->UNIQUEIDENTIFIER);
}
void WeaponAcid::causeEffectToTank(GLfloat distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationAcid(specialNumber);
        tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
    }
}

void WeaponAcid::playExplosionSFX() { playSFX(EXPLOSION_ACID); }