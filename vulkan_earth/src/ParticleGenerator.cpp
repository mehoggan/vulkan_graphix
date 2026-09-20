#include "ParticleGenerator.h"
#include <cstdlib>
#include "math.h"

ParticleGenerator::ParticleGenerator() = default;
ParticleGenerator::ParticleGenerator(
        int spawn, int rate, int speed, int life, int type) {
    this->max = 1000;
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->particlesPerEmission = spawn;
    this->emissionRate = rate;
    this->emissionSpeed = speed;
    this->emissionLife = life;
    this->type = type;
    for (int i = 0; i < max; i++) particleArray[i] = nullptr;
}

void ParticleGenerator::update(GLfloat x, GLfloat y, GLfloat z) {
    this->x = x;
    this->y = y;
    this->z = z;
    for (int i = 0; i < this->max; i++) {
        if (this->particleArray[i] != nullptr) {
            if (!this->particleArray[i]->update()) {
                delete particleArray[i];
                this->particleArray[i] = nullptr;
            }
        }
    }
    this->addParticles();
}
void ParticleGenerator::draw() {
    for (int i = 0; i < this->max; i++) {
        if (this->particleArray[i] != nullptr) {
            this->particleArray[i]->draw();
        }
    }
}
void ParticleGenerator::addParticles() {
    int i = 0, count = 0;
    while ((i < max) && (count < particlesPerEmission)) {
        if (particleArray[i] == nullptr) {
            GLfloat dirX = static_cast<GLfloat>(rand()) * 2 / RAND_MAX - 1;
            GLfloat dirY = static_cast<GLfloat>(rand()) * 2 / RAND_MAX - 1;
            GLfloat dirZ = static_cast<GLfloat>(rand()) * 2 / RAND_MAX - 1;
            GLfloat mag = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
            dirX /= mag;
            dirY /= mag;
            dirZ /= mag;
            switch (this->type) {
                case 0:
                    particleArray[i] = new ParticleSmoke(x,
                                                         y,
                                                         z,
                                                         dirX,
                                                         dirY,
                                                         dirZ,
                                                         emissionSpeed,
                                                         emissionLife);
                    break;
                case 1:
                    particleArray[i] = new ParticleAcid(x,
                                                        y,
                                                        z,
                                                        dirX,
                                                        dirY,
                                                        dirZ,
                                                        emissionSpeed,
                                                        emissionLife);
                    break;
                case 2:
                    particleArray[i] = new ParticleFloat(x,
                                                         y,
                                                         z,
                                                         dirX,
                                                         dirY,
                                                         dirZ,
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
    for (int i = 0; i < this->max; i++) {
        if (this->particleArray[i] != nullptr) {
            delete this->particleArray[i];
        }
    }
}