#ifndef TEX_COORD_H
#define TEX_COORD_H

#include <stdio.h>

class TexCoord {
public:
    TexCoord();
    TexCoord(float s, float t);
    ~TexCoord();
    float texcoord_s;
    float texcoord_t;
};
#endif  // TEX_COORD_H