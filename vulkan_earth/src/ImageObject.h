#include <GL/glew.h>
#include <GL/freeglut.h>
#include "stdio.h"

#ifndef IMAGEOBJECT
#define IMAGEOBJECT


class ImageObject{
public:
	ImageObject();
	ImageObject(GLfloat xPos, GLfloat yPos, GLfloat zPos, 
				GLint width, GLint height,GLfloat border,
				GLint iwidth,GLint iheight,char* filename);
	~ImageObject();
	GLfloat getXpos();
	GLfloat getYpos();
	GLfloat getZpos();
	GLint getWidth();
	GLint getHeight();
	void setXpos(GLfloat x);
	void setYpos(GLfloat y);
	void setZpos(GLfloat z);
	void setWidth(GLint w);
	void setHeight(GLint h);
	void draw();	
private:
	GLfloat xPos;
	GLfloat yPos;
	GLfloat zPos;
	GLint width;
	GLint height;
	GLfloat borderSize;
	GLuint texture;
	unsigned char* data;
	FILE* file;
};
#endif