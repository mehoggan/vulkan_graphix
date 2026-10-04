#ifndef VULKAN_EARTH_READYMENU_H
#define VULKAN_EARTH_READYMENU_H

#include <cstdint>
#include <string>
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

namespace vulkan_graphix::Render {
class RenderContext;
}

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
    void draw(vulkan_graphix::Render::RenderContext& context);
    void showPreviousPlayerPage();
    void showNextPlayerPage();
    void setPlayerPageNum(std::int32_t i);
    void updatePageInfo();
    void updateMouse(std::int32_t x, std::int32_t y);
    void saveCurrentPlayerData();
    void updateNumPlayers(std::int32_t n);

private:
    float m_pos[3], m_width, m_height, m_color[4], m_border;
    float m_percent_border;
    float m_tank_prv_scr_pos[3], m_tank_prv_scr_width, m_tank_prv_scr_height;
    float m_tank_prv_scr_color[3];
    MainMenuButton* m_buttons[num_buttons];
    ImageObject* m_stat_images[num_stat_images];
    ControlItem* m_control_items[num_control_items];
    TextObject* m_player_page_num;
    TextObject* m_tank_stat_labels[num_tank_stats];
    MainMenuButton* m_button_pressed;
    std::string m_caption;
    std::int32_t m_current_player_index;
    std::int32_t m_num_players;
    std::int32_t* m_current_game_state;
    std::int32_t m_prv_scr_color_control;
    ControlItemTextField* m_text_field;
    GlobalSettings* m_global_settings;
    PlayerFactory* m_player_factory;
    Tank* m_tanks[num_tank_types];
    float m_tank_angle;
    bool m_start_music_played;
    vulkan_graphix::Render::UiMesh m_panel_mesh;
    float m_built_width = -1.0f;
    float m_built_height = -1.0f;
};

#endif