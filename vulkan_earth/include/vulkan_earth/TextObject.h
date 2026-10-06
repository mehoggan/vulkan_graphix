#ifndef VULKAN_EARTH_TEXTOBJECT_H
#define VULKAN_EARTH_TEXTOBJECT_H

#include <string>
#include "vulkan_earth/GameRenderer.h"

namespace vulkan_graphix::Render {
class RenderContext;
}

class TextObject {
public:
    TextObject();
    // font is the GLUT bitmap font the text was written for; as in the
    // original, a window narrower than 1300 pixels always uses the 9x15
    // font instead.
    TextObject(const std::string& input,
      float new_pos_x,
      float new_pos_y,
      float new_pos_z,
      vulkan_earth::FontId font,
      float red,
      float green,
      float blue);
    ~TextObject();
    const std::string& getOutput();
    void setXpos(float x);
    void setYpos(float y);
    void setZpos(float z);
    void draw(vulkan_graphix::Render::RenderContext& context);

private:
    std::string m_output;
    float m_pos_x;
    float m_pos_y;
    float m_pos_z;
    float m_color[4];
    vulkan_earth::FontId m_font_size;
};

#endif
