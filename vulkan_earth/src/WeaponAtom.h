#ifndef WEAPON_ATOM_H
#define WEAPON_ATOM_H

#include <GL/freeglut.h>

class WeaponAtom : public Weapon{
public:
	WeaponAtom();
	WeaponAtom(int ID);
	~WeaponAtom();
	WeaponAtom* getWeaponInstance();
};

#endif