#include "Normal.h"
#include "macro_crtdbg.h"

Normal::Normal() = default;

Normal::Normal(GLfloat x, GLfloat y, GLfloat z) {
    compoX = x;
    compoY = y;
    compoZ = z;
}

Normal::~Normal() = default;