#include "ControlItem.h"
#include "ControlItemSliderbar.h"
#include "TextObject.h"
#include "sound.h"
#include <stdio.h>
#include <iostream>
#include "macro_crtdbg.h"

extern void playSFX(int sfx);

ControlItemSliderbar::ControlItemSliderbar() {
}
ControlItemSliderbar::ControlItemSliderbar(		GLfloat xPos,GLfloat yPos,GLfloat zPos,
														GLfloat red,GLfloat green,GLfloat blue,
														GLint width,GLint height,char* caption,
														char* menuString, int sliderStartingIndex) {

	this->xPos = xPos; this->yPos = yPos; this->zPos = zPos;
	this->color[0] = red; this->color[1] = green; this->color[2] = blue;
	this->color[3] = 1.0;
	this->width = width; this->height = height; 

	this->barWidth = width-width*0.1;
	this->barXPos = xPos+(width-barWidth)/2;
	this->barYPos = yPos-height/1.5;
	this->barZPos = zPos+0.5;
	
	this->sliderXPos = barXPos - barWidth*0.012;
	this->sliderYPos = yPos-height/1.7;
	this->sliderZPos = barZPos+0.5;
	this->sliderWidth = width/5*0.1;
	this->sliderHeight = height*0.15;
	
	this->caption=caption;
	this->menuInfo=menuString;
	this->isSliderClicked = false;

	//count number of "arguments" in menuString
	int counter=0;
	const char* strlength=this->menuInfo;
	for(int i=0;i<strlen(strlength);i++){
		char test = this->menuInfo[i];
		if(test=='/') counter++;
	}
	this->numberOfOptions=counter;
	this->interval = barWidth/(numberOfOptions-1.0);	// if it's divided by an integer, the whole thing becomes an integer value???

	char insert_container[256];
	memset(insert_container,0,256);
	this->allOptions=new char*[this->numberOfOptions];
	int items=0;
	for(int i=0;i<strlen(strlength);i++){
		int j=0;
		while(this->menuInfo[i]!='/'){
			insert_container[j++]=this->menuInfo[i++];
		}
		char* next=insert_container;
		this->allOptions[items]=new char[strlen(next)+1];
		strcpy(this->allOptions[items++],next);
		memset(insert_container,0,256);
	}
		
	this->menuState=sliderStartingIndex;
	this->buttonState=0;
	this->currentOption=NULL;
	this->optionText=NULL;
	this->setOptionText(menuState); //set option to first option
	/*	BUTTON TEXT PLACEMENT	*/
	const char* cpchar = this->caption;
	int length = strlen(cpchar);
	char* pchar = this->caption;
	int realLength=0;
	for(int i=0;i<length;i++){
		realLength+=glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24,*(pchar));
		*(pchar)++;
	}
	GLfloat labelXPos=barXPos;
	GLfloat labelYPos=this->yPos-this->height*0.45;
	/*	END OF BUTTON TEXT PLACEMENT	*/
	this->label = new TextObject(this->caption,labelXPos,labelYPos,this->zPos,GLUT_BITMAP_TIMES_ROMAN_24,0.0f,0.0f,0.0f);
}
ControlItemSliderbar::~ControlItemSliderbar() {
	delete optionText;
	delete label;
	delete[] currentOption;
	for(int i=0 ; i<numberOfOptions ; i++)
		delete[] allOptions[i];
	delete[] allOptions;
}






void ControlItemSliderbar::draw(){
	//draw main button box
	glBegin(GL_QUADS);
		glColor4f(this->color[0]-0.2f, this->color[1]-0.2f, this->color[2]-0.2f, this->color[3]);
		glVertex3f(this->xPos, this->yPos, this->zPos);
		glVertex3f(this->xPos-3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos+width, this->yPos, this->zPos);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]-0.2f, this->color[1]-0.2f, this->color[2]-0.2f, this->color[3]);
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
		glColor4f(this->color[0]+0.4f, this->color[1]+0.4f, this->color[2]+0.4f, this->color[3]);
		glVertex3f(this->xPos-3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos+width, this->yPos-height, this->zPos);
		glVertex3f(this->xPos, this->yPos-height, this->zPos);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(this->color[0]+0.4f, this->color[1]+0.4f, this->color[2]+0.4f, this->color[3]);
		glVertex3f(this->xPos+width, this->yPos, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos+3, this->zPos);
		glVertex3f(this->xPos+width+3, this->yPos-height-3, this->zPos);
		glVertex3f(this->xPos+width, this->yPos+-height, this->zPos);
	glEnd();

	// draw bar lines
	glBegin(GL_LINES);
		glColor4f(0, 0, 0, 1);
		glVertex3f(this->barXPos, this->barYPos+1, this->barZPos);
		glVertex3f(this->barXPos+this->barWidth, this->barYPos+1, this->barZPos);
	glEnd();
	glBegin(GL_LINES);
		glColor4f(0, 0, 0, 1);
		glVertex3f(this->barXPos, this->barYPos, this->barZPos);
		glVertex3f(this->barXPos+this->barWidth, this->barYPos, this->barZPos);
	glEnd();
	glBegin(GL_LINES);
		glColor4f(0, 0, 0, 1);
		glVertex3f(this->barXPos, this->barYPos-1, this->barZPos);
		glVertex3f(this->barXPos+this->barWidth, this->barYPos-1, this->barZPos);
	glEnd();
	for(int i=0 ; i<this->numberOfOptions ; i++)	{
		glBegin(GL_LINES);
			glColor3f(0, 0, 0);
			glVertex3f(barXPos+(interval*i), barYPos+this->height*0.07, barZPos);
			glVertex3f(barXPos+(interval*i), barYPos-this->height*0.07, barZPos);
		glEnd();
	}

	// draw slider
	if(!isSliderClicked) 	{
		glBegin(GL_QUADS);
		glColor4f(this->color[0]+0.2f, this->color[1]+0.2f, this->color[2]+0.2f, this->color[3]);
			glVertex3f(this->sliderXPos, this->sliderYPos, this->sliderZPos);
			glVertex3f(this->sliderXPos-3, this->sliderYPos+3, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth+3, this->sliderYPos+3, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth, this->sliderYPos, this->sliderZPos);
		glEnd();
		glBegin(GL_QUADS);
			glColor4f(this->color[0]+0.2f, this->color[1]+0.2f, this->color[2]+0.2f, this->color[3]);
			glVertex3f(this->sliderXPos-3, this->sliderYPos+3, this->sliderZPos);
			glVertex3f(this->sliderXPos-3, this->sliderYPos-sliderHeight-3, this->sliderZPos);
			glVertex3f(this->sliderXPos, this->sliderYPos-sliderHeight, this->sliderZPos);
			glVertex3f(this->sliderXPos, this->sliderYPos, this->sliderZPos);
		glEnd();
		glBegin(GL_QUADS);
			glColor4f(this->color[0], this->color[1], this->color[2], this->color[3]);
			glVertex3f(this->sliderXPos, this->sliderYPos, this->sliderZPos);
			glVertex3f(this->sliderXPos, this->sliderYPos-sliderHeight, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth, this->sliderYPos-sliderHeight, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth, this->sliderYPos, this->sliderZPos);
		glEnd();
		glBegin(GL_QUADS);
			glColor4f(this->color[0]-0.4f, this->color[1]-0.4f, this->color[2]-0.4f, this->color[3]);
			glVertex3f(this->sliderXPos-3, this->sliderYPos-sliderHeight-3, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth+3, this->sliderYPos-sliderHeight-3, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth, this->sliderYPos-sliderHeight, this->sliderZPos);
			glVertex3f(this->sliderXPos, this->sliderYPos-sliderHeight, this->sliderZPos);
		glEnd();
		glBegin(GL_QUADS);
			glColor4f(this->color[0]-0.4f, this->color[1]-0.4f, this->color[2]-0.4f, this->color[3]);
			glVertex3f(this->sliderXPos+sliderWidth, this->sliderYPos, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth+3, this->sliderYPos+3, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth+3, this->sliderYPos-sliderHeight-3, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth, this->sliderYPos+-sliderHeight, this->sliderZPos);
		glEnd();

	}else{
		glBegin(GL_QUADS);
		glColor4f(this->color[0]+0.4f, this->color[1]+0.4f, this->color[2]+0.4f, this->color[3]);
			glVertex3f(this->sliderXPos, this->sliderYPos, this->sliderZPos);
			glVertex3f(this->sliderXPos-3, this->sliderYPos+3, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth+3, this->sliderYPos+3, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth, this->sliderYPos, this->sliderZPos);
		glEnd();
		glBegin(GL_QUADS);
			glColor4f(this->color[0]+0.4f, this->color[1]+0.4f, this->color[2]+0.4f, this->color[3]);
			glVertex3f(this->sliderXPos-3, this->sliderYPos+3, this->sliderZPos);
			glVertex3f(this->sliderXPos-3, this->sliderYPos-sliderHeight-3, this->sliderZPos);
			glVertex3f(this->sliderXPos, this->sliderYPos-sliderHeight, this->sliderZPos);
			glVertex3f(this->sliderXPos, this->sliderYPos, this->sliderZPos);
		glEnd();
		glBegin(GL_QUADS);
			glColor4f(this->color[0]+0.2f, this->color[1]+0.2f, this->color[2]+0.2f, this->color[3]);
			glVertex3f(this->sliderXPos, this->sliderYPos, this->sliderZPos);
			glVertex3f(this->sliderXPos, this->sliderYPos-sliderHeight, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth, this->sliderYPos-sliderHeight, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth, this->sliderYPos, this->sliderZPos);
		glEnd();
		glBegin(GL_QUADS);
			glColor4f(this->color[0]-0.2f, this->color[1]-0.2f, this->color[2]-0.2f, this->color[3]);
			glVertex3f(this->sliderXPos-3, this->sliderYPos-sliderHeight-3, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth+3, this->sliderYPos-sliderHeight-3, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth, this->sliderYPos-sliderHeight, this->sliderZPos);
			glVertex3f(this->sliderXPos, this->sliderYPos-sliderHeight, this->sliderZPos);
		glEnd();
		glBegin(GL_QUADS);
			glColor4f(this->color[0]-0.2f, this->color[1]-0.2f, this->color[2]-0.2f, this->color[3]);
			glVertex3f(this->sliderXPos+sliderWidth, this->sliderYPos, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth+3, this->sliderYPos+3, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth+3, this->sliderYPos-sliderHeight-3, this->sliderZPos);
			glVertex3f(this->sliderXPos+sliderWidth, this->sliderYPos+-sliderHeight, this->sliderZPos);
		glEnd();
	}

	this->label->draw();
	this->optionText->draw();
}






GLfloat ControlItemSliderbar::getXPos()			{ return this->xPos;}
GLfloat ControlItemSliderbar::getYPos()			{ return this->yPos;}
GLfloat ControlItemSliderbar::getHeight()			{ return this->height;}
GLfloat ControlItemSliderbar::getWidth()			{ return this->width;}
GLfloat ControlItemSliderbar::getBarXPos()		{ return this->barXPos;}
GLfloat ControlItemSliderbar::getInterval()		{ return this->interval;}
GLfloat ControlItemSliderbar::getSliderXPos()	{ return this->sliderXPos;}
void ControlItemSliderbar::setSliderXPos(GLfloat x)		{this->sliderXPos = x;}
char* ControlItemSliderbar::collectData()		{ return this->currentOption;}



void ControlItemSliderbar::setOptionText(char* newText){
}

void ControlItemSliderbar::setOptionText(int index){
	if(this->currentOption){
		delete[] this->currentOption;
	}
	this->currentOption=new char[strlen(this->allOptions[index])+1];
	strcpy(this->currentOption,this->allOptions[index]);
	int realLength=0;
	int wordLength=strlen(this->currentOption);
	for(int i=0;i<wordLength;i++){
		realLength+=glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24,this->currentOption[i]);
	}
	GLfloat labelXPos=this->xPos+(this->width/2)-(realLength/2);
	GLfloat labelYPos=this->yPos-this->height*0.45;
	delete optionText;
	this->optionText = new TextObject(this->currentOption,labelXPos,labelYPos,this->zPos,GLUT_BITMAP_TIMES_ROMAN_24,0.0f,0.0f,0.0f);
	this->menuState = index;
	this->sliderXPos = this->barXPos+interval*index;
}






void ControlItemSliderbar::mouseClickEvent(GLint x, GLint y, GLint state,  bool stillOverControlItemSliderbar){
	if(state == 1){
		// check if the click is on the slider
		if((sliderXPos<x && x<sliderXPos+sliderWidth) && (sliderYPos-sliderHeight<y && y<sliderYPos)){
			this->isSliderClicked = true;
		}
		// check if the click is either left or right side from the slider
		else if((barXPos<x && x<sliderXPos) && (sliderYPos-sliderHeight-5.5<y && y<sliderYPos+5.5)){
			playSFX(SMALL_CLICK);
			sliderXPos -= interval;
			this->menuState--;
			if(this->menuState<0)			//wrap around check
				this->menuState+=numberOfOptions;
			this->setOptionText(menuState);
		}
		else if((sliderXPos+sliderWidth<x && x<barXPos+barWidth) && (sliderYPos-sliderHeight-5.5<y && y<sliderYPos+5.5)){
			playSFX(SMALL_CLICK);
			sliderXPos += interval;
			this->menuState++;
			if(this->menuState==this->numberOfOptions)			//wrap around check
				this->menuState-=numberOfOptions;
			this->setOptionText(menuState);
		}
	}else{
		this->isSliderClicked = false;
	}
}





void ControlItemSliderbar::updateMouse(int x, int y){
	if(isSliderClicked){
		// check if the mouse pointer is either left or right side from the slider
		if(barXPos<x && x<sliderXPos-interval/2){
			sliderXPos -= interval;
			this->menuState--;
			if(this->menuState<0)			//wrap around check
				this->menuState+=numberOfOptions;
			this->setOptionText(menuState);
			playSFX(SMALL_CLICK);
		}
		else if(sliderXPos+interval/2 + sliderWidth<x && x<barXPos+barWidth){
			sliderXPos += interval;
			this->menuState++;
			if(this->menuState==this->numberOfOptions)			//wrap around check
				this->menuState-=numberOfOptions;
			this->setOptionText(menuState);
			playSFX(SMALL_CLICK);
		}
	}
}


