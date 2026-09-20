#include "Weapon.h"
#include "WeaponNuke.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

WeaponNuke::WeaponNuke(){}
WeaponNuke::WeaponNuke(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->maxStack = 1;
	this->packageNum = 1;
	this->remaining = 1;
	this->scale = 70;
	this->imageFileName = "WeaponNuke.raw";
	this->description = "Nuke:     Do NOT use this weapon!!";
	this->price = 500;
	this->radius = 50;
	this->damage = 800;
	this->specialNumber = 0;

	GLfloat tempColors1[3] = {White};
	GLfloat tempColors2[3] = {Silver};
	GLfloat tempColors3[3] = {Red};
	GLfloat tempColors4[3] = {Red};
	for(int i=0 ; i<3 ; i++){
		explosionColor1[i] = tempColors1[i];
		explosionColor2[i] = tempColors2[i];
		explosionColor3[i] = tempColors3[i];
		explosionColor4[i] = tempColors4[i];
	}
}
WeaponNuke::~WeaponNuke(){
}

WeaponNuke* WeaponNuke::getWeaponInstance()		{return new WeaponNuke(this->UNIQUEIDENTIFIER);}

void WeaponNuke::playExplosionSFX(){
	playSFX(EXPLOSION3);
}