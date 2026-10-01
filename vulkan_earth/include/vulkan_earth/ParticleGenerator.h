#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <cstdint>
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
    ParticleGenerator(std::int32_t spawn,
                      std::int32_t rate,
                      std::int32_t speed,
                      std::int32_t life,
                      std::int32_t new_type);

    void update(float new_x, float new_y, float new_z);
    void draw();
    void addParticles();
    void killGenerator();

private:
    float x, y, z;
    std::int32_t max, particles_per_emission, emission_rate, emission_speed,
            emission_life, type;
    Particle* particle_array[1000];
};

#endif