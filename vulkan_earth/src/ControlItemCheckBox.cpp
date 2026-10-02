#include "vulkan_earth/ControlItemCheckBox.h"
#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <string>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/render/Font.h"
#include "vulkan_earth/render/Renderer.h"
#include "vulkan_earth/render/UiBuilders.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_earth::render;

extern void playSFX(std::int32_t sfx);

ControlItemCheckBox::ControlItemCheckBox() = default;

ControlItemCheckBox::ControlItemCheckBox(float new_x_pos,
                                         float new_y_pos,
                                         float new_z_pos,
                                         float red,
                                         float green,
                                         float blue,
                                         std::int32_t new_width,
                                         std::int32_t new_height,
                                         const std::string& new_caption) {
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

    button_state = 0;
    menu_state = 0;

    /*	BUTTON TEXT PLACEMENT	*/
    std::int32_t real_length = 0;
    for (char ch : caption) {
        real_length += vulkan_earth::render::glutBitmapWidth(
                vulkan_earth::render::FontId::TimesRoman24, ch);
    }

    float label_x_pos = x_pos + (width / 2) - (real_length / 2);
    float label_y_pos = y_pos + ((y_pos - (y_pos + height)) / 2) - height / 4;
    /*	END OF BUTTON TEXT PLACEMENT	*/
    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           z_pos,
                           vulkan_earth::render::FontId::TimesRoman24,
                           0.0f,
                           0.0f,
                           0.0f);
}

ControlItemCheckBox::~ControlItemCheckBox() { delete label; }

void ControlItemCheckBox::draw(render::RenderContext& context) {
    using render::Vec3;
    using render::Vec4;
    if (built_button_state != button_state || built_menu_state != menu_state) {
        mesh.clear();
        // draw main button box (sunken bevel: -0.2 top/left, +0.4
        // bottom/right)
        Vec4 const dark(
                color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
        Vec4 const face(color[0], color[1], color[2], color[3]);
        Vec4 const light(
                color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
        render::appendQuad(mesh,
                           Vec3(x_pos, y_pos, z_pos),
                           Vec3(x_pos - 3, y_pos + 3, z_pos),
                           Vec3(x_pos + width + 3, y_pos + 3, z_pos),
                           Vec3(x_pos + width, y_pos, z_pos),
                           dark);
        render::appendQuad(mesh,
                           Vec3(x_pos - 3, y_pos + 3, z_pos),
                           Vec3(x_pos - 3, y_pos - height - 3, z_pos),
                           Vec3(x_pos, y_pos - height, z_pos),
                           Vec3(x_pos, y_pos, z_pos),
                           dark);
        render::appendQuad(mesh,
                           Vec3(x_pos, y_pos, z_pos),
                           Vec3(x_pos, y_pos - height, z_pos),
                           Vec3(x_pos + width, y_pos - height, z_pos),
                           Vec3(x_pos + width, y_pos, z_pos),
                           face);
        render::appendQuad(mesh,
                           Vec3(x_pos - 3, y_pos - height - 3, z_pos),
                           Vec3(x_pos + width + 3, y_pos - height - 3, z_pos),
                           Vec3(x_pos + width, y_pos - height, z_pos),
                           Vec3(x_pos, y_pos - height, z_pos),
                           light);
        render::appendQuad(mesh,
                           Vec3(x_pos + width, y_pos, z_pos),
                           Vec3(x_pos + width + 3, y_pos + 3, z_pos),
                           Vec3(x_pos + width + 3, y_pos - height - 3, z_pos),
                           Vec3(x_pos + width, y_pos + -height, z_pos),
                           light);

        // draw the actual check box itself: 4 smaller squares (2 triangles
        // each) whose innermost vertex is colored darker when pressed.
        Vec4 const up(color[0] + .2, color[1] + .2, color[2] + .2, 1.0f);
        Vec4 const center_color = button_state == 1 ? Vec4(color[0] - .2,
                                                           color[1] - .2,
                                                           color[2] - .2,
                                                           1.0f)
                                                    : up;
        float const z1 = z_pos + 1;
        Vec3 const center(
                x_pos + (width - height / 2), (y_pos - height / 2), z1);
        Vec3 const center4(x_pos + (width - height) + (height / 2),
                           (y_pos - height / 2),
                           z1);
        // square 1
        mesh.addTriangle({Vec3(x_pos + (width - height * 0.9),
                               (y_pos - height * 0.1),
                               z1),
                          Vec3(x_pos + (width - height * 0.9),
                               (y_pos - height / 2),
                               z1),
                          center},
                         {up, up, center_color});
        mesh.addTriangle({Vec3(x_pos + (width - height * 0.9),
                               (y_pos - height * 0.1),
                               z1),
                          center,
                          Vec3(x_pos + (width - height / 2),
                               (y_pos - height * 0.1),
                               z1)},
                         {up, center_color, up});
        // square 2
        mesh.addTriangle({Vec3(x_pos + (width - height * 0.9),
                               (y_pos - height / 2),
                               z1),
                          Vec3(x_pos + (width - height * 0.9),
                               (y_pos - height * 0.9),
                               z1),
                          center},
                         {up, up, center_color});
        mesh.addTriangle({Vec3(x_pos + (width - height * 0.9),
                               (y_pos - height * 0.9),
                               z1),
                          Vec3(x_pos + (width - height / 2),
                               (y_pos - height * 0.9),
                               z1),
                          center},
                         {up, up, center_color});
        // square 3
        mesh.addTriangle({center,
                          Vec3(x_pos + (width - height / 2),
                               (y_pos - height * 0.9),
                               z1),
                          Vec3(x_pos + (width - height) + height * 0.9,
                               (y_pos - height * 0.9),
                               z1)},
                         {center_color, up, up});
        mesh.addTriangle({center,
                          Vec3(x_pos + (width - height * 0.1),
                               (y_pos - height * 0.9),
                               z1),
                          Vec3(x_pos + (width - height * 0.1),
                               (y_pos - height / 2),
                               z1)},
                         {center_color, up, up});
        // square 4
        mesh.addTriangle({Vec3(x_pos + (width - height / 2),
                               (y_pos - height * 0.1),
                               z1),
                          center4,
                          Vec3(x_pos + (width - height * 0.1),
                               (y_pos - height * 0.1),
                               z1)},
                         {up, center_color, up});
        mesh.addTriangle({center4,
                          Vec3(x_pos + (width - height * 0.1),
                               (y_pos - height / 2),
                               z1),
                          Vec3(x_pos + (width - height * 0.1),
                               (y_pos - height * 0.1),
                               z1)},
                         {center_color, up, up});

        // draw check mark if it was toggled on, otherwise dont
        if (menu_state == 1) {
            Vec4 const green(0.0f, 1.0f, 0.0f, 1.0f);
            float const z2 = z_pos + 2;
            render::appendQuad(mesh,
                               Vec3(x_pos + (width - height + (height * 0.2)),
                                    (y_pos - height / 2) + (0.015 * width),
                                    z2),
                               Vec3(x_pos + (width - height + (height * 0.2)),
                                    (y_pos - height / 2) + (0.005 * width),
                                    z2),
                               Vec3(x_pos + (width - height / 2),
                                    (y_pos - height / 2) - (0.015 * width),
                                    z2),
                               Vec3(x_pos + (width - height / 2),
                                    (y_pos - height / 2),
                                    z2),
                               green);
            render::appendQuad(mesh,
                               Vec3(x_pos + (width - height / 2),
                                    (y_pos - height / 2) - (0.015 * width),
                                    z2),
                               Vec3(x_pos + (width - height / 2),
                                    (y_pos - height / 2),
                                    z2),
                               Vec3(x_pos + (width - height) + (0.95 * height),
                                    (y_pos - height / 2) + (0.025 * width),
                                    z2),
                               Vec3(x_pos + (width - height) + (0.95 * height),
                                    (y_pos - height / 2) + (0.015 * width),
                                    z2),
                               green);
        }
        built_button_state = button_state;
        built_menu_state = menu_state;
    }
    context.draw(mesh);

    label->draw(context);
}

float ControlItemCheckBox::getXPos() { return x_pos; }
float ControlItemCheckBox::getYPos() { return y_pos; }
float ControlItemCheckBox::getHeight() { return height; }
float ControlItemCheckBox::getWidth() { return width; }
std::string ControlItemCheckBox::collectData() {
    if (menu_state == 0)
        return "false";
    else
        return "true";
}

void ControlItemCheckBox::setOptionText(std::int32_t index) {}
void ControlItemCheckBox::setOptionText(const std::string& new_text) {}

// NOTE: I use height for the x value check, this is intentional to
// maintain a square
void ControlItemCheckBox::mouseClickEvent(
        std::int32_t x,
        std::int32_t y,
        std::int32_t state,
        bool still_over_control_item_check_box) {
    if ((x >= (x_pos + width - (height * 0.9)) &&
         (x <= x_pos + width - (height * 0.1))) &&
        ((y <= (y_pos - (height * 0.1))) && (y >= (y_pos - height * 0.9)))) {
        if (state == 1) {
            button_state = 1;
        } else if (state == 0) {
            if (still_over_control_item_check_box) {
                if (menu_state == 1)
                    menu_state = 0;
                else
                    menu_state = 1;
                playSFX(SMALL_CLICK);
            }

            button_state = 0;
        }
    }

    if (state == 0) {
        button_state = 0;
    }
}

void ControlItemCheckBox::updateMouse(std::int32_t x, std::int32_t y) {}