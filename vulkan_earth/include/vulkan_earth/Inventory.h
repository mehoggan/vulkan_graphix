#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdio.h>
#include <cstdint>
#include "vulkan_earth/Player.h"

namespace vulkan_earth::render {
class RenderContext;
}

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
    Inventory(float x, float y, std::int32_t width, std::int32_t height);
    ~Inventory();
    void draw(vulkan_earth::render::RenderContext& context);
    void setupInventory(Player* player);
    void handleInventory(Player* current_player, std::int32_t inven_index);
    void keyHandler(std::int32_t key);
    std::int32_t getSelectedIndex();

private:
    float x_pos, y_pos;
    std::int32_t width, height;
    ControlItemGrid* inven_grid;
    TextObject* title;
    TextObject* descript;
    TextObject* explain;
    ImageObject* img_inven[player_max_weapons + player_max_items];
    TextObject* remainings[player_max_weapons + player_max_items];
    Weapon* weapons[player_max_weapons];
    Item* items[player_max_items];
    std::int32_t select_cell_row, select_cell_col;
};
#endif  // INVENTORY_H