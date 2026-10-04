#include <cstdint>
#include <string>

#ifndef Control_ITEM_BUTTON_H
#define Control_ITEM_BUTTON_H

#include "vulkan_earth/ControlItem.h"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

class TextObject;
class SubMenu;
class SubMenuLandscape;

class ControlItemButton : public ControlItem {
public:
    ControlItemButton();
    ControlItemButton(SubMenuLandscape* new_parent,
                      float new_x_pos,
                      float new_y_pos,
                      float new_z_pos,
                      float red,
                      float green,
                      float blue,
                      std::int32_t new_width,
                      std::int32_t new_height,
                      const std::string& new_caption);
    ~ControlItemButton() override;
    void draw(vulkan_graphix::Render::RenderContext& context) override;
    void mouseClickEvent(std::int32_t x,
                         std::int32_t y,
                         std::int32_t state,
                         bool still_over_control_item_button) override;
    float getXPos() override;
    float getYPos() override;
    float getHeight() override;
    float getWidth() override;
    std::string collectData() override;
    void updateMouse(std::int32_t x, std::int32_t y) override;
    void setOptionText(std::int32_t index) override;
    void setOptionText(const std::string& new_text) override;
    void updateButtonState();
    bool isToggled();
    void setToggled(bool t);

private:
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
    SubMenuLandscape* parent;
    bool toggled;
    vulkan_graphix::Render::UiMesh mesh;
    std::int32_t built_button_state = -1;
};
#endif  // Control_ITEM_BUTTON_H