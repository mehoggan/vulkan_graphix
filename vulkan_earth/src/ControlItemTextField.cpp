#include "vulkan_earth/ControlItemTextField.h"
#include <stdio.h>
#include <cstdint>
#include <string>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

const std::int32_t max_chars = 15;

extern void playSFX(std::int32_t sfx);

ControlItemTextField::ControlItemTextField() = default;

ControlItemTextField::ControlItemTextField(float new_x_pos,
    float new_y_pos,
    float new_z_pos,
    float red,
    float green,
    float blue,
    std::int32_t new_width,
    std::int32_t new_height) {
  m_x_pos = new_x_pos;
  m_y_pos = new_y_pos;
  m_z_pos = new_z_pos;
  m_color[0] = red;
  m_color[1] = green;
  m_color[2] = blue;
  m_color[3] = 1.0;
  m_width = new_width;
  m_height = new_height;
  m_current_text = nullptr;
  m_text_field_active = false;
  clearTextBuffer();
  m_current_length = 0;
  m_number_of_frames = 0;
  m_text_cursor_on = 1;
  setOptionText("");
}

ControlItemTextField::~ControlItemTextField() { delete m_current_text; }

void ControlItemTextField::draw(render::RenderContext& context) {
  using Vec3 = math::Vec3<float>;
  using Vec4 = math::Vec4<float>;
  if (m_number_of_frames == 50) {
    m_text_cursor_on *= -1;  // toggle
    m_number_of_frames = 0;
  }
  m_number_of_frames++;

  if (m_frame_mesh.triangles().empty()) {
    vulkan_earth::appendFrame(m_frame_mesh,
        m_x_pos,
        m_y_pos,
        m_z_pos,
        m_width,
        m_height,
        Vec4(m_color[0] - 0.6f,
            m_color[1] - 0.6f,
            m_color[2] - 0.6f,
            m_color[3]),
        Vec4(m_color[0], m_color[1], m_color[2], m_color[3]),
        Vec4(m_color[0] - 0.3f,
            m_color[1] - 0.3f,
            m_color[2] - 0.3f,
            m_color[3]));
  }
  context.draw(m_frame_mesh);

  const bool cursor_visible = m_text_field_active && m_text_cursor_on == 1;
  if (cursor_visible != m_cursor_built_visible ||
      (cursor_visible && m_cursor_built_chars != m_current_chars)) {
    m_cursor_mesh.clear();
    if (cursor_visible) {
      std::int32_t real_length = 0;
      for (char ch : m_current_chars) {
        if (ch != ' ') {
          real_length += vulkan_earth::textAdvance(
              vulkan_earth::FontId::TimesRoman24, ch);
        }
      }
      m_cursor_mesh.addQuad(
          {Vec3(m_x_pos + 0.02 * m_width + real_length,
               m_y_pos - 0.15 * m_height,
               m_z_pos + 0.1),
              Vec3(m_x_pos + 0.02 * m_width + real_length,
                  m_y_pos - m_height + 0.15 * m_height,
                  m_z_pos + 0.1),
              Vec3(m_x_pos + 0.02 * m_width + real_length + 2,
                  m_y_pos - m_height + 0.15 * m_height,
                  m_z_pos + 0.1),
              Vec3(m_x_pos + 0.02 * m_width + real_length + 2,
                  m_y_pos - 0.15 * m_height,
                  m_z_pos + 0.1)},
          Vec4(0, 0, 0, 1));
    }
    m_cursor_built_visible = cursor_visible;
    m_cursor_built_chars = m_current_chars;
  }
  context.draw(m_cursor_mesh);

  if (m_current_text) m_current_text->draw(context);
}

float ControlItemTextField::getXPos() { return m_x_pos; }
float ControlItemTextField::getYPos() { return m_y_pos; }
float ControlItemTextField::getHeight() { return m_height; }
float ControlItemTextField::getWidth() { return m_width; }
bool ControlItemTextField::isTextFieldActive() { return m_text_field_active; }
std::string ControlItemTextField::collectData() {
  return m_current_text->getOutput();
}
void ControlItemTextField::deactivate() { m_text_field_active = false; }
void ControlItemTextField::setOptionText(std::int32_t index) {}

void ControlItemTextField::setOptionText(const std::string& new_text) {
  delete m_current_text;

  std::int32_t real_length = 0;
  for (char ch : new_text) {
    real_length +=
        vulkan_earth::textAdvance(vulkan_earth::FontId::TimesRoman24, ch);
  }
  float label_x_pos = m_x_pos + 0.02 * m_width;
  float label_y_pos =
      m_y_pos + ((m_y_pos - (m_y_pos + m_height)) / 2) - m_height / 4;
  m_current_text = new TextObject(new_text,
      label_x_pos,
      label_y_pos,
      m_z_pos + 0.1,
      vulkan_earth::FontId::TimesRoman24,
      0.0f,
      0.0f,
      0.0f);
}

void ControlItemTextField::mouseClickEvent(std::int32_t /*x*/,
    std::int32_t /*y*/,
    std::int32_t state,
    bool still_over_control_item_text_field) {
  if (state == 0)
    if (still_over_control_item_text_field) m_text_field_active = true;
}

void ControlItemTextField::updateMouse(std::int32_t x, std::int32_t y) {}
void ControlItemTextField::keyHandler(std::uint8_t key) {
  if (m_text_field_active) {
    if ((((key >= 48) && (key <= 57)) || ((key >= 65) && (key < 90))) ||
        ((key >= 97) && (key <= 122))) {
      if (m_current_text == nullptr) {
        setOptionText("");
        m_current_length = 0;
      }
      if (m_current_length < max_chars) {
        playSFX(KEYTYPING);
        m_current_chars[m_current_length] = key;
        m_current_length++;
        setOptionText(m_current_chars);
      }

    } else if (key == 8) {
      if (m_current_text) {
        if (m_current_length != 0) {
          playSFX(KEYTYPING);
          m_current_chars[m_current_length - 1] = ' ';
          m_current_length--;
          setOptionText(m_current_chars);
        }
      }
    }
  }
}

void ControlItemTextField::clearTextBuffer() {
  m_current_chars.assign(max_chars, ' ');
  m_current_length = 0;
}

void ControlItemTextField::setTextBuffer(const std::string& new_text) {
  setOptionText(new_text);
  clearTextBuffer();
  std::int32_t new_length = 0;
  for (size_t i = 0; i < new_text.size() && i < m_current_chars.size(); i++) {
    m_current_chars[i] = new_text[i];
    if (new_text[i] != ' ') new_length++;
  }
  m_current_length = new_length;
}