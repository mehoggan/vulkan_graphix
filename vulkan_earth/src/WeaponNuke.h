#ifndef WEAPON_NUKE_H
#define WEAPON_NUKE_H

#include <GL/freeglut.h>

class WeaponNuke : public Weapon{
public:
	WeaponNuke();
	WeaponNuke(int ID);
	~WeaponNuke();
	WeaponNuke* getWeaponInstance();
	void playExplosionSFX();
};

#endif