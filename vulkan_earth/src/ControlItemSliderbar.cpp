#include "vulkan_earth/ControlItemSliderbar.h"
#include <cstdint>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/TextObject.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

extern void playSFX(std::int32_t sfx);

ControlItemSliderbar::ControlItemSliderbar() = default;
ControlItemSliderbar::ControlItemSliderbar(float new_x_pos,
    float new_y_pos,
    float new_z_pos,
    float red,
    float green,
    float blue,
    std::int32_t new_width,
    std::int32_t new_height,
    const std::string& new_caption,
    const std::string& menu_string,
    std::int32_t slider_starting_index) {
  m_x_pos = new_x_pos;
  m_y_pos = new_y_pos;
  m_z_pos = new_z_pos;
  m_color[0] = red;
  m_color[1] = green;
  m_color[2] = blue;
  m_color[3] = 1.0;
  m_width = new_width;
  m_height = new_height;

  m_bar_width = new_width - new_width * 0.1;
  m_bar_x_pos = new_x_pos + (new_width - m_bar_width) / 2;
  m_bar_y_pos = new_y_pos - new_height / 1.5;
  m_bar_z_pos = new_z_pos + 0.5;

  m_slider_x_pos = m_bar_x_pos - m_bar_width * 0.012;
  m_slider_y_pos = new_y_pos - new_height / 1.7;
  m_slider_z_pos = m_bar_z_pos + 0.5;
  m_slider_width = new_width / 5 * 0.1;
  m_slider_height = new_height * 0.15;

  m_caption = new_caption;
  m_menu_info = menu_string;
  m_is_slider_clicked = false;

  // split menuInfo on '/' into allOptions
  std::string current;
  for (char ch : m_menu_info) {
    if (ch == '/') {
      m_all_options.push_back(current);
      current.clear();
    } else {
      current += ch;
    }
  }
  m_number_of_options = static_cast<std::int32_t>(m_all_options.size());
  m_interval = m_bar_width /
      (m_number_of_options - 1.0);  // if it's divided by an integer, the whole
                                    // thing becomes an integer value???

  m_menu_state = slider_starting_index;
  m_button_state = 0;
  m_option_text = nullptr;
  setOptionText(m_menu_state);  // set option to first option
  /*	BUTTON TEXT PLACEMENT	*/
  std::int32_t real_length = 0;
  for (char ch : m_caption) {
    real_length +=
        vulkan_earth::textAdvance(vulkan_earth::FontId::TimesRoman24, ch);
  }
  float label_x_pos = m_bar_x_pos;
  float label_y_pos = m_y_pos - m_height * 0.45;
  /*	END OF BUTTON TEXT PLACEMENT	*/
  m_label = new TextObject(m_caption,
      label_x_pos,
      label_y_pos,
      m_z_pos,
      vulkan_earth::FontId::TimesRoman24,
      0.0f,
      0.0f,
      0.0f);
}
ControlItemSliderbar::~ControlItemSliderbar() {
  delete m_option_text;
  delete m_label;
}

void ControlItemSliderbar::draw(render::RenderContext& context) {
  using Vec3 = math::Vec3<float>;
  using Vec4 = math::Vec4<float>;
  if (m_frame_mesh.triangles().empty()) {
    // draw main button box (sunken bevel: -0.2 top/left, +0.4
    // bottom/right)
    vulkan_earth::appendFrame(m_frame_mesh,
        m_x_pos,
        m_y_pos,
        m_z_pos,
        m_width,
        m_height,
        Vec4(m_color[0] - 0.2f,
            m_color[1] - 0.2f,
            m_color[2] - 0.2f,
            m_color[3]),
        Vec4(m_color[0], m_color[1], m_color[2], m_color[3]),
        Vec4(m_color[0] + 0.4f,
            m_color[1] + 0.4f,
            m_color[2] + 0.4f,
            m_color[3]));
    // draw bar lines
    const Vec4 black(0, 0, 0, 1);
    m_frame_mesh.addLine(Vec3(m_bar_x_pos, m_bar_y_pos + 1, m_bar_z_pos),
        Vec3(m_bar_x_pos + m_bar_width, m_bar_y_pos + 1, m_bar_z_pos),
        black);
    m_frame_mesh.addLine(Vec3(m_bar_x_pos, m_bar_y_pos, m_bar_z_pos),
        Vec3(m_bar_x_pos + m_bar_width, m_bar_y_pos, m_bar_z_pos),
        black);
    m_frame_mesh.addLine(Vec3(m_bar_x_pos, m_bar_y_pos - 1, m_bar_z_pos),
        Vec3(m_bar_x_pos + m_bar_width, m_bar_y_pos - 1, m_bar_z_pos),
        black);
    for (std::int32_t i = 0; i < m_number_of_options; i++) {
      m_frame_mesh.addLine(Vec3(m_bar_x_pos + (m_interval * i),
                               m_bar_y_pos + m_height * 0.07,
                               m_bar_z_pos),
          Vec3(m_bar_x_pos + (m_interval * i),
              m_bar_y_pos - m_height * 0.07,
              m_bar_z_pos),
          black);
    }
  }
  context.draw(m_frame_mesh);

  // draw slider: raised when idle, highlighted while being dragged
  if (m_slider_built_x != m_slider_x_pos ||
      m_slider_built_y != m_slider_y_pos ||
      m_slider_built_clicked !=
          static_cast<std::int32_t>(m_is_slider_clicked)) {
    m_slider_mesh.clear();
    if (!m_is_slider_clicked) {
      vulkan_earth::appendBevel(m_slider_mesh,
          m_slider_x_pos,
          m_slider_y_pos,
          m_slider_z_pos,
          m_slider_width,
          m_slider_height,
          Vec4(m_color[0], m_color[1], m_color[2], m_color[3]),
          false);
    } else {
      vulkan_earth::appendFrame(m_slider_mesh,
          m_slider_x_pos,
          m_slider_y_pos,
          m_slider_z_pos,
          m_slider_width,
          m_slider_height,
          Vec4(m_color[0] + 0.4f,
              m_color[1] + 0.4f,
              m_color[2] + 0.4f,
              m_color[3]),
          Vec4(m_color[0] + 0.2f,
              m_color[1] + 0.2f,
              m_color[2] + 0.2f,
              m_color[3]),
          Vec4(m_color[0] - 0.2f,
              m_color[1] - 0.2f,
              m_color[2] - 0.2f,
              m_color[3]));
    }
    m_slider_built_x = m_slider_x_pos;
    m_slider_built_y = m_slider_y_pos;
    m_slider_built_clicked = static_cast<std::int32_t>(m_is_slider_clicked);
  }
  context.draw(m_slider_mesh);

  m_label->draw(context);
  m_option_text->draw(context);
}

float ControlItemSliderbar::getXPos() { return m_x_pos; }
float ControlItemSliderbar::getYPos() { return m_y_pos; }
float ControlItemSliderbar::getHeight() { return m_height; }
float ControlItemSliderbar::getWidth() { return m_width; }
float ControlItemSliderbar::getBarXPos() { return m_bar_x_pos; }
float ControlItemSliderbar::getInterval() { return m_interval; }
float ControlItemSliderbar::getSliderXPos() { return m_slider_x_pos; }
void ControlItemSliderbar::setSliderXPos(float x) { m_slider_x_pos = x; }
std::string ControlItemSliderbar::collectData() { return m_current_option; }

void ControlItemSliderbar::setOptionText(const std::string& new_text) {}

void ControlItemSliderbar::setOptionText(std::int32_t index) {
  m_current_option = m_all_options[index];
  std::int32_t real_length = 0;
  for (char ch : m_current_option) {
    real_length +=
        vulkan_earth::textAdvance(vulkan_earth::FontId::TimesRoman24, ch);
  }
  float label_x_pos = m_x_pos + (m_width / 2) - (real_length / 2);
  float label_y_pos = m_y_pos - m_height * 0.45;
  delete m_option_text;
  m_option_text = new TextObject(m_current_option,
      label_x_pos,
      label_y_pos,
      m_z_pos,
      vulkan_earth::FontId::TimesRoman24,
      0.0f,
      0.0f,
      0.0f);
  m_menu_state = index;
  m_slider_x_pos = m_bar_x_pos + m_interval * index;
}

void ControlItemSliderbar::mouseClickEvent(std::int32_t x,
    std::int32_t y,
    std::int32_t state,
    bool /*still_over_control_item_sliderbar*/) {
  if (state == 1) {
    // check if the click is on the slider
    if ((m_slider_x_pos < x && x < m_slider_x_pos + m_slider_width) &&
        (m_slider_y_pos - m_slider_height < y && y < m_slider_y_pos)) {
      m_is_slider_clicked = true;
    }
    // check if the click is either left or right side from the slider
    else if ((m_bar_x_pos < x && x < m_slider_x_pos) &&
        (m_slider_y_pos - m_slider_height - 5.5 < y &&
            y < m_slider_y_pos + 5.5)) {
      playSFX(SMALL_CLICK);
      m_slider_x_pos -= m_interval;
      m_menu_state--;
      if (m_menu_state < 0)  // wrap around check
        m_menu_state += m_number_of_options;
      setOptionText(m_menu_state);
    } else if ((m_slider_x_pos + m_slider_width < x &&
                   x < m_bar_x_pos + m_bar_width) &&
        (m_slider_y_pos - m_slider_height - 5.5 < y &&
            y < m_slider_y_pos + 5.5)) {
      playSFX(SMALL_CLICK);
      m_slider_x_pos += m_interval;
      m_menu_state++;
      if (m_menu_state == m_number_of_options)  // wrap around check
        m_menu_state -= m_number_of_options;
      setOptionText(m_menu_state);
    }
  } else {
    m_is_slider_clicked = false;
  }
}

void ControlItemSliderbar::updateMouse(std::int32_t x, std::int32_t /*y*/) {
  if (m_is_slider_clicked) {
    // check if the mouse pointer is either left or right side from the
    // slider
    if (m_bar_x_pos < x && x < m_slider_x_pos - m_interval / 2) {
      m_slider_x_pos -= m_interval;
      m_menu_state--;
      if (m_menu_state < 0)  // wrap around check
        m_menu_state += m_number_of_options;
      setOptionText(m_menu_state);
      playSFX(SMALL_CLICK);
    } else if (m_slider_x_pos + m_interval / 2 + m_slider_width < x &&
        x < m_bar_x_pos + m_bar_width) {
      m_slider_x_pos += m_interval;
      m_menu_state++;
      if (m_menu_state == m_number_of_options)  // wrap around check
        m_menu_state -= m_number_of_options;
      setOptionText(m_menu_state);
      playSFX(SMALL_CLICK);
    }
  }
}
