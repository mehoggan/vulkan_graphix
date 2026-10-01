#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include "vulkan_earth/SpecialEffect.h"

#ifndef PARTICLE_H
#define PARTICLE_H

class Particle {
public:
    Particle();
    virtual bool update() = 0;
    void draw();

protected:
    float size, x, y, z, speed, dir[3], red, green, blue;
    std::int32_t active_frames, current_frame;
};

#endif