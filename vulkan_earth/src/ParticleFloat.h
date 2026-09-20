#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "SpecialEffect.h"
#include "Particle.h"

#ifndef PARTICLEFLOAT_H
#define PARTICLEFLOAT_H

class ParticleFloat : public Particle{
public:
	ParticleFloat();
	ParticleFloat(	GLfloat x,GLfloat y, GLfloat z, 
				GLfloat dirX,GLfloat dirY,GLfloat dirZ,
				GLfloat speed,int frames);

	virtual bool update();
};

#endif