#ifndef VERTEX
#define VERTEX

#include <math.h>
#include <stdio.h>

class Vertex {
public:
    Vertex();
    Vertex(float x, float y, float z);
    ~Vertex();
    float coord_x;
    float coord_y;
    float coord_z;
};
#endif  // Vertex