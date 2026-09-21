#include "ParticleAcid.h"

ParticleAcid::ParticleAcid() = default;
ParticleAcid::ParticleAcid(GLfloat new_x,
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
    size = 2;
    red = 0;
    green = 1;
    blue = 0;
}

bool ParticleAcid::update() {
    x += dir[0] * speed;
    y += dir[1] * speed;
    z += dir[2] * speed;
    current_frame++;
    return current_frame != active_frames;
}
