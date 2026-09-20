#include "ParticleAcid.h"

ParticleAcid::ParticleAcid() = default;
ParticleAcid::ParticleAcid(GLfloat x,
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
