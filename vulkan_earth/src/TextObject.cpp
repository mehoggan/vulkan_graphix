#include "TextObject.h"
#include <string>
#include "macro_crtdbg.h"

using namespace std;

TextObject::TextObject(){
}

TextObject::TextObject(	char* input, 
						GLfloat posX, GLfloat posY, GLfloat posZ, 
						void* font_size, 
						GLfloat red, GLfloat green, GLfloat blue) {
	sprintf(this->output,input);
	this->posX=posX;
	this->posY=posY;
	this->posZ=posZ+1;
	this->font_size=font_size;
	if(glutGet(GLUT_SCREEN_WIDTH) < 1300){
		this->font_size = GLUT_BITMAP_9_BY_15;
	}
	this->color[0]=red;
	this->color[1]=green;
	this->color[2]=blue;
	this->color[3]=1.0;
}

TextObject::~TextObject() {
}

char* TextObject::getOutput() {return this->output;}
void TextObject::setXpos(GLfloat x)		{this->posX = x;}
void TextObject::setYpos(GLfloat y)		{this->posY = y;}
void TextObject::setZpos(GLfloat z)		{this->posZ = z;}

GLvoid TextObject::draw() {
	this->string_iterator=this->output;
	GLfloat xPos=this->posX;
	glColor3f(this->color[0],this->color[1],this->color[2]);
	while(*(this->string_iterator) != '\0'){
		int step=glutBitmapWidth(this->font_size, *(this->string_iterator));
		glRasterPos3f(xPos,this->posY,this->posZ);
		glutBitmapCharacter((void*)this->font_size,*(this->string_iterator));
		xPos+=step;
		(this->string_iterator)++;
	}
}