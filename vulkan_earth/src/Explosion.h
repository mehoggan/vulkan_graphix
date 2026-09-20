/*
 * skybox.h
 *
 *  Created on: Sep 10, 2010
 *      Author: Matthew Hoggan
 */
#ifndef EXPLOSION_H_
#define EXPLOSION_H_

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "SpecialEffect.h"

using namespace std;

class Shader;

class Explosion : public SpecialEffect {
public:
	/*
	 * Constructors and De-constructor
	 */
	Explosion();
	Explosion(GLfloat x, GLfloat y, GLfloat z, int weaponRadius);
	~Explosion();
	void draw();
	void setColors1(GLfloat* colors1);
	void setColors2(GLfloat* colors2);
	void setColors3(GLfloat* colors3);
	void setColors4(GLfloat* colors4);
	void setDefaultColors();
private:
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat time;
	GLfloat transMatrix[16];
	Shader* shader;
	GLfloat timer;
	GLfloat radius;
	GLfloat colors1[3];
	GLfloat colors2[3];
	GLfloat colors3[3];
	GLfloat colors4[3];
	int weaponRadius;
};

#endif /* EXPLOSION_H_ */