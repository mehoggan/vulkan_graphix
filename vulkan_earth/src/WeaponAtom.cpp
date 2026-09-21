#include "WeaponAtom.h"
#include "Weapon.h"
#include "macro_crtdbg.h"

WeaponAtom::WeaponAtom() = default;
WeaponAtom::WeaponAtom(int id) {
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

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Red};
    GLfloat temp_colors3[3] = {White};
    GLfloat temp_colors4[3] = {Red};
    for (int i = 0; i < 3; i++) {
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
