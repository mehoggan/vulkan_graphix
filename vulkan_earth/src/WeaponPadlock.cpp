#include "Weapon.h"
#include "WeaponPadlock.h"
#include "macro_crtdbg.h"


WeaponPadlock::WeaponPadlock(){}
WeaponPadlock::WeaponPadlock(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->maxStack = 8;
	this->packageNum = 2;
	this->remaining = 2;
	this->scale = 60;
	this->imageFileName = "WeaponPadlock.raw";
	this->description = "Padlock:     Damage: 50, Locks target's inventory for 4 turns";
	this->price = 40;
	this->radius = 7;
	this->damage = 50;
	this->specialNumber = 4;

	GLfloat tempColors1[3] = {DimGray};
	GLfloat tempColors2[3] = {Violet};
	GLfloat tempColors3[3] = {DarkSlateBlue};
	GLfloat tempColors4[3] = {DarkPurple};
	for(int i=0 ; i<3 ; i++){
		explosionColor1[i] = tempColors1[i];
		explosionColor2[i] = tempColors2[i];
		explosionColor3[i] = tempColors3[i];
		explosionColor4[i] = tempColors4[i];
	}
}
WeaponPadlock::~WeaponPadlock(){
}

WeaponPadlock* WeaponPadlock::getWeaponInstance()		{return new WeaponPadlock(this->UNIQUEIDENTIFIER);}
void WeaponPadlock::causeEffectToTank(GLfloat distance, Tank* tank){
	if(tank->getDurationShield() == 0){
		tank->setDurationPadlock(specialNumber);
		tank->dealDamage(getDamage()*(1-(distance/(getRadius()*100))));
	}
}