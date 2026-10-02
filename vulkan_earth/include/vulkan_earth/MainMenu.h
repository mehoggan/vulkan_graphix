#ifndef MAINMENU_H
#define MAINMENU_H

#include <stdio.h>
#include <cstdint>
#include <iostream>
#include "vulkan_earth/render/Mesh.h"

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

const std::int32_t quit = 9;

const std::int32_t num_button = 10;
const std::int32_t num_submenus = 8;
const std::int32_t num_images = 2;
const std::int32_t num_arrow_buttons = 2;

namespace vulkan_earth::render {
class RenderContext;
}

class MainMenu {
public:
    MainMenu();
    MainMenu(float new_width,
             float new_height,
             float new_percent_border,
             GlobalSettings* new_global_settings,
             PlayerFactory* new_player_factory,
             std::int32_t* game_state);
    ~MainMenu();
    float* getPos();
    float getHeight();
    void setHeight(float new_height);
    float getWidth();
    void setWidth(float new_width);
    float* getColor();
    SubMenu* getSubMenuI(std::int32_t i);
    SubMenu* getActiveSubMenu();
    void draw(vulkan_earth::render::RenderContext& context);
    void buttonTest(std::int32_t x, std::int32_t y, std::int32_t button_down);
    void collectData();
    SubMenuLandscape* getSubMenuLandscape();

private:
    float pos[3], width, height, color[4], border;
    std::int32_t* current_game_state;
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
    vulkan_earth::render::UiMesh background_mesh;
    float built_width = -1.0f;
    float built_height = -1.0f;
};

#endif