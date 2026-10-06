#include "vulkan_earth/SubMenuPhysics.h"
#include <cstdint>
#include <string>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemCheckBox.h"
#include "vulkan_earth/ControlItemSelectionBox.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

SubMenuPhysics::SubMenuPhysics() = default;

SubMenuPhysics::SubMenuPhysics(std::int32_t id,
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
          "Air Viscosity",
          "Low/Medium/High/");
  m_sub_menu_button[1] =
      new ControlItemSelectionBox(m_x_pos + (m_width / 2) - (0.3 * m_width),
          m_y_pos - (m_height * 0.27),
          m_z_pos + 1,
          0.5f,
          0.5f,
          0.5f,
          0.6f * m_width,
          0.06 * (m_height),
          "Gravity",
          "0.1/0.2/0.5/1.0/1.5/2.0/");
  m_sub_menu_button[2] =
      new ControlItemCheckBox(m_x_pos + (m_width / 2) - (0.3 * m_width),
          m_y_pos - (m_height * 0.34),
          m_z_pos + 1,
          0.5f,
          0.5f,
          0.5f,
          0.6f * m_width,
          0.06 * (m_height),
          "Tanks Fall");
}

SubMenuPhysics::~SubMenuPhysics() {
  for (std::int32_t i = 0; i < num_control_items_phy; i++)
    delete m_sub_menu_button[i];
  delete m_label;
}

std::int32_t SubMenuPhysics::getUNIQUEIDENTIFIER() {
  return m_uniqueidentifier;
}
void SubMenuPhysics::setUNIQUEIDENTIFIER(std::int32_t id) {
  m_uniqueidentifier = id;
}
float SubMenuPhysics::getXPos() { return m_x_pos; }
void SubMenuPhysics::setXPos(float new_xpos) { m_x_pos = new_xpos; }
float SubMenuPhysics::getYPos() { return m_y_pos; }
void SubMenuPhysics::setYPos(float new_ypos) { m_y_pos = new_ypos; }
float SubMenuPhysics::getZPos() { return m_z_pos; }
void SubMenuPhysics::setZPos(float new_zpos) { m_z_pos = new_zpos; }
float SubMenuPhysics::getRed() { return m_color[0]; }
void SubMenuPhysics::setRed(float red) { m_color[0] = red; }
float SubMenuPhysics::getGreen() { return m_color[1]; }
void SubMenuPhysics::setGreen(float green) { m_color[1] = green; }
float SubMenuPhysics::getBlue() { return m_color[2]; }
void SubMenuPhysics::setBlue(float blue) { m_color[2] = blue; }
std::int32_t SubMenuPhysics::getWidth() { return m_width; }
void SubMenuPhysics::setWdith(std::int32_t new_width) { m_width = new_width; }
std::int32_t SubMenuPhysics::getHeight() { return m_height; }
void SubMenuPhysics::setHeight(std::int32_t new_height) {
  m_height = new_height;
}
std::string SubMenuPhysics::getCaption() { return m_caption; }
void SubMenuPhysics::setCaption(const std::string& new_caption) {
  m_caption = new_caption;
}
float SubMenuPhysics::getPerecentBorder() { return m_percent_border; }
void SubMenuPhysics::setPercentBorder(float percent) {
  m_percent_border = percent;
}

void SubMenuPhysics::draw(render::RenderContext& context) {
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
  for (std::int32_t i = 0; i < num_control_items_phy; i++) {
    if (m_sub_menu_button[i]) {
      m_sub_menu_button[i]->draw(context);
    }
  }
}

std::string SubMenuPhysics::collectData() {
  std::string optionsarray = "/Physics/";
  for (std::int32_t x = 0; x < num_control_items_phy; x++) {
    if (m_sub_menu_button[x]) {
      optionsarray += m_sub_menu_button[x]->collectData();
      optionsarray += "/";
    }
  }
  return optionsarray;
}

void SubMenuPhysics::subMenuMouseTest(
    std::int32_t x, std::int32_t y, std::int32_t button_down) {
  if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
    for (std::int32_t button_i = 0; button_i < num_control_items_phy;
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

void SubMenuPhysics::updateMouse(std::int32_t x, std::int32_t y) {}