#include "vulkan_earth/SubMenuTest.h"
#include <cstdint>
#include <string>
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/TextObject.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

SubMenuTest::SubMenuTest() = default;

SubMenuTest::SubMenuTest(std::int32_t id,
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
}

SubMenuTest::~SubMenuTest() = default;

std::int32_t SubMenuTest::getUNIQUEIDENTIFIER() { return m_uniqueidentifier; }
void SubMenuTest::setUNIQUEIDENTIFIER(std::int32_t id) {
  m_uniqueidentifier = id;
}
float SubMenuTest::getXPos() { return m_x_pos; }
void SubMenuTest::setXPos(float new_xpos) { m_x_pos = new_xpos; }
float SubMenuTest::getYPos() { return m_y_pos; }
void SubMenuTest::setYPos(float new_ypos) { m_y_pos = new_ypos; }
float SubMenuTest::getZPos() { return m_z_pos; }
void SubMenuTest::setZPos(float new_zpos) { m_z_pos = new_zpos; }
float SubMenuTest::getRed() { return m_color[0]; }
void SubMenuTest::setRed(float red) { m_color[0] = red; }
float SubMenuTest::getGreen() { return m_color[1]; }
void SubMenuTest::setGreen(float green) { m_color[1] = green; }
float SubMenuTest::getBlue() { return m_color[2]; }
void SubMenuTest::setBlue(float blue) { m_color[2] = blue; }
std::int32_t SubMenuTest::getWidth() { return m_width; }
void SubMenuTest::setWdith(std::int32_t new_width) { m_width = new_width; }
std::int32_t SubMenuTest::getHeight() { return m_height; }
void SubMenuTest::setHeight(std::int32_t new_height) { m_height = new_height; }
std::string SubMenuTest::getCaption() { return m_caption; }
void SubMenuTest::setCaption(const std::string& new_caption) {
  m_caption = new_caption;
}
float SubMenuTest::getPerecentBorder() { return m_percent_border; }
void SubMenuTest::setPercentBorder(float percent) {
  m_percent_border = percent;
}

void SubMenuTest::draw(render::RenderContext& context) {
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
}

std::string SubMenuTest::collectData() { return "Test:"; }