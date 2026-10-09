#include "vulkan_earth/ControlItemButton.h"
#include <cstdint>
#include <string>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/SubMenuLandscape.h"
#include "vulkan_earth/TerrainMaker.h"
#include "vulkan_earth/TextObject.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

extern void playSFX(std::int32_t sfx);

ControlItemButton::ControlItemButton() = default;

ControlItemButton::ControlItemButton(
    SubMenuLandscape* new_parent,
    float new_x_pos,
    float new_y_pos,
    float new_z_pos,
    float red,
    float green,
    float blue,
    std::int32_t new_width,
    std::int32_t new_height,
    const std::string& new_caption) {
  m_parent = new_parent;

  m_x_pos = new_x_pos;
  m_y_pos = new_y_pos;
  m_z_pos = new_z_pos;
  m_color[0] = red;
  m_color[1] = green;
  m_color[2] = blue;
  m_color[3] = 1.0;
  m_width = new_width;
  m_height = new_height;
  m_caption = new_caption;

  m_toggled = false;
  m_button_state = 0;
  m_menu_state = 0;

  /*	BUTTON TEXT PLACEMENT	*/
  std::int32_t real_length = 0;
  for (char ch : m_caption) {
    real_length +=
        vulkan_earth::textAdvance(vulkan_earth::FontId::TimesRoman24, ch);
  }
  float label_x_pos = m_x_pos + ((m_width) / 2) - (real_length / 2);
  float label_y_pos =
      m_y_pos + ((m_y_pos - (m_y_pos + m_height)) / 2) - m_height / 4;
  /*	END OF BUTTON TEXT PLACEMENT	*/
  m_label = new TextObject(
      m_caption,
      label_x_pos,
      label_y_pos,
      m_z_pos,
      vulkan_earth::FontId::TimesRoman24,
      0.0f,
      0.0f,
      0.0f);
}

ControlItemButton::~ControlItemButton() { delete m_label; }

void ControlItemButton::draw(render::RenderContext& context) {
  if (m_built_button_state != m_button_state) {
    m_mesh.clear();
    vulkan_earth::appendBevel(
        m_mesh,
        m_x_pos,
        m_y_pos,
        m_z_pos,
        m_width,
        m_height,
        math::Vec4<float>(m_color[0], m_color[1], m_color[2], m_color[3]),
        m_button_state != 0);
    m_built_button_state = m_button_state;
  }
  context.draw(m_mesh);
  m_label->draw(context);
}

float ControlItemButton::getXPos() { return m_x_pos; }
float ControlItemButton::getYPos() { return m_y_pos; }
float ControlItemButton::getHeight() { return m_height; }
float ControlItemButton::getWidth() { return m_width; }
bool ControlItemButton::isToggled() { return m_toggled; }
void ControlItemButton::updateButtonState() {
  if (m_toggled)
    m_button_state = 1;
  else
    m_button_state = 0;
}
void ControlItemButton::setToggled(bool t) { m_toggled = t; }
void ControlItemButton::setOptionText(std::int32_t index) {}
void ControlItemButton::setOptionText(const std::string& new_text) {}
std::string ControlItemButton::collectData() { return "Button"; }

void ControlItemButton::mouseClickEvent(
    std::int32_t x,
    std::int32_t y,
    std::int32_t state,
    bool /*still_over_control_item_button*/) {
  if (state) {
    if ((x >= (m_x_pos) && x <= ((m_x_pos) + (m_width))) &&
        (y <= (m_y_pos) &&
         y >= ((m_y_pos) - (m_height)))) {  // This if statement -->
                                            // stillOverControlItemButton
      m_button_state = 1;
      m_toggled = true;
    } else {
      m_button_state = 0;
    }
  } else {
    m_button_state = 0;
  }
}

void ControlItemButton::updateMouse(std::int32_t x, std::int32_t y) {}
