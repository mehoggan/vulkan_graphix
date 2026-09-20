#include "TexCoord.h"
#include "macro_crtdbg.h"

TexCoord::TexCoord() = default;

TexCoord::TexCoord(GLfloat s, GLfloat t) {
    this->texcoordS = s;
    this->texcoordT = t;
}

TexCoord::~TexCoord() = default;