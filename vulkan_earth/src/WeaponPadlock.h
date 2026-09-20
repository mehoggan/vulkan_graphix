#ifndef WEAPON_PADLOCK_H
#define WEAPON_PADLOCK_H

#include <GL/freeglut.h>

class WeaponPadlock : public Weapon{
public:
	WeaponPadlock();
	WeaponPadlock(int ID);
	~WeaponPadlock();
	WeaponPadlock* getWeaponInstance();
	virtual void causeEffectToTank(GLfloat distance, Tank* tank);
};

#endif