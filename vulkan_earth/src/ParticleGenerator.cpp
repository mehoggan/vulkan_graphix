#include "vulkan_earth/ParticleGenerator.h"
#include <cstdint>
#include <cstdlib>
#include "math.h"

ParticleGenerator::ParticleGenerator() = default;
ParticleGenerator::ParticleGenerator(std::int32_t spawn,
                                     std::int32_t rate,
                                     std::int32_t speed,
                                     std::int32_t life,
                                     std::int32_t new_type) {
    max = 1000;
    x = 0;
    y = 0;
    z = 0;
    particles_per_emission = spawn;
    emission_rate = rate;
    emission_speed = speed;
    emission_life = life;
    type = new_type;
    for (std::int32_t i = 0; i < max; i++) particle_array[i] = nullptr;
}

void ParticleGenerator::update(float new_x, float new_y, float new_z) {
    x = new_x;
    y = new_y;
    z = new_z;
    for (std::int32_t i = 0; i < max; i++) {
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
    for (std::int32_t i = 0; i < max; i++) {
        if (particle_array[i] != nullptr) {
            particle_array[i]->draw();
        }
    }
}
void ParticleGenerator::addParticles() {
    std::int32_t i = 0, count = 0;
    while ((i < max) && (count < particles_per_emission)) {
        if (particle_array[i] == nullptr) {
            float dir_x = static_cast<float>(rand()) * 2 / RAND_MAX - 1;
            float dir_y = static_cast<float>(rand()) * 2 / RAND_MAX - 1;
            float dir_z = static_cast<float>(rand()) * 2 / RAND_MAX - 1;
            float mag = sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
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
    for (std::int32_t i = 0; i < max; i++) {
        if (particle_array[i] != nullptr) {
            delete particle_array[i];
        }
    }
}