#ifndef SHOP_MENU_H
#define SHOP_MENU_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>

class TextObject;
class ImageObject;
class ControlItem;
class ControlItemButton;
class ControlItemGrid;
class Weapon;
class Item;
class GlobalSettings;
class PlayerFactory;

const int shop_grid_row = 3;
const int shop_grid_col = 4;
const int inven_grid_row = 5;
const int inven_grid_col = 2;
const int num_sales_weapon = 10;
const int num_sales_item = 8;

class ShopMenu {
public:
    ShopMenu();
    ShopMenu(GLfloat width,
             GLfloat height,
             GLfloat percent_border,
             GlobalSettings* global_settings,
             PlayerFactory* player_factory,
             int* game_state);
    ~ShopMenu();
    void draw();
    void buttonTest(int x, int y, int button_down);
    void saveCurrentPlayerInfo();
    void displayCurrentPlayerInfo();
    void updateBuyDiscriptLabel();
    void updateSellLabel();
    void buyHandler();
    void sellHandler();
    void updateNumPlayers(int n);

private:
    void printDebugInfo();
    GLfloat pos[3], width, height, color[4], border;
    GLfloat percent_border;
    int* current_game_state;
    int num_players;
    int current_player_index;
    int current_player_balance;
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
};

#endif
