#include <stdlib.h>
#include "ImageObject.h"
#include "macro_crtdbg.h"



ImageObject::ImageObject(){
}

ImageObject::ImageObject(	GLfloat xPos,GLfloat yPos,GLfloat zPos,
							GLint width,GLint height,GLfloat border,
							GLint iWidth,GLint iHeight,char* filename){
	this->xPos = xPos;
	this->yPos = yPos;
	this->zPos = zPos;
	this->width = width;
	this->height = height;
	this->borderSize = border;

	int imgWidth=iWidth;
	int imgHeight=iHeight;
			
	data = (unsigned char *) malloc( imgWidth * imgHeight * 3 );
	file = fopen( filename, "rb" );
	fread( data, imgWidth * imgHeight * 3, 1, file );
	fclose(file);
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, data);
	free(data);
}

ImageObject::~ImageObject(){
	glDeleteTextures(1, &texture);
}



/*GETTERS & SETTERS*/
GLfloat ImageObject::getXpos()		{return this->xPos;}
GLfloat ImageObject::getYpos()		{return this->yPos;}
GLfloat ImageObject::getZpos()		{return this->zPos;}
GLint ImageObject::getWidth()		{return this->width;}
GLint ImageObject::getHeight()		{return this->height;}
void ImageObject::setXpos(GLfloat x)		{this->xPos = x;}
void ImageObject::setYpos(GLfloat y)		{this->yPos = y;}
void ImageObject::setZpos(GLfloat z)		{this->zPos = z;}
void ImageObject::setWidth(GLint w)			{this->width = w;}
void ImageObject::setHeight(GLint h)			{this->height = h;}
	



void ImageObject::draw(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	
	glPushMatrix();
	//image plane
	glBegin(GL_QUADS);
		glTexCoord2f(0,1); glVertex3f(this->xPos,this->yPos,this->zPos);
		glTexCoord2f(0,0); glVertex3f(this->xPos,this->yPos-height,this->zPos);
		glTexCoord2f(1,0); glVertex3f(this->xPos+width,this->yPos-height,this->zPos);
		glTexCoord2f(1,1); glVertex3f(this->xPos+width,this->yPos,this->zPos);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	if(borderSize!=0){
		//top and right borders
		glBegin(GL_QUADS);
			glColor3f(0.45,0.45,0.45);
	
			glVertex3f(this->xPos, this->yPos, this->zPos);
			glVertex3f(this->xPos-borderSize, this->yPos+borderSize, this->zPos);
			glVertex3f(this->xPos+width+borderSize, this->yPos+borderSize, this->zPos);
			glVertex3f(this->xPos+width, this->yPos, this->zPos);
	
			glVertex3f(this->xPos-borderSize, this->yPos+borderSize, this->zPos);
			glVertex3f(this->xPos-borderSize, this->yPos-height-borderSize, this->zPos);
			glVertex3f(this->xPos, this->yPos-height, this->zPos);
			glVertex3f(this->xPos, this->yPos, this->zPos);
		glEnd();
	
		//bottom and left borders
		glBegin(GL_QUADS);
			glColor3f(0.85,0.85,0.85);
	
			glVertex3f(this->xPos-borderSize, this->yPos-height-borderSize, this->zPos);
			glVertex3f(this->xPos+width+borderSize, this->yPos-height-borderSize, this->zPos);
			glVertex3f(this->xPos+width, this->yPos-height, this->zPos);
			glVertex3f(this->xPos, this->yPos-height, this->zPos);
		
			glVertex3f(this->xPos+width, this->yPos, this->zPos);
			glVertex3f(this->xPos+width+borderSize, this->yPos+borderSize, this->zPos);
			glVertex3f(this->xPos+width+borderSize, this->yPos-height-borderSize, this->zPos);
			glVertex3f(this->xPos+width, this->yPos+-height, this->zPos);
		glEnd();
	}
	glPopMatrix();
}