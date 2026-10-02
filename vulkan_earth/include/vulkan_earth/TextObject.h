#ifndef TEXT_OBJECT_H
#define TEXT_OBJECT_H

#include <string>

#include "vulkan_earth/render/Font.h"

namespace vulkan_earth::render {
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
               vulkan_earth::render::FontId font,
               float red,
               float green,
               float blue);
    ~TextObject();
    const std::string& getOutput();
    void setXpos(float x);
    void setYpos(float y);
    void setZpos(float z);
    void draw(vulkan_earth::render::RenderContext& context);

private:
    std::string output;
    float pos_x;
    float pos_y;
    float pos_z;
    float color[4];
    vulkan_earth::render::FontId font_size;
};

#endif
