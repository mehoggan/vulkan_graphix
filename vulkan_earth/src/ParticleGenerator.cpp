#include "ParticleGenerator.h"
#include <cstdlib>
#include "math.h"

ParticleGenerator::ParticleGenerator() = default;
ParticleGenerator::ParticleGenerator(
        int spawn, int rate, int speed, int life, int new_type) {
    max = 1000;
    x = 0;
    y = 0;
    z = 0;
    particles_per_emission = spawn;
    emission_rate = rate;
    emission_speed = speed;
    emission_life = life;
    type = new_type;
    for (int i = 0; i < max; i++) particle_array[i] = nullptr;
}

void ParticleGenerator::update(GLfloat new_x, GLfloat new_y, GLfloat new_z) {
    x = new_x;
    y = new_y;
    z = new_z;
    for (int i = 0; i < max; i++) {
        if (particle_array[i] != nullptr) {
            if (!particle_array[i]->update()) {
                delete particle_array[i];
                particle_array[i] = nullptr;
            }
        }
    }
    addParticles();
}
void ParticleGenerator::draw() {
    for (int i = 0; i < max; i++) {
        if (particle_array[i] != nullptr) {
            particle_array[i]->draw();
        }
    }
}
void ParticleGenerator::addParticles() {
    int i = 0, count = 0;
    while ((i < max) && (count < particles_per_emission)) {
        if (particle_array[i] == nullptr) {
            GLfloat dir_x = static_cast<GLfloat>(rand()) * 2 / RAND_MAX - 1;
            GLfloat dir_y = static_cast<GLfloat>(rand()) * 2 / RAND_MAX - 1;
            GLfloat dir_z = static_cast<GLfloat>(rand()) * 2 / RAND_MAX - 1;
            GLfloat mag = sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
            dir_x /= mag;
            dir_y /= mag;
            dir_z /= mag;
            switch (type) {
                case 0:
                    particle_array[i] = new ParticleSmoke(x,
                                                          y,
                                                          z,
                                                          dir_x,
                                                          dir_y,
                                                          dir_z,
                                                          emission_speed,
                                                          emission_life);
                    break;
                case 1:
                    particle_array[i] = new ParticleAcid(x,
                                                         y,
                                                         z,
                                                         dir_x,
                                                         dir_y,
                                                         dir_z,
                                                         emission_speed,
                                                         emission_life);
                    break;
                case 2:
                    particle_array[i] = new ParticleFloat(x,
                                                          y,
                                                          z,
                                                          dir_x,
                                                          dir_y,
                                                          dir_z,
                                                          emission_speed,
                                                          emission_life);
                    break;
                default:;
            }
            count++;
        }
        i++;
    }
}
void ParticleGenerator::killGenerator() {
    for (int i = 0; i < max; i++) {
        if (particle_array[i] != nullptr) {
            delete particle_array[i];
        }
    }
}