#ifndef VECTOR_H
#define VECTOR_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>

class Vector {
public:
    Vector();
    Vector(GLfloat new_compo_x, GLfloat new_compo_y, GLfloat new_compo_z);
    ~Vector();
    GLfloat compo_x;
    GLfloat compo_y;
    GLfloat compo_z;
};

#endif /*	VECTOR_H	*/