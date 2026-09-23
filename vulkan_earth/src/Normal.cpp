#include "Normal.h"
#include "MacroCrtdbg.h"

Normal::Normal() = default;

Normal::Normal(GLfloat x, GLfloat y, GLfloat z) {
    compo_x = x;
    compo_y = y;
    compo_z = z;
}

Normal::~Normal() = default;