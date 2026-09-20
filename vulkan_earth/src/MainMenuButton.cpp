#include "MainMenuButton.h"
#include "TextObject.h"
#include "SubMenu.h"
#include "sound.h"
#include <stdio.h>
#include <iostream>
#include "macro_crtdbg.h"

using namespace std;

extern void playSFX(int sfx);

MainMenuButton::MainMenuButton() {
}

MainMenuButton::MainMenuButton(	int ID,
								GLfloat xPos,GLfloat yPos,GLfloat zPos,
								GLfloat red,GLfloat green,GLfloat blue,
								GLint width,GLint height,char* caption,
								SubMenu* submenu) {
	this->UNIQUEIDENTIFIER=ID;
	this->pressed=false;
	this->active=false;
	this->xPos = xPos; this->yPos = yPos; this->zPos = zPos;
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
	GLfloat labelYPos=this->yPos+((this->yPos-(this->yPos+this->height))/2)-this->height/4;
	/*	END OF BUTTON TEXT PLACEMENT	*/
	this->label = new TextObject(this->caption,labelXPos,labelYPos,this->zPos,GLUT_BITMAP_TIMES_ROMAN_24,0.0f,0.0f,0.0f);
	this->submenu=submenu;
}

MainMenuButton::~MainMenuButton() {
	delete this->label;
}

void MainMenuButton::pressDraw() {
	glPushMatrix();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]-.4, this->color[1]-.4, this->color[2]-.4, this->color[3]);
		glVertex3f(this->xPos, this->yPos, this->zPos);
		glVertex3f(this->xPos-3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos+width, this->yPos, this->zPos);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]-.4, this->color[1]-.4, this->color[2]-.4, this->color[3]);
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
		glColor4f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2, this->color[3]);
		glVertex3f(this->xPos-3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos+width, this->yPos-height, this->zPos);
		glVertex3f(this->xPos, this->yPos-height, this->zPos);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2, this->color[3]);
		glVertex3f(this->xPos+width, this->yPos, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos+width, this->yPos+-height, this->zPos);
	glEnd();
	glPopMatrix();
}

void MainMenuButton::draw(){
	glPushMatrix();
	if(this->pressed)
		pressDraw();
	else {
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
	}
	this->label->draw();

	if(active){
		if(this->submenu!=NULL){
			this->submenu->draw();
		}
	}
	glPopMatrix();
}

int MainMenuButton::getUNIQUEIDENTIFIER() {return this->UNIQUEIDENTIFIER;}
GLfloat MainMenuButton::getXPos(){ return this->xPos;}
GLfloat MainMenuButton::getYPos(){ return this->yPos;}
GLfloat MainMenuButton::getHeight(){ return this->height;}
GLfloat MainMenuButton::getWidth(){ return this->width;}
SubMenu* MainMenuButton::getSubMenu() {return this->submenu;}
GLfloat* MainMenuButton::getColor() {return &this->color[0];}
void MainMenuButton::setColor(GLfloat r, GLfloat g, GLfloat b){
	this->color[0] = r; this->color[1] = g; this->color[2] = b;
}



void MainMenuButton::setLabel(char* c){
	delete this->label;
	this->caption = c;

	const char* cpchar = this->caption;
	int length = strlen(cpchar);
	char* pchar = this->caption;
	int realLength=0;
	for(int i=0;i<length;i++){
		realLength+=glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24,*(pchar));
		*(pchar)++;
	}
	GLfloat labelXPos=this->xPos+((this->width)/2)-(realLength/2);
	GLfloat labelYPos=this->yPos+((this->yPos-(this->yPos+this->height))/2)-this->height/4;

	this->label = new TextObject(this->caption,labelXPos,labelYPos,this->zPos,GLUT_BITMAP_TIMES_ROMAN_24,0.0f,0.0f,0.0f);

}


bool MainMenuButton::isPressed(){return pressed;}
bool MainMenuButton::isActive(){return active;}

void MainMenuButton::pressButton(){
	if(Mix_Playing(0) == 0)
		playSFX(BIG_CLICK);
	this->pressed=true;
}

void MainMenuButton::depressButton(){
	this->pressed=false;
}

void MainMenuButton::activateSubMenu() {
	this->active=true;
}

void MainMenuButton::deactivateSubMenu(){
	this->active=false;
}

void MainMenuButton::printSelf(int i){
	cout 	<<" Button["<<i<<"].x="<<(this->getXPos())
			<<" Button["<<i<<"].y="<<(this->getYPos())
			<<" Button["<<i<<"].width="<<(this->getWidth())
			<<" Button["<<i<<"].height="<<(this->getHeight())
			<<endl;
}
