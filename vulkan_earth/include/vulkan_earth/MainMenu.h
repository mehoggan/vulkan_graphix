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
    MainMenu(float new_width,
             float new_height,
             float new_percent_border,
             GlobalSettings* new_global_settings,
             PlayerFactory* new_player_factory,
             int* game_state);
    ~MainMenu();
    float* getPos();
    float getHeight();
    void setHeight(float new_height);
    float getWidth();
    void setWidth(float new_width);
    float* getColor();
    SubMenu* getSubMenuI(int i);
    SubMenu* getActiveSubMenu();
    void draw();
    void buttonTest(int x, int y, int button_down);
    void collectData();
    SubMenuLandscape* getSubMenuLandscape();

private:
    float pos[3], width, height, color[4], border;
    int* current_game_state;
    float percent_border;
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