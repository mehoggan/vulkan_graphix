#include "SubMenu.h"
#include "SubMenuSound.h"
#include "TextObject.h"
#include "ControlItem.h"
#include "ControlItemSelectionBox.h"
#include "ControlItemCheckBox.h"
#include "sound.h"
#include "ControlItemSliderbar.h"
#include <string>
#include "macro_crtdbg.h"


using namespace std;

SubMenuSound::SubMenuSound(){
}

SubMenuSound::SubMenuSound(	int ID,
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
	this->buttonPressed = NULL;
	/*
	this->subMenuButton[0] = new ControlItemSelectionBox(this->xPos+(this->width/2)-(0.3*this->width),this->yPos-(this->height*0.2), this->zPos+1,
														0.5f, 0.5f, 0.5f,
														0.6f*this->width,0.06*(this->height),
														"SFX Volume","100/0/10/20/30/40/50/60/70/80/90/");
	this->subMenuButton[1] = new ControlItemSelectionBox(	this->xPos+(this->width/2)-(0.3*this->width),this->yPos-(this->height*0.27), this->zPos+1,
														0.5f, 0.5f, 0.5f,
														0.6f*this->width,0.06*(this->height),
														"Music Volume","100/0/10/20/30/40/50/60/70/80/90/");
	//*/
	this->subMenuButton[0] = new ControlItemSliderbar(this->xPos+(this->width/2)-(0.3*this->width),this->yPos-(this->height*0.2), this->zPos+1,
														0.5f, 0.5f, 0.5f,
														0.6f*this->width,0.07*(this->height),
														"SFX Volume","0/10/20/30/40/50/60/70/80/90/100/", 10);
	this->subMenuButton[1] = new ControlItemSliderbar(	this->xPos+(this->width/2)-(0.3*this->width),this->yPos-(this->height*0.3), this->zPos+1,
														0.5f, 0.5f, 0.5f,
														0.6f*this->width,0.07*(this->height),
														"Music Volume","0/10/20/30/40/50/60/70/80/90/100/", 10);
}

SubMenuSound::~SubMenuSound(){
	for(int i=0 ; i<NUM_CONTROL_ITEMS_SND ; i++)
		delete subMenuButton[i];
	delete label;
}

int SubMenuSound::getUNIQUEIDENTIFIER(){return this->UNIQUEIDENTIFIER;}
void SubMenuSound::setUNIQUEIDENTIFIER(int ID){this->UNIQUEIDENTIFIER=ID;}
GLfloat SubMenuSound::getXPos(){return this->xPos;}
void SubMenuSound::setXPos(GLfloat newXpos){this->xPos=newXpos;}
GLfloat SubMenuSound::getYPos(){return this->yPos;}
void SubMenuSound::setYPos(GLfloat newYpos){this->yPos=newYpos;}
GLfloat SubMenuSound::getZPos(){return this->zPos;}
void SubMenuSound::setZPos(GLfloat newZpos){this->zPos=newZpos;}
GLfloat SubMenuSound::getRed(){return this->color[0];}
void SubMenuSound::setRed(GLfloat red){this->color[0]=red;}
GLfloat SubMenuSound::getGreen(){return this->color[1];}
void SubMenuSound::setGreen(GLfloat green){this->color[1]=green;}
GLfloat SubMenuSound::getBlue(){return this->color[2];}
void SubMenuSound::setBlue(GLfloat blue){this->color[2]=blue;}
GLint SubMenuSound::getWidth(){return this->width;}
void SubMenuSound::setWdith(GLint width){this->width=width;}
GLint SubMenuSound::getHeight(){return this->height;}
void SubMenuSound::setHeight(GLint height){this->height=height;}
char* SubMenuSound::getCaption(){return this->caption;}
void SubMenuSound::setCaption(char* caption){this->caption=caption;}
GLfloat SubMenuSound::getPerecentBorder(){return this->percentBorder;}
void SubMenuSound::setPercentBorder(GLfloat percent){this->percentBorder=percentBorder;}

void SubMenuSound::draw(){
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
	for(int i=0 ; i<NUM_CONTROL_ITEMS_SND ; i++){
		if(this->subMenuButton[i]){
			this->subMenuButton[i]->draw();
		}
	}
}

const char* SubMenuSound::collectData() {
	char* retrn="Sound:";
	const char* realretrn=retrn;
	return realretrn;
}

void SubMenuSound::subMenuMouseTest(int x, int y, int buttonDown){
	if(buttonDown){																		//FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
		for(int button_i=0;button_i<NUM_CONTROL_ITEMS_SND;button_i++){								//SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
																								//IF YOU DID NOT CLICK A BUTTON PERHAPS YOU CLICKED A ARROW BUTTON???
			if	((x>=this->subMenuButton[button_i]->getXPos())	&&	(x<=(this->subMenuButton[button_i]->getXPos()+this->subMenuButton[button_i]->getWidth()))
																			&&	
				(y<=this->subMenuButton[button_i]->getYPos())	&&	(y>=(this->subMenuButton[button_i]->getYPos()-this->subMenuButton[button_i]->getHeight()))){
					this->subMenuButton[button_i]->mouseClickEvent(x,y,buttonDown,true);	//YOU PRESSED OVER A ARROWBUTTON
					this->buttonPressed=this->subMenuButton[button_i];
			}
		}
	}else if(!buttonDown){																//IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON GOES UP
		if(this->buttonPressed!=NULL){										//IF YOU MANAGED TO CLICK INSIDE AN ARROW BUTTON 
																						//CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
			if	((x>=this->buttonPressed->getXPos())		&&	(x<=(this->buttonPressed->getXPos()+this->buttonPressed->getWidth()))
																&&	
				(y<=this->buttonPressed->getYPos())		&&	(y>=(this->buttonPressed->getYPos()-this->buttonPressed->getHeight()))) {
					this->buttonPressed->mouseClickEvent(x,y,buttonDown,true);			//IF YOU ARE THEN TELL THE ARROW BUTTON YOU RELEASE THE MOUSE
					changeVolumes(this->buttonPressed);
			}else {
				this->buttonPressed->mouseClickEvent(x,y,buttonDown,false);		//IF YOU ARE THEN TELL THE ARROW BUTTON YOU RELEASE THE MOUSE
				this->buttonPressed=NULL;
			}
		}
	}
}

void SubMenuSound::updateMouse(int x, int y){
	this->subMenuButton[0]->updateMouse(x, y);
	this->subMenuButton[1]->updateMouse(x, y);
	changeVolumes(this->buttonPressed);
}

void SubMenuSound::changeVolumes(ControlItem* TheSubMenuButton){
	// SFX volume handler
	if (TheSubMenuButton == this->subMenuButton[0]){
		int newVolume = atoi(TheSubMenuButton->collectData());
		Mix_Volume(-1, (int)128/100*newVolume);	//-1 is to apply to all allocated channels, 128 is the maximum volume
	}
	// Music volume handler
	else if(TheSubMenuButton == this->subMenuButton[1]){
		int newVolume = atoi(TheSubMenuButton->collectData());
		Mix_VolumeMusic((int)128/100*newVolume);	// music has its special channel, so don't need to specify which channel.
	}
}