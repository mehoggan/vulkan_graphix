#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include "vulkan_earth/Particle.h"
#include "vulkan_earth/SpecialEffect.h"

#ifndef PARTICLEACID_H
#define PARTICLEACID_H

class ParticleAcid : public Particle {
public:
    ParticleAcid();
    ParticleAcid(float new_x,
                 float new_y,
                 float new_z,
                 float dir_x,
                 float dir_y,
                 float dir_z,
                 float new_speed,
                 std::int32_t frames);

    bool update() override;
};

#endif