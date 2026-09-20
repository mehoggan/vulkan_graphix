#include "ParticleSmoke.h"

ParticleSmoke::ParticleSmoke(){}
ParticleSmoke::ParticleSmoke(	GLfloat x,GLfloat y, GLfloat z, 
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
	this->red=1;
	this->green=1;
	this->blue=1;
}

bool ParticleSmoke::update(){
	x+=dir[0]*speed;
	//y+=dir[1]*speed;
	y+=7*((float)currentFrame/(float)activeFrames);
	z+=dir[2]*speed;

	//color test
	if(this->currentFrame<=30){
		this->blue=1-((GLfloat)currentFrame/30.0);
	} else if(this->currentFrame<=60){
		this->green=1-(((GLfloat)currentFrame-30.0)/30.0);
	} else {
		this->red=1-(((GLfloat)currentFrame-30.0)/40.0);
	}

	currentFrame++;
	if(currentFrame==activeFrames) return false;
	return true;
}
