#include "ParticleFloat.h"

ParticleFloat::ParticleFloat() = default;
ParticleFloat::ParticleFloat(GLfloat x,
                             GLfloat y,
                             GLfloat z,
                             GLfloat dirX,
                             GLfloat dirY,
                             GLfloat dirZ,
                             GLfloat speed,
                             int frames) {
    this->x = x;
    this->y = y;
    this->z = z;
    dir[0] = dirX;
    dir[1] = dirY;
    dir[2] = dirZ;
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
