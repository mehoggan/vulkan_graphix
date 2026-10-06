#ifndef VULKAN_EARTH_CONTROLITEMCHECKBOX_H
#define VULKAN_EARTH_CONTROLITEMCHECKBOX_H

#include <cstdint>
#include <string>

#include "vulkan_earth/ControlItem.h"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

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
    void draw(vulkan_graphix::Render::RenderContext& context) override;
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
    float m_x_pos;
    float m_y_pos;
    float m_z_pos;
    float m_color[4];
    std::int32_t m_width;
    std::int32_t m_height;
    TextObject* m_label;
    std::string m_caption;
    std::int32_t m_menu_state;
    std::int32_t m_button_state;  // 0 = no button pressed, 1 = up button
                                  // pressed, 2 = down button pressed
    vulkan_graphix::Render::UiMesh m_mesh;
    std::int32_t m_built_button_state = -1;
    std::int32_t m_built_menu_state = -1;
};
#endif  // VULKAN_EARTH_CONTROLITEMCHECKBOX_H