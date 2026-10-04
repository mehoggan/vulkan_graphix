#ifndef VULKAN_EARTH_CONTROLITEMSELECTIONBOX_H
#define VULKAN_EARTH_CONTROLITEMSELECTIONBOX_H

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
    float m_x_pos;
    float m_y_pos;
    float m_z_pos;
    float m_color[4];
    std::int32_t m_width;
    std::int32_t m_height;
    TextObject* m_label;
    TextObject* m_option_text;
    std::string m_caption;
    std::string m_menu_info;
    std::string m_current_option;
    std::int32_t m_menu_state;
    std::int32_t m_button_state;  // 0 = no button pressed, 1 = up button
                                  // pressed, 2 = down button pressed
    std::int32_t m_number_of_options;
    std::vector<std::string> m_all_options;
    vulkan_graphix::Render::UiMesh m_frame_mesh;
    vulkan_graphix::Render::UiMesh m_arrow_mesh;
    std::int32_t m_arrows_built_for = -1;
};
#endif  // VULKAN_EARTH_CONTROLITEMSELECTIONBOX_H