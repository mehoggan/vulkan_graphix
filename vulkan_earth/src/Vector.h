#ifndef VECTOR_H
#define VECTOR_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>

class Vector {
public:
    Vector();
    Vector(GLfloat compoX, GLfloat compoY, GLfloat compoZ);
    ~Vector();
    GLfloat compoX;
    GLfloat compoY;
    GLfloat compoZ;
};

#endif /*	VECTOR_H	*/