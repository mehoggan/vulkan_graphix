#include "vulkan_earth/ControlItemButton.h"
#include <cstdint>
#include <string>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/SubMenuLandscape.h"
#include "vulkan_earth/TerrainMaker.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

extern void playSFX(std::int32_t sfx);

ControlItemButton::ControlItemButton() = default;

ControlItemButton::ControlItemButton(SubMenuLandscape* new_parent,
                                     float new_x_pos,
                                     float new_y_pos,
                                     float new_z_pos,
                                     float red,
                                     float green,
                                     float blue,
                                     std::int32_t new_width,
                                     std::int32_t new_height,
                                     const std::string& new_caption) {
    parent = new_parent;

    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    width = new_width;
    height = new_height;
    caption = new_caption;

    toggled = false;
    button_state = 0;
    menu_state = 0;

    /*	BUTTON TEXT PLACEMENT	*/
    std::int32_t real_length = 0;
    for (char ch : caption) {
        real_length += vulkan_earth::textAdvance(
                vulkan_earth::FontId::TimesRoman24, ch);
    }
    float label_x_pos = x_pos + ((width) / 2) - (real_length / 2);
    float label_y_pos = y_pos + ((y_pos - (y_pos + height)) / 2) - height / 4;
    /*	END OF BUTTON TEXT PLACEMENT	*/
    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           z_pos,
                           vulkan_earth::FontId::TimesRoman24,
                           0.0f,
                           0.0f,
                           0.0f);
}

ControlItemButton::~ControlItemButton() { delete label; }

void ControlItemButton::draw(render::RenderContext& context) {
    if (built_button_state != button_state) {
        mesh.clear();
        vulkan_earth::appendBevel(
                mesh,
                x_pos,
                y_pos,
                z_pos,
                width,
                height,
                math::Vec4<float>(color[0], color[1], color[2], color[3]),
                button_state != 0);
        built_button_state = button_state;
    }
    context.draw(mesh);
    label->draw(context);
}

float ControlItemButton::getXPos() { return x_pos; }
float ControlItemButton::getYPos() { return y_pos; }
float ControlItemButton::getHeight() { return height; }
float ControlItemButton::getWidth() { return width; }
bool ControlItemButton::isToggled() { return toggled; }
void ControlItemButton::updateButtonState() {
    if (toggled)
        button_state = 1;
    else
        button_state = 0;
}
void ControlItemButton::setToggled(bool t) { toggled = t; }
void ControlItemButton::setOptionText(std::int32_t index) {}
void ControlItemButton::setOptionText(const std::string& new_text) {}
std::string ControlItemButton::collectData() { return "Button"; }

void ControlItemButton::mouseClickEvent(
        std::int32_t x,
        std::int32_t y,
        std::int32_t state,
        bool /*still_over_control_item_button*/) {
    if (state) {
        if ((x >= (x_pos) && x <= ((x_pos) + (width))) &&
            (y <= (y_pos) &&
             y >= ((y_pos) - (height)))) {  // This if statement -->
                                            // stillOverControlItemButton
            button_state = 1;
            toggled = true;
        } else {
            button_state = 0;
        }
    } else {
        button_state = 0;
    }
}

void ControlItemButton::updateMouse(std::int32_t x, std::int32_t y) {}
