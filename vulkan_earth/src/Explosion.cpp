/*
 * Explosion.cpp
 *
 *  Created on: Sep 16, 2010
 *      Author: Matthew Hoggan
 */

#include "Explosion.h"
#include "OpenGLColors.h"
#include "Shader.h"
#include "Vector.h"

/*
 * Constructors and De-constructors
 */
Explosion::Explosion() = default;
Explosion::Explosion(GLfloat new_x,
                     GLfloat new_y,
                     GLfloat new_z,
                     int new_weapon_radius) {
    glPushMatrix();
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, trans_matrix);
    glPopMatrix();
    this->x = new_x;
    this->y = new_y;
    this->z = new_z;
    trans_matrix[12] = this->x;
    trans_matrix[13] = this->y;
    trans_matrix[14] = this->z;
    this->weapon_radius = new_weapon_radius;

    shader = new Shader();
    shader->init("VertexExplosion.vs", "FragmentExplosion.vs");

    timer = 50.0f;
    radius = 0.0f;

    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Yellow};
    GLfloat temp_colors3[3] = {Orange};
    GLfloat temp_colors4[3] = {Red};
    for (int i = 0; i < 3; i++) {
        colors1[i] = temp_colors1[i];
        colors2[i] = temp_colors2[i];
        colors3[i] = temp_colors3[i];
        colors4[i] = temp_colors4[i];
    }
}

Explosion::~Explosion() { delete shader; }

void Explosion::draw() {
    shader->bind();
    radius += .1;
    timer += .5;
    // Please See OpenGLColors.h for definitions of colors
    if (timer > 0 && timer < 25) {
        glColor4f(colors1[0], colors1[1], colors1[2], 1.0 - timer / 150.0);
    } else if (timer >= 25 && timer < 50) {
        glColor4f(colors2[0], colors2[1], colors2[2], 1.0 - timer / 150.0);
    } else if (timer >= 50 && timer < 75) {
        glColor4f(colors3[0], colors3[1], colors3[2], 1.0 - timer / 150.0);
    } else if (timer >= 100) {
        glColor4f(colors4[0], colors4[1], colors4[2], 1.0 - timer / 150.0);
    }

    glPushMatrix();
    glTranslatef(this->x, this->y, this->z);
    // ASSUMING SCALE ON TERRAIN IS 150 I NEED TO GET ACTUAL VALUE
    glutSolidSphere(radius * (weapon_radius * 5.56 + 22.22), 90, 180);
    glPopMatrix();
    shader->unbind();
}

void Explosion::setColors1(GLfloat* new_colors1) {
    this->colors1[0] = new_colors1[0];
    this->colors1[1] = new_colors1[1];
    this->colors1[2] = new_colors1[2];
}
void Explosion::setColors2(GLfloat* new_colors2) {
    this->colors2[0] = new_colors2[0];
    this->colors2[1] = new_colors2[1];
    this->colors2[2] = new_colors2[2];
}
void Explosion::setColors3(GLfloat* new_colors3) {
    this->colors3[0] = new_colors3[0];
    this->colors3[1] = new_colors3[1];
    this->colors3[2] = new_colors3[2];
}
void Explosion::setColors4(GLfloat* new_colors4) {
    this->colors4[0] = new_colors4[0];
    this->colors4[1] = new_colors4[1];
    this->colors4[2] = new_colors4[2];
}
void Explosion::setDefaultColors() {
    GLfloat temp_colors1[3] = {White};
    GLfloat temp_colors2[3] = {Yellow};
    GLfloat temp_colors3[3] = {Orange};
    GLfloat temp_colors4[3] = {Red};
    for (int i = 0; i < 3; i++) {
        colors1[i] = temp_colors1[i];
        colors2[i] = temp_colors2[i];
        colors3[i] = temp_colors3[i];
        colors4[i] = temp_colors4[i];
    }
}