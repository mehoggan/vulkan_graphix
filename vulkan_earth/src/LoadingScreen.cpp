#include "LoadingScreen.h"
#include "ImageObject.h"
#include "macro_crtdbg.h"

LoadingScreen::LoadingScreen(){}
LoadingScreen::LoadingScreen(GLfloat x, GLfloat y, GLfloat z, GLint width, GLint height, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha){
	this->pos[0] = x;
	this->pos[1] = y;
	this->pos[2] = z;
	this->width = width;
	this->height = height;
	this->color[0] = red;
	this->color[1] = green;
	this->color[2] = blue;
	this->color[3] = alpha;
	this->image = new ImageObject(pos[0]+(width*0.03), pos[1]-(height*0.03), pos[2]+2, width*0.94, height*0.94, .006*width,
													1024, 1024, "loading_screen.raw");
}
LoadingScreen::~LoadingScreen(){
	delete this->image;
}

void LoadingScreen::draw(){
	glBegin(GL_QUADS);
		glColor4f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2, this->color[3]);
		glVertex3f(this->pos[0], this->pos[1], this->pos[2]);
		glVertex3f(this->pos[0]-3, this->pos[1]+3, this->pos[2]);
		glVertex3f(this->pos[0]+width+3, this->pos[1]+3, this->pos[2]);
		glVertex3f(this->pos[0]+width, this->pos[1], this->pos[2]);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2, this->color[3]);
		glVertex3f(this->pos[0]-3, this->pos[1]+3, this->pos[2]);
		glVertex3f(this->pos[0]-3, this->pos[1]-height-3, this->pos[2]);
		glVertex3f(this->pos[0], this->pos[1]-height, this->pos[2]);
		glVertex3f(this->pos[0], this->pos[1], this->pos[2]);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0], this->color[1], this->color[2], this->color[3]);
		glVertex3f(this->pos[0], this->pos[1], this->pos[2]);
		glVertex3f(this->pos[0], this->pos[1]-height, this->pos[2]);
		glVertex3f(this->pos[0]+width, this->pos[1]-height, this->pos[2]);
		glVertex3f(this->pos[0]+width, this->pos[1], this->pos[2]);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]-.4, this->color[1]-.4, this->color[2]-.4, this->color[3]);
		glVertex3f(this->pos[0]-3, this->pos[1]-height-3, this->pos[2]);
		glVertex3f(this->pos[0]+width+3, this->pos[1]-height-3, this->pos[2]);
		glVertex3f(this->pos[0]+width, this->pos[1]-height, this->pos[2]);
		glVertex3f(this->pos[0], this->pos[1]-height, this->pos[2]);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]-.4, this->color[1]-.4, this->color[2]-.4, this->color[3]);
		glVertex3f(this->pos[0]+width, this->pos[1], this->pos[2]);
		glVertex3f(this->pos[0]+width+3, this->pos[1]+3, this->pos[2]);
		glVertex3f(this->pos[0]+width+3, this->pos[1]-height-3, this->pos[2]);
		glVertex3f(this->pos[0]+width, this->pos[1]+-height, this->pos[2]);
	glEnd();

	this->image->draw();
}

