#ifndef VERTEX
#define VERTEX

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>

class Vertex {
public:
    Vertex();
    Vertex(float x, float y, float z);
    ~Vertex();
    float coordX;
    float coordY;
    float coordZ;
};
#endif  // Vertex