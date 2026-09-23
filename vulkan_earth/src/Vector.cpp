#include "Vector.h"
#include "MacroCrtdbg.h"

Vector::Vector() {
    compo_x = 0.0f;
    compo_y = 0.0f;
    compo_z = 0.0f;
}

Vector::Vector(GLfloat new_compo_x, GLfloat new_compo_y, GLfloat new_compo_z) {
    compo_x = new_compo_x;
    compo_y = new_compo_y;
    compo_z = new_compo_z;
}

Vector::~Vector() = default;