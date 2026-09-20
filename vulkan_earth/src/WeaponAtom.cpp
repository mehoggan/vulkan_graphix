#include "WeaponAtom.h"
#include "Weapon.h"
#include "macro_crtdbg.h"

WeaponAtom::WeaponAtom() = default;
WeaponAtom::WeaponAtom(int ID) {
    UNIQUEIDENTIFIER = ID;
    maxStack = 5;
    packageNum = 1;
    remaining = 1;
    scale = 30;
    imageFileName = "WeaponAtom.raw";
    description = "Atom:     Damage: 999, Very small radius.";
    price = 200;
    radius = 1;
    damage = 999;
    specialNumber = 0;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Red};
    GLfloat temp_colors3[3] = {White};
    GLfloat temp_colors4[3] = {Red};
    for (int i = 0; i < 3; i++) {
        explosionColor1[i] = temp_colors1[i];
        explosionColor2[i] = temp_colors2[i];
        explosionColor3[i] = temp_colors3[i];
        explosionColor4[i] = temp_colors4[i];
    }
}
WeaponAtom::~WeaponAtom() = default;

WeaponAtom* WeaponAtom::getWeaponInstance() {
    return new WeaponAtom(UNIQUEIDENTIFIER);
}
