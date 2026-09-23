#include "WeaponMFB.h"
#include "Weapon.h"
#include "MacroCrtdbg.h"

WeaponMFB::WeaponMFB() = default;
WeaponMFB::WeaponMFB(int id) {
    uniqueidentifier = id;
    max_stack = 12;
    package_num = 2;
    remaining = 2;
    scale = 60;
    image_file_name = "WeaponMFB.raw";
    description = "MFB:     (Medium Force Bomb) Damage:300";
    price = 60;
    radius = 15;
    damage = 300;
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
WeaponMFB::~WeaponMFB() = default;

WeaponMFB* WeaponMFB::getWeaponInstance() {
    return new WeaponMFB(uniqueidentifier);
}
