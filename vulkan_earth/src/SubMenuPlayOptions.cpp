#include "vulkan_earth/SubMenuPlayOptions.h"
#include <cstdint>
#include <string>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemCheckBox.h"
#include "vulkan_earth/ControlItemSelectionBox.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/TextObject.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

SubMenuPlayOptions::SubMenuPlayOptions() = default;

SubMenuPlayOptions::SubMenuPlayOptions(std::int32_t id,
    float new_x_pos,
    float new_y_pos,
    float new_z_pos,
    float red,
    float green,
    float blue,
    std::int32_t new_width,
    std::int32_t new_height,
    const std::string& new_caption,
    float new_percent_border) {
  m_uniqueidentifier = id;
  m_x_pos = new_x_pos;
  m_y_pos = new_y_pos;
  m_z_pos = new_z_pos;
  m_percent_border = new_percent_border;
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
  float label_y_pos = m_y_pos - m_height / 20;
  /*	END OF BUTTON TEXT PLACEMENT	*/

  m_label = new TextObject(m_caption,
      label_x_pos,
      label_y_pos,
      (m_z_pos + 1),
      vulkan_earth::FontId::TimesRoman24,
      0.0f,
      0.0f,
      0.0f);
  m_button_pressed = nullptr;
  m_sub_menu_button[0] =
      new ControlItemSelectionBox(m_x_pos + (m_width / 2) - (0.3 * m_width),
          m_y_pos - (m_height * 0.2),
          m_z_pos + 1,
          0.5f,
          0.5f,
          0.5f,
          0.6f * m_width,
          0.06 * (m_height),
          "Teams",
          "Allowed/Random/Not Allowed/");
  m_sub_menu_button[1] =
      new ControlItemCheckBox(m_x_pos + (m_width / 2) - (0.3 * m_width),
          m_y_pos - (m_height * 0.27),
          m_z_pos + 1,
          0.5f,
          0.5f,
          0.5f,
          0.6f * m_width,
          0.06 * (m_height),
          "Status Bar");
  m_sub_menu_button[2] =
      new ControlItemSelectionBox(m_x_pos + (m_width / 2) - (0.3 * m_width),
          m_y_pos - (m_height * 0.34),
          m_z_pos + 1,
          0.5f,
          0.5f,
          0.5f,
          0.6f * m_width,
          0.06 * (m_height),
          "Play Order",
          "Sequential/Random/");
  m_sub_menu_button[3] =
      new ControlItemCheckBox(m_x_pos + (m_width / 2) - (0.3 * m_width),
          m_y_pos - (m_height * 0.41),
          m_z_pos + 1,
          0.5f,
          0.5f,
          0.5f,
          0.6f * m_width,
          0.06 * (m_height),
          "Fast Computers");
  m_sub_menu_button[4] =
      new ControlItemSelectionBox(m_x_pos + (m_width / 2) - (0.3 * m_width),
          m_y_pos - (m_height * 0.48),
          m_z_pos + 1,
          0.5f,
          0.5f,
          0.5f,
          0.6f * m_width,
          0.06 * (m_height),
          "Talking Tanks",
          "Yes/No/");
  m_sub_menu_button[5] =
      new ControlItemSelectionBox(m_x_pos + (m_width / 2) - (0.3 * m_width),
          m_y_pos - (m_height * 0.55),
          m_z_pos + 1,
          0.5f,
          0.5f,
          0.5f,
          0.6f * m_width,
          0.06 * (m_height),
          "Talk Probability",
          "0.1/0.2/0.3/0.4/0.5/0.6/0.7/0.8/0.9/1.0/");
}

SubMenuPlayOptions::~SubMenuPlayOptions() {
  for (std::int32_t i = 0; i < num_control_items_po; i++)
    delete m_sub_menu_button[i];
  delete m_label;
}

std::int32_t SubMenuPlayOptions::getUNIQUEIDENTIFIER() {
  return m_uniqueidentifier;
}
void SubMenuPlayOptions::setUNIQUEIDENTIFIER(std::int32_t id) {
  m_uniqueidentifier = id;
}
float SubMenuPlayOptions::getXPos() { return m_x_pos; }
void SubMenuPlayOptions::setXPos(float new_xpos) { m_x_pos = new_xpos; }
float SubMenuPlayOptions::getYPos() { return m_y_pos; }
void SubMenuPlayOptions::setYPos(float new_ypos) { m_y_pos = new_ypos; }
float SubMenuPlayOptions::getZPos() { return m_z_pos; }
void SubMenuPlayOptions::setZPos(float new_zpos) { m_z_pos = new_zpos; }
float SubMenuPlayOptions::getRed() { return m_color[0]; }
void SubMenuPlayOptions::setRed(float red) { m_color[0] = red; }
float SubMenuPlayOptions::getGreen() { return m_color[1]; }
void SubMenuPlayOptions::setGreen(float green) { m_color[1] = green; }
float SubMenuPlayOptions::getBlue() { return m_color[2]; }
void SubMenuPlayOptions::setBlue(float blue) { m_color[2] = blue; }
std::int32_t SubMenuPlayOptions::getWidth() { return m_width; }
void SubMenuPlayOptions::setWdith(std::int32_t new_width) {
  m_width = new_width;
}
std::int32_t SubMenuPlayOptions::getHeight() { return m_height; }
void SubMenuPlayOptions::setHeight(std::int32_t new_height) {
  m_height = new_height;
}
std::string SubMenuPlayOptions::getCaption() { return m_caption; }
void SubMenuPlayOptions::setCaption(const std::string& new_caption) {
  m_caption = new_caption;
}
float SubMenuPlayOptions::getPerecentBorder() { return m_percent_border; }
void SubMenuPlayOptions::setPercentBorder(float percent) {
  m_percent_border = percent;
}

void SubMenuPlayOptions::draw(render::RenderContext& context) {
  // The same raised 3-pixel bevel every button draws.
  if (m_frame_mesh.triangles().empty()) {
    vulkan_earth::appendBevel(m_frame_mesh,
        m_x_pos,
        m_y_pos,
        m_z_pos,
        m_width,
        m_height,
        math::Vec4<float>(m_color[0], m_color[1], m_color[2], m_color[3]),
        false);
  }
  context.draw(m_frame_mesh);
  m_label->draw(context);
  for (std::int32_t i = 0; i < num_control_items_po; i++) {
    if (m_sub_menu_button[i]) {
      m_sub_menu_button[i]->draw(context);
    }
  }
}

std::string SubMenuPlayOptions::collectData() {
  std::string optionsarray = "/Game Options/";
  for (std::int32_t x = 0; x < num_control_items_po; x++) {
    if (m_sub_menu_button[x]) {
      optionsarray += m_sub_menu_button[x]->collectData();
      optionsarray += "/";
    }
  }
  return optionsarray;
}

void SubMenuPlayOptions::subMenuMouseTest(
    std::int32_t x, std::int32_t y, std::int32_t button_down) {
  if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
    for (std::int32_t button_i = 0; button_i < num_control_items_po;
        button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                       // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                       // CLICKED A ARROW BUTTON???
      if ((x >= m_sub_menu_button[button_i]->getXPos()) &&
          (x <= (m_sub_menu_button[button_i]->getXPos() +
                    m_sub_menu_button[button_i]->getWidth())) &&
          (y <= m_sub_menu_button[button_i]->getYPos()) &&
          (y >= (m_sub_menu_button[button_i]->getYPos() -
                    m_sub_menu_button[button_i]->getHeight()))) {
        m_sub_menu_button[button_i]->mouseClickEvent(x,
            y,
            button_down,
            true);  // YOU PRESSED OVER A ARROWBUTTON
        m_button_pressed = m_sub_menu_button[button_i];
      }
    }
  } else if (!button_down) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                              // GOES UP
    if (m_button_pressed !=
        nullptr) {  // IF YOU MANAGED TO CLICK INSIDE AN ARROW BUTTON
                    // CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
      if ((x >= m_button_pressed->getXPos()) &&
          (x <=
              (m_button_pressed->getXPos() + m_button_pressed->getWidth())) &&
          (y <= m_button_pressed->getYPos()) &&
          (y >=
              (m_button_pressed->getYPos() - m_button_pressed->getHeight()))) {
        m_button_pressed->mouseClickEvent(x,
            y,
            button_down,
            true);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                    // RELEASE THE MOUSE
      } else {
        m_button_pressed->mouseClickEvent(x,
            y,
            button_down,
            false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                     // RELEASE THE MOUSE
        m_button_pressed = nullptr;
      }
    }
  }
}

void SubMenuPlayOptions::updateMouse(std::int32_t x, std::int32_t y) {}