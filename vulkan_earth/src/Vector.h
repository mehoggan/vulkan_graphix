#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

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