#include "vulkan_earth/SubMenuLandscape.h"
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include "math.h"
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemButton.h"
#include "vulkan_earth/ControlItemCheckBox.h"
#include "vulkan_earth/ControlItemSelectionBox.h"
#include "vulkan_earth/ControlItemSliderbar.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/TerrainMaker.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_graphix/Tools.h"

#define PI 3.1415926535898

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;
extern void playSFX(std::int32_t sfx);

SubMenuLandscape::SubMenuLandscape() = default;

SubMenuLandscape::SubMenuLandscape(
    std::int32_t id,
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

  m_cam_x = -4000;
  m_cam_y = 10000;
  m_cam_z = -4000;

  m_tm = new TerrainMaker(100, 256);
  m_tm->prepareData(0, 0, 0, 0, 0);

  m_old_mouse_x = -1;
  m_old_mouse_y = -1;

  /*	BUTTON TEXT PLACEMENT	*/
  std::int32_t real_length = 0;
  for (char ch : m_caption) {
    real_length +=
        vulkan_earth::textAdvance(vulkan_earth::FontId::TimesRoman24, ch);
  }
  float label_x_pos = m_x_pos + ((m_width) / 2) - (real_length / 2);
  float label_y_pos = m_y_pos - m_height / 20;
  /*	END OF BUTTON TEXT PLACEMENT	*/

  m_label = new TextObject(
      m_caption,
      label_x_pos,
      label_y_pos,
      (m_z_pos + 1),
      vulkan_earth::FontId::TimesRoman24,
      0.0f,
      0.0f,
      0.0f);
  m_button_pressed = nullptr;

  m_sub_menu_button[0] = new ControlItemSliderbar(
      m_x_pos + (m_width / 2) - (0.48 * m_width),
      m_y_pos - (m_height * 0.7),
      m_z_pos + 1,
      0.5f,
      0.5f,
      0.5f,
      0.6f * m_width,
      0.085 * (m_height),
      "Smoothness",
      "0/1/2/3/4/5/",
      5);
  m_sub_menu_button[1] = new ControlItemSliderbar(
      m_x_pos + (m_width / 2) - (0.48 * m_width),
      m_y_pos - (m_height * 0.8),
      m_z_pos + 1,
      0.5f,
      0.5f,
      0.5f,
      0.6f * m_width,
      0.085 * (m_height),
      "Hill Height",
      "0/1/2/3/4/5/",
      5);
  m_sub_menu_button[2] = new ControlItemSliderbar(
      m_x_pos + (m_width / 2) - (0.48 * m_width),
      m_y_pos - (m_height * 0.9),
      m_z_pos + 1,
      0.5f,
      0.5f,
      0.5f,
      0.6f * m_width,
      0.085 * (m_height),
      "Terrain Selection",
      "Rock/Snow/Ice/Mars/Desert/Lava/",
      0);
  m_sub_menu_button[3] = new ControlItemButton(
      this,
      m_x_pos + (0.655 * m_width),
      m_y_pos - (m_height * 0.91),
      m_z_pos + 1,
      0.75f,
      0.0f,
      0.0f,
      0.3f * m_width,
      0.05 * (m_height),
      "Sample");
}

SubMenuLandscape::~SubMenuLandscape() {
  delete m_label;
  delete m_tm;
  for (std::int32_t i = 0; i < num_control_items_lnd; i++)
    delete m_sub_menu_button[i];
}

std::int32_t SubMenuLandscape::getUNIQUEIDENTIFIER() {
  return m_uniqueidentifier;
}
void SubMenuLandscape::setUNIQUEIDENTIFIER(std::int32_t id) {
  m_uniqueidentifier = id;
}
float SubMenuLandscape::getXPos() { return m_x_pos; }
void SubMenuLandscape::setXPos(float new_xpos) { m_x_pos = new_xpos; }
float SubMenuLandscape::getYPos() { return m_y_pos; }
void SubMenuLandscape::setYPos(float new_ypos) { m_y_pos = new_ypos; }
float SubMenuLandscape::getZPos() { return m_z_pos; }
void SubMenuLandscape::setZPos(float new_zpos) { m_z_pos = new_zpos; }
float SubMenuLandscape::getRed() { return m_color[0]; }
void SubMenuLandscape::setRed(float red) { m_color[0] = red; }
float SubMenuLandscape::getGreen() { return m_color[1]; }
void SubMenuLandscape::setGreen(float green) { m_color[1] = green; }
float SubMenuLandscape::getBlue() { return m_color[2]; }
void SubMenuLandscape::setBlue(float blue) { m_color[2] = blue; }
std::int32_t SubMenuLandscape::getWidth() { return m_width; }
void SubMenuLandscape::setWdith(std::int32_t new_width) {
  m_width = new_width;
}
std::int32_t SubMenuLandscape::getHeight() { return m_height; }
void SubMenuLandscape::setHeight(std::int32_t new_height) {
  m_height = new_height;
}
std::string SubMenuLandscape::getCaption() { return m_caption; }
void SubMenuLandscape::setCaption(const std::string& new_caption) {
  m_caption = new_caption;
}
float SubMenuLandscape::getPerecentBorder() { return m_percent_border; }
void SubMenuLandscape::setPercentBorder(float percent) {
  m_percent_border = percent;
}

void SubMenuLandscape::draw(render::RenderContext& context) {
  using Vec3 = math::Vec3<float>;
  using Vec4 = math::Vec4<float>;
  // The same raised 3-pixel bevel every button draws.
  if (m_frame_mesh.triangles().empty()) {
    vulkan_earth::appendBevel(
        m_frame_mesh,
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
  for (std::int32_t i = 0; i < num_control_items_lnd; i++) {
    if (m_sub_menu_button[i]) {
      m_sub_menu_button[i]->draw(context);
    }
  }

  // The preview's sunken border (top/left -0.2, bottom/right +0.4).
  if (m_border_mesh.triangles().empty()) {
    float border_x = m_x_pos + 0.03 * m_width;
    float border_y = m_y_pos - 0.07 * m_height;
    const float z1 = m_z_pos + 1;
    const Vec4 dark(
        m_color[0] - .2, m_color[1] - .2, m_color[2] - .2, m_color[3]);
    const Vec4 light(
        m_color[0] + .4, m_color[1] + .4, m_color[2] + .4, m_color[3]);
    // top-left
    m_border_mesh.addQuad(
        {Vec3(border_x, border_y, z1),
         Vec3(border_x - 3, border_y + 3, z1),
         Vec3(border_x + 0.936 * m_width + 3, border_y + 3, z1),
         Vec3(border_x + 0.936 * m_width, border_y, z1)},
        dark);
    m_border_mesh.addQuad(
        {Vec3(border_x - 3, border_y + 3, z1),
         Vec3(border_x - 3, border_y - 0.597 * m_height - 3, z1),
         Vec3(border_x, border_y - 0.597 * m_height, z1),
         Vec3(border_x, border_y, z1)},
        dark);
    // bottom-right
    m_border_mesh.addQuad(
        {Vec3(border_x - 3, border_y - 0.597 * m_height - 3, z1),
         Vec3(
             border_x + 0.936 * m_width + 3,
             border_y - 0.597 * m_height - 3,
             z1),
         Vec3(border_x + 0.936 * m_width, border_y - 0.597 * m_height, z1),
         Vec3(border_x, border_y - 0.597 * m_height, z1)},
        light);
    m_border_mesh.addQuad(
        {Vec3(border_x + 0.936 * m_width, border_y, z1),
         Vec3(border_x + 0.936 * m_width + 3, border_y + 3, z1),
         Vec3(
             border_x + 0.936 * m_width + 3,
             border_y - 0.597 * m_height - 3,
             z1),
         Vec3(border_x + 0.936 * m_width, border_y + -0.597 * m_height, z1)},
        light);
  }
  context.draw(m_border_mesh);

  // The live terrain preview, in its own viewport (glViewport()'s float
  // -> int truncation kept), cleared to black, then the menu's own
  // viewport and camera restored.
  const render::Rect saved_viewport = context.viewport();
  const math::Mat4<float> saved_projection = context.projection();
  const math::Mat4<float> saved_view = context.view();
  const render::Rect preview = vulkan_earth::glRect(
      static_cast<std::int32_t>(m_x_pos + 0.8 * m_width),
      static_cast<std::int32_t>(m_y_pos),
      static_cast<std::int32_t>(0.9417 * m_width),
      static_cast<std::int32_t>(0.6 * m_height));
  context.setViewport(preview);
  context.clearColorAndDepth(Vec4(0, 0, 0, 0));
  context.setCamera(
      vulkan_graphix::Tools::getPerspectiveProjectionMatrix(
          ((0.9417 * m_width) / (0.6 * m_height)), 45.0, 1, 2.0e8f),
      glm::lookAt(
          Vec3(m_cam_x, m_cam_y, m_cam_z),
          Vec3(
              (m_tm->getActualSize() / 2.0),
              0.0f,
              (m_tm->getActualSize() / 2.0)),
          Vec3(0.0f, 1.0f, 0.0f)));
  m_tm->draw(context);
  context.setViewport(saved_viewport);
  context.setCamera(saved_projection, saved_view);
}

std::string SubMenuLandscape::collectData() {
  std::string optionsarray = "/Landscape/";
  for (std::int32_t x = 0; x < num_control_items_lnd; x++) {
    if (m_sub_menu_button[x]) {
      optionsarray += m_sub_menu_button[x]->collectData();
      optionsarray += "/";
    }
  }
  return optionsarray;
}

void SubMenuLandscape::subMenuMouseTest(
    std::int32_t x, std::int32_t y, std::int32_t button_down) {
  if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
    for (std::int32_t button_i = 0; button_i < num_control_items_lnd;
         button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                        // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                        // CLICKED A ARROW BUTTON???
      if ((x >= m_sub_menu_button[button_i]->getXPos()) &&
          (x <= (m_sub_menu_button[button_i]->getXPos() +
                 m_sub_menu_button[button_i]->getWidth())) &&
          (y <= m_sub_menu_button[button_i]->getYPos()) &&
          (y >= (m_sub_menu_button[button_i]->getYPos() -
                 m_sub_menu_button[button_i]->getHeight()))) {
        m_sub_menu_button[button_i]->mouseClickEvent(
            x,
            y,
            button_down,
            true);  // YOU PRESSED OVER A ARROWBUTTON
        m_button_pressed = m_sub_menu_button[button_i];
        m_numberpressed = button_i;
      }
    }
    m_old_mouse_x = x;
    m_old_mouse_y = y;
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
        if (m_numberpressed == preview_button) {
          // RIGHT NOW THERE ARE ONLY 3 OPTIONS AND subMenuButton[3]
          // IS THE BUTTON ITSELF
          /********************************************************************************/
          /*		Smoothness		--			out1,i1
           */
          /*		Hill Height		--			out2,i2
           */
          /*		Terrain Texture	--			out3,i3
           */
          /********************************************************************************/
          stringstream ss1(m_sub_menu_button[0]->collectData());
          std::int32_t i1;
          if (!(ss1 >> i1)) i1 = 0;
          stringstream ss2(m_sub_menu_button[1]->collectData());
          std::int32_t i2;
          if (!(ss2 >> i2)) i2 = 0;
          m_tm->prepareData(
              2500,          // int steps
              i2 * i2 + 10,  // int increase
              30,            // float radius
              5,             // int randomJump % (1-100)
              i1);           // int smoothness
          m_tm->selectTexture(m_sub_menu_button[2]->collectData());
          m_button_pressed->mouseClickEvent(x, y, button_down, false);
          m_numberpressed = -1;
          m_button_pressed = nullptr;
          playSFX(SMALL_CLICK);
        } else {
          m_button_pressed->mouseClickEvent(
              x,
              y,
              button_down,
              true);  // IF YOU ARE THEN TELL THE ARROW BUTTON
                      // YOU RELEASE THE MOUSE
          m_numberpressed = -1;
          m_button_pressed = nullptr;
        }
      } else {
        m_button_pressed->mouseClickEvent(
            x,
            y,
            button_down,
            false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                     // RELEASE THE MOUSE
        m_button_pressed = nullptr;
        m_numberpressed = -1;
      }
    }
    m_old_mouse_x = -1;
    m_old_mouse_y = -1;
  }
}

void SubMenuLandscape::updateMouse(std::int32_t x, std::int32_t y) {
  if (((x >= m_x_pos + 0.8 * m_width) &&
       (x <= m_x_pos + 0.8 * m_width + (0.9417 * m_width))) &&
      ((y >= m_y_pos - 0.15 * m_height) &&
       (y <= m_y_pos - 0.15 * m_height + (0.6 * m_height)))) {
    float new_cam_x = m_cam_x, new_cam_y = m_cam_y, new_cam_z = m_cam_z;
    if (x < m_old_mouse_x) {
      new_cam_x = (m_cam_x - (m_tm->getActualSize() / 2.0)) * cos(-PI / 180) -
          (m_cam_z - (m_tm->getActualSize() / 2.0)) * sin(-PI / 180) +
          (m_tm->getActualSize() / 2.0);
      new_cam_z = (m_cam_x - (m_tm->getActualSize() / 2.0)) * sin(-PI / 180) +
          (m_cam_z - (m_tm->getActualSize() / 2.0)) * cos(-PI / 180) +
          (m_tm->getActualSize() / 2.0);
    }
    if (x > m_old_mouse_x) {
      new_cam_x = (m_cam_x - (m_tm->getActualSize() / 2.0)) * cos(PI / 180) -
          (m_cam_z - (m_tm->getActualSize() / 2.0)) * sin(PI / 180) +
          (m_tm->getActualSize() / 2.0);
      new_cam_z = (m_cam_x - (m_tm->getActualSize() / 2.0)) * sin(PI / 180) +
          (m_cam_z - (m_tm->getActualSize() / 2.0)) * cos(PI / 180) +
          (m_tm->getActualSize() / 2.0);
    }
    if (y < m_old_mouse_y) {
    }
    if (y > m_old_mouse_y) {
    }
    m_cam_x = new_cam_x;
    m_cam_y = new_cam_y;
    m_cam_z = new_cam_z;
    m_old_mouse_x = x;
    m_old_mouse_y = y;
  }
  std::int32_t win_width = vulkan_earth::windowWidth();
  std::int32_t win_height = vulkan_earth::windowHeight();
  m_sub_menu_button[0]->updateMouse(x - (win_width / 2), (win_height / 2) - y);
  m_sub_menu_button[1]->updateMouse(x - (win_width / 2), (win_height / 2) - y);
  m_sub_menu_button[2]->updateMouse(x - (win_width / 2), (win_height / 2) - y);
}
