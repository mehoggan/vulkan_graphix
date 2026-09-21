#ifndef WEAPON_ATOM_H
#define WEAPON_ATOM_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Weapon.h"

class WeaponAtom : public Weapon {
public:
    WeaponAtom();
    WeaponAtom(int id);
    ~WeaponAtom() override;
    WeaponAtom* getWeaponInstance() override;
};

#endif