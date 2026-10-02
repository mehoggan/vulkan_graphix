#ifndef SHOP_MENU_H
#define SHOP_MENU_H

#include <stdio.h>
#include <cstdint>
#include <iostream>

#include "vulkan_earth/render/Mesh.h"

namespace vulkan_earth::render {
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
    void draw(vulkan_earth::render::RenderContext& context);
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
    float pos[3], width, height, color[4], border;
    float percent_border;
    std::int32_t* current_game_state;
    std::int32_t num_players;
    std::int32_t current_player_index;
    std::int32_t current_player_balance;
    ControlItemButton* buttons[5];
    ControlItemGrid* grids[2];
    TextObject* label_wpn;
    TextObject* label_item;
    TextObject* label_player_num;
    TextObject* label_player_balance;
    TextObject* label_discription;
    TextObject* label_buy_price;
    TextObject* label_sell_price;
    TextObject* label_shop_wpn_remains[num_sales_weapon];
    TextObject* label_shop_item_remains[num_sales_item];
    TextObject* label_inven_wpn_remains[inven_grid_row];
    TextObject* label_inven_item_remains[inven_grid_row];
    ImageObject* img_shop_wpns[num_sales_weapon];
    ImageObject* img_shop_items[num_sales_weapon];
    ImageObject* img_inven_wpns[inven_grid_row];
    ImageObject* img_inven_items[inven_grid_row];
    Weapon* shop_wpns[num_sales_weapon];
    Item* shop_items[num_sales_item];
    Weapon* inven_wpns[inven_grid_row];
    Item* inven_items[inven_grid_row];
    GlobalSettings* global_settings;
    PlayerFactory* player_factory;
    vulkan_earth::render::UiMesh panel_mesh;
    float built_width = -1.0f;
    float built_height = -1.0f;
};

#endif
