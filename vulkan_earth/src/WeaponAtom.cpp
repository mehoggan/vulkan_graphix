#include "Weapon.h"
#include "WeaponAtom.h"
#include "macro_crtdbg.h"


WeaponAtom::WeaponAtom(){}
WeaponAtom::WeaponAtom(int ID){
	this->UNIQUEIDENTIFIER = ID;
	this->maxStack = 5;
	this->packageNum = 1;
	this->remaining = 1;
	this->scale = 30;
	this->imageFileName = "WeaponAtom.raw";
	this->description = "Atom:     Damage: 999, Very small radius.";
	this->price = 200;
	this->radius = 1;
	this->damage = 999;
	this->specialNumber = 0;

	GLfloat tempColors1[3] = {White};
	GLfloat tempColors2[3] = {Red};
	GLfloat tempColors3[3] = {White};
	GLfloat tempColors4[3] = {Red};
	for(int i=0 ; i<3 ; i++){
		explosionColor1[i] = tempColors1[i];
		explosionColor2[i] = tempColors2[i];
		explosionColor3[i] = tempColors3[i];
		explosionColor4[i] = tempColors4[i];
	}
}
WeaponAtom::~WeaponAtom(){
}

WeaponAtom* WeaponAtom::getWeaponInstance()		{return new WeaponAtom(this->UNIQUEIDENTIFIER);}
