#ifndef Control_ITEM_SELECTION_BOX_H
#define Control_ITEM_SELECTION_BOX_H

#include <cstdint>
#include <string>
#include <vector>

#include "vulkan_earth/ControlItem.h"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

class TextObject;

class ControlItemSelectionBox : public ControlItem {
public:
    ControlItemSelectionBox();
    ControlItemSelectionBox(float new_x_pos,
                            float new_y_pos,
                            float new_z_pos,
                            float red,
                            float green,
                            float blue,
                            std::int32_t new_width,
                            std::int32_t new_height,
                            const std::string& new_caption,
                            const std::string& menu_string);
    ~ControlItemSelectionBox() override;
    void draw(vulkan_graphix::Render::RenderContext& context) override;
    void mouseClickEvent(std::int32_t x,
                         std::int32_t y,
                         std::int32_t state,
                         bool still_over_control_item_selection_box) override;
    float getXPos() override;
    float getYPos() override;
    float getHeight() override;
    float getWidth() override;
    std::string collectData() override;
    void updateMouse(std::int32_t x, std::int32_t y) override;

private:
    void setOptionText(std::int32_t index) override;
    void setOptionText(const std::string& new_text) override;
    float x_pos;
    float y_pos;
    float z_pos;
    float color[4];
    std::int32_t width;
    std::int32_t height;
    TextObject* label;
    TextObject* option_text;
    std::string caption;
    std::string menu_info;
    std::string current_option;
    std::int32_t menu_state;
    std::int32_t button_state;  // 0 = no button pressed, 1 = up button
                                // pressed, 2 = down button pressed
    std::int32_t number_of_options;
    std::vector<std::string> all_options;
    vulkan_graphix::Render::UiMesh frame_mesh;
    vulkan_graphix::Render::UiMesh arrow_mesh;
    std::int32_t arrows_built_for = -1;
};
#endif  // Control_ITEM_SELECTION_BOX_H