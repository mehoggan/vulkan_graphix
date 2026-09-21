#include "ParticleSmoke.h"

ParticleSmoke::ParticleSmoke() = default;
ParticleSmoke::ParticleSmoke(GLfloat x,
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
    active_frames = frames;
    current_frame = 0;
    size = 2;
    red = 1;
    green = 1;
    blue = 1;
}

bool ParticleSmoke::update() {
    x += dir[0] * speed;
    // y+=dir[1]*speed;
    y += 7 * (static_cast<float>(current_frame) /
              static_cast<float>(active_frames));
    z += dir[2] * speed;

    // color test
    if (current_frame <= 30) {
        blue = 1 - (static_cast<GLfloat>(current_frame) / 30.0);
    } else if (current_frame <= 60) {
        green = 1 - ((static_cast<GLfloat>(current_frame) - 30.0) / 30.0);
    } else {
        red = 1 - ((static_cast<GLfloat>(current_frame) - 30.0) / 40.0);
    }

    current_frame++;
    return current_frame != active_frames;
}
