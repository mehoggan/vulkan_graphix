#include "vulkan_earth/WeaponDefault.h"
#include <cstdint>
#include "vulkan_earth/Weapon.h"
#include "vulkan_earth/MacroCrtdbg.h"

WeaponDefault::WeaponDefault() = default;
WeaponDefault::WeaponDefault(std::int32_t id) {
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

    float temp_colors1[3] = {White};
    float temp_colors2[3] = {Yellow};
    float temp_colors3[3] = {Orange};
    float temp_colors4[3] = {Red};
    for (std::int32_t i = 0; i < 3; i++) {
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