#ifndef VERTEX
#define VERTEX

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>

class Vertex {
public:
    Vertex();
    Vertex(GLfloat x, GLfloat y, GLfloat z);
    ~Vertex();
    GLfloat coordX;
    GLfloat coordY;
    GLfloat coordZ;
};
#endif  // Vertex