#ifndef VULKAN_EARTH_INVENTORY_H
#define VULKAN_EARTH_INVENTORY_H

#include <cstdint>
#include "vulkan_earth/Player.h"
#include "vulkan_graphix/Render/Renderer.h"

namespace vulkan_graphix::Render {
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
    void draw(vulkan_graphix::Render::RenderContext& context);
    void setupInventory(Player* player);
    void handleInventory(Player* current_player, std::int32_t inven_index);
    void keyHandler(std::int32_t key);
    std::int32_t getSelectedIndex();

private:
    float m_x_pos, m_y_pos;
    std::int32_t m_width, m_height;
    ControlItemGrid* m_inven_grid;
    TextObject* m_title;
    TextObject* m_descript;
    TextObject* m_explain;
    ImageObject* m_img_inven[player_max_weapons + player_max_items];
    TextObject* m_remainings[player_max_weapons + player_max_items];
    Weapon* m_weapons[player_max_weapons];
    Item* m_items[player_max_items];
    std::int32_t m_select_cell_row, m_select_cell_col;
};
#endif  // VULKAN_EARTH_INVENTORY_H