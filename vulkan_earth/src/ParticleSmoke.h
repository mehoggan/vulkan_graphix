#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "SpecialEffect.h"
#include "Particle.h"

#ifndef PARTICLESMOKE_H
#define PARTICLESMOKE_H

class ParticleSmoke : public Particle{
public:
	ParticleSmoke();
	ParticleSmoke(	GLfloat x,GLfloat y, GLfloat z, 
				GLfloat dirX,GLfloat dirY,GLfloat dirZ,
				GLfloat speed,int frames);

	virtual bool update();
};

#endif