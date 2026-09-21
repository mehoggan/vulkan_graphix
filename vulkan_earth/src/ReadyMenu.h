#ifndef READYMENU_H
#define READYMENU_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>
#include <string>

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

// using namespace std;

const int num_buttons = 4;
const int num_stat_images = 60;
const int num_control_items = 3;
const int num_tank_stats = 3;
const int num_tank_types = 8;
const int player_attributes = 4;

class ReadyMenu {
public:
    ReadyMenu();
    ReadyMenu(GLfloat width,
              GLfloat height,
              GLfloat percent_border,
              GlobalSettings* global_settings,
              PlayerFactory* player_factory,
              int* game_state);
    ~ReadyMenu();
    GLfloat* getPos();
    GLfloat getHeight();
    void setHeight(GLfloat height);
    GLfloat getWidth();
    void setWidth(GLfloat width);
    GLfloat* getColor();
    void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void buttonTest(int x, int y, int button_state);
    void keyTest(unsigned char key);
    void draw();
    void showPreviousPlayerPage();
    void showNextPlayerPage();
    void setPlayerPageNum(int current_player_index);
    void updatePageInfo();
    void updateMouse(int x, int y);
    void saveCurrentPlayerData();
    void updateNumPlayers(int n);

private:
    GLfloat pos[3], width, height, color[4], border;
    GLfloat percent_border;
    GLfloat tank_prv_scr_pos[3], tank_prv_scr_width, tank_prv_scr_height;
    GLfloat tank_prv_scr_color[3];
    MainMenuButton* buttons[num_buttons];
    ImageObject* stat_images[num_stat_images];
    ControlItem* control_items[num_control_items];
    TextObject* player_page_num;
    TextObject* tank_stat_labels[num_tank_stats];
    MainMenuButton* button_pressed;
    std::string caption;
    int current_player_index;
    int num_players;
    int* current_game_state;
    int prv_scr_color_control;
    ControlItemTextField* text_field;
    GlobalSettings* global_settings;
    PlayerFactory* player_factory;
    Tank* tanks[num_tank_types];
    float tank_angle;
    bool start_music_played;
};

#endif