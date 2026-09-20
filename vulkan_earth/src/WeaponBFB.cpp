#include "WeaponBFB.h"
#include "Weapon.h"
#include "macro_crtdbg.h"

WeaponBFB::WeaponBFB() = default;
WeaponBFB::WeaponBFB(int ID) {
    UNIQUEIDENTIFIER = ID;
    remaining = 2;
    scale = 100;
    imageFileName = "WeaponBFB.raw";
    description = "BFB:     (Big Force Bomb) Damage:400";
    price = 100;
    packageNum = 2;
    maxStack = 6;
    radius = 30;
    damage = 400;
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
WeaponBFB::~WeaponBFB() = default;

WeaponBFB* WeaponBFB::getWeaponInstance() {
    return new WeaponBFB(UNIQUEIDENTIFIER);
}
