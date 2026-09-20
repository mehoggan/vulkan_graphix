#ifndef WEAPON_TELEPORT_H
#define WEAPON_TELEPORT_H

#include <GL/freeglut.h>

class WeaponTeleport : public Weapon{
public:
	WeaponTeleport();
	WeaponTeleport(int ID);
	~WeaponTeleport();
	WeaponTeleport* getWeaponInstance();
	virtual void causeEffectToTank(GLfloat distance, Tank* tank);
	void playExplosionSFX();
};

#endif