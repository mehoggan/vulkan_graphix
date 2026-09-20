#include "WeaponAtom.h"
#include "Weapon.h"
#include "macro_crtdbg.h"

WeaponAtom::WeaponAtom() = default;
WeaponAtom::WeaponAtom(int ID) {
    this->UNIQUEIDENTIFIER = ID;
    this->maxStack = 5;
    this->packageNum = 1;
    this->remaining = 1;
    this->scale = 30;
    this->imageFileName = "WeaponAtom.raw";
    this->description = "Atom:     Damage: 999, Very small radius.";
    this->price = 200;
    this->radius = 1;
    this->damage = 999;
    this->specialNumber = 0;

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
    return new WeaponAtom(this->UNIQUEIDENTIFIER);
}
