#ifndef VULKAN_EARTH_MAINMENU_H
#define VULKAN_EARTH_MAINMENU_H

#include <cstdint>
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

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

namespace vulkan_graphix::Render {
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
    void draw(vulkan_graphix::Render::RenderContext& context);
    void buttonTest(std::int32_t x, std::int32_t y, std::int32_t button_down);
    void collectData();
    SubMenuLandscape* getSubMenuLandscape();

private:
    float m_pos[3], m_width, m_height, m_color[4], m_border;
    std::int32_t* m_current_game_state;
    float m_percent_border;
    MainMenuButton* m_buttons[num_button];
    SubMenu* m_submenus[num_submenus];
    ImageObject* m_images[num_images];
    ControlItem* m_arrowsbutton[num_arrow_buttons];
    TextObject* m_text;
    MainMenuButton* m_button_pressed;
    SubMenu* m_active_sub_menu;
    ControlItem* m_arrow_button_pressed;
    GlobalSettings* m_global_settings;
    PlayerFactory* m_player_factory;
    vulkan_graphix::Render::UiMesh m_background_mesh;
    float m_built_width = -1.0f;
    float m_built_height = -1.0f;
};

#endif