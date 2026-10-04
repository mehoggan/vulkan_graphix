#ifndef VULKAN_EARTH_SHOPMENU_H
#define VULKAN_EARTH_SHOPMENU_H

#include <cstdint>
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

namespace vulkan_graphix::Render {
class RenderContext;
}

class TextObject;
class ImageObject;
class ControlItem;
class ControlItemButton;
class ControlItemGrid;
class Weapon;
class Item;
class GlobalSettings;
class PlayerFactory;

const std::int32_t shop_grid_row = 3;
const std::int32_t shop_grid_col = 4;
const std::int32_t inven_grid_row = 5;
const std::int32_t inven_grid_col = 2;
const std::int32_t num_sales_weapon = 10;
const std::int32_t num_sales_item = 8;

class ShopMenu {
public:
    ShopMenu();
    ShopMenu(float new_width,
             float new_height,
             float new_percent_border,
             GlobalSettings* new_global_settings,
             PlayerFactory* new_player_factory,
             std::int32_t* game_state);
    ~ShopMenu();
    void draw(vulkan_graphix::Render::RenderContext& context);
    void buttonTest(std::int32_t x, std::int32_t y, std::int32_t button_down);
    void saveCurrentPlayerInfo();
    void displayCurrentPlayerInfo();
    void updateBuyDiscriptLabel();
    void updateSellLabel();
    void buyHandler();
    void sellHandler();
    void updateNumPlayers(std::int32_t n);

private:
    void printDebugInfo();
    float m_pos[3], m_width, m_height, m_color[4], m_border;
    float m_percent_border;
    std::int32_t* m_current_game_state;
    std::int32_t m_num_players;
    std::int32_t m_current_player_index;
    std::int32_t m_current_player_balance;
    ControlItemButton* m_buttons[5];
    ControlItemGrid* m_grids[2];
    TextObject* m_label_wpn;
    TextObject* m_label_item;
    TextObject* m_label_player_num;
    TextObject* m_label_player_balance;
    TextObject* m_label_discription;
    TextObject* m_label_buy_price;
    TextObject* m_label_sell_price;
    TextObject* m_label_shop_wpn_remains[num_sales_weapon];
    TextObject* m_label_shop_item_remains[num_sales_item];
    TextObject* m_label_inven_wpn_remains[inven_grid_row];
    TextObject* m_label_inven_item_remains[inven_grid_row];
    ImageObject* m_img_shop_wpns[num_sales_weapon];
    ImageObject* m_img_shop_items[num_sales_weapon];
    ImageObject* m_img_inven_wpns[inven_grid_row];
    ImageObject* m_img_inven_items[inven_grid_row];
    Weapon* m_shop_wpns[num_sales_weapon];
    Item* m_shop_items[num_sales_item];
    Weapon* m_inven_wpns[inven_grid_row];
    Item* m_inven_items[inven_grid_row];
    GlobalSettings* m_global_settings;
    PlayerFactory* m_player_factory;
    vulkan_graphix::Render::UiMesh m_panel_mesh;
    float m_built_width = -1.0f;
    float m_built_height = -1.0f;
};

#endif
