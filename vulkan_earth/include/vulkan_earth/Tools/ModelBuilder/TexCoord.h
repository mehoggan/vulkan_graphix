#ifndef TEX_COORD_H
#define TEX_COORD_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>

class TexCoord {
public:
    TexCoord();
    TexCoord(float s, float t);
    ~TexCoord();
    float texcoordS;
    float texcoordT;
};
#endif  // TEX_COORD_H