#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "SpecialEffect.h"
#include "Particle.h"

#ifndef PARTICLEACID_H
#define PARTICLEACID_H

class ParticleAcid : public Particle{
public:
	ParticleAcid();
	ParticleAcid(	GLfloat x,GLfloat y, GLfloat z, 
				GLfloat dirX,GLfloat dirY,GLfloat dirZ,
				GLfloat speed,int frames);

	virtual bool update();
};

#endif