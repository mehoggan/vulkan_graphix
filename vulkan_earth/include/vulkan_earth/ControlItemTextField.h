#ifndef VULKAN_EARTH_CONTROLITEMTEXTFIELD_H
#define VULKAN_EARTH_CONTROLITEMTEXTFIELD_H

#include <cstdint>
#include <string>

#include "vulkan_earth/ControlItem.h"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

class TextObject;

class ControlItemTextField : public ControlItem {
public:
  ControlItemTextField();
  ControlItemTextField(float new_x_pos,
      float new_y_pos,
      float new_z_pos,
      float red,
      float green,
      float blue,
      std::int32_t new_width,
      std::int32_t new_height);
  ~ControlItemTextField() override;
  void draw(vulkan_graphix::Render::RenderContext& context) override;
  void mouseClickEvent(std::int32_t x,
      std::int32_t y,
      std::int32_t state,
      bool still_over_control_item_text_field) override;
  float getXPos() override;
  float getYPos() override;
  float getHeight() override;
  float getWidth() override;
  bool isTextFieldActive();
  std::string collectData() override;
  void updateMouse(std::int32_t x, std::int32_t y) override;
  void keyHandler(std::uint8_t key);
  void deactivate();
  void setOptionText(const std::string& new_text) override;
  void clearTextBuffer();
  void setTextBuffer(const std::string& new_text);

private:
  void setOptionText(std::int32_t index) override;

  float m_x_pos;
  float m_y_pos;
  float m_z_pos;
  float m_color[4];
  std::int32_t m_width;
  std::int32_t m_height;
  TextObject* m_current_text;
  bool m_text_field_active;
  std::string m_current_chars;
  std::int32_t m_current_length;
  std::int32_t m_number_of_frames;
  std::int32_t m_text_cursor_on;  // this is a toggle, -1 off, 1 on
  vulkan_graphix::Render::UiMesh m_frame_mesh;
  vulkan_graphix::Render::UiMesh m_cursor_mesh;
  bool m_cursor_built_visible = false;
  std::string m_cursor_built_chars;
};
#endif  // VULKAN_EARTH_CONTROLITEMTEXTFIELD_H