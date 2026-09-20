#include "WeaponNuke.h"
#include "Weapon.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponNuke::WeaponNuke() = default;
WeaponNuke::WeaponNuke(int ID) {
    this->UNIQUEIDENTIFIER = ID;
    this->maxStack = 1;
    this->packageNum = 1;
    this->remaining = 1;
    this->scale = 70;
    this->imageFileName = "WeaponNuke.raw";
    this->description = "Nuke:     Do NOT use this weapon!!";
    this->price = 500;
    this->radius = 50;
    this->damage = 800;
    this->specialNumber = 0;

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
    return new WeaponNuke(this->UNIQUEIDENTIFIER);
}

void WeaponNuke::playExplosionSFX() { playSFX(EXPLOSION3); }