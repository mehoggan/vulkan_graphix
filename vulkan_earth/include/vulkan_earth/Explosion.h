/*
 * skybox.h
 *
 *  Created on: Sep 10, 2010
 *      Author: Matthew Hoggan
 */
#ifndef EXPLOSION_H_
#define EXPLOSION_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include "vulkan_earth/SpecialEffect.h"

using namespace std;

class Shader;

class Explosion : public SpecialEffect {
public:
    /*
     * Constructors and De-constructor
     */
    Explosion();
    Explosion(float new_x,
              float new_y,
              float new_z,
              std::int32_t new_weapon_radius);
    ~Explosion() override;
    void draw() override;
    void setColors1(float* new_colors1) override;
    void setColors2(float* new_colors2) override;
    void setColors3(float* new_colors3) override;
    void setColors4(float* new_colors4) override;
    void setDefaultColors() override;

private:
    float x;
    float y;
    float z;
    float time;
    float trans_matrix[16];
    Shader* shader;
    float timer;
    float radius;
    float colors1[3];
    float colors2[3];
    float colors3[3];
    float colors4[3];
    std::int32_t weapon_radius;
};

#endif /* EXPLOSION_H_ */