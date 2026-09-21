#include "Vector.h"
#include "macro_crtdbg.h"

Vector::Vector() {
    this->compo_x = 0.0f;
    this->compo_y = 0.0f;
    this->compo_z = 0.0f;
}

Vector::Vector(GLfloat compo_x, GLfloat compo_y, GLfloat compo_z) {
    this->compo_x = compo_x;
    this->compo_y = compo_y;
    this->compo_z = compo_z;
}

Vector::~Vector() = default;