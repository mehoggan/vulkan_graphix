#include "vulkan_earth/WeaponAtom.h"
#include <cstdint>
#include "vulkan_earth/Weapon.h"
#include "vulkan_earth/MacroCrtdbg.h"

WeaponAtom::WeaponAtom() = default;
WeaponAtom::WeaponAtom(std::int32_t id) {
    uniqueidentifier = id;
    max_stack = 5;
    package_num = 1;
    remaining = 1;
    scale = 30;
    image_file_name = "WeaponAtom.raw";
    description = "Atom:     Damage: 999, Very small radius.";
    price = 200;
    radius = 1;
    damage = 999;
    special_number = 0;

    float temp_colors1[3] = {White};
    float temp_colors2[3] = {Red};
    float temp_colors3[3] = {White};
    float temp_colors4[3] = {Red};
    for (std::int32_t i = 0; i < 3; i++) {
        explosion_color1[i] = temp_colors1[i];
        explosion_color2[i] = temp_colors2[i];
        explosion_color3[i] = temp_colors3[i];
        explosion_color4[i] = temp_colors4[i];
    }
}
WeaponAtom::~WeaponAtom() = default;

WeaponAtom* WeaponAtom::getWeaponInstance() {
    return new WeaponAtom(uniqueidentifier);
}
