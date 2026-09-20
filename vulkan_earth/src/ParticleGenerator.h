#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "SpecialEffect.h"
#include "Particle.h"
#include "ParticleSmoke.h"
#include "ParticleAcid.h"
#include "ParticleFloat.h"

#ifndef PARTICLEGENERATOR_H
#define PARTICLEGENERATOR_H

class ParticleGenerator {
public:
	ParticleGenerator();
	ParticleGenerator(int spawn,int rate,int speed,int life, int type);

	void update(GLfloat x, GLfloat y, GLfloat z);
	void draw();
	void addParticles();
	void killGenerator();

private:
	GLfloat x,y,z;
	int max,particlesPerEmission,emissionRate,emissionSpeed,emissionLife,type;
	Particle* particleArray[1000];
};

#endif