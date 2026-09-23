#include "WeaponPadlock.h"
#include "Weapon.h"
#include "MacroCrtdbg.h"

WeaponPadlock::WeaponPadlock() = default;
WeaponPadlock::WeaponPadlock(int id) {
    uniqueidentifier = id;
    max_stack = 8;
    package_num = 2;
    remaining = 2;
    scale = 60;
    image_file_name = "WeaponPadlock.raw";
    description =
            "Padlock:     Damage: 50, Locks target's inventory for 4 turns";
    price = 40;
    radius = 7;
    damage = 50;
    special_number = 4;

    GLfloat temp_colors1[3] = {DimGray};
    GLfloat temp_colors2[3] = {Violet};
    GLfloat temp_colors3[3] = {DarkSlateBlue};
    GLfloat temp_colors4[3] = {DarkPurple};
    for (int i = 0; i < 3; i++) {
        explosion_color1[i] = temp_colors1[i];
        explosion_color2[i] = temp_colors2[i];
        explosion_color3[i] = temp_colors3[i];
        explosion_color4[i] = temp_colors4[i];
    }
}
WeaponPadlock::~WeaponPadlock() = default;

WeaponPadlock* WeaponPadlock::getWeaponInstance() {
    return new WeaponPadlock(uniqueidentifier);
}
void WeaponPadlock::causeEffectToTank(GLfloat distance, Tank* tank) {
    if (tank->getDurationShield() == 0) {
        tank->setDurationPadlock(special_number);
        tank->dealDamage(getDamage() * (1 - (distance / (getRadius() * 100))));
    }
}