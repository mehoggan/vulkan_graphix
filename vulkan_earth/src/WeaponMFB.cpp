#include "WeaponMFB.h"
#include "Weapon.h"
#include "macro_crtdbg.h"

WeaponMFB::WeaponMFB() = default;
WeaponMFB::WeaponMFB(int ID) {
    this->UNIQUEIDENTIFIER = ID;
    this->maxStack = 12;
    this->packageNum = 2;
    this->remaining = 2;
    this->scale = 60;
    this->imageFileName = "WeaponMFB.raw";
    this->description = "MFB:     (Medium Force Bomb) Damage:300";
    this->price = 60;
    this->radius = 15;
    this->damage = 300;
    this->specialNumber = 0;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Yellow};
    GLfloat temp_colors3[3] = {Orange};
    GLfloat temp_colors4[3] = {Red};
    for (int i = 0; i < 3; i++) {
        explosionColor1[i] = temp_colors1[i];
        explosionColor2[i] = temp_colors2[i];
        explosionColor3[i] = temp_colors3[i];
        explosionColor4[i] = temp_colors4[i];
    }
}
WeaponMFB::~WeaponMFB() = default;

WeaponMFB* WeaponMFB::getWeaponInstance() {
    return new WeaponMFB(this->UNIQUEIDENTIFIER);
}
