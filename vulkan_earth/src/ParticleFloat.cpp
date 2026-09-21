#include "ParticleFloat.h"

ParticleFloat::ParticleFloat() = default;
ParticleFloat::ParticleFloat(GLfloat new_x,
                             GLfloat new_y,
                             GLfloat new_z,
                             GLfloat dir_x,
                             GLfloat dir_y,
                             GLfloat dir_z,
                             GLfloat new_speed,
                             int frames) {
    this->x = new_x;
    this->y = new_y;
    this->z = new_z;
    dir[0] = dir_x;
    dir[1] = dir_y;
    dir[2] = dir_z;
    this->speed = new_speed;
    active_frames = frames;
    current_frame = 0;
    size = 4;
    red = 1;
    green = 1;
    blue = 1;
}

bool ParticleFloat::update() {
    x += dir[0] * speed;
    y += dir[1] * speed / 10;
    z += dir[2] * speed;
    current_frame++;
    return current_frame != active_frames;
}
