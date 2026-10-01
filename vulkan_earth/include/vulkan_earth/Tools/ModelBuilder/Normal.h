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
    float compoX;
    float compoY;
    float compoZ;
};
#endif  // NORMAL