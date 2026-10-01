#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include "vulkan_earth/Particle.h"
#include "vulkan_earth/SpecialEffect.h"

#ifndef PARTICLEFLOAT_H
#define PARTICLEFLOAT_H

class ParticleFloat : public Particle {
public:
    ParticleFloat();
    ParticleFloat(float new_x,
                  float new_y,
                  float new_z,
                  float dir_x,
                  float dir_y,
                  float dir_z,
                  float new_speed,
                  int frames);

    bool update() override;
};

#endif