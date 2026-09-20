#include "Weapon.h"
#include "WeaponTeleport.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponTeleport::WeaponTeleport(){}
WeaponTeleport::WeaponTeleport(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->maxStack = 15;
	this->packageNum = 3;
	this->remaining = 3;
	this->scale = 60;
	this->imageFileName = "WeaponTeleport.raw";
	this->description = "Teleport:     Teleport to where the projectile lands on.";
	this->price = 50;
	this->radius = 0;
	this->damage = 0;
	this->specialNumber = 0;

	GLfloat tempColors1[3] = {White};
	GLfloat tempColors2[3] = {Silver};
	GLfloat tempColors3[3] = {Silver};
	GLfloat tempColors4[3] = {Quartz};
	for(int i=0 ; i<3 ; i++){
		explosionColor1[i] = tempColors1[i];
		explosionColor2[i] = tempColors2[i];
		explosionColor3[i] = tempColors3[i];
		explosionColor4[i] = tempColors4[i];
	}
}
WeaponTeleport::~WeaponTeleport(){
}

WeaponTeleport* WeaponTeleport::getWeaponInstance()		{return new WeaponTeleport(this->UNIQUEIDENTIFIER);}
void WeaponTeleport::causeEffectToTank(GLfloat distance, Tank* tank)		{}

void WeaponTeleport::playExplosionSFX(){
	playSFX(SHIELD);
}