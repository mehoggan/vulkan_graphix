#ifndef CONTROL_ITEM_GRID_H
#define CONTROL_ITEM_GRID_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>
#include <string>

#include "vulkan_earth/ControlItem.h"

class ControlItemButton;
class ImageObject;
class TextObject;

class ControlItemGrid : public ControlItem {
public:
    ControlItemGrid();
    ControlItemGrid(float new_x_pos,
                    float new_y_pos,
                    float new_z_pos,
                    std::int32_t new_width,
                    std::int32_t new_height,
                    std::int32_t new_rows,
                    std::int32_t new_cols,
                    float active_cell_color_red,
                    float active_cell_color_green,
                    float active_cell_color_blue,
                    bool new_visible_lines,
                    bool new_multi_selectable);
    ~ControlItemGrid() override;
    void draw() override;
    void mouseClickEvent(std::int32_t x,
                         std::int32_t y,
                         std::int32_t state,
                         bool still_over_arrow_button) override;
    void updateMouse(std::int32_t x, std::int32_t y) override;
    float getXPos() override;
    float getYPos() override;
    float getHeight() override;
    float getWidth() override;
    std::string collectData() override;
    void setOptionText(std::int32_t index) override;
    void setOptionText(const std::string& new_text) override;
    void setImageSizeToCell(ImageObject* img, float scale);
    void placeImageToCell(ImageObject* img,
                          std::int32_t row,
                          std::int32_t col);
    void placeTextToCell(TextObject* text, std::int32_t row, std::int32_t col);
    void deselectAllCells();
    bool* getSelectedCells();
    void selectCell(std::int32_t row, std::int32_t col);

private:
    float x_pos, y_pos, z_pos;
    float active_cell_color[4];
    std::int32_t width, height;
    float cell_width, cell_height;
    std::int32_t rows, cols;
    bool visible_lines;
    bool multi_selectable;
    bool* selected_cells;
    ControlItemButton** buttons;
};

#endif