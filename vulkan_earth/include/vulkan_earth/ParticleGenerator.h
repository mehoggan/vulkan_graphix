#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include "vulkan_earth/Particle.h"
#include "vulkan_earth/ParticleAcid.h"
#include "vulkan_earth/ParticleFloat.h"
#include "vulkan_earth/ParticleSmoke.h"
#include "vulkan_earth/SpecialEffect.h"

#ifndef PARTICLEGENERATOR_H
#define PARTICLEGENERATOR_H

class ParticleGenerator {
public:
    ParticleGenerator();
    ParticleGenerator(int spawn, int rate, int speed, int life, int new_type);

    void update(GLfloat new_x, GLfloat new_y, GLfloat new_z);
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