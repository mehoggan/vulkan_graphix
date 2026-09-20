#include <cstring>
#include "SubMenu.h"
#include "SubMenuTest.h"
#include "TextObject.h"
#include <string>
#include "macro_crtdbg.h"


using namespace std;

SubMenuTest::SubMenuTest(){
}

SubMenuTest::SubMenuTest(	int ID,
							GLfloat xPos, GLfloat yPos, GLfloat zPos, 
							GLfloat red, GLfloat green, GLfloat blue, 
							GLint width, GLint height,char* caption,
							GLfloat percentBorder){
	this->UNIQUEIDENTIFIER=ID;
	this->xPos = xPos; this->yPos = yPos; this->zPos = zPos;
	this->percentBorder=percentBorder;
	this->color[0] = red; this->color[1] = green; this->color[2] = blue;
	this->color[3] = 1.0;
	this->width = width; this->height = height; 
	this->caption=caption;
	
	/*	BUTTON TEXT PLACEMENT	*/
	const char* cpchar = this->caption;
	int length = strlen(cpchar);
	char* pchar = this->caption;
	int realLength=0;
	for(int i=0;i<length;i++){
		realLength+=glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24,*(pchar));
		*(pchar)++;
	}
	GLfloat labelXPos=this->xPos+((this->width)/2)-(realLength/2);
	GLfloat labelYPos=this->yPos-this->height/20;
	/*	END OF BUTTON TEXT PLACEMENT	*/
	
	this->label = new TextObject(this->caption,labelXPos,labelYPos,(this->zPos+1),GLUT_BITMAP_TIMES_ROMAN_24,0.0f,0.0f,0.0f);
}

SubMenuTest::~SubMenuTest(){
}

int SubMenuTest::getUNIQUEIDENTIFIER(){return this->UNIQUEIDENTIFIER;}
void SubMenuTest::setUNIQUEIDENTIFIER(int ID){this->UNIQUEIDENTIFIER=ID;}
GLfloat SubMenuTest::getXPos(){return this->xPos;}
void SubMenuTest::setXPos(GLfloat newXpos){this->xPos=newXpos;}
GLfloat SubMenuTest::getYPos(){return this->yPos;}
void SubMenuTest::setYPos(GLfloat newYpos){this->yPos=newYpos;}
GLfloat SubMenuTest::getZPos(){return this->zPos;}
void SubMenuTest::setZPos(GLfloat newZpos){this->zPos=newZpos;}
GLfloat SubMenuTest::getRed(){return this->color[0];}
void SubMenuTest::setRed(GLfloat red){this->color[0]=red;}
GLfloat SubMenuTest::getGreen(){return this->color[1];}
void SubMenuTest::setGreen(GLfloat green){this->color[1]=green;}
GLfloat SubMenuTest::getBlue(){return this->color[2];}
void SubMenuTest::setBlue(GLfloat blue){this->color[2]=blue;}
GLint SubMenuTest::getWidth(){return this->width;}
void SubMenuTest::setWdith(GLint width){this->width=width;}
GLint SubMenuTest::getHeight(){return this->height;}
void SubMenuTest::setHeight(GLint height){this->height=height;}
char* SubMenuTest::getCaption(){return this->caption;}
void SubMenuTest::setCaption(char* caption){this->caption=caption;}
GLfloat SubMenuTest::getPerecentBorder(){return this->percentBorder;}
void SubMenuTest::setPercentBorder(GLfloat percent){this->percentBorder=percentBorder;}

void SubMenuTest::draw(){
	glBegin(GL_QUADS);
		glColor4f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2, this->color[3]);
		glVertex3f(this->xPos, this->yPos, this->zPos);
		glVertex3f(this->xPos-3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos+width, this->yPos, this->zPos);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2, this->color[3]);
		glVertex3f(this->xPos-3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos-3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos, this->yPos-height, this->zPos);
		glVertex3f(this->xPos, this->yPos, this->zPos);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0], this->color[1], this->color[2], this->color[3]);
		glVertex3f(this->xPos, this->yPos, this->zPos);
		glVertex3f(this->xPos, this->yPos-height, this->zPos);
		glVertex3f(this->xPos+width, this->yPos-height, this->zPos);
		glVertex3f(this->xPos+width, this->yPos, this->zPos);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]-.4, this->color[1]-.4, this->color[2]-.4, this->color[3]);
		glVertex3f(this->xPos-3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos+width, this->yPos-height, this->zPos);
		glVertex3f(this->xPos, this->yPos-height, this->zPos);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]-.4, this->color[1]-.4, this->color[2]-.4, this->color[3]);
		glVertex3f(this->xPos+width, this->yPos, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos+width, this->yPos+-height, this->zPos);
	glEnd();
	this->label->draw();
}

const char* SubMenuTest::collectData() {
	char* retrn="Test:";
	const char* realretrn=retrn;
	return realretrn;
}