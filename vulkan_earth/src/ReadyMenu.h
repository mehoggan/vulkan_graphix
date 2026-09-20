#ifndef READYMENU_H
#define READYMENU_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>

class TextObject;
class ImageObject;
class ControlItem;
class ControlItemSelectionBox;
class ControlItemSliderbar;
class ControlItemTextField;
class MainMenuButton;
class GlobalSettings;
class PlayerFactory;
class Tank;
class TankA;

//using namespace std;

const int NUM_BUTTONS = 4;
const int NUM_STAT_IMAGES = 60;
const int NUM_CONTROL_ITEMS = 3;
const int NUM_TANK_STATS = 3;
const int NUM_TANK_TYPES = 8;
const int PLAYER_ATTRIBUTES = 4;

class ReadyMenu {
public:
	ReadyMenu();
	ReadyMenu(GLfloat width, GLfloat height, GLfloat percentBorder, GlobalSettings* global_settings, PlayerFactory* player_factory, int* gameState);
	~ReadyMenu();
	GLfloat* getPos();
	GLfloat getHeight();
	void setHeight(GLfloat height);
	GLfloat getWidth();
	void setWidth(GLfloat width);
	GLfloat* getColor();
	void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	void buttonTest(int x, int y, int buttonState);
	void keyTest(unsigned char key);
	void draw();
	void showPreviousPlayerPage();
	void showNextPlayerPage();
	void setPlayerPageNum(int currentPlayerIndex);
	void	updatePageInfo();
	void updateMouse(int x, int y);
	void saveCurrentPlayerData();
	void updateNumPlayers(int n);

private:
	GLfloat pos[3], width, height, color[4], border;
	GLfloat percentBorder;
	GLfloat tankPrvScrPos[3],	tankPrvScrWidth, tankPrvScrHeight;
	GLfloat tankPrvScrColor[3];
	MainMenuButton* buttons[NUM_BUTTONS];
	ImageObject* statImages[NUM_STAT_IMAGES];
	ControlItem* controlItems[NUM_CONTROL_ITEMS];
	TextObject* playerPageNum;
	TextObject* tankStatLabels[NUM_TANK_STATS];
	MainMenuButton* buttonPressed;
	char* caption;
	char* options;
	int currentPlayerIndex;
	int numPlayers;
	int* currentGameState;
	int prvScrColorControl;
	ControlItemTextField* textField;
	GlobalSettings* global_settings;
	PlayerFactory* player_factory;
	Tank* tanks[NUM_TANK_TYPES];
	float tankAngle;
	bool startMusicPlayed;
};

#endif