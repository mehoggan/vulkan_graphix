#ifndef NORMAL
#define NORMAL

#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>

class Normal{
	public:
		Normal();
		Normal(GLfloat x, GLfloat y, GLfloat z);
		~Normal();
		GLfloat compoX;
		GLfloat compoY;
		GLfloat compoZ;
};
#endif // NORMAL