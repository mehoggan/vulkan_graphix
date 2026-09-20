/*
 * Explosion.cpp
 *
 *  Created on: Sep 16, 2010
 *      Author: Matthew Hoggan
 */

#include "Explosion.h"
#include "Shader.h"
#include "Vector.h"
#include "OpenGLColors.h"

/*
 * Constructors and De-constructors
 */
Explosion::Explosion(){
}
Explosion::Explosion(GLfloat x, GLfloat y, GLfloat z, int weaponRadius){
	glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, this->transMatrix);
	glPopMatrix();
	this->x = x;
	this->y = y;
	this->z = z;
	this->transMatrix[12]=this->x;
	this->transMatrix[13]=this->y;
	this->transMatrix[14]=this->z;
	this->weaponRadius = weaponRadius;

	this->shader=new Shader();
	this->shader->init("VertexExplosion.vs","FragmentExplosion.vs");

	this->timer = 50.0f;
	this->radius = 0.0f;

	GLfloat tempColors1[3] = {White};
	GLfloat tempColors2[3] = {Yellow};
	GLfloat tempColors3[3] = {Orange};
	GLfloat tempColors4[3] = {Red};
	for(int i=0 ; i<3 ; i++){
		colors1[i] = tempColors1[i];
		colors2[i] = tempColors2[i];
		colors3[i] = tempColors3[i];
		colors4[i] = tempColors4[i];
	}
}

Explosion::~Explosion(){
	delete this->shader;
}

void Explosion::draw(){
	this->shader->bind();
	this->radius += .1;
	this->timer+=.5;
	//Please See OpenGLColors.h for definitions of colors
	if(timer > 0 && timer < 25) {
		glColor4f(colors1[0], colors1[1], colors1[2],1.0-timer/150.0);
	}else if(timer >= 25 && timer < 50) {
		glColor4f(colors2[0], colors2[1], colors2[2],1.0-timer/150.0);
	}else if(timer >=50 && timer < 75) {
		glColor4f(colors3[0], colors3[1], colors3[2],1.0-timer/150.0);
	}else if(timer >= 100) {
		glColor4f(colors4[0], colors4[1], colors4[2],1.0-timer/150.0);
	}

	glPushMatrix();
		glTranslatef(this->x,this->y,this->z);
		//ASSUMING SCALE ON TERRAIN IS 150 I NEED TO GET ACTUAL VALUE
		glutSolidSphere(this->radius*(weaponRadius*5.56+22.22), 90, 180);
	glPopMatrix();
	this->shader->unbind();
}


void Explosion::setColors1(GLfloat* colors1){
	this->colors1[0] = colors1[0];
	this->colors1[1] = colors1[1];
	this->colors1[2] = colors1[2];
}
void Explosion::setColors2(GLfloat* colors2){
	this->colors2[0] = colors2[0];
	this->colors2[1] = colors2[1];
	this->colors2[2] = colors2[2];
}
void Explosion::setColors3(GLfloat* colors3){
	this->colors3[0] = colors3[0];
	this->colors3[1] = colors3[1];
	this->colors3[2] = colors3[2];
}
void Explosion::setColors4(GLfloat* colors4){
	this->colors4[0] = colors4[0];
	this->colors4[1] = colors4[1];
	this->colors4[2] = colors4[2];
}
void Explosion::setDefaultColors(){
	GLfloat tempColors1[3] = {White};
	GLfloat tempColors2[3] = {Yellow};
	GLfloat tempColors3[3] = {Orange};
	GLfloat tempColors4[3] = {Red};
	for(int i=0 ; i<3 ; i++){
		colors1[i] = tempColors1[i];
		colors2[i] = tempColors2[i];
		colors3[i] = tempColors3[i];
		colors4[i] = tempColors4[i];
	}
}