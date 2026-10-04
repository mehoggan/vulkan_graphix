#include "vulkan_earth/ControlItemSelectionBox.h"
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

ControlItemSelectionBox::ControlItemSelectionBox() = default;

ControlItemSelectionBox::ControlItemSelectionBox(
        float new_x_pos,
        float new_y_pos,
        float new_z_pos,
        float red,
        float green,
        float blue,
        std::int32_t new_width,
        std::int32_t new_height,
        const std::string& new_caption,
        const std::string& menu_string) {
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
    m_menu_info = menu_string;

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

    m_menu_state = 0;
    m_button_state = 0;
    m_option_text = nullptr;
    setOptionText(m_menu_state);  // set option to first option
    /*	BUTTON TEXT PLACEMENT	*/
    std::int32_t real_length = 0;
    for (char ch : m_caption) {
        real_length += vulkan_earth::textAdvance(
                vulkan_earth::FontId::TimesRoman24, ch);
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

ControlItemSelectionBox::~ControlItemSelectionBox() {
    delete m_option_text;
    delete m_label;
}

void ControlItemSelectionBox::draw(render::RenderContext& context) {
    using Vec3 = math::Vec3<float>;
    using Vec4 = math::Vec4<float>;
    // draw main button box (a sunken bevel: -0.2 top/left, +0.4
    // bottom/right)
    if (m_frame_mesh.triangles().empty()) {
        const Vec4 dark(m_color[0] - 0.2f,
                        m_color[1] - 0.2f,
                        m_color[2] - 0.2f,
                        m_color[3]);
        const Vec4 face(m_color[0], m_color[1], m_color[2], m_color[3]);
        const Vec4 light(m_color[0] + 0.4f,
                         m_color[1] + 0.4f,
                         m_color[2] + 0.4f,
                         m_color[3]);
        m_frame_mesh.addQuad(
                {Vec3(m_x_pos, m_y_pos, m_z_pos),
                 Vec3(m_x_pos - 3, m_y_pos + 3, m_z_pos),
                 Vec3(m_x_pos + m_width + 3, m_y_pos + 3, m_z_pos),
                 Vec3(m_x_pos + m_width, m_y_pos, m_z_pos)},
                dark);
        m_frame_mesh.addQuad(
                {Vec3(m_x_pos - 3, m_y_pos + 3, m_z_pos),
                 Vec3(m_x_pos - 3, m_y_pos - m_height - 3, m_z_pos),
                 Vec3(m_x_pos, m_y_pos - m_height, m_z_pos),
                 Vec3(m_x_pos, m_y_pos, m_z_pos)},
                dark);
        m_frame_mesh.addQuad(
                {Vec3(m_x_pos, m_y_pos, m_z_pos),
                 Vec3(m_x_pos, m_y_pos - m_height, m_z_pos),
                 Vec3(m_x_pos + m_width, m_y_pos - m_height, m_z_pos),
                 Vec3(m_x_pos + m_width, m_y_pos, m_z_pos)},
                face);
        m_frame_mesh.addQuad(
                {Vec3(m_x_pos - 3, m_y_pos - m_height - 3, m_z_pos),
                 Vec3(m_x_pos + m_width + 3, m_y_pos - m_height - 3, m_z_pos),
                 Vec3(m_x_pos + m_width, m_y_pos - m_height, m_z_pos),
                 Vec3(m_x_pos, m_y_pos - m_height, m_z_pos)},
                light);
        m_frame_mesh.addQuad(
                {Vec3(m_x_pos + m_width, m_y_pos, m_z_pos),
                 Vec3(m_x_pos + m_width + 3, m_y_pos + 3, m_z_pos),
                 Vec3(m_x_pos + m_width + 3, m_y_pos - m_height - 3, m_z_pos),
                 Vec3(m_x_pos + m_width, m_y_pos + -m_height, m_z_pos)},
                light);
    }
    context.draw(m_frame_mesh);

    m_label->draw(context);
    m_option_text->draw(context);

    if (m_arrows_built_for != m_button_state) {
        m_arrow_mesh.clear();
        const Vec4 raised(
                m_color[0] + .2, m_color[1] + .2, m_color[2] + .2, 1.0f);
        const Vec4 pressed(
                m_color[0] - .2, m_color[1] - .2, m_color[2] - .2, 1.0f);
        // draw up arrow
        m_arrow_mesh.addTriangle(
                {Vec3(m_x_pos + 0.02 * m_width,
                      (m_y_pos - m_height / 2) + 0.05 * m_height,
                      m_z_pos + 1),
                 Vec3(m_x_pos + 0.02 * m_width + m_height * 0.7,
                      (m_y_pos - m_height / 2) + 0.05 * m_height,
                      m_z_pos + 1),
                 Vec3(m_x_pos + 0.02 * (m_width) + m_height * 0.35,
                      m_y_pos - 0.05 * m_height,
                      m_z_pos + 1)},
                {raised, raised, m_button_state == 1 ? pressed : raised});
        // draw down arrow
        m_arrow_mesh.addTriangle(
                {Vec3(m_x_pos + 0.02 * m_width,
                      (m_y_pos - m_height / 2) - 0.05 * m_height,
                      m_z_pos + 1),
                 Vec3(m_x_pos + 0.02 * m_width + m_height * 0.7,
                      (m_y_pos - m_height / 2) - 0.05 * m_height,
                      m_z_pos + 1),
                 Vec3(m_x_pos + 0.02 * m_width + m_height * 0.35,
                      m_y_pos - m_height + 0.05 * m_height,
                      m_z_pos + 1)},
                {raised, raised, m_button_state == 2 ? pressed : raised});
        m_arrows_built_for = m_button_state;
    }
    context.draw(m_arrow_mesh);
}

float ControlItemSelectionBox::getXPos() { return m_x_pos; }
float ControlItemSelectionBox::getYPos() { return m_y_pos; }
float ControlItemSelectionBox::getHeight() { return m_height; }
float ControlItemSelectionBox::getWidth() { return m_width; }
std::string ControlItemSelectionBox::collectData() { return m_current_option; }

void ControlItemSelectionBox::setOptionText(std::int32_t index) {
    m_current_option = m_all_options[index];
    std::int32_t real_length = 0;
    for (char ch : m_current_option) {
        real_length += vulkan_earth::textAdvance(
                vulkan_earth::FontId::TimesRoman24, ch);
    }
    float label_x_pos = m_x_pos + m_width - real_length - m_width / 50;
    float label_y_pos =
            m_y_pos + ((m_y_pos - (m_y_pos + m_height)) / 2) - m_height / 4;
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
}
void ControlItemSelectionBox::setOptionText(const std::string& new_text) {}

void ControlItemSelectionBox::mouseClickEvent(
        std::int32_t x,
        std::int32_t y,
        std::int32_t state,
        bool still_over_control_item_selection_box) {
    // up arrow test
    if ((x >= (m_x_pos + 0.02 * (m_width)) &&
         (x <= m_x_pos + 0.02 * (m_width) + 0.1 * (m_width))) &&
        ((y <= m_y_pos - 3) &&
         (y >= (m_y_pos - m_height / 2) +
                       3))) {  // YOU HAVE CLICKED INSIDE THE UP ARROW
        if (state == 1) {  // IF MOUSE BUTTON DOWN (YOU ARE INSIDE UP ARROW)
            m_button_state = 1;  // THEN UP ARROW HAS BEEN PRESSED
        } else if (state == 0) {
            if (still_over_control_item_selection_box) {  // ONCE YOU RELEASE
                                                          // MOUSE
                                                          // BUTTON
                playSFX(SMALL_CLICK);
                m_menu_state++;
                if (m_menu_state == m_number_of_options)  // wrap around check
                    m_menu_state -= m_number_of_options;
                setOptionText(m_menu_state);
            }
            m_button_state = 0;
        }
    }
    if (((x >= m_x_pos + 0.02 * (m_width)) &&
         (x <= m_x_pos + 0.02 * (m_width) + 0.1 * (m_width))) &&
        ((y <= (m_y_pos - m_height / 2) - 3) &&
         (y >= (m_y_pos - m_height) +
                       3))) {  // YOU HAVE CLICKED INSIDE THE UP ARROW
        if (state == 1) {  // IF MOUSE BUTTON DOWN (YOU ARE INSIDE UP ARROW)
            m_button_state = 2;  // THEN UP ARROW HAS BEEN PRESSED
        } else if (state == 0) {
            if (still_over_control_item_selection_box) {  // ONCE YOU RELEASE
                                                          // MOUSE
                                                          // BUTTON
                playSFX(SMALL_CLICK);
                m_menu_state--;
                if (m_menu_state < 0)  // wrap around check
                    m_menu_state += m_number_of_options;
                setOptionText(m_menu_state);
            }
            m_button_state = 0;
        }
    }
    if (state == 0) {
        m_button_state = 0;
    }
}

void ControlItemSelectionBox::updateMouse(std::int32_t x, std::int32_t y) {}