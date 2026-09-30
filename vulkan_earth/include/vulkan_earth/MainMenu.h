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

const int quit = 9;

const int num_button = 10;
const int num_submenus = 8;
const int num_images = 2;
const int num_arrow_buttons = 2;

class MainMenu {
public:
    MainMenu();
    MainMenu(GLfloat new_width,
             GLfloat new_height,
             GLfloat new_percent_border,
             GlobalSettings* new_global_settings,
             PlayerFactory* new_player_factory,
             int* game_state);
    ~MainMenu();
    GLfloat* getPos();
    GLfloat getHeight();
    void setHeight(GLfloat new_height);
    GLfloat getWidth();
    void setWidth(GLfloat new_width);
    GLfloat* getColor();
    SubMenu* getSubMenuI(int i);
    SubMenu* getActiveSubMenu();
    void draw();
    void buttonTest(int x, int y, int button_down);
    void collectData();
    SubMenuLandscape* getSubMenuLandscape();

private:
    GLfloat pos[3], width, height, color[4], border;
    int* current_game_state;
    GLfloat percent_border;
    MainMenuButton* buttons[num_button];
    SubMenu* submenus[num_submenus];
    ImageObject* images[num_images];
    ControlItem* arrowsbutton[num_arrow_buttons];
    TextObject* text;
    MainMenuButton* button_pressed;
    SubMenu* active_sub_menu;
    ControlItem* arrow_button_pressed;
    GlobalSettings* global_settings;
    PlayerFactory* player_factory;
};

#endif