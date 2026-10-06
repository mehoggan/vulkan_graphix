#include "vulkan_earth/MainMenuButton.h"
#include <cstdint>
#include <iostream>
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

extern void playSFX(std::int32_t sfx);

MainMenuButton::MainMenuButton() = default;

MainMenuButton::MainMenuButton(std::int32_t id,
  float new_x_pos,
  float new_y_pos,
  float new_z_pos,
  float red,
  float green,
  float blue,
  std::int32_t new_width,
  std::int32_t new_height,
  const std::string& new_caption,
  SubMenu* new_submenu) {
    m_uniqueidentifier = id;
    m_pressed = false;
    m_active = false;
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
    m_label = new TextObject(m_caption,
      label_x_pos,
      label_y_pos,
      m_z_pos,
      vulkan_earth::FontId::TimesRoman24,
      0.0f,
      0.0f,
      0.0f);
    m_submenu = new_submenu;
}

MainMenuButton::~MainMenuButton() { delete m_label; }

void MainMenuButton::draw(render::RenderContext& context) {
    const math::Vec4<float> current_color(
      m_color[0], m_color[1], m_color[2], m_color[3]);
    if (m_mesh.triangles().empty() || m_built_pressed != m_pressed ||
      m_built_color != current_color) {
        m_mesh.clear();
        vulkan_earth::appendBevel(m_mesh,
          m_x_pos,
          m_y_pos,
          m_z_pos,
          m_width,
          m_height,
          current_color,
          m_pressed);
        m_built_pressed = m_pressed;
        m_built_color = current_color;
    }
    context.draw(m_mesh);
    m_label->draw(context);

    if (m_active) {
        if (m_submenu != nullptr) {
            m_submenu->draw(context);
        }
    }
}

std::int32_t MainMenuButton::getUNIQUEIDENTIFIER() {
    return m_uniqueidentifier;
}
float MainMenuButton::getXPos() { return m_x_pos; }
float MainMenuButton::getYPos() { return m_y_pos; }
float MainMenuButton::getHeight() { return m_height; }
float MainMenuButton::getWidth() { return m_width; }
SubMenu* MainMenuButton::getSubMenu() { return m_submenu; }
float* MainMenuButton::getColor() { return &m_color[0]; }
void MainMenuButton::setColor(float r, float g, float b) {
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;
}

void MainMenuButton::setLabel(const std::string& c) {
    delete m_label;
    m_caption = c;

    std::int32_t real_length = 0;
    for (char ch : m_caption) {
        real_length +=
          vulkan_earth::textAdvance(vulkan_earth::FontId::TimesRoman24, ch);
    }
    float label_x_pos = m_x_pos + ((m_width) / 2) - (real_length / 2);
    float label_y_pos =
      m_y_pos + ((m_y_pos - (m_y_pos + m_height)) / 2) - m_height / 4;

    m_label = new TextObject(m_caption,
      label_x_pos,
      label_y_pos,
      m_z_pos,
      vulkan_earth::FontId::TimesRoman24,
      0.0f,
      0.0f,
      0.0f);
}

bool MainMenuButton::isPressed() { return m_pressed; }
bool MainMenuButton::isActive() { return m_active; }

void MainMenuButton::pressButton() {
    if (Mix_Playing(0) == 0) playSFX(BIG_CLICK);
    m_pressed = true;
}

void MainMenuButton::depressButton() { m_pressed = false; }

void MainMenuButton::activateSubMenu() { m_active = true; }

void MainMenuButton::deactivateSubMenu() { m_active = false; }

void MainMenuButton::printSelf(std::int32_t i) {
    cout << " Button[" << i << "].x=" << (getXPos()) << " Button[" << i
         << "].y=" << (getYPos()) << " Button[" << i
         << "].width=" << (getWidth()) << " Button[" << i
         << "].height=" << (getHeight()) << endl;
}
