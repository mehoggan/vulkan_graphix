#include "WeaponBFB.h"
#include "Weapon.h"
#include "macro_crtdbg.h"

WeaponBFB::WeaponBFB() = default;
WeaponBFB::WeaponBFB(int ID) {
    this->UNIQUEIDENTIFIER = ID;
    this->remaining = 2;
    this->scale = 100;
    this->imageFileName = "WeaponBFB.raw";
    this->description = "BFB:     (Big Force Bomb) Damage:400";
    this->price = 100;
    this->packageNum = 2;
    this->maxStack = 6;
    this->radius = 30;
    this->damage = 400;
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
WeaponBFB::~WeaponBFB() = default;

WeaponBFB* WeaponBFB::getWeaponInstance() {
    return new WeaponBFB(this->UNIQUEIDENTIFIER);
}
