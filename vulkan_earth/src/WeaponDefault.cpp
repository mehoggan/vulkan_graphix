#include "WeaponDefault.h"
#include "Weapon.h"
#include "macro_crtdbg.h"

WeaponDefault::WeaponDefault() = default;
WeaponDefault::WeaponDefault(int ID) {
    this->UNIQUEIDENTIFIER = ID;
    this->maxStack = 12;
    this->packageNum = 2;
    this->remaining = 2;
    this->scale = 60;
    this->imageFileName = "TestImage.raw";
    this->description = "Default     (Default Bomb) Damage:100";
    this->price = 60;
    this->radius = 5;
    this->damage = 100;
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
WeaponDefault::~WeaponDefault() = default;

WeaponDefault* WeaponDefault::getWeaponInstance() {
    return new WeaponDefault(this->UNIQUEIDENTIFIER);
}