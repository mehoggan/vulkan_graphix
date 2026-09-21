#ifndef TEX_COORD_H
#define TEX_COORD_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>

class TexCoord {
public:
    TexCoord();
    TexCoord(GLfloat s, GLfloat t);
    ~TexCoord();
    GLfloat texcoord_s;
    GLfloat texcoord_t;
};
#endif  // TEX_COORD_H