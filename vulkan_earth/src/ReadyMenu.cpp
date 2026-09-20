#include "MainMenu.h"
#include "ReadyMenu.h"
#include "ShopMenu.h"
#include "MainMenuButton.h"
#include "TextObject.h"
#include "ImageObject.h"
#include "ControlItem.h"
#include "ControlItemSelectionBox.h"
#include "ControlItemSliderbar.h"
#include "ControlItemTextField.h"
#include "GlobalSettings.h"
#include "PlayerFactory.h"
#include "sound.h"
#include "possibleGameStates.h"
#include "Tank.h"
#include "TankA.h"
#include "TankB.h"
#include "TankC.h"
#include "TankD.h"
#include "TankE.h"
#include "TankF.h"
#include "TankG.h"
#include "TankH.h"
#include "Player.h"
#include "PlayerCPU.h"
#include "PlayerHuman.h"
#include "TerrainMaker.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "macro_crtdbg.h"
#include "Vertex.h"

extern void playMusic(int music);

using namespace std;

ReadyMenu::ReadyMenu(){
}
ReadyMenu::ReadyMenu(GLfloat width, GLfloat height, GLfloat percentBorder, GlobalSettings* global_settings, PlayerFactory* player_factory, int* gameState){
	startMusicPlayed = false;
	this->global_settings = global_settings;
	this->player_factory = player_factory;
	this->currentGameState = gameState;

	numPlayers = MAX_NUM_PLAYERS;
	for(int i=0;i<NUM_BUTTONS;i++){
		this->buttons[i]=NULL;
	}
	for(int i=0;i<NUM_STAT_IMAGES;i++){
		this->statImages[i]=NULL;
	}
	for(int i=0;i<NUM_CONTROL_ITEMS;i++){
		this->controlItems[i]=NULL;
	}
		
	this->buttonPressed=NULL;
	this->width = width;
	this->height = height;
	this->percentBorder = percentBorder;
	this->pos[0] = this->pos[1] = this->pos[2] = 0;
	this->color[0] = this->color[1] = this->color[2] = 1;
	this->color[3] = 1;
	this->tankPrvScrPos[0] = this->width*0.07;
	this->tankPrvScrPos[1] = this->height*0.40;
	this->tankPrvScrPos[2] = 0.1;
	this->tankPrvScrWidth = this->width*0.35;
	this->tankPrvScrHeight = this->height*0.45;
	this->tankPrvScrColor[0] = this->tankPrvScrColor[1] = this->tankPrvScrColor[2] = 0;
	this->prvScrColorControl = 1;
	
	this->caption = "Player 1";
	this->currentPlayerIndex = 0;

	/*BUTTONS AND CONTROL ITEMS*/
	this->buttons[0] = new MainMenuButton(	0, this->pos[0]-this->width*0.35, this->pos[1]+this->height*0.35, 1,
											this->player_factory->collectPlayerColor(currentPlayerIndex)[0],
											this->player_factory->collectPlayerColor(currentPlayerIndex)[1],
											this->player_factory->collectPlayerColor(currentPlayerIndex)[2],
											0.1*(this->width), 0.04*(this->height),
											"CPU", NULL);
											
	this->buttons[0]->pressButton();
	Mix_HaltChannel(0);
	this->buttons[0]->activateSubMenu();
	this->buttons[1] = new MainMenuButton(	1, this->pos[0]-this->width*0.2, this->pos[1]+this->height*0.35, 1,
											this->player_factory->collectPlayerColor(currentPlayerIndex)[0],
											this->player_factory->collectPlayerColor(currentPlayerIndex)[1],
											this->player_factory->collectPlayerColor(currentPlayerIndex)[2],
											0.1*(this->width), 0.04*(this->height),
											"HUMAN", NULL);
	this->buttons[2] = new MainMenuButton(	2, this->pos[0]+this->width*0.125, this->pos[1]-this->height*0.35, 1,
											0.65, 0.15, 0.15,
											0.1*(this->width), 0.04*(this->height),
											"Back", NULL);
	this->buttons[3] = new MainMenuButton(	3, this->pos[0]+this->width*0.275, this->pos[1]-this->height*0.35, 1,
											0.75, 0.75, 0.75,
											0.1*(this->width), 0.04*(this->height),
											"Next", NULL);
	this->controlItems[0] = new ControlItemSelectionBox(this->pos[0]-this->width*0.375, this->pos[1]+this->height*0.275, 1, 
														0.55, 0.55, 0.55,
														0.175*(this->width), 0.04*(this->height),
														"AI", "Moron/Tosser/Cyborg/Shooter/Chooser/Poolshark/Spoiler/Unknown/");
	this->controlItems[1] = new ControlItemSliderbar(	this->pos[0]-this->width*0.35, this->pos[1]-this->height*0.24, 1,
														0.55, 0.55, 0.55,
														0.7*(this->width), 0.06*(this->height),
														"Tank Type", "Rhinoxx/Hellfire/HeavyD/Panzer/Eggroid/Behemoth/Cubix/Predator/", 0);
	this->controlItems[2] = new ControlItemSelectionBox(this->pos[0]-this->width*0.175, this->pos[1]+this->height*0.275, 1, 
														0.55, 0.55, 0.55,
														0.1*(this->width), 0.04*(this->height),
														"Team", "-/1/2/3/4/5/");
	this->textField = new ControlItemTextField(	this->pos[0]-this->width*0.375, this->pos[1]+this->height*0.275, 1, 
												1.0, 1.0, 1.0,
												0.175*(this->width), 0.04*(this->height));

	/*LABEL PLACEMENT*/
	this->playerPageNum = new TextObject(this->caption,pos[0]-this->width*0.25,pos[1]+this->height*0.4,(this->pos[2]+1),GLUT_BITMAP_TIMES_ROMAN_24,0.0f,0.0f,0.0f);

	char* stat = new char[30];
	for(int i=0 ; i<NUM_TANK_STATS ; i++){
		if(i==0)				strcpy(stat, "Power:");
		else if(i==1)		strcpy(stat, "Armor:");
		else if(i==2)		strcpy(stat, "Speed:");
		else					strcpy(stat, "Meh...:");
		GLfloat statLabelXPos = this->pos[0] - this->width*0.385;
		GLfloat statLabelYPos = this->pos[1] + this->height*0.12 - this->height*(i*0.07);
		this->tankStatLabels[i] = new TextObject(stat, statLabelXPos, statLabelYPos, (this->pos[2]+1), GLUT_BITMAP_TIMES_ROMAN_24,0.0f,0.0f,0.0f);
	}
	delete[] stat;

	// TANKS
	tanks[0] = new TankA(0,0,0);
	tanks[1] = new TankB(0,0,0);
	tanks[2] = new TankC(0,0,0);
	tanks[3] = new TankD(0,0,0);
	tanks[4] = new TankE(0,0,0);
	tanks[5] = new TankF(0,0,0);
	tanks[6] = new TankG(0,0,0);
	tanks[7] = new TankH(0,0,0);
	tankAngle = 0;

	for(int i=0 ; i<NUM_TANK_TYPES ; i++){
		tanks[i]->setTankPos(0, 0, 0);
	}

	// STAT IMAGES
	GLfloat imgStartPosX = this->pos[0] - this->width*0.325;
	for(int i=0 ; i<NUM_STAT_IMAGES ; i++){
		// For Off Lights
		if(i < 30){
			// For Power Lights
			if(i<10){
				statImages[i] =	new ImageObject(imgStartPosX+this->width*(i%10)*0.03, this->pos[1]+height*0.14 - this->height*((i/10)*0.07), this->pos[2]+0.5, this->width*0.03, this->height*0.032, 0.0006*(this->width), 64, 64, "lightOff.raw");
			}
			// For Armor Lights
			else if(i<20){
				statImages[i] =	new ImageObject(imgStartPosX+this->width*(i%10)*0.03, this->pos[1]+height*0.14 - this->height*((i/10)*0.07), this->pos[2]+0.5, this->width*0.03, this->height*0.032, 0.0006*(this->width), 64, 64, "lightOff.raw");
			}
			// For Speed Lights
			else{
				statImages[i] =	new ImageObject(imgStartPosX+this->width*(i%10)*0.03, this->pos[1]+height*0.14 - this->height*((i/10)*0.07), this->pos[2]+0.5, this->width*0.03, this->height*0.032, 0.0006*(this->width), 64, 64, "lightOff.raw");
			}
		}
		// For On Lights
		else{
			// For Power Lights
			if(i<40){
				statImages[i] =	new ImageObject(imgStartPosX+this->width*(i%10)*0.03, this->pos[1]+height*0.14 - this->height*(((i-30)/10)*0.07), this->pos[2]+1, this->width*0.03, this->height*0.032, 0.0006*(this->width), 64, 64, "lightRed.raw");
			}
			// For Armor Lights
			else if(i<50){
				statImages[i] =	new ImageObject(imgStartPosX+this->width*(i%10)*0.03, this->pos[1]+height*0.14 - this->height*(((i-30)/10)*0.07), this->pos[2]+1, this->width*0.03, this->height*0.032, 0.0006*(this->width), 64, 64, "lightBlue.raw");
			}
			// For Speed Lights
			else{
				statImages[i] =	new ImageObject(imgStartPosX+this->width*(i%10)*0.03, this->pos[1]+height*0.14 - this->height*(((i-30)/10)*0.07), this->pos[2]+1, this->width*0.03, this->height*0.032, 0.0006*(this->width), 64, 64, "lightGreen.raw");
			}
		}
	}

	this->updatePageInfo();
	
}

ReadyMenu::~ReadyMenu(){
	for(int i=0;i<NUM_BUTTONS;i++)
		delete this->buttons[i];
	for(int i=0;i<NUM_STAT_IMAGES;i++)
		delete this->statImages[i];
	for(int i=0;i<NUM_CONTROL_ITEMS;i++)
		delete this->controlItems[i];
	for(int i=0;i<NUM_TANK_TYPES;i++)
		delete this->tanks[i];
	for(int i=0 ; i<NUM_TANK_STATS ; i++)
		delete this->tankStatLabels[i];
	delete this->textField;
	delete this->playerPageNum;
}

// GETTERS & SETTERS //
GLfloat* ReadyMenu::getPos()		{return &(this->pos[0]);}
GLfloat ReadyMenu::getWidth()		{return this->width;}
GLfloat ReadyMenu::getHeight()		{return this->height;}
GLfloat* ReadyMenu::getColor()		{return &(this->color[0]);}
void ReadyMenu::setWidth(GLfloat width)		{this->width = width;}
void ReadyMenu::setHeight(GLfloat height)	{this->height = height;}
void ReadyMenu::updateNumPlayers(int n)	{this->numPlayers = n;}
void ReadyMenu::setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a){
	this->color[0] = r;
	this->color[1] = g;
	this->color[2] = b;
	this->color[3] = a;
}

void ReadyMenu::saveCurrentPlayerData(){
	char* aitype=this->controlItems[0]->collectData();
	char* name=this->textField->collectData();
	char* tank=this->controlItems[1]->collectData();
	char teamLabel=(this->controlItems[2]->collectData()[0]);

	if(this->buttons[0]->isActive()){
		this->player_factory->updatePlayerBasicStrings(	"CPU", aitype, "", teamLabel, tank, currentPlayerIndex);
	}
	else{
		this->player_factory->updatePlayerBasicStrings(	"HUMAN", "", name, teamLabel, tank, currentPlayerIndex);
		this->textField->clearTextBuffer();
		this->textField->deactivate();
	}
}

void ReadyMenu::showPreviousPlayerPage(){
	if(currentPlayerIndex == 0){
		Mix_FadeOutMusic(300);
		Mix_HaltMusic();
		*currentGameState = MAIN_MENU;
		startMusicPlayed = false;
		
	}
	else{
		if(currentPlayerIndex == 1){
			this->buttons[2]->setColor(0.65, 0.15, 0.15);
			this->textField->setTextBuffer("");
		}
		this->buttons[3]->setLabel("Next");
		this->buttons[3]->setColor(0.75, 0.75, 0.75);
		currentPlayerIndex--;
		setPlayerPageNum(currentPlayerIndex);
		this->textField->setTextBuffer("");
		updatePageInfo();
	}
}

void ReadyMenu::showNextPlayerPage(){
	if(currentPlayerIndex+1 == numPlayers){
		Mix_FadeOutMusic(300);
		Mix_HaltMusic();
		*currentGameState = SHOP_MENU;
		this->textField->setTextBuffer("");
	}
	else{
		if(currentPlayerIndex+1 == numPlayers-1){
			this->buttons[3]->setLabel("Done");
			this->buttons[3]->setColor(0.65, 0.15, 0.15);
		}
		this->buttons[2]->setColor(0.75, 0.75, 0.75);
		currentPlayerIndex++;
		setPlayerPageNum(currentPlayerIndex);
		this->textField->setTextBuffer("");
		updatePageInfo();
	}
}

void ReadyMenu::updatePageInfo(){
	for(int i=0 ; i<NUM_TANK_TYPES ; i++){
		tanks[i]->changeHeadTexture(currentPlayerIndex);
	}
	char playerType[20];
	memset(playerType,0,20);
	const char* tempPlayerType=this->player_factory->getPlayer(currentPlayerIndex)->getPlayer_Type();
	strcpy(playerType,tempPlayerType);

	//test to see if it's CPU
	if(strcmp(playerType,"CPU")==0){
		this->buttons[0]->pressButton();
		Mix_HaltChannel(0);
		this->buttons[0]->activateSubMenu();
		this->buttons[1]->depressButton();
		this->buttons[1]->deactivateSubMenu();
		
		
		char aiType[20];
		memset(aiType,0,20);
		const char* tempAIType=this->player_factory->getPlayer(currentPlayerIndex)->getAI_Type();
		strcpy(aiType,tempAIType);
		int i=0;
		while(strcmp(this->controlItems[0]->collectData(), aiType) != 0)	{
			this->controlItems[0]->setOptionText(i);
			i++;
		}
	}
	else{
		char name[20];
		memset(name,0,20);
		const char* playerName=this->player_factory->getPlayer(currentPlayerIndex)->getPlayerName();

		strcpy(name,playerName);
		char* test = name;
		this->buttons[1]->pressButton();
		Mix_HaltChannel(0);
		this->buttons[1]->activateSubMenu();
		this->buttons[0]->depressButton();
		this->buttons[0]->deactivateSubMenu();

		this->textField->setTextBuffer(test);
	}
	
	if(	this->player_factory->getPlayer(currentPlayerIndex)->getTeamLabel() == '-' ){
		this->controlItems[2]->setOptionText(0);
	}
	else{
		this->controlItems[2]->setOptionText(this->player_factory->getPlayer(currentPlayerIndex)->getTeamLabel()-48);
	}

	char tankType[25];
	memset(tankType,0,25);
	const char* tank=this->player_factory->getPlayer(currentPlayerIndex)->getTankType();
	strcpy(tankType,tank);
	int i=0;
	while(strcmp(this->controlItems[1]->collectData(), tankType) != 0)	{
		this->controlItems[1]->setOptionText(i);
		i++;
	}
 
	this->buttons[0]->setColor(	this->player_factory->collectPlayerColor(currentPlayerIndex)[0],
								this->player_factory->collectPlayerColor(currentPlayerIndex)[1],
								this->player_factory->collectPlayerColor(currentPlayerIndex)[2]);
	this->buttons[1]->setColor(	this->player_factory->collectPlayerColor(currentPlayerIndex)[0],
								this->player_factory->collectPlayerColor(currentPlayerIndex)[1],
								this->player_factory->collectPlayerColor(currentPlayerIndex)[2]);
	
	this->tankPrvScrColor[0] = this->player_factory->collectPlayerColor(currentPlayerIndex)[0];
	this->tankPrvScrColor[1] = this->player_factory->collectPlayerColor(currentPlayerIndex)[1];
	this->tankPrvScrColor[2] = this->player_factory->collectPlayerColor(currentPlayerIndex)[2];

}

void ReadyMenu::setPlayerPageNum(int i){
	delete this->playerPageNum;
	this->caption = "";
	char add[20];
	sprintf(add, "Player %d", i+1);
	this->caption = add;
	GLfloat labelXPos=this->pos[0]-this->width*0.25;
	GLfloat labelYPos=this->pos[1]+this->height*0.4;
	this->playerPageNum = new TextObject(this->caption,labelXPos,labelYPos,(this->pos[2]+1),GLUT_BITMAP_TIMES_ROMAN_24,0.0f,0.0f,0.0f);
}



void ReadyMenu::buttonTest(int x, int y, int buttonDown){
	if(buttonDown){																		//FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
		for(int button_i=0;button_i<NUM_BUTTONS;button_i++){								//SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
																						//IF CLICK LANDS IN BUTTON I
			if(buttons[button_i]){														//JUST TO MAKE SURE																			
				if	((x>=this->buttons[button_i]->getXPos())	&&	(x<=(this->buttons[button_i]->getXPos()+this->buttons[button_i]->getWidth()))
																&&	
					(y<=this->buttons[button_i]->getYPos())		&&	(y>=(this->buttons[button_i]->getYPos()-this->buttons[button_i]->getHeight()))){
					buttons[button_i]->pressButton();									//PRESS BUTTON
					this->buttonPressed=buttons[button_i];								//KEEP TRACK OF WHICH BUTTON WAS PRESSED
				}
			}
		}
		for(int control_i=0;control_i<NUM_CONTROL_ITEMS;control_i++){								//SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
																								//IF YOU DID NOT CLICK A BUTTON PERHAPS YOU CLICKED A ARROW BUTTON???
			if	((x>=this->controlItems[control_i]->getXPos())	&&	(x<=(this->controlItems[control_i]->getXPos()+this->controlItems[control_i]->getWidth()))
																			&&	
				(y<=this->controlItems[control_i]->getYPos())	&&	(y>=(this->controlItems[control_i]->getYPos()-this->controlItems[control_i]->getHeight()))){
					this->controlItems[control_i]->mouseClickEvent(x,y,buttonDown,true);	//YOU PRESSED OVER A ARROWBUTTON
			}
		}
		if	((x>=this->textField->getXPos())	&&	(x<=(this->textField->getXPos()+this->textField->getWidth()))
												&&	
			(y<=this->textField->getYPos())		&&	(y>=(this->textField->getYPos()-this->textField->getHeight()))){
				this->textField->mouseClickEvent(x,y,buttonDown,true);	
		} else {
			this->textField->mouseClickEvent(x,y,buttonDown,false);
		}
	}


	else if(!buttonDown){													//IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON GOES UP
		if(this->textField->isTextFieldActive()) this->textField->deactivate();
		if(this->buttonPressed!=NULL) {													//IF THE LEFT CLICK WAS VALID AND INSIDE A BUTTON
																						//CHECK TO SEE IF YOU ARE STILL OVER SAME BUTTON
			if((x>=this->buttonPressed->getXPos())	&&	(x<=(this->buttonPressed->getXPos()+this->buttonPressed->getWidth()))
														&&	
				(y<=this->buttonPressed->getYPos())	&&	(y>=(this->buttonPressed->getYPos()-this->buttonPressed->getHeight()))){
				if(this->buttonPressed->getUNIQUEIDENTIFIER()==0){			// YOU CLICKED CPU TOGGLE BUTTON
					this->buttons[1]->depressButton();
					this->buttons[1]->deactivateSubMenu();
					this->buttons[0]->activateSubMenu();
					this->tanks[5]->getBaseHP();
					this->buttonPressed=NULL;
				}
				else if(this->buttonPressed->getUNIQUEIDENTIFIER()==1){		// YOU CLICKED HUMAN TOGGLE BUTTON
					this->buttons[0]->deactivateSubMenu();
					this->buttons[0]->depressButton();
					this->buttons[1]->activateSubMenu();
					this->buttonPressed=NULL;
				}
				else if(this->buttonPressed->getUNIQUEIDENTIFIER()==2){		// YOU CLICKED BACK BUTTON
					this->saveCurrentPlayerData();
					this->showPreviousPlayerPage();
					this->buttonPressed->depressButton();
					this->buttonPressed=NULL;				
				}
				else if(this->buttonPressed->getUNIQUEIDENTIFIER()==3){		// YOU CLICKED NEXT BUTTON
					this->saveCurrentPlayerData();
					this->showNextPlayerPage();
					this->buttonPressed->depressButton();
					this->buttonPressed=NULL;				
				}
			}else {																		//IF YOU RELEASE OUTSIDE ALL BUTTONS
				if(!this->buttonPressed->isActive())					//IF THE BUTTON PRESSED IS NOT THE BUTTON THAT'S TOGGLED
				{
					this->buttonPressed->depressButton();
					this->buttonPressed=NULL;
				}
			}
		}
		for(int control_i=0;control_i<NUM_CONTROL_ITEMS;control_i++){								//SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
																								//IF YOU DID NOT CLICK A BUTTON PERHAPS YOU CLICKED A ARROW BUTTON???
			if	((x>=this->controlItems[control_i]->getXPos())	&&	(x<=(this->controlItems[control_i]->getXPos()+this->controlItems[control_i]->getWidth()))
																			&&	
				(y<=this->controlItems[control_i]->getYPos())	&&	(y>=(this->controlItems[control_i]->getYPos()-this->controlItems[control_i]->getHeight()))){
					if(control_i == 0 && this->buttons[control_i+1]->isActive());	// IF HUMAN BUTTON IS TOGGLED, SELECTION BUTTON SHOULD DO NOTHING
					else{
						this->controlItems[control_i]->mouseClickEvent(x,y,buttonDown,true);
					}
			}
			else{
				if(control_i == 0 && this->buttons[control_i+1]->isActive());	// IF HUMAN BUTTON IS TOGGLED, SELECTION BUTTON SHOULD DO NOTHING
				else{
					this->controlItems[control_i]->mouseClickEvent(x,y,buttonDown,false);
				}
			}
		}
		if	((x>=this->textField->getXPos())	&&	(x<=(this->textField->getXPos()+this->textField->getWidth()))
												&&	
			(y<=this->textField->getYPos())		&&	(y>=(this->textField->getYPos()-this->textField->getHeight()))){
					this->textField->mouseClickEvent(x,y,buttonDown,true);	//YOU PRESSED OVER A ARROWBUTTON
			}

	}
}



void ReadyMenu::updateMouse(int x, int y){
	this->controlItems[1]->updateMouse(x, y);
}




void ReadyMenu::draw(){
	if(!startMusicPlayed){
		playMusic(READYMENU_START);
		startMusicPlayed = true;
	}
	playMusic(READYMENU_LOOP);
	glPushMatrix();
	/*	ALWAYS START AT UPPER LEFT CORNER -> LOWER LEFT CORNER -> LOWER RIGHT CORNER -> UPPER RIGHT	*/
	glBegin(GL_QUADS);		
		glColor3f(0.85f,0.85f,0.85f);
		glVertex3f(-1*(this->width/2.0),(this->height/2.0),0);																				/*	|\ 	*/
		glVertex3f(-1*(this->width/2.0),-1*(this->height/2.0),0);																			/*	| |	*/
		glVertex3f(-1*(this->width/2.0)+(this->percentBorder*(this->height)),-1*(this->height/2.0)+(this->percentBorder*(this->height)),0);	/*	|/ 	*/
		glVertex3f(-1*(this->width/2.0)+(this->percentBorder*(this->height)),(this->height/2.0)-(this->percentBorder*(this->height)),0);
	glEnd();
	glBegin(GL_QUADS);
		glColor3f(0.80f,0.80f,0.80f);
		glVertex3f(-1*(this->width/2.0),(this->height/2.0),0);																				/*_____ */
		glVertex3f(-1*(this->width/2.0)+(this->percentBorder*(this->height)),(this->height/2.0)-(this->percentBorder*(this->height)),0);	/*\	  / */
		glVertex3f((this->width/2.0)-(this->percentBorder*(this->height)),(this->height/2.0)-(this->percentBorder*(this->height)),0);		/* ---	*/
		glVertex3f((this->width/2.0),(this->height/2.0),0);		
	glEnd();
	glBegin(GL_QUADS);
		glColor3f(0.75f,0.75f,0.75f);
		glVertex3f(-1*(this->width/2.0)+(this->percentBorder*(this->height)),(this->height/2.0)-(this->percentBorder*(this->height)),0);	/*_____ */
		glVertex3f(-1*(this->width/2.0)+(this->percentBorder*(this->height)),-1*(this->height/2.0)+(this->percentBorder*(this->height)),0);	/*|	  | */
		glVertex3f((this->width/2.0)-(this->percentBorder*(this->height)),-1*(this->height/2.0)+(this->percentBorder*(this->height)),0);	/*----- */
		glVertex3f((this->width/2.0)-(this->percentBorder*(this->height)),(this->height/2.0)-(this->percentBorder*(this->height)),0);	
	glEnd();
	glBegin(GL_QUADS);
		glColor3f(0.45f,0.45f,0.45f);
		glVertex3f(-1*(this->width/2.0)+(this->percentBorder*(this->height)),-1*(this->height/2.0)+(this->percentBorder*(this->height)),0);	/* ___  */
		glVertex3f(-1*(this->width/2.0),-1*(this->height/2.0),0);																			/*/	  \ */
		glVertex3f((this->width/2.0),-1*(this->height/2.0),0);																				/*----- */
		glVertex3f((this->width/2.0)-(this->percentBorder*(this->height)),-1*(this->height/2.0)+(this->percentBorder*(this->height)),0);		
	glEnd();
	glBegin(GL_QUADS);
		glColor3f(0.40f,0.40f,0.40f);
		glVertex3f((this->width/2.0)-(this->percentBorder*(this->height)),(this->height/2.0)-(this->percentBorder*(this->height)),0);		/*	 /| 	*/
		glVertex3f((this->width/2.0)-(this->percentBorder*(this->height)),-1*(this->height/2.0)+(this->percentBorder*(this->height)),0);	/*	| | 	*/
		glVertex3f((this->width/2.0),-1*(this->height/2.0),0);																				/* 	 \| 	*/
		glVertex3f((this->width/2.0),(this->height/2.0),0);		
	glEnd();

	// Draw tank preview screen
	glBegin(GL_QUADS);
		glColor4f(0.45, 0.45, 0.45, 1);
		glVertex3f(this->tankPrvScrPos[0], this->tankPrvScrPos[1], this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0]-6, this->tankPrvScrPos[1]+6, this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0]+tankPrvScrWidth+6, this->tankPrvScrPos[1]+6, this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0]+tankPrvScrWidth, this->tankPrvScrPos[1], this->tankPrvScrPos[2]);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(0.4, 0.4, 0.4, 1);
		glVertex3f(this->tankPrvScrPos[0]-6, this->tankPrvScrPos[1]+6, this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0]-6, this->tankPrvScrPos[1]-tankPrvScrHeight-6, this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0], this->tankPrvScrPos[1]-tankPrvScrHeight, this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0], this->tankPrvScrPos[1], this->tankPrvScrPos[2]);
	glEnd();

	/* Middle pane replaced by viewport 
	glBegin(GL_QUADS);
		glColor4f(tankPrvScrColor[0], tankPrvScrColor[1], tankPrvScrColor[2], 1);
		glVertex3f(this->tankPrvScrPos[0], this->tankPrvScrPos[1], this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0], this->tankPrvScrPos[1]-tankPrvScrHeight, this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0]+tankPrvScrWidth, this->tankPrvScrPos[1]-tankPrvScrHeight, this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0]+tankPrvScrWidth, this->tankPrvScrPos[1], this->tankPrvScrPos[2]);
	glEnd();*/
	
	glBegin(GL_QUADS);
		glColor4f(0.8, 0.8, 0.8, 1);
		glVertex3f(this->tankPrvScrPos[0]-6, this->tankPrvScrPos[1]-tankPrvScrHeight-6, this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0]+tankPrvScrWidth+6, this->tankPrvScrPos[1]-tankPrvScrHeight-6, this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0]+tankPrvScrWidth, this->tankPrvScrPos[1]-tankPrvScrHeight, this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0], this->tankPrvScrPos[1]-tankPrvScrHeight, this->tankPrvScrPos[2]);
	glEnd();
	glBegin(GL_QUADS);
		glColor4f(0.85, 0.85, 0.85, 1);
		glVertex3f(this->tankPrvScrPos[0]+tankPrvScrWidth, this->tankPrvScrPos[1], this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0]+tankPrvScrWidth+6, this->tankPrvScrPos[1]+6, this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0]+tankPrvScrWidth+6, this->tankPrvScrPos[1]-tankPrvScrHeight-6, this->tankPrvScrPos[2]);
		glVertex3f(this->tankPrvScrPos[0]+tankPrvScrWidth, this->tankPrvScrPos[1]+-tankPrvScrHeight, this->tankPrvScrPos[2]);
	glEnd();

	for(int i=0;i<NUM_TANK_STATS;i++){
		this->tankStatLabels[i]->draw();
	}
	for(int i=0;i<NUM_BUTTONS;i++){
		this->buttons[i]->draw();
	}
	if(this->buttons[0]->isActive()){
		this->controlItems[0]->draw();
	}else{
		this->textField->draw();
	}
	this->controlItems[1]->draw();
	this->controlItems[2]->draw();
	this->playerPageNum->draw();

	// Flashing color effect in the tank preview screen
	if(	0<= this->tankPrvScrColor[0] && this->tankPrvScrColor[0] <= 
		this->player_factory->collectPlayerColor(currentPlayerIndex)[0]*1.12)
			tankPrvScrColor[0] += (tankPrvScrColor[0]+0.1)/100*prvScrColorControl;
	if(	0<= this->tankPrvScrColor[1] && this->tankPrvScrColor[1] <= 
		this->player_factory->collectPlayerColor(currentPlayerIndex)[1]*1.12)
			tankPrvScrColor[1] += (tankPrvScrColor[1]+0.1)/100*prvScrColorControl;
	if(	0<= this->tankPrvScrColor[2] && this->tankPrvScrColor[2] <= 
		this->player_factory->collectPlayerColor(currentPlayerIndex)[2]*1.12)
			tankPrvScrColor[2] += (tankPrvScrColor[2]+0.1)/100*prvScrColorControl;
	if(tankPrvScrColor[0]+tankPrvScrColor[1]+tankPrvScrColor[2] < 0)	{
		tankPrvScrColor[0] = tankPrvScrColor[1] = tankPrvScrColor[2] = 0;
		prvScrColorControl = 1;
	}
	
	if(	tankPrvScrColor[0]+tankPrvScrColor[1]+tankPrvScrColor[2] > 
		(	this->player_factory->collectPlayerColor(currentPlayerIndex)[0]+
			this->player_factory->collectPlayerColor(currentPlayerIndex)[1]+
			this->player_factory->collectPlayerColor(currentPlayerIndex)[2])*1.12)	{
		tankPrvScrColor[0] = this->player_factory->collectPlayerColor(currentPlayerIndex)[0]*1.11;
		tankPrvScrColor[1] = this->player_factory->collectPlayerColor(currentPlayerIndex)[1]*1.11;
		tankPrvScrColor[2] = this->player_factory->collectPlayerColor(currentPlayerIndex)[2]*1.11;
		prvScrColorControl = -1;
	}
	// Draw Stat Images
	for(int i=0 ; i<NUM_STAT_IMAGES ; i++){
		if(i<30){
			statImages[i]->draw();
		}
		else{
			if(i<40){
				if(strcmp(this->controlItems[1]->collectData(), "Rhinoxx") == 0 &&	i-30 < this->tanks[0]->getBasePower()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Hellfire") == 0 &&	i-30 < this->tanks[1]->getBasePower()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "HeavyD") == 0 &&	i-30 < this->tanks[2]->getBasePower()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Panzer") == 0 &&	i-30 < this->tanks[3]->getBasePower()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Eggroid") == 0 &&	i-30 < this->tanks[4]->getBasePower()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Behemoth") == 0 &&	i-30 < this->tanks[5]->getBasePower()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Cubix") == 0 &&	i-30 < this->tanks[6]->getBasePower()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Predator") == 0 &&	i-30 < this->tanks[7]->getBasePower()){
					statImages[i]->draw();
				}
			}
			else if(i<50){
				if(strcmp(this->controlItems[1]->collectData(), "Rhinoxx") == 0 &&	i-40 < this->tanks[0]->getBaseArmor()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Hellfire") == 0 &&	i-40 < this->tanks[1]->getBaseArmor()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "HeavyD") == 0 &&	i-40 < this->tanks[2]->getBaseArmor()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Panzer") == 0 &&	i-40 < this->tanks[3]->getBaseArmor()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Eggroid") == 0 &&	i-40 < this->tanks[4]->getBaseArmor()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Behemoth") == 0 &&	i-40 < this->tanks[5]->getBaseArmor()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Cubix") == 0 &&	i-40 < this->tanks[6]->getBaseArmor()){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Predator") == 0 &&	i-40 < this->tanks[7]->getBaseArmor()){
					statImages[i]->draw();
				}
			}
			else{
				if(strcmp(this->controlItems[1]->collectData(), "Rhinoxx") == 0 &&	i-50 < this->tanks[0]->getBaseSpeed()/10){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Hellfire") == 0 &&	i-50 < this->tanks[1]->getBaseSpeed()/10){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "HeavyD") == 0 &&	i-50 < this->tanks[2]->getBaseSpeed()/10){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Panzer") == 0 &&	i-50 < this->tanks[3]->getBaseSpeed()/10){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Eggroid") == 0 &&	i-50 < this->tanks[4]->getBaseSpeed()/10){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Behemoth") == 0 &&	i-50 < this->tanks[5]->getBaseSpeed()/10){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Cubix") == 0 &&	i-50 < this->tanks[6]->getBaseSpeed()/10){
					statImages[i]->draw();
				}
				else if(strcmp(this->controlItems[1]->collectData(), "Predator") == 0 &&	i-50 < this->tanks[7]->getBaseSpeed()/10){
					statImages[i]->draw();
				}
			}
		}
	}

	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(this->tankPrvScrPos[0]+this->getWidth()/2, tankPrvScrHeight+1, tankPrvScrWidth, tankPrvScrHeight);
	gluPerspective(60.0, ((float)width/(1.5*(float)height)), 1, 199999999);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScissor(this->tankPrvScrPos[0]+this->getWidth()/2, tankPrvScrHeight+1, tankPrvScrWidth, tankPrvScrHeight);
	glClearColor(tankPrvScrColor[0], tankPrvScrColor[1], tankPrvScrColor[2], 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//gluLookAt(	0,0,400,	0, 0, 0,		0.0f,1.0f,0.0f);
	gluLookAt(0,200,500,0,0,0,0.0f,1.0f,0.0f);

	// Draw Tanks
	glTranslatef(this->pos[0], this->pos[1]-tankPrvScrHeight*0.2, this->pos[2]);
	glRotatef(tankAngle, 0, 1, 0);
	if(strcmp(this->controlItems[1]->collectData(), "Rhinoxx") == 0)			this->tanks[0]->draw();
	else if(strcmp(this->controlItems[1]->collectData(), "Hellfire") == 0)		this->tanks[1]->draw();
	else if(strcmp(this->controlItems[1]->collectData(), "HeavyD") == 0)		this->tanks[2]->draw();
	else if(strcmp(this->controlItems[1]->collectData(), "Panzer") == 0)		this->tanks[3]->draw();
	else if(strcmp(this->controlItems[1]->collectData(), "Eggroid") == 0)		this->tanks[4]->draw();
	else if(strcmp(this->controlItems[1]->collectData(), "Behemoth") == 0)		this->tanks[5]->draw();
	else if(strcmp(this->controlItems[1]->collectData(), "Cubix") == 0)			this->tanks[6]->draw();
	else if(strcmp(this->controlItems[1]->collectData(), "Predator") == 0)		this->tanks[7]->draw();
	else	{
		printf("ERROR: Unkown tank type\n");
		glutSolidSphere(100, 30, 30);
	}
		
	tankAngle += 0.25f;
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT1);
	glEnable(GL_LIGHT0);
	glPopMatrix();


	int winWidth=glutGet(GLUT_SCREEN_WIDTH);
	int winHeight=glutGet(GLUT_SCREEN_HEIGHT);

	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	glViewport(0, 0, winWidth, winHeight);
	gluPerspective(60.0, (float)winWidth / (float)winHeight, 1.0, 1000000.0);

	glMatrixMode(GL_MODELVIEW);
	glScissor(0, 0, winWidth, winHeight);
	glLoadIdentity();

}

void ReadyMenu::keyTest(unsigned char key){
	if(this->textField->isTextFieldActive()){
		this->textField->keyHandler(key);
	}
}