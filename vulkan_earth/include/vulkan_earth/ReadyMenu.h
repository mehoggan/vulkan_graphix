#ifndef READYMENU_H
#define READYMENU_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <cstdint>
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

const std::int32_t num_buttons = 4;
const std::int32_t num_stat_images = 60;
const std::int32_t num_control_items = 3;
const std::int32_t num_tank_stats = 3;
const std::int32_t num_tank_types = 8;
const std::int32_t player_attributes = 4;

class ReadyMenu {
public:
    ReadyMenu();
    ReadyMenu(float new_width,
              float new_height,
              float new_percent_border,
              GlobalSettings* new_global_settings,
              PlayerFactory* new_player_factory,
              std::int32_t* game_state);
    ~ReadyMenu();
    float* getPos();
    float getHeight();
    void setHeight(float new_height);
    float getWidth();
    void setWidth(float new_width);
    float* getColor();
    void setColor(float r, float g, float b, float a);
    void buttonTest(std::int32_t x, std::int32_t y, std::int32_t button_down);
    void keyTest(std::uint8_t key);
    void draw();
    void showPreviousPlayerPage();
    void showNextPlayerPage();
    void setPlayerPageNum(std::int32_t i);
    void updatePageInfo();
    void updateMouse(std::int32_t x, std::int32_t y);
    void saveCurrentPlayerData();
    void updateNumPlayers(std::int32_t n);

private:
    float pos[3], width, height, color[4], border;
    float percent_border;
    float tank_prv_scr_pos[3], tank_prv_scr_width, tank_prv_scr_height;
    float tank_prv_scr_color[3];
    MainMenuButton* buttons[num_buttons];
    ImageObject* stat_images[num_stat_images];
    ControlItem* control_items[num_control_items];
    TextObject* player_page_num;
    TextObject* tank_stat_labels[num_tank_stats];
    MainMenuButton* button_pressed;
    std::string caption;
    std::int32_t current_player_index;
    std::int32_t num_players;
    std::int32_t* current_game_state;
    std::int32_t prv_scr_color_control;
    ControlItemTextField* text_field;
    GlobalSettings* global_settings;
    PlayerFactory* player_factory;
    Tank* tanks[num_tank_types];
    float tank_angle;
    bool start_music_played;
};

#endif