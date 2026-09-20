#include "WeaponDefault.h"
#include "Weapon.h"
#include "macro_crtdbg.h"

WeaponDefault::WeaponDefault() = default;
WeaponDefault::WeaponDefault(int ID) {
    UNIQUEIDENTIFIER = ID;
    maxStack = 12;
    packageNum = 2;
    remaining = 2;
    scale = 60;
    imageFileName = "TestImage.raw";
    description = "Default     (Default Bomb) Damage:100";
    price = 60;
    radius = 5;
    damage = 100;
    specialNumber = 0;

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
    return new WeaponDefault(UNIQUEIDENTIFIER);
}