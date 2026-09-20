#ifndef INVENTORY_H
#define INVENTORY_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include "Player.h"

class TextObject;
class ControlItemGrid;
class ImageObject;
class TextObject;
class Player;
class Weapon;
class Item;

class Inventory {
public:
    Inventory();
    Inventory(GLfloat x, GLfloat y, int width, int height);
    ~Inventory();
    void draw();
    void setupInventory(Player* player);
    void handleInventory(Player* currentPlayer, int invenIndex);
    void keyHandler(int key);
    int getSelectedIndex();

private:
    GLfloat xPos, yPos;
    int width, height;
    ControlItemGrid* invenGrid;
    TextObject* title;
    TextObject* descript;
    TextObject* explain;
    ImageObject* imgInven[PLAYER_MAX_WEAPONS + PLAYER_MAX_ITEMS];
    TextObject* remainings[PLAYER_MAX_WEAPONS + PLAYER_MAX_ITEMS];
    Weapon* weapons[PLAYER_MAX_WEAPONS];
    Item* items[PLAYER_MAX_ITEMS];
    int selectCellRow, selectCellCol;
};
#endif INVENTORY_H