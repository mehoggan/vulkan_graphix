#include "ControlItem.h"
#include "ControlItemTextField.h"
#include "TextObject.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "sound.h"
#include "macro_crtdbg.h"

using namespace std;

const int MAX_CHARS = 15;

extern void playSFX(int sfx);

ControlItemTextField::ControlItemTextField() {
}

ControlItemTextField::ControlItemTextField(		GLfloat xPos,GLfloat yPos,GLfloat zPos,
														GLfloat red,GLfloat green,GLfloat blue,
														GLint width,GLint height) {

	this->xPos = xPos; this->yPos = yPos; this->zPos = zPos;
	this->color[0] = red; this->color[1] = green; this->color[2] = blue;
	this->color[3] = 1.0;
	this->width = width; this->height = height; 
	this->currentText=NULL;
	this->textFieldActive=false;
	this->currentChars= new char[MAX_CHARS+1];
	this->clearTextBuffer();
	this->currentLength=0;
	this->numberOfFrames=0;
	this->textCursorOn=1;
	this->setOptionText("");
}

ControlItemTextField::~ControlItemTextField() {
	delete this->currentText;
	delete[] currentChars;
}

void ControlItemTextField::draw(){
	if(numberOfFrames==50){	
		this->textCursorOn*=-1;	//toggle
		this->numberOfFrames=0;
	}
	this->numberOfFrames++;

	glBegin(GL_QUADS);
		glColor4f(this->color[0]-0.6f, this->color[1]-0.6f, this->color[2]-0.6f, this->color[3]);
		glVertex3f(this->xPos, this->yPos, this->zPos);
		glVertex3f(this->xPos-3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos+width, this->yPos, this->zPos);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]-0.6f, this->color[1]-0.6f, this->color[2]-0.6f, this->color[3]);
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
		glColor4f(this->color[0]-0.3f, this->color[1]-0.3f, this->color[2]-0.3f, this->color[3]);
		glVertex3f(this->xPos-3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos+width, this->yPos-height, this->zPos);
		glVertex3f(this->xPos, this->yPos-height, this->zPos);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]-0.3f, this->color[1]-0.3f, this->color[2]-0.3f, this->color[3]);
		glVertex3f(this->xPos+width, this->yPos, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos+width, this->yPos+-height, this->zPos);
	glEnd();

	if(this->textFieldActive){
		if(this->textCursorOn==1){
			int realLength=0;
			for(int i=0;i<strlen(this->currentChars);i++){
				if(this->currentChars[i]!=' '){
					realLength+=glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24,this->currentChars[i]);
				}
			}
			glBegin(GL_QUADS);
				glColor3f(0,0,0);
				glVertex3f(this->xPos+0.02*this->width+realLength,this->yPos-0.15*height,this->zPos+0.1);
				glVertex3f(this->xPos+0.02*this->width+realLength,this->yPos-this->height+0.15*height,this->zPos+0.1);
				glVertex3f(this->xPos+0.02*this->width+realLength+2,this->yPos-this->height+0.15*height,this->zPos+0.1);
				glVertex3f(this->xPos+0.02*this->width+realLength+2,this->yPos-0.15*height,this->zPos+0.1);
			glEnd();
		}
	}

	if(this->currentText) this->currentText->draw();
}


GLfloat ControlItemTextField::getXPos(){ return this->xPos;}
GLfloat ControlItemTextField::getYPos(){ return this->yPos;}
GLfloat ControlItemTextField::getHeight(){ return this->height;}
GLfloat ControlItemTextField::getWidth(){ return this->width;}
bool ControlItemTextField::isTextFieldActive(){ return this->textFieldActive;}
char* ControlItemTextField::collectData(){ return this->currentText->getOutput();}
void ControlItemTextField::deactivate(){ textFieldActive=false;}
void ControlItemTextField::setOptionText(int index){}

void ControlItemTextField::setOptionText(char* newText){

	if(this->currentText){
		delete this->currentText;
	}
	
	int realLength=0;
	for(int i=0;i<strlen(newText);i++){
		realLength+=glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24,newText[i]);
	}
	GLfloat labelXPos=this->xPos+0.02*this->width;
	GLfloat labelYPos=this->yPos+((this->yPos-(this->yPos+this->height))/2)-this->height/4;
	this->currentText = new TextObject(newText,labelXPos,labelYPos,this->zPos+0.1,GLUT_BITMAP_TIMES_ROMAN_24,0.0f,0.0f,0.0f);
}

void ControlItemTextField::mouseClickEvent(GLint x, GLint y, GLint state,  bool stillOverControlItemTextField){
	if(state==0)
		if(stillOverControlItemTextField)
			this->textFieldActive=true;
}



void ControlItemTextField::updateMouse(int x, int y){}
void ControlItemTextField::keyHandler(unsigned char key){
	if(this->textFieldActive){
		if((((key>=48)&&(key<=57))||((key>=65)&&(key<90)))||((key>=97)&&(key<=122))){
			if(this->currentText==NULL){
				this->setOptionText("");
				this->currentLength=0;
			}
			if(currentLength<MAX_CHARS){
				playSFX(KEYTYPING);
				this->currentChars[currentLength]=key;
				this->currentLength++;
				this->setOptionText(currentChars);
			}
	
		} else if(key==8){
			if(this->currentText){
				if(currentLength!=0){
					playSFX(KEYTYPING);
					this->currentChars[currentLength-1]=' ';
					this->currentLength--;
					this->setOptionText(currentChars);
				}
			}
		}
	}
}

void ControlItemTextField::clearTextBuffer(){
	for(int i=0;i<MAX_CHARS;i++)
		this->currentChars[i]=' ';
	this->currentChars[MAX_CHARS]='\0';
	this->currentLength=0;
}

void ControlItemTextField::setTextBuffer(char* newText){
	this->setOptionText(newText);
	this->clearTextBuffer();
	int newLength=0;
	for(int i=0;i<strlen(newText);i++){
		this->currentChars[i]=newText[i];
		if(newText[i]!=' ') newLength++;
	}
	this->currentLength=newLength;
}