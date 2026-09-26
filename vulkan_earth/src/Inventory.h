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
    void handleInventory(Player* current_player, int inven_index);
    void keyHandler(int key);
    int getSelectedIndex();

private:
    GLfloat x_pos, y_pos;
    int width, height;
    ControlItemGrid* inven_grid;
    TextObject* title;
    TextObject* descript;
    TextObject* explain;
    ImageObject* img_inven[player_max_weapons + player_max_items];
    TextObject* remainings[player_max_weapons + player_max_items];
    Weapon* weapons[player_max_weapons];
    Item* items[player_max_items];
    int select_cell_row, select_cell_col;
};
#endif  // INVENTORY_H