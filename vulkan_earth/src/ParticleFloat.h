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
    ParticleFloat(GLfloat x,
                  GLfloat y,
                  GLfloat z,
                  GLfloat dirX,
                  GLfloat dirY,
                  GLfloat dirZ,
                  GLfloat speed,
                  int frames);

    bool update() override;
};

#endif