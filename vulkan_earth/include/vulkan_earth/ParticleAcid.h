#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include "vulkan_earth/Particle.h"
#include "vulkan_earth/SpecialEffect.h"

#ifndef PARTICLEACID_H
#define PARTICLEACID_H

class ParticleAcid : public Particle {
public:
    ParticleAcid();
    ParticleAcid(GLfloat new_x,
                 GLfloat new_y,
                 GLfloat new_z,
                 GLfloat dir_x,
                 GLfloat dir_y,
                 GLfloat dir_z,
                 GLfloat new_speed,
                 int frames);

    bool update() override;
};

#endif