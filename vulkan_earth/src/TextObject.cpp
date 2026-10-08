#include "vulkan_earth/TextObject.h"
#include <string>
#include "vulkan_earth/GameRenderer.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

TextObject::TextObject() = default;

TextObject::TextObject(const std::string& input,
    float new_pos_x,
    float new_pos_y,
    float new_pos_z,
    vulkan_earth::FontId font,
    float red,
    float green,
    float blue) {
  m_output = input;
  m_pos_x = new_pos_x;
  m_pos_y = new_pos_y;
  m_pos_z = new_pos_z + 1;
  m_font_size = font;
  if (render::Renderer::instance().extent().width < 1300) {
    m_font_size = vulkan_earth::FontId::Fixed9By15;
  }
  m_color[0] = red;
  m_color[1] = green;
  m_color[2] = blue;
  m_color[3] = 1.0;
}

TextObject::~TextObject() = default;

const std::string& TextObject::getOutput() { return m_output; }
void TextObject::setXpos(float x) { m_pos_x = x; }
void TextObject::setYpos(float y) { m_pos_y = y; }
void TextObject::setZpos(float z) { m_pos_z = z; }

// One raster position per character, advancing by the glyph width in menu
// units, exactly as the original's glRasterPos3f()/glutBitmapCharacter()
// loop did.
void TextObject::draw(render::RenderContext& context) {
  const render::Font& font = vulkan_earth::font(m_font_size);
  const math::Vec4<float> text_color(m_color[0], m_color[1], m_color[2], 1.0f);
  float x_pos = m_pos_x;
  for (char ch : m_output) {
    context.drawText(font,
        math::Vec3<float>(x_pos, m_pos_y, m_pos_z),
        std::string_view(&ch, 1),
        text_color);
    x_pos += static_cast<float>(vulkan_earth::textAdvance(m_font_size, ch));
  }
}
