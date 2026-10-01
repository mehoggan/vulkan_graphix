#ifndef NORMAL
#define NORMAL

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>

class Normal {
public:
    Normal();
    Normal(float x, float y, float z);
    ~Normal();
    float compo_x;
    float compo_y;
    float compo_z;
};
#endif  // NORMAL