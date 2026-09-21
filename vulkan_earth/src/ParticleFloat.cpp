#include "ParticleFloat.h"

ParticleFloat::ParticleFloat() = default;
ParticleFloat::ParticleFloat(GLfloat x,
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
    size = 4;
    red = 1;
    green = 1;
    blue = 1;
}

bool ParticleFloat::update() {
    x += dir[0] * speed;
    y += dir[1] * speed / 10;
    z += dir[2] * speed;
    currentFrame++;
    return currentFrame != activeFrames;
}
