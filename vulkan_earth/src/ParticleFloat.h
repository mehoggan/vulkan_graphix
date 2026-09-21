#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include "Particle.h"
#include "SpecialEffect.h"

#ifndef PARTICLEFLOAT_H
#define PARTICLEFLOAT_H

class ParticleFloat : public Particle {
public:
    ParticleFloat();
    ParticleFloat(GLfloat new_x,
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