#ifndef MAINMENU_H
#define MAINMENU_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>

class MainMenuButton;
class TextObject;
class ImageObject;
class SubMenu;
class SubMenuTest;
class SubMenuSelectTanks;
class SubMenuSound;
class SubMenuHardware;
class SubMenuEconomics;
class SubMenuPhysics;
class SubMenuLandscape;
class SubMenuPlayOptions;
class SubMenuWeapons;
class ControlItem;
class ControlItemSelectionBox;
class ControlItemCheckBox;
class GlobalSettings;
class PlayerFactory;

using namespace std;

const int QUIT = 9;

const int NUM_BUTTON = 10;
const int NUM_SUBMENUS = 8;
const int NUM_IMAGES = 2;
const int NUM_ARROW_BUTTONS = 2;

class MainMenu {
public:
    MainMenu();
    MainMenu(GLfloat width,
             GLfloat height,
             GLfloat percentBorder,
             GlobalSettings* global_settings,
             PlayerFactory* player_factory,
             int* gameState);
    ~MainMenu();
    GLfloat* getPos();
    GLfloat getHeight();
    void setHeight(GLfloat height);
    GLfloat getWidth();
    void setWidth(GLfloat width);
    GLfloat* getColor();
    SubMenu* getSubMenuI(int I);
    SubMenu* getActiveSubMenu();
    void draw();
    void buttonTest(int x, int y, int buttonDown);
    void collectData();
    SubMenuLandscape* getSubMenuLandscape();

private:
    GLfloat pos[3], width, height, color[4], border;
    int* currentGameState;
    GLfloat percentBorder;
    MainMenuButton* buttons[NUM_BUTTON];
    SubMenu* submenus[NUM_SUBMENUS];
    ImageObject* images[NUM_IMAGES];
    ControlItem* arrowsbutton[NUM_ARROW_BUTTONS];
    TextObject* text;
    MainMenuButton* buttonPressed;
    SubMenu* activeSubMenu;
    ControlItem* arrowButtonPressed;
    GlobalSettings* global_settings;
    PlayerFactory* player_factory;
};

#endif