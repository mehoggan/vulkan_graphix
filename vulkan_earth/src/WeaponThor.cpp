#include "Weapon.h"
#include "WeaponThor.h"
#include "sound.h"
#include "macro_crtdbg.h"

extern void playSFX(int sfx);

WeaponThor::WeaponThor(){}
WeaponThor::WeaponThor(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->maxStack = 6;
	this->packageNum = 1;
	this->remaining = 1;
	this->scale = 70;
	this->imageFileName = "WeaponThor.raw";
	this->description = "Thor:     Damage: 200, Paralyze targets for 1 turn";
	this->price = 80;
	this->radius = 6;
	this->damage = 200;
	this->specialNumber = 1;

	GLfloat tempColors1[3] = {White};
	GLfloat tempColors2[3] = {MediumSlateBlue};
	GLfloat tempColors3[3] = {White};
	GLfloat tempColors4[3] = {MediumBlue};
	for(int i=0 ; i<3 ; i++){
		explosionColor1[i] = tempColors1[i];
		explosionColor2[i] = tempColors2[i];
		explosionColor3[i] = tempColors3[i];
		explosionColor4[i] = tempColors4[i];
	}
}
WeaponThor::~WeaponThor(){
}

WeaponThor* WeaponThor::getWeaponInstance()		{return new WeaponThor(this->UNIQUEIDENTIFIER);}
void WeaponThor::causeEffectToTank(GLfloat distance, Tank* tank)		{
	if(tank->getDurationShield() == 0){
		tank->setDurationParalyze(specialNumber);
		tank->dealDamage(getDamage()*(1-(distance/(getRadius()*100))));
	}
}

void WeaponThor::playFireSFX(){
	playSFX(TANK_FIRE5);
}

void WeaponThor::playExplosionSFX(){
	playSFX(EXPLOSION_THOR);
}