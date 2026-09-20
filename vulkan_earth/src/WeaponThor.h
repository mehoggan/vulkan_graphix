#ifndef WEAPON_THOR_H
#define WEAPON_THOR_H

#include <GL/freeglut.h>

class WeaponThor : public Weapon{
public:
	WeaponThor();
	WeaponThor(int ID);
	~WeaponThor();
	WeaponThor* getWeaponInstance();
	virtual void causeEffectToTank(GLfloat distance, Tank* tank);
	void playFireSFX();
	void playExplosionSFX();
};

#endif