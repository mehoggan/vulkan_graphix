#include "Weapon.h"
#include "WeaponRevive.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponRevive::WeaponRevive(){}
WeaponRevive::WeaponRevive(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->maxStack = 6;
	this->packageNum = 1;
	this->remaining = 1;
	this->scale = 60;
	this->imageFileName = "WeaponRevive.raw";
	this->description = "Revive:     Revive/repair tanks in the target area";
	this->price = 50;
	this->radius = 6;
	this->damage = 0;
	this->specialNumber = 400;

	GLfloat tempColors1[3] = {White};
	GLfloat tempColors2[3] = {Silver};
	GLfloat tempColors3[3] = {White};
	GLfloat tempColors4[3] = {LightGray};
	for(int i=0 ; i<3 ; i++){
		explosionColor1[i] = tempColors1[i];
		explosionColor2[i] = tempColors2[i];
		explosionColor3[i] = tempColors3[i];
		explosionColor4[i] = tempColors4[i];
	}
}
WeaponRevive::~WeaponRevive(){
}

WeaponRevive* WeaponRevive::getWeaponInstance()		{return new WeaponRevive(this->UNIQUEIDENTIFIER);}
void WeaponRevive::causeEffectToTank(GLfloat distance, Tank* tank){
	if(tank->getDurationShield() == 0){
		tank->setHP(tank->getHP()+specialNumber);
			if(tank->getHP() > tank->getArmor()*100){
			tank->setHP(tank->getArmor()*100);
			tank->tankRevive();
		}
	}
}

void WeaponRevive::playExplosionSFX(){
	playSFX(EXPLOSION_REVIVE);
}