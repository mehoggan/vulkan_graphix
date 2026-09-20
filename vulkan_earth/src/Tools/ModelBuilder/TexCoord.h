#ifndef TEX_COORD_H
#define TEX_COORD_H

#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

class TexCoord{
	public:
		TexCoord();
		TexCoord(GLfloat s, GLfloat t);
		~TexCoord();
		GLfloat texcoordS;
		GLfloat texcoordT;
};
#endif // TEX_COORD_H