#include "ControlItem.h"
#include "ControlItemCheckBox.h"
#include "TextObject.h"
#include "sound.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "macro_crtdbg.h"

using namespace std;

extern void playSFX(int sfx);

ControlItemCheckBox::ControlItemCheckBox() {
}

ControlItemCheckBox::ControlItemCheckBox(		GLfloat xPos,GLfloat yPos,GLfloat zPos,
												GLfloat red,GLfloat green,GLfloat blue,
												GLint width,GLint height,char* caption) {

	this->xPos = xPos; this->yPos = yPos; this->zPos = zPos;
	this->color[0] = red; this->color[1] = green; this->color[2] = blue;
	this->color[3] = 1.0;
	this->width = width; this->height = height; 
	this->caption=caption;

	this->buttonState=0;
	this->menuState=0;
	
	/*	BUTTON TEXT PLACEMENT	*/
	const char* cpchar = this->caption;
	int length = strlen(cpchar);
	char* pchar = this->caption;
	int realLength=0;
	for(int i=0;i<length;i++){
		realLength+=glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24,*(pchar));
		*(pchar)++;
	}

	GLfloat labelXPos=this->xPos+(this->width/2)-(realLength/2);
	GLfloat labelYPos=this->yPos+((this->yPos-(this->yPos+this->height))/2)-this->height/4;
	/*	END OF BUTTON TEXT PLACEMENT	*/
	this->label = new TextObject(this->caption,labelXPos,labelYPos,this->zPos,GLUT_BITMAP_TIMES_ROMAN_24,0.0f,0.0f,0.0f);
	
}

ControlItemCheckBox::~ControlItemCheckBox() {
	delete this->label;
}


void ControlItemCheckBox::draw(){

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


	//draw the actual check box itself
	//composed of 4 smallers squares, the inside vertex of each square gets colored darker when pressed
	//square order:    1 4							
	//				   2 3								
	//NOTE: Squares are actually composed of 2 triangles, drawn counter clockwise
	//      You can identify the innermost (center) vertex by the if statement preceding it

	
	
	//square 1
	glBegin(GL_TRIANGLES);
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);								
		glVertex3f(this->xPos+(width-height*0.9),(this->yPos-height*0.1),this->zPos+1);											
		glVertex3f(this->xPos+(width-height*0.9),(this->yPos-height/2),this->zPos+1);									
		if(this->buttonState==1) glColor3f(this->color[0]-.2, this->color[1]-.2, this->color[2]-.2);
		glVertex3f(this->xPos+(width-height/2),(this->yPos-height/2),this->zPos+1);
	glEnd();
	glBegin(GL_TRIANGLES);
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);
		glVertex3f(this->xPos+(width-height*0.9),(this->yPos-height*0.1),this->zPos+1);
		if(this->buttonState==1) glColor3f(this->color[0]-.2, this->color[1]-.2, this->color[2]-.2);
		glVertex3f(this->xPos+(width-height/2),(this->yPos-height/2),this->zPos+1);
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);
		glVertex3f(this->xPos+(width-height/2),(this->yPos-height*0.1),this->zPos+1);
	glEnd();

	//square 2
	glBegin(GL_TRIANGLES);
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);								
		glVertex3f(this->xPos+(width-height*0.9),(this->yPos-height/2),this->zPos+1);											
		glVertex3f(this->xPos+(width-height*0.9),(this->yPos-height*0.9),this->zPos+1);									
		if(this->buttonState==1) glColor3f(this->color[0]-.2, this->color[1]-.2, this->color[2]-.2);
		glVertex3f(this->xPos+(width-height/2),(this->yPos-height/2),this->zPos+1);
	glEnd();
	glBegin(GL_TRIANGLES);
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);																		
		glVertex3f(this->xPos+(width-height*0.9),(this->yPos-height*0.9),this->zPos+1);									
		glVertex3f(this->xPos+(width-height/2),(this->yPos-height*0.9),this->zPos+1);
		if(this->buttonState==1) glColor3f(this->color[0]-.2, this->color[1]-.2, this->color[2]-.2);
		glVertex3f(this->xPos+(width-height/2),(this->yPos-height/2),this->zPos+1);
	glEnd();

	//square 3
	glBegin(GL_TRIANGLES);
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);
		if(this->buttonState==1) glColor3f(this->color[0]-.2, this->color[1]-.2, this->color[2]-.2);								
		glVertex3f(this->xPos+(width-height/2),(this->yPos-height/2),this->zPos+1);	
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);
		glVertex3f(this->xPos+(width-height/2),(this->yPos-height*0.9),this->zPos+1);									
		glVertex3f(this->xPos+(width-height)+height*0.9,(this->yPos-height*0.9),this->zPos+1);
	glEnd();
	glBegin(GL_TRIANGLES);
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);
		if(this->buttonState==1) glColor3f(this->color[0]-.2, this->color[1]-.2, this->color[2]-.2);								
		glVertex3f(this->xPos+(width-height/2),(this->yPos-height/2),this->zPos+1);	
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);									
		glVertex3f(this->xPos+(width-height*0.1),(this->yPos-height*0.9),this->zPos+1);
		glVertex3f(this->xPos+(width-height*0.1),(this->yPos-height/2),this->zPos+1);
	glEnd();

	//square 4
	glBegin(GL_TRIANGLES);
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);								
		glVertex3f(this->xPos+(width-height/2),(this->yPos-height*0.1),this->zPos+1);		
		if(this->buttonState==1) glColor3f(this->color[0]-.2, this->color[1]-.2, this->color[2]-.2);
		glVertex3f(this->xPos+(width-height)+(height/2),(this->yPos-height/2),this->zPos+1);									
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);
		glVertex3f(this->xPos+(width-height*0.1),(this->yPos-height*0.1),this->zPos+1);
	glEnd();
	glBegin(GL_TRIANGLES);
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);
		if(this->buttonState==1) glColor3f(this->color[0]-.2, this->color[1]-.2, this->color[2]-.2);
		glVertex3f(this->xPos+(width-height)+(height/2),(this->yPos-height/2),this->zPos+1);
		glColor3f(this->color[0]+.2, this->color[1]+.2, this->color[2]+.2);			
		glVertex3f(this->xPos+(width-height*0.1),(this->yPos-height/2),this->zPos+1);
		glVertex3f(this->xPos+(width-height*0.1),(this->yPos-height*0.1),this->zPos+1);
	glEnd();

	
	//draw check mark if it was toggled on, otherwise dont
	if(menuState==1){
		glColor3f(0.0f,1.0f,0.0f);
		glBegin(GL_QUADS);		
			glVertex3f(this->xPos+(width-height+(height*0.2)),(this->yPos-height/2)+(0.015*width),this->zPos+2);											
			glVertex3f(this->xPos+(width-height+(height*0.2)),(this->yPos-height/2)+(0.005*width),this->zPos+2);									
			glVertex3f(this->xPos+(width-height/2),(this->yPos-height/2)-(0.015*width),this->zPos+2);
			glVertex3f(this->xPos+(width-height/2),(this->yPos-height/2),this->zPos+2);
		glEnd();
		glBegin(GL_QUADS);											
			glVertex3f(this->xPos+(width-height/2),(this->yPos-height/2)-(0.015*width),this->zPos+2);
			glVertex3f(this->xPos+(width-height/2),(this->yPos-height/2),this->zPos+2);
			glVertex3f(this->xPos+(width-height)+(0.95*height),(this->yPos-height/2)+(0.025*width),this->zPos+2);											
			glVertex3f(this->xPos+(width-height)+(0.95*height),(this->yPos-height/2)+(0.015*width),this->zPos+2);
		glEnd();
	}

	this->label->draw();

}


GLfloat ControlItemCheckBox::getXPos(){ return this->xPos;}
GLfloat ControlItemCheckBox::getYPos(){ return this->yPos;}
GLfloat ControlItemCheckBox::getHeight(){ return this->height;}
GLfloat ControlItemCheckBox::getWidth(){ return this->width;}
char* ControlItemCheckBox::collectData(){
	if(this->menuState==0) return "false";
	else return "true";
}

void ControlItemCheckBox::setOptionText(int index){
}
void ControlItemCheckBox::setOptionText(char* newText){
}


//NOTE: I use this->height for the x value check, this is intentional to maintain a square
void ControlItemCheckBox::mouseClickEvent(GLint x, GLint y, GLint state,  bool stillOverControlItemCheckBox){
	if(	 (x>=(this->xPos+this->width-(this->height*0.9))	&&	(x<=this->xPos+this->width-(this->height*0.1)))
														    &&	
		((y<=(this->yPos-(this->height*0.1)))				&&	(y>=(this->yPos-this->height*0.9)))) { 
		if(state==1) {																
			this->buttonState=1;	
		} else if(state==0){
			if(stillOverControlItemCheckBox) {												
				if(this->menuState==1) this->menuState=0;
				else this->menuState=1;
				playSFX(SMALL_CLICK);
			}
			
			this->buttonState=0;
		}
	}
	
	if(state==0) {
		this->buttonState=0;	
	}
}


void ControlItemCheckBox::updateMouse(int x, int y){
}