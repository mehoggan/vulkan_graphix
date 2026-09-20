#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include "Particle.h"
#include "SpecialEffect.h"

#ifndef PARTICLESMOKE_H
#define PARTICLESMOKE_H

class ParticleSmoke : public Particle {
public:
    ParticleSmoke();
    ParticleSmoke(GLfloat x,
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