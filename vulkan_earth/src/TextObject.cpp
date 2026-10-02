#include "vulkan_earth/TextObject.h"
#include <cstdint>
#include <string>
#include "vulkan_earth/render/Renderer.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_earth::render;

TextObject::TextObject() = default;

TextObject::TextObject(const std::string& input,
                       float new_pos_x,
                       float new_pos_y,
                       float new_pos_z,
                       render::FontId font,
                       float red,
                       float green,
                       float blue) {
    output = input;
    pos_x = new_pos_x;
    pos_y = new_pos_y;
    pos_z = new_pos_z + 1;
    font_size = font;
    if (render::Renderer::instance().extent().width < 1300) {
        font_size = render::FontId::Fixed9By15;
    }
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
}

TextObject::~TextObject() = default;

const std::string& TextObject::getOutput() { return output; }
void TextObject::setXpos(float x) { pos_x = x; }
void TextObject::setYpos(float y) { pos_y = y; }
void TextObject::setZpos(float z) { pos_z = z; }

// One raster position per character, advancing by the glyph width in menu
// units, exactly as the original's glRasterPos3f()/glutBitmapCharacter()
// loop did.
void TextObject::draw(render::RenderContext& context) {
    render::GlutFont const& font =
            render::Renderer::instance().font(font_size);
    render::Vec4 const text_color(color[0], color[1], color[2], 1.0f);
    float x_pos = pos_x;
    for (char ch : output) {
        context.drawBitmapText(font,
                               render::Vec3(x_pos, pos_y, pos_z),
                               std::string_view(&ch, 1),
                               text_color);
        x_pos += static_cast<float>(font.advance(ch));
    }
}
