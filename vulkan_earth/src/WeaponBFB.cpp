#include "Weapon.h"
#include "WeaponBFB.h"
#include "macro_crtdbg.h"

WeaponBFB::WeaponBFB(){}
WeaponBFB::WeaponBFB(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->remaining = 2;
	this->scale = 100;
	this->imageFileName = "WeaponBFB.raw";
	this->description = "BFB:     (Big Force Bomb) Damage:400";
	this->price = 100;
	this->packageNum = 2;
	this->maxStack = 6;
	this->radius = 30;
	this->damage = 400;
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
WeaponBFB::~WeaponBFB(){
}

WeaponBFB* WeaponBFB::getWeaponInstance()		{return new WeaponBFB(this->UNIQUEIDENTIFIER);}
