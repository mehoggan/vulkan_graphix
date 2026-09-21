#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include "Particle.h"
#include "ParticleAcid.h"
#include "ParticleFloat.h"
#include "ParticleSmoke.h"
#include "SpecialEffect.h"

#ifndef PARTICLEGENERATOR_H
#define PARTICLEGENERATOR_H

class ParticleGenerator {
public:
    ParticleGenerator();
    ParticleGenerator(int spawn, int rate, int speed, int life, int type);

    void update(GLfloat x, GLfloat y, GLfloat z);
    void draw();
    void addParticles();
    void killGenerator();

private:
    GLfloat x, y, z;
    int max, particles_per_emission, emission_rate, emission_speed,
            emission_life, type;
    Particle* particle_array[1000];
};

#endif