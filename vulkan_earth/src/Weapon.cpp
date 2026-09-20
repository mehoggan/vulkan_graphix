#include "Weapon.h"
#include "ImageObject.h"
#include "sound.h"
#include <string>
#include "macro_crtdbg.h"

extern void playSFX(int sfx);

using namespace std;

Weapon::Weapon(){}
Weapon::~Weapon(){
}
/*GETTERS*/
int Weapon::getUNIQUEIDENTIFIER()		{return this->UNIQUEIDENTIFIER;}
float Weapon::getScale()							{return this->scale;}
int Weapon::getRemaining()						{return this->remaining;}
const char* Weapon::getImageFileName()	{return this->imageFileName;}
const char* Weapon::getDescription()		{return this->description;}
int Weapon::getPrice()								{return this->price;}
int Weapon::getPackageNum()					{return this->packageNum;}
int Weapon::getMaxStack()						{return this->maxStack;}
float Weapon::getRadius()							{return this->radius;}
int Weapon::getDamage()							{return this->damage;}
GLfloat* Weapon::getExplosionColor1()		{return this->explosionColor1;}
GLfloat* Weapon::getExplosionColor2()		{return this->explosionColor2;}
GLfloat* Weapon::getExplosionColor3()		{return this->explosionColor3;}
GLfloat* Weapon::getExplosionColor4()		{return this->explosionColor4;}
/*SETTERS*/
void Weapon::setScale(float scale)					{this->scale = scale;}
void Weapon::setRemaining(int r)						{this->remaining = r;}
void Weapon::causeEffectToTank(GLfloat distance, Tank *tank){
	if(tank->getDurationShield() == 0){
		tank->dealDamage(getDamage()*(1-(distance/(getRadius()*100))));
	}
}



void Weapon::playFireSFX(){
	int random = rand()%3;

	if(random == 0)				playSFX(TANK_FIRE1);
	else if(random == 1)		playSFX(TANK_FIRE2);
	else								playSFX(TANK_FIRE3);
}

void Weapon::playExplosionSFX(){
	int random = rand()%2;
	Mix_HaltChannel(0);

	if(random == 0)				playSFX(EXPLOSION1);
	else								playSFX(EXPLOSION2);
}
