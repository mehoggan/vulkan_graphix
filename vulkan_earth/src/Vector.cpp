#include "Vector.h"
#include "macro_crtdbg.h"

Vector::Vector() {
    this->compoX = 0.0f;
    this->compoY = 0.0f;
    this->compoZ = 0.0f;
}

Vector::Vector(GLfloat compo_x, GLfloat compo_y, GLfloat compo_z) {
    this->compoX = compo_x;
    this->compoY = compo_y;
    this->compoZ = compo_z;
}

Vector::~Vector() = default;