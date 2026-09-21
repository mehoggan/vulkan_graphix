#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include "SpecialEffect.h"

#ifndef PARTICLE_H
#define PARTICLE_H

class Particle {
public:
    Particle();
    virtual bool update() = 0;
    void draw();

protected:
    GLfloat size, x, y, z, speed, dir[3], red, green, blue;
    int active_frames, current_frame;
};

#endif