#include "Inventory.h"
#include "ControlItem.h"
#include "ControlItemGrid.h"
#include "TextObject.h"
#include "ImageObject.h"
#include "Weapon.h"
#include "Item.h"
#include "Player.h"
#include "PlayerHuman.h"
#include <math.h>
#include "macro_crtdbg.h"

Inventory::Inventory(){}
Inventory::Inventory(GLfloat x, GLfloat y, int w, int h){
	xPos = x-x*0.01;
	yPos = y-y*0.01;
	this->width = w+w*0.01;
	this->height = h+h*0.01;
	invenGrid = new ControlItemGrid(-width/4.0, height/4.0, 1, width*0.5, height*0.4, 2, 5, 0.5, 0.5, 1, true, false);
	for(int i=0 ; i<PLAYER_MAX_WEAPONS+PLAYER_MAX_ITEMS ; i++){
		imgInven[i] = NULL;
		remainings[i] = NULL;
	}
	title = new TextObject("Inventory", -width/3.3, height/2.5, 1, GLUT_BITMAP_TIMES_ROMAN_24, 0, 0, 0);
	explain = new TextObject("(Press Enter to load/unload a weapon or use an item)",
											-width/3.3, height/3.0, 1, GLUT_BITMAP_TIMES_ROMAN_24, 0, 0, 0);
	descript = new TextObject("",
											-width/3.6, -height/3.0, 1, GLUT_BITMAP_TIMES_ROMAN_24, 0, 0, 0);
	selectCellRow = 0;
	selectCellCol = 0;
}
Inventory::~Inventory(){
	delete invenGrid;
	for(int i=0 ; i<PLAYER_MAX_WEAPONS+PLAYER_MAX_ITEMS ; i++){
		if(imgInven[i]){
			delete imgInven[i];
			delete remainings[i];
		}
	}
	delete title;
	delete explain;
	delete descript;
}

void Inventory::setupInventory(Player* player){
	for(int i=0 ; i<PLAYER_MAX_WEAPONS ; i++){
		weapons[i] = player->getCurrentWeapons()[i];
		items[i] = player->getCurrentItems()[i];
	}
	for(int i=0 ; i<PLAYER_MAX_WEAPONS ; i++){
		// If Player Has A Weapon
		if(weapons[i] != NULL){
			if(imgInven[i]){
				delete imgInven[i];
				delete remainings[i];
			}
			imgInven[i] = new ImageObject(0,0,0,0,0,0, 256,256,	(char*)weapons[i]->getImageFileName());
			invenGrid->setImageSizeToCell(this->imgInven[i], 0.8);
			char* remain = "";
			char add[10];
			sprintf(add, "x %d", weapons[i]->getRemaining());
			remain = add;
			remainings[i] = new TextObject(remain, 0, 0, 0, GLUT_BITMAP_TIMES_ROMAN_24,0.6f,0.3f,0.4f);
		}
		else{
			if(imgInven[i]){
				delete imgInven[i];
				delete remainings[i];
			}
			imgInven[i] = NULL;
			remainings[i] = NULL;
		}
	}
	for(int i=0 ; i<PLAYER_MAX_ITEMS ; i++){
		// If Player Has An Item
		if(items[i] != NULL){
			if(imgInven[PLAYER_MAX_WEAPONS+i]){
				delete imgInven[PLAYER_MAX_WEAPONS+i];
				delete remainings[PLAYER_MAX_WEAPONS+i];
			}
			imgInven[PLAYER_MAX_WEAPONS+i] = new ImageObject(0,0,0,0,0,0, 256,256,	(char*)items[i]->getImageFileName());
			invenGrid->setImageSizeToCell(this->imgInven[PLAYER_MAX_WEAPONS+i], 0.8);
			char* remain = "";
			char add[10];
			sprintf(add, "x %d", items[i]->getRemaining());
			remain = add;
			remainings[PLAYER_MAX_WEAPONS+i] = new TextObject(remain, 0, 0, 0, GLUT_BITMAP_TIMES_ROMAN_24,0.6f,0.3f,0.4f);
		}
		else{
			if(imgInven[PLAYER_MAX_WEAPONS+i]){
				delete imgInven[PLAYER_MAX_WEAPONS+i];
				delete remainings[PLAYER_MAX_WEAPONS+i];
			}
			imgInven[PLAYER_MAX_WEAPONS+i] = NULL;
			remainings[PLAYER_MAX_WEAPONS+i] = NULL;
		}
	}
	// Place Images and remaining labels to the cells
	int index=0;
	for(int r=0 ; r<2 ; r++){
		for(int c=0 ; c<PLAYER_MAX_WEAPONS ; c++){
			if(imgInven[index] != NULL){
				invenGrid->placeImageToCell(imgInven[index], r, c);
				invenGrid->placeTextToCell(remainings[index], r, c);
			}
			index++;
		}
	}
	selectCellRow = 0;
	selectCellCol = 0;
	invenGrid->selectCell(selectCellRow,selectCellCol);
	if(weapons[0] != NULL){
		if(descript)	delete descript;
		descript = new TextObject((char*)weapons[0]->getDescription(), -width/3.6, -height/3.0, 1, GLUT_BITMAP_TIMES_ROMAN_24, 0, 0, 0);
	}
	else{
		if(descript)	delete descript;
		descript = new TextObject("", -width/3.6, -height/3.0, 1, GLUT_BITMAP_TIMES_ROMAN_24, 0, 0, 0);
	}
}



void Inventory::handleInventory(Player* currentPlayer, int invenIndex){
	// IF PLAYER HAS SHOT WITH A SPECIAL WEAPON
	if(invenIndex <	PLAYER_MAX_WEAPONS){
		if(currentPlayer->getLoadedWeapon() != NULL){
			if(currentPlayer->getLoadedWeapon()->getRemaining() > 1){
				currentPlayer->getLoadedWeapon()->setRemaining(currentPlayer->getLoadedWeapon()->getRemaining()-1);
			}
			else{
				int index;
				for(index=0 ; index<PLAYER_MAX_WEAPONS ; index++){
					if(currentPlayer->getCurrentWeapons()[index] == NULL){
						//do nothing
					}
					else if(	currentPlayer->getCurrentWeapons()[index]->getUNIQUEIDENTIFIER() 
																== 
								currentPlayer->getLoadedWeapon()->getUNIQUEIDENTIFIER()){
						delete currentPlayer->getLoadedWeapon();
						currentPlayer->getCurrentWeapons()[index] = NULL;
						currentPlayer->setLoadedWeapon(NULL);
						break;
					}
				}
			}
		}
	}
	// IF PLAYER HAS USED AN ITEM
	else{
		if(currentPlayer->getCurrentItems()[invenIndex-PLAYER_MAX_WEAPONS]->getRemaining() > 1){
			currentPlayer->getCurrentItems()[invenIndex-PLAYER_MAX_WEAPONS]->	setRemaining(currentPlayer->getCurrentItems()[invenIndex-PLAYER_MAX_WEAPONS]->getRemaining()-1);
		}
		else{
			delete currentPlayer->getCurrentItems()[invenIndex-PLAYER_MAX_WEAPONS];
			currentPlayer->getCurrentItems()[invenIndex-PLAYER_MAX_WEAPONS] = NULL;
		}
	}
}


int Inventory::getSelectedIndex(){
	return PLAYER_MAX_WEAPONS*selectCellRow+selectCellCol;
}


void Inventory::keyHandler(int key){
	// LEFT KEY
	if(key == 1 && selectCellCol > 0){
		selectCellCol--;
		invenGrid->selectCell(selectCellRow, selectCellCol);
	}
	// UP KEY
	else if(key == 2 && selectCellRow > 0){
		selectCellRow--;
		invenGrid->selectCell(selectCellRow, selectCellCol);
	}
	// RIGHT KEY
	else if(key == 3 && selectCellCol < PLAYER_MAX_WEAPONS-1){
		selectCellCol++;
		invenGrid->selectCell(selectCellRow, selectCellCol);
	}
	// DOWN KEY
	else if(key == 4 && selectCellRow < 1)	{
		selectCellRow++;
		invenGrid->selectCell(selectCellRow, selectCellCol);
	}

	// IF ONE OF WEAPONS IS SELECTED
	if(selectCellRow == 0){
		if(weapons[selectCellCol] != NULL){
			if(descript)	delete descript;
			descript = new TextObject((char*)weapons[selectCellCol]->getDescription(), -width/3.6, -height/3.0, 1, GLUT_BITMAP_TIMES_ROMAN_24, 0, 0, 0);
		}
		else{
			if(descript)	delete descript;
			descript = new TextObject("", -width/3.6, -height/3.0, 1, GLUT_BITMAP_TIMES_ROMAN_24, 0, 0, 0);
		}
	}
	// IF ONE OF ITEMS IS SELECTED
	else{
		if(items[selectCellCol] != NULL){
			if(descript)	delete descript;
			descript = new TextObject((char*)items[selectCellCol]->getDescription(), -width/3.6, -height/3.0, 1, GLUT_BITMAP_TIMES_ROMAN_24, 0, 0, 0);
		}
		else{
			if(descript)	delete descript;
			descript = new TextObject("", -width/3.6, -height/3.0, 1, GLUT_BITMAP_TIMES_ROMAN_24, 0, 0, 0);
		}
	}
}


void Inventory::draw(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(xPos, yPos, width, height);
	gluPerspective(60.0, ((float)width/(1.5*(float)height)), 1, 199999999);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScissor(xPos, yPos, width, height);
	glClearColor(0.75, 0.75, 0.75, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int distance = (int)(glutGet(GLUT_SCREEN_HEIGHT)/4*tan(1.04719755));
	gluLookAt(0,0,distance,0,0,0,0.0f,1.0f,0.0f);

	invenGrid->draw();
	title->draw();
	explain->draw();
	descript->draw();
	for(int i=0 ; i<PLAYER_MAX_WEAPONS+PLAYER_MAX_ITEMS ; i++){
		if(imgInven[i] != NULL){
			imgInven[i]->draw();
			remainings[i]->draw();
		}
	}

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

