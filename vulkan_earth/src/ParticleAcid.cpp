#include "ParticleAcid.h"

ParticleAcid::ParticleAcid() = default;
ParticleAcid::ParticleAcid(GLfloat x,
                           GLfloat y,
                           GLfloat z,
                           GLfloat dir_x,
                           GLfloat dir_y,
                           GLfloat dir_z,
                           GLfloat speed,
                           int frames) {
    this->x = x;
    this->y = y;
    this->z = z;
    dir[0] = dir_x;
    dir[1] = dir_y;
    dir[2] = dir_z;
    this->speed = speed;
    activeFrames = frames;
    currentFrame = 0;
    size = 2;
    red = 0;
    green = 1;
    blue = 0;
}

bool ParticleAcid::update() {
    x += dir[0] * speed;
    y += dir[1] * speed;
    z += dir[2] * speed;
    currentFrame++;
    return currentFrame != activeFrames;
}
