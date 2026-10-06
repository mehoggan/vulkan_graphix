#ifndef VULKAN_EARTH_CONTROLITEMGRID_H
#define VULKAN_EARTH_CONTROLITEMGRID_H

#include <cstdint>
#include <string>
#include <vector>

#include "vulkan_earth/ControlItem.h"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

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
    void draw(vulkan_graphix::Render::RenderContext& context) override;
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
    void placeImageToCell(
      ImageObject* img, std::int32_t row, std::int32_t col);
    void placeTextToCell(TextObject* text, std::int32_t row, std::int32_t col);
    void deselectAllCells();
    bool* getSelectedCells();
    void selectCell(std::int32_t row, std::int32_t col);

private:
    float m_x_pos, m_y_pos, m_z_pos;
    float m_active_cell_color[4];
    std::int32_t m_width, m_height;
    float m_cell_width, m_cell_height;
    std::int32_t m_rows, m_cols;
    bool m_visible_lines;
    bool m_multi_selectable;
    bool* m_selected_cells;
    ControlItemButton** m_buttons;
    vulkan_graphix::Render::UiMesh m_mesh;
    std::vector<bool> m_built_toggled;
    bool m_mesh_built = false;
};

#endif