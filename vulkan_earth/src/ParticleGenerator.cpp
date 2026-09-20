#include "ParticleGenerator.h"
#include <cstdlib>
#include "math.h"

ParticleGenerator::ParticleGenerator() = default;
ParticleGenerator::ParticleGenerator(
        int spawn, int rate, int speed, int life, int type) {
    max = 1000;
    this->x = 0;
    this->y = 0;
    this->z = 0;
    particlesPerEmission = spawn;
    emissionRate = rate;
    emissionSpeed = speed;
    emissionLife = life;
    this->type = type;
    for (int i = 0; i < max; i++) particleArray[i] = nullptr;
}

void ParticleGenerator::update(GLfloat x, GLfloat y, GLfloat z) {
    this->x = x;
    this->y = y;
    this->z = z;
    for (int i = 0; i < max; i++) {
        if (particleArray[i] != nullptr) {
            if (!particleArray[i]->update()) {
                delete particleArray[i];
                particleArray[i] = nullptr;
            }
        }
    }
    addParticles();
}
void ParticleGenerator::draw() {
    for (int i = 0; i < max; i++) {
        if (particleArray[i] != nullptr) {
            particleArray[i]->draw();
        }
    }
}
void ParticleGenerator::addParticles() {
    int i = 0, count = 0;
    while ((i < max) && (count < particlesPerEmission)) {
        if (particleArray[i] == nullptr) {
            GLfloat dir_x = static_cast<GLfloat>(rand()) * 2 / RAND_MAX - 1;
            GLfloat dir_y = static_cast<GLfloat>(rand()) * 2 / RAND_MAX - 1;
            GLfloat dir_z = static_cast<GLfloat>(rand()) * 2 / RAND_MAX - 1;
            GLfloat mag = sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
            dir_x /= mag;
            dir_y /= mag;
            dir_z /= mag;
            switch (this->type) {
                case 0:
                    particleArray[i] = new ParticleSmoke(x,
                                                         y,
                                                         z,
                                                         dir_x,
                                                         dir_y,
                                                         dir_z,
                                                         emissionSpeed,
                                                         emissionLife);
                    break;
                case 1:
                    particleArray[i] = new ParticleAcid(x,
                                                        y,
                                                        z,
                                                        dir_x,
                                                        dir_y,
                                                        dir_z,
                                                        emissionSpeed,
                                                        emissionLife);
                    break;
                case 2:
                    particleArray[i] = new ParticleFloat(x,
                                                         y,
                                                         z,
                                                         dir_x,
                                                         dir_y,
                                                         dir_z,
                                                         emissionSpeed,
                                                         emissionLife);
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
        if (particleArray[i] != nullptr) {
            delete particleArray[i];
        }
    }
}