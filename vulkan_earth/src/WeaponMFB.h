#ifndef WEAPON_MFB_H
#define WEAPON_MFB_H

#include <GL/freeglut.h>

class WeaponMFB : public Weapon{
public:
	WeaponMFB();
	WeaponMFB(int ID);
	~WeaponMFB();
	WeaponMFB* getWeaponInstance();

};

#endif