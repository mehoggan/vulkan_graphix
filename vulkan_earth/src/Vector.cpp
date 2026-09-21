#include "Vector.h"
#include "macro_crtdbg.h"

Vector::Vector() {
    this->compo_x = 0.0f;
    this->compo_y = 0.0f;
    this->compo_z = 0.0f;
}

Vector::Vector(GLfloat new_compo_x, GLfloat new_compo_y, GLfloat new_compo_z) {
    this->compo_x = new_compo_x;
    this->compo_y = new_compo_y;
    this->compo_z = new_compo_z;
}

Vector::~Vector() = default;