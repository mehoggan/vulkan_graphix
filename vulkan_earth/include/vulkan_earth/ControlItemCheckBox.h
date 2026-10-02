#ifndef Control_ITEM_Check_BOX_H
#define Control_ITEM_Check_BOX_H

#include <cstdint>
#include <string>

#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/render/Mesh.h"

class TextObject;

class ControlItemCheckBox : public ControlItem {
public:
    ControlItemCheckBox();
    ControlItemCheckBox(float new_x_pos,
                        float new_y_pos,
                        float new_z_pos,
                        float red,
                        float green,
                        float blue,
                        std::int32_t new_width,
                        std::int32_t new_height,
                        const std::string& new_caption);
    ~ControlItemCheckBox() override;
    void draw(vulkan_earth::render::RenderContext& context) override;
    void mouseClickEvent(std::int32_t x,
                         std::int32_t y,
                         std::int32_t state,
                         bool still_over_control_item_check_box) override;
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
    std::string caption;
    std::int32_t menu_state;
    std::int32_t button_state;  // 0 = no button pressed, 1 = up button
                                // pressed, 2 = down button pressed
    vulkan_earth::render::UiMesh mesh;
    std::int32_t built_button_state = -1;
    std::int32_t built_menu_state = -1;
};
#endif  // Control_ITEM_Check_BOX_H