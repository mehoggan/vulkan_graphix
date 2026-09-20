#include "Normal.h"
#include "macro_crtdbg.h"

Normal::Normal() = default;

Normal::Normal(GLfloat x, GLfloat y, GLfloat z) {
    this->compoX = x;
    this->compoY = y;
    this->compoZ = z;
}

Normal::~Normal() = default;