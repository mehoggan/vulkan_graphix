#include "ParticleSmoke.h"

ParticleSmoke::ParticleSmoke() = default;
ParticleSmoke::ParticleSmoke(GLfloat x,
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
    red = 1;
    green = 1;
    blue = 1;
}

bool ParticleSmoke::update() {
    x += dir[0] * speed;
    // y+=dir[1]*speed;
    y += 7 *
         (static_cast<float>(currentFrame) / static_cast<float>(activeFrames));
    z += dir[2] * speed;

    // color test
    if (currentFrame <= 30) {
        blue = 1 - (static_cast<GLfloat>(currentFrame) / 30.0);
    } else if (currentFrame <= 60) {
        green = 1 - ((static_cast<GLfloat>(currentFrame) - 30.0) / 30.0);
    } else {
        red = 1 - ((static_cast<GLfloat>(currentFrame) - 30.0) / 40.0);
    }

    currentFrame++;
    return currentFrame != activeFrames;
}
