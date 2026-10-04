#include "vulkan_earth/ControlItemTextField.h"
#include <stdio.h>
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

const std::int32_t max_chars = 15;

extern void playSFX(std::int32_t sfx);

ControlItemTextField::ControlItemTextField() = default;

ControlItemTextField::ControlItemTextField(float new_x_pos,
                                           float new_y_pos,
                                           float new_z_pos,
                                           float red,
                                           float green,
                                           float blue,
                                           std::int32_t new_width,
                                           std::int32_t new_height) {
    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    width = new_width;
    height = new_height;
    current_text = nullptr;
    text_field_active = false;
    clearTextBuffer();
    current_length = 0;
    number_of_frames = 0;
    text_cursor_on = 1;
    setOptionText("");
}

ControlItemTextField::~ControlItemTextField() { delete current_text; }

void ControlItemTextField::draw(render::RenderContext& context) {
    using Vec3 = math::Vec3<float>;
    using Vec4 = math::Vec4<float>;
    if (number_of_frames == 50) {
        text_cursor_on *= -1;  // toggle
        number_of_frames = 0;
    }
    number_of_frames++;

    if (frame_mesh.triangles().empty()) {
        vulkan_earth::appendFrame(frame_mesh,
                                  x_pos,
                                  y_pos,
                                  z_pos,
                                  width,
                                  height,
                                  Vec4(color[0] - 0.6f,
                                       color[1] - 0.6f,
                                       color[2] - 0.6f,
                                       color[3]),
                                  Vec4(color[0], color[1], color[2], color[3]),
                                  Vec4(color[0] - 0.3f,
                                       color[1] - 0.3f,
                                       color[2] - 0.3f,
                                       color[3]));
    }
    context.draw(frame_mesh);

    bool const cursor_visible = text_field_active && text_cursor_on == 1;
    if (cursor_visible != cursor_built_visible ||
        (cursor_visible && cursor_built_chars != current_chars)) {
        cursor_mesh.clear();
        if (cursor_visible) {
            std::int32_t real_length = 0;
            for (char ch : current_chars) {
                if (ch != ' ') {
                    real_length += vulkan_earth::textAdvance(
                            vulkan_earth::FontId::TimesRoman24, ch);
                }
            }
            cursor_mesh.addQuad({Vec3(x_pos + 0.02 * width + real_length,
                                      y_pos - 0.15 * height,
                                      z_pos + 0.1),
                                 Vec3(x_pos + 0.02 * width + real_length,
                                      y_pos - height + 0.15 * height,
                                      z_pos + 0.1),
                                 Vec3(x_pos + 0.02 * width + real_length + 2,
                                      y_pos - height + 0.15 * height,
                                      z_pos + 0.1),
                                 Vec3(x_pos + 0.02 * width + real_length + 2,
                                      y_pos - 0.15 * height,
                                      z_pos + 0.1)},
                                Vec4(0, 0, 0, 1));
        }
        cursor_built_visible = cursor_visible;
        cursor_built_chars = current_chars;
    }
    context.draw(cursor_mesh);

    if (current_text) current_text->draw(context);
}

float ControlItemTextField::getXPos() { return x_pos; }
float ControlItemTextField::getYPos() { return y_pos; }
float ControlItemTextField::getHeight() { return height; }
float ControlItemTextField::getWidth() { return width; }
bool ControlItemTextField::isTextFieldActive() { return text_field_active; }
std::string ControlItemTextField::collectData() {
    return current_text->getOutput();
}
void ControlItemTextField::deactivate() { text_field_active = false; }
void ControlItemTextField::setOptionText(std::int32_t index) {}

void ControlItemTextField::setOptionText(const std::string& new_text) {
    delete current_text;

    std::int32_t real_length = 0;
    for (char ch : new_text) {
        real_length += vulkan_earth::textAdvance(
                vulkan_earth::FontId::TimesRoman24, ch);
    }
    float label_x_pos = x_pos + 0.02 * width;
    float label_y_pos = y_pos + ((y_pos - (y_pos + height)) / 2) - height / 4;
    current_text = new TextObject(new_text,
                                  label_x_pos,
                                  label_y_pos,
                                  z_pos + 0.1,
                                  vulkan_earth::FontId::TimesRoman24,
                                  0.0f,
                                  0.0f,
                                  0.0f);
}

void ControlItemTextField::mouseClickEvent(
        std::int32_t /*x*/,
        std::int32_t /*y*/,
        std::int32_t state,
        bool still_over_control_item_text_field) {
    if (state == 0)
        if (still_over_control_item_text_field) text_field_active = true;
}

void ControlItemTextField::updateMouse(std::int32_t x, std::int32_t y) {}
void ControlItemTextField::keyHandler(std::uint8_t key) {
    if (text_field_active) {
        if ((((key >= 48) && (key <= 57)) || ((key >= 65) && (key < 90))) ||
            ((key >= 97) && (key <= 122))) {
            if (current_text == nullptr) {
                setOptionText("");
                current_length = 0;
            }
            if (current_length < max_chars) {
                playSFX(KEYTYPING);
                current_chars[current_length] = key;
                current_length++;
                setOptionText(current_chars);
            }

        } else if (key == 8) {
            if (current_text) {
                if (current_length != 0) {
                    playSFX(KEYTYPING);
                    current_chars[current_length - 1] = ' ';
                    current_length--;
                    setOptionText(current_chars);
                }
            }
        }
    }
}

void ControlItemTextField::clearTextBuffer() {
    current_chars.assign(max_chars, ' ');
    current_length = 0;
}

void ControlItemTextField::setTextBuffer(const std::string& new_text) {
    setOptionText(new_text);
    clearTextBuffer();
    std::int32_t new_length = 0;
    for (size_t i = 0; i < new_text.size() && i < current_chars.size(); i++) {
        current_chars[i] = new_text[i];
        if (new_text[i] != ' ') new_length++;
    }
    current_length = new_length;
}