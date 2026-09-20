#include <iostream>
#include <sstream>
#include "Projectile.h"
#include "Normal.h"
#include "Vector.h"
#include "ChaseCam.h"
#include "Weapon.h"
#include "WeaponDefault.h"
#include "VBOShaderLibrary.h"
#include "macro_crtdbg.h"
#include "GameState.h"

using namespace std;

Projectile::Projectile() {
	
}

Projectile::Projectile(GameState* parent, GLfloat* turretMatrix, GLfloat speed, VBOShaderLibrary** projectileModels) {
	this->defaultWeapon = new WeaponDefault(10);
	this->parent = parent;
	this->scalar = 500;
	pos[0]=turretMatrix[12]-this->scalar*turretMatrix[8];
	pos[1]=turretMatrix[13]-this->scalar*turretMatrix[9];
	pos[2]=turretMatrix[14]-this->scalar*turretMatrix[10];

	this->Xo = pos[0];
	this->Yo = pos[1];
	this->Zo = pos[2];
	
	/*	THE COORD SYSTEM WE USE HAS X AND Z INVERSED X = -X and Z = -Z	*/
	vVec[0]=-turretMatrix[8]*speed;
	vVec[1]=-turretMatrix[9]*speed;
	vVec[2]=-turretMatrix[10]*speed;

	chaseCam=new ChaseCam(pos,vVec);
	weapon = NULL;
	this->projectileModels = projectileModels;

	if(!(VBOShaderLibrary::InitGlew())) {
		exit(1);
	}else if(!(VBOShaderLibrary::AreVBOsSupported())){
		exit(1);
	}
	projectileDefault = new VBOShaderLibrary();
	projectileDefault->getVBOPointerFunctions();
	projectileDefault->loadClientData("Projectiles/projectileDefault.ogl");
	projectileDefault->loadShaders("VertexTank.vs","FragmentTank.vs");
	projectileDefault->LoadTexture("Projectiles/projectileDefault.raw", 512, 512);

	rotate = 4;
	this->timer = 0;
	this->printed = false;
}

Projectile::~Projectile() {
	delete chaseCam;
	delete projectileDefault;
}

//void Projectile::update(GLfloat gravity) {
//	pos[0]+=vVec[0];
//	pos[1]+=vVec[1];
//	pos[2]+=vVec[2];
//	vVec[1]+=gravity;
//}

void Projectile::update(GLfloat X, GLfloat Y, GLfloat Z) {
	pos[0] = X;
	pos[1] = Y;
	pos[2] = Z;
}

void Projectile::draw() {
	glPushMatrix();
		glTranslatef(pos[0],pos[1],pos[2]);
		glRotatef(rotate, -1, .3, -.4);
		if(weapon == NULL){
			glScalef(60, 60, 60);
			projectileDefault->drawClientData();
		}
		else{
			glScalef(weapon->getScale(), weapon->getScale(), weapon->getScale());
			projectileModels[weapon->getUNIQUEIDENTIFIER()]->drawClientData();
		}
	glPopMatrix();
	rotate += 4;
}

GLfloat* Projectile::getPos(){
	return pos;
}

void Projectile::chaseView(){
	this->chaseCam->view();
}

Weapon* Projectile::getWeapon()					{return this->weapon;}
void Projectile::setWeapon(Weapon* wpn)			{this->weapon = wpn;}
Weapon* Projectile::getDefaultWeapon()			{return this->defaultWeapon;}
int Projectile::getDefaultDamage()				{return DEFAULT_DAMAGE;}
int Projectile::getDefaultRadius()				{return DEFAULT_RADIUS;}
ChaseCam* Projectile::getChaseCam()				{return this->chaseCam;}
int Projectile::getRadius()						{return DEFAULT_RADIUS;}
int Projectile::getDamage()						{return DEFAULT_DAMAGE;}
GLfloat Projectile::getInitialPositionScalar()	{return this->scalar;}
GLfloat Projectile::getVox()					{return this->vVec[0];}
GLfloat Projectile::getVoy()					{return this->vVec[1];}
GLfloat Projectile::getVoz()					{return this->vVec[2];}
GLfloat Projectile::getXo()						{return this->Xo;}
GLfloat Projectile::getYo()						{return this->Yo;}
GLfloat Projectile::getZo()						{return this->Zo;}