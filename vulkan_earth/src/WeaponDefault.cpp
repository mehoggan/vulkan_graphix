#include "WeaponDefault.h"
#include "Weapon.h"
#include "MacroCrtdbg.h"

WeaponDefault::WeaponDefault() = default;
WeaponDefault::WeaponDefault(int id) {
    uniqueidentifier = id;
    max_stack = 12;
    package_num = 2;
    remaining = 2;
    scale = 60;
    image_file_name = "TestImage.raw";
    description = "Default     (Default Bomb) Damage:100";
    price = 60;
    radius = 5;
    damage = 100;
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
WeaponDefault::~WeaponDefault() = default;

WeaponDefault* WeaponDefault::getWeaponInstance() {
    return new WeaponDefault(uniqueidentifier);
}