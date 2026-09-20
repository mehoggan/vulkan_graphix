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

const int SHOP_GRID_ROW = 3;
const int SHOP_GRID_COL = 4;
const int INVEN_GRID_ROW = 5;
const int INVEN_GRID_COL = 2;
const int NUM_SALES_WEAPON = 10;
const int NUM_SALES_ITEM = 8;

class ShopMenu {
public:
    ShopMenu();
    ShopMenu(GLfloat width,
             GLfloat height,
             GLfloat percentBorder,
             GlobalSettings* global_settings,
             PlayerFactory* player_factory,
             int* gameState);
    ~ShopMenu();
    void draw();
    void buttonTest(int x, int y, int buttonDown);
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
    GLfloat percentBorder;
    int* currentGameState;
    int numPlayers;
    int currentPlayerIndex;
    int currentPlayerBalance;
    ControlItemButton* buttons[5];
    ControlItemGrid* grids[2];
    TextObject* labelWpn;
    TextObject* labelItem;
    TextObject* labelPlayerNum;
    TextObject* labelPlayerBalance;
    TextObject* labelDiscription;
    TextObject* labelBuyPrice;
    TextObject* labelSellPrice;
    TextObject* labelShopWpnRemains[NUM_SALES_WEAPON];
    TextObject* labelShopItemRemains[NUM_SALES_ITEM];
    TextObject* labelInvenWpnRemains[INVEN_GRID_ROW];
    TextObject* labelInvenItemRemains[INVEN_GRID_ROW];
    ImageObject* imgShopWpns[NUM_SALES_WEAPON];
    ImageObject* imgShopItems[NUM_SALES_WEAPON];
    ImageObject* imgInvenWpns[INVEN_GRID_ROW];
    ImageObject* imgInvenItems[INVEN_GRID_ROW];
    Weapon* shopWpns[NUM_SALES_WEAPON];
    Item* shopItems[NUM_SALES_ITEM];
    Weapon* invenWpns[INVEN_GRID_ROW];
    Item* invenItems[INVEN_GRID_ROW];
    GlobalSettings* global_settings;
    PlayerFactory* player_factory;
};

#endif
