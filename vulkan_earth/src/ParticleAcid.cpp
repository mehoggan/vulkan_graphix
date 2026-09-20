#include "ParticleAcid.h"

ParticleAcid::ParticleAcid(){}
ParticleAcid::ParticleAcid(	GLfloat x,GLfloat y, GLfloat z, 
								GLfloat dirX,GLfloat dirY,GLfloat dirZ,
								GLfloat speed,int frames){
	this->x=x;
	this->y=y;
	this->z=z;
	this->dir[0]=dirX;
	this->dir[1]=dirY;
	this->dir[2]=dirZ;
	this->speed=speed;
	this->activeFrames=frames;
	this->currentFrame=0;
	this->size=2;
	this->red=0;
	this->green=1;
	this->blue=0;
}

bool ParticleAcid::update(){
	x+=dir[0]*speed;
	y+=dir[1]*speed;
	z+=dir[2]*speed;
	currentFrame++;
	if(currentFrame==activeFrames) return false;
	return true;
}
