#ifndef WEAPON_ACID_H
#define WEAPON_ACID_H

#include <GL/freeglut.h>

class WeaponAcid : public Weapon{
public:
	WeaponAcid();
	WeaponAcid(int ID);
	~WeaponAcid();
	WeaponAcid* getWeaponInstance();
	virtual void causeEffectToTank(GLfloat distance, Tank* tank);
	void playExplosionSFX();
};

#endif