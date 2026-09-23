#include "WeaponBFB.h"
#include "Weapon.h"
#include "MacroCrtdbg.h"

WeaponBFB::WeaponBFB() = default;
WeaponBFB::WeaponBFB(int id) {
    uniqueidentifier = id;
    remaining = 2;
    scale = 100;
    image_file_name = "WeaponBFB.raw";
    description = "BFB:     (Big Force Bomb) Damage:400";
    price = 100;
    package_num = 2;
    max_stack = 6;
    radius = 30;
    damage = 400;
    special_number = 0;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Yellow};
    GLfloat temp_colors3[3] = {Orange};
    GLfloat temp_colors4[3] = {Red};
    for (int i = 0; i < 3; i++) {
        explosion_color1[i] = temp_colors1[i];
        explosion_color2[i] = temp_colors2[i];
        explosion_color3[i] = temp_colors3[i];
        explosion_color4[i] = temp_colors4[i];
    }
}
WeaponBFB::~WeaponBFB() = default;

WeaponBFB* WeaponBFB::getWeaponInstance() {
    return new WeaponBFB(uniqueidentifier);
}
