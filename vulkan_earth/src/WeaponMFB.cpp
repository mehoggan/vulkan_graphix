#include "Weapon.h"
#include "WeaponMFB.h"
#include "macro_crtdbg.h"


WeaponMFB::WeaponMFB(){}
WeaponMFB::WeaponMFB(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->maxStack = 12;
	this->packageNum = 2;
	this->remaining = 2;
	this->scale = 60;
	this->imageFileName = "WeaponMFB.raw";
	this->description = "MFB:     (Medium Force Bomb) Damage:300";
	this->price = 60;
	this->radius = 15;
	this->damage = 300;
	this->specialNumber = 0;

	GLfloat tempColors1[3] = {White};
	GLfloat tempColors2[3] = {Yellow};
	GLfloat tempColors3[3] = {Orange};
	GLfloat tempColors4[3] = {Red};
	for(int i=0 ; i<3 ; i++){
		explosionColor1[i] = tempColors1[i];
		explosionColor2[i] = tempColors2[i];
		explosionColor3[i] = tempColors3[i];
		explosionColor4[i] = tempColors4[i];
	}
}
WeaponMFB::~WeaponMFB(){
}

WeaponMFB* WeaponMFB::getWeaponInstance()		{return new WeaponMFB(this->UNIQUEIDENTIFIER);}
