#include "WeaponPadlock.h"
#include "Weapon.h"
#include "macro_crtdbg.h"

WeaponPadlock::WeaponPadlock() = default;
WeaponPadlock::WeaponPadlock(int id) {
    UNIQUEIDENTIFIER = id;
    maxStack = 8;
    packageNum = 2;
    remaining = 2;
    scale = 60;
    imageFileName = "WeaponPadlock.raw";
    description =
            "Padlock:     Damage: 50, Locks target's inventory for 4 turns";
    price = 40;
    radius = 7;
    damage = 50;
    specialNumber = 4;

    GLfloat temp_colors1[3] = {DimGray};
    GLfloat temp_colors2[3] = {Violet};
    GLfloat temp_colors3[3] = {DarkSlateBlue};
    GLfloat temp_colors4[3] = {DarkPurple};
    for (int i = 0; i < 3; i++) {
        explosionColor1[i] = temp_colors1[i];
        explosionColor2[i] = temp_colors2[i];
        explosionColor3[i] = temp_colors3[i];
        explosionColor4[i] = temp_colors4[i];
    }
}
WeaponPadlock::~WeaponPadlock() = default;

WeaponPadlock* WeaponPadlock::getWeaponInstance() {
    return new WeaponPadlock(UNIQUEIDENTIFIER);
}
void WeaponPadlock::causeEffectToTank(GLfloat distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationPadlock(specialNumber);
        tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
    }
}