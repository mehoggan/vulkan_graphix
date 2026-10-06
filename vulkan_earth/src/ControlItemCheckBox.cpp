#include "vulkan_earth/ControlItemCheckBox.h"
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

extern void playSFX(std::int32_t sfx);

ControlItemCheckBox::ControlItemCheckBox() = default;

ControlItemCheckBox::ControlItemCheckBox(float new_x_pos,
  float new_y_pos,
  float new_z_pos,
  float red,
  float green,
  float blue,
  std::int32_t new_width,
  std::int32_t new_height,
  const std::string& new_caption) {
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

    m_button_state = 0;
    m_menu_state = 0;

    /*	BUTTON TEXT PLACEMENT	*/
    std::int32_t real_length = 0;
    for (char ch : m_caption) {
        real_length +=
          vulkan_earth::textAdvance(vulkan_earth::FontId::TimesRoman24, ch);
    }

    float label_x_pos = m_x_pos + (m_width / 2) - (real_length / 2);
    float label_y_pos =
      m_y_pos + ((m_y_pos - (m_y_pos + m_height)) / 2) - m_height / 4;
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

ControlItemCheckBox::~ControlItemCheckBox() { delete m_label; }

void ControlItemCheckBox::draw(render::RenderContext& context) {
    using Vec3 = math::Vec3<float>;
    using Vec4 = math::Vec4<float>;
    if (m_built_button_state != m_button_state ||
      m_built_menu_state != m_menu_state) {
        m_mesh.clear();
        // draw main button box (sunken bevel: -0.2 top/left, +0.4
        // bottom/right)
        const Vec4 dark(
          m_color[0] - 0.2f, m_color[1] - 0.2f, m_color[2] - 0.2f, m_color[3]);
        const Vec4 face(m_color[0], m_color[1], m_color[2], m_color[3]);
        const Vec4 light(
          m_color[0] + 0.4f, m_color[1] + 0.4f, m_color[2] + 0.4f, m_color[3]);
        m_mesh.addQuad({Vec3(m_x_pos, m_y_pos, m_z_pos),
                         Vec3(m_x_pos - 3, m_y_pos + 3, m_z_pos),
                         Vec3(m_x_pos + m_width + 3, m_y_pos + 3, m_z_pos),
                         Vec3(m_x_pos + m_width, m_y_pos, m_z_pos)},
          dark);
        m_mesh.addQuad({Vec3(m_x_pos - 3, m_y_pos + 3, m_z_pos),
                         Vec3(m_x_pos - 3, m_y_pos - m_height - 3, m_z_pos),
                         Vec3(m_x_pos, m_y_pos - m_height, m_z_pos),
                         Vec3(m_x_pos, m_y_pos, m_z_pos)},
          dark);
        m_mesh.addQuad({Vec3(m_x_pos, m_y_pos, m_z_pos),
                         Vec3(m_x_pos, m_y_pos - m_height, m_z_pos),
                         Vec3(m_x_pos + m_width, m_y_pos - m_height, m_z_pos),
                         Vec3(m_x_pos + m_width, m_y_pos, m_z_pos)},
          face);
        m_mesh.addQuad(
          {Vec3(m_x_pos - 3, m_y_pos - m_height - 3, m_z_pos),
            Vec3(m_x_pos + m_width + 3, m_y_pos - m_height - 3, m_z_pos),
            Vec3(m_x_pos + m_width, m_y_pos - m_height, m_z_pos),
            Vec3(m_x_pos, m_y_pos - m_height, m_z_pos)},
          light);
        m_mesh.addQuad(
          {Vec3(m_x_pos + m_width, m_y_pos, m_z_pos),
            Vec3(m_x_pos + m_width + 3, m_y_pos + 3, m_z_pos),
            Vec3(m_x_pos + m_width + 3, m_y_pos - m_height - 3, m_z_pos),
            Vec3(m_x_pos + m_width, m_y_pos + -m_height, m_z_pos)},
          light);

        // draw the actual check box itself: 4 smaller squares (2 triangles
        // each) whose innermost vertex is colored darker when pressed.
        const Vec4 raised(
          m_color[0] + .2, m_color[1] + .2, m_color[2] + .2, 1.0f);
        const Vec4 center_color = m_button_state == 1
          ? Vec4(m_color[0] - .2, m_color[1] - .2, m_color[2] - .2, 1.0f)
          : raised;
        const float z1 = m_z_pos + 1;
        const Vec3 center(
          m_x_pos + (m_width - m_height / 2), (m_y_pos - m_height / 2), z1);
        const Vec3 center4(m_x_pos + (m_width - m_height) + (m_height / 2),
          (m_y_pos - m_height / 2),
          z1);
        // square 1
        m_mesh.addTriangle({Vec3(m_x_pos + (m_width - m_height * 0.9),
                              (m_y_pos - m_height * 0.1),
                              z1),
                             Vec3(m_x_pos + (m_width - m_height * 0.9),
                               (m_y_pos - m_height / 2),
                               z1),
                             center},
          {raised, raised, center_color});
        m_mesh.addTriangle({Vec3(m_x_pos + (m_width - m_height * 0.9),
                              (m_y_pos - m_height * 0.1),
                              z1),
                             center,
                             Vec3(m_x_pos + (m_width - m_height / 2),
                               (m_y_pos - m_height * 0.1),
                               z1)},
          {raised, center_color, raised});
        // square 2
        m_mesh.addTriangle({Vec3(m_x_pos + (m_width - m_height * 0.9),
                              (m_y_pos - m_height / 2),
                              z1),
                             Vec3(m_x_pos + (m_width - m_height * 0.9),
                               (m_y_pos - m_height * 0.9),
                               z1),
                             center},
          {raised, raised, center_color});
        m_mesh.addTriangle({Vec3(m_x_pos + (m_width - m_height * 0.9),
                              (m_y_pos - m_height * 0.9),
                              z1),
                             Vec3(m_x_pos + (m_width - m_height / 2),
                               (m_y_pos - m_height * 0.9),
                               z1),
                             center},
          {raised, raised, center_color});
        // square 3
        m_mesh.addTriangle(
          {center,
            Vec3(m_x_pos + (m_width - m_height / 2),
              (m_y_pos - m_height * 0.9),
              z1),
            Vec3(m_x_pos + (m_width - m_height) + m_height * 0.9,
              (m_y_pos - m_height * 0.9),
              z1)},
          {center_color, raised, raised});
        m_mesh.addTriangle({center,
                             Vec3(m_x_pos + (m_width - m_height * 0.1),
                               (m_y_pos - m_height * 0.9),
                               z1),
                             Vec3(m_x_pos + (m_width - m_height * 0.1),
                               (m_y_pos - m_height / 2),
                               z1)},
          {center_color, raised, raised});
        // square 4
        m_mesh.addTriangle({Vec3(m_x_pos + (m_width - m_height / 2),
                              (m_y_pos - m_height * 0.1),
                              z1),
                             center4,
                             Vec3(m_x_pos + (m_width - m_height * 0.1),
                               (m_y_pos - m_height * 0.1),
                               z1)},
          {raised, center_color, raised});
        m_mesh.addTriangle({center4,
                             Vec3(m_x_pos + (m_width - m_height * 0.1),
                               (m_y_pos - m_height / 2),
                               z1),
                             Vec3(m_x_pos + (m_width - m_height * 0.1),
                               (m_y_pos - m_height * 0.1),
                               z1)},
          {center_color, raised, raised});

        // draw check mark if it was toggled on, otherwise dont
        if (m_menu_state == 1) {
            const Vec4 green(0.0f, 1.0f, 0.0f, 1.0f);
            const float z2 = m_z_pos + 2;
            m_mesh.addQuad(
              {Vec3(m_x_pos + (m_width - m_height + (m_height * 0.2)),
                 (m_y_pos - m_height / 2) + (0.015 * m_width),
                 z2),
                Vec3(m_x_pos + (m_width - m_height + (m_height * 0.2)),
                  (m_y_pos - m_height / 2) + (0.005 * m_width),
                  z2),
                Vec3(m_x_pos + (m_width - m_height / 2),
                  (m_y_pos - m_height / 2) - (0.015 * m_width),
                  z2),
                Vec3(m_x_pos + (m_width - m_height / 2),
                  (m_y_pos - m_height / 2),
                  z2)},
              green);
            m_mesh.addQuad(
              {Vec3(m_x_pos + (m_width - m_height / 2),
                 (m_y_pos - m_height / 2) - (0.015 * m_width),
                 z2),
                Vec3(m_x_pos + (m_width - m_height / 2),
                  (m_y_pos - m_height / 2),
                  z2),
                Vec3(m_x_pos + (m_width - m_height) + (0.95 * m_height),
                  (m_y_pos - m_height / 2) + (0.025 * m_width),
                  z2),
                Vec3(m_x_pos + (m_width - m_height) + (0.95 * m_height),
                  (m_y_pos - m_height / 2) + (0.015 * m_width),
                  z2)},
              green);
        }
        m_built_button_state = m_button_state;
        m_built_menu_state = m_menu_state;
    }
    context.draw(m_mesh);

    m_label->draw(context);
}

float ControlItemCheckBox::getXPos() { return m_x_pos; }
float ControlItemCheckBox::getYPos() { return m_y_pos; }
float ControlItemCheckBox::getHeight() { return m_height; }
float ControlItemCheckBox::getWidth() { return m_width; }
std::string ControlItemCheckBox::collectData() {
    if (m_menu_state == 0)
        return "false";
    else
        return "true";
}

void ControlItemCheckBox::setOptionText(std::int32_t index) {}
void ControlItemCheckBox::setOptionText(const std::string& new_text) {}

// NOTE: I use height for the x value check, this is intentional to
// maintain a square
void ControlItemCheckBox::mouseClickEvent(std::int32_t x,
  std::int32_t y,
  std::int32_t state,
  bool still_over_control_item_check_box) {
    if ((x >= (m_x_pos + m_width - (m_height * 0.9)) &&
          (x <= m_x_pos + m_width - (m_height * 0.1))) &&
      ((y <= (m_y_pos - (m_height * 0.1))) &&
        (y >= (m_y_pos - m_height * 0.9)))) {
        if (state == 1) {
            m_button_state = 1;
        } else if (state == 0) {
            if (still_over_control_item_check_box) {
                if (m_menu_state == 1)
                    m_menu_state = 0;
                else
                    m_menu_state = 1;
                playSFX(SMALL_CLICK);
            }

            m_button_state = 0;
        }
    }

    if (state == 0) {
        m_button_state = 0;
    }
}

void ControlItemCheckBox::updateMouse(std::int32_t x, std::int32_t y) {}