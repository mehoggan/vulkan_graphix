#include "TexCoord.h"
#include "macro_crtdbg.h"

TexCoord::TexCoord() = default;

TexCoord::TexCoord(GLfloat s, GLfloat t) {
    texcoordS = s;
    texcoordT = t;
}

TexCoord::~TexCoord() = default;