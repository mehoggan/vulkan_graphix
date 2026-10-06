#ifndef VULKAN_EARTH_CONTROLITEMSLIDERBAR_H
#define VULKAN_EARTH_CONTROLITEMSLIDERBAR_H

#include <cstdint>
#include <string>
#include <vector>

#include "vulkan_earth/ControlItem.h"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

class TextObject;

class ControlItemSliderbar : public ControlItem {
public:
    ControlItemSliderbar();
    ControlItemSliderbar(float new_x_pos,
      float new_y_pos,
      float new_z_pos,
      float red,
      float green,
      float blue,
      std::int32_t new_width,
      std::int32_t new_height,
      const std::string& new_caption,
      const std::string& menu_string,
      std::int32_t slider_starting_index);
    ~ControlItemSliderbar() override;
    void draw(vulkan_graphix::Render::RenderContext& context) override;
    void mouseClickEvent(std::int32_t x,
      std::int32_t y,
      std::int32_t state,
      bool still_over_control_item_sliderbar) override;
    float getXPos() override;
    float getYPos() override;
    float getHeight() override;
    float getWidth() override;
    float getBarXPos();
    float getSliderXPos();
    void setSliderXPos(float x);
    float getInterval();
    std::string collectData() override;
    void updateMouse(std::int32_t x, std::int32_t y) override;

private:
    void setOptionText(std::int32_t index) override;
    void setOptionText(const std::string& new_text) override;
    float m_x_pos;
    float m_y_pos;
    float m_z_pos;
    float m_bar_x_pos;
    float m_bar_y_pos;
    float m_bar_z_pos;
    std::int32_t m_bar_width;
    float m_slider_x_pos;
    float m_slider_y_pos;
    float m_slider_z_pos;
    std::int32_t m_slider_width;
    std::int32_t m_slider_height;
    float m_color[4];
    std::int32_t m_width;
    std::int32_t m_height;
    float m_interval;
    TextObject* m_option_text;
    TextObject* m_label;
    std::string m_caption;
    std::string m_menu_info;
    std::string m_current_option;
    std::int32_t m_menu_state;
    std::int32_t m_button_state;  // 0 = no button pressed, 1 = up button
                                  // pressed, 2 = down button pressed
    std::int32_t m_number_of_options;
    std::vector<std::string> m_all_options;
    bool m_is_slider_clicked;
    vulkan_graphix::Render::UiMesh m_frame_mesh;
    vulkan_graphix::Render::UiMesh m_slider_mesh;
    float m_slider_built_x = -1.0e30f;
    float m_slider_built_y = -1.0e30f;
    std::int32_t m_slider_built_clicked = -1;
};

#endif