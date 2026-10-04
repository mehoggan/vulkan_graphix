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
    uniqueidentifier = id;
    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    percent_border = new_percent_border;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    width = new_width;
    height = new_height;
    caption = new_caption;

    /*	BUTTON TEXT PLACEMENT	*/
    std::int32_t real_length = 0;
    for (char ch : caption) {
        real_length += vulkan_earth::textAdvance(
                vulkan_earth::FontId::TimesRoman24, ch);
    }
    float label_x_pos = x_pos + ((width) / 2) - (real_length / 2);
    float label_y_pos = y_pos - height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           (z_pos + 1),
                           vulkan_earth::FontId::TimesRoman24,
                           0.0f,
                           0.0f,
                           0.0f);
    button_pressed = nullptr;
    sub_menu_button[0] =
            new ControlItemSelectionBox(x_pos + (width / 2) - (0.3 * width),
                                        y_pos - (height * 0.2),
                                        z_pos + 1,
                                        0.5f,
                                        0.5f,
                                        0.5f,
                                        0.6f * width,
                                        0.06 * (height),
                                        "Air Viscosity",
                                        "Low/Medium/High/");
    sub_menu_button[1] =
            new ControlItemSelectionBox(x_pos + (width / 2) - (0.3 * width),
                                        y_pos - (height * 0.27),
                                        z_pos + 1,
                                        0.5f,
                                        0.5f,
                                        0.5f,
                                        0.6f * width,
                                        0.06 * (height),
                                        "Gravity",
                                        "0.1/0.2/0.5/1.0/1.5/2.0/");
    sub_menu_button[2] =
            new ControlItemCheckBox(x_pos + (width / 2) - (0.3 * width),
                                    y_pos - (height * 0.34),
                                    z_pos + 1,
                                    0.5f,
                                    0.5f,
                                    0.5f,
                                    0.6f * width,
                                    0.06 * (height),
                                    "Tanks Fall");
}

SubMenuPhysics::~SubMenuPhysics() {
    for (std::int32_t i = 0; i < num_control_items_phy; i++)
        delete sub_menu_button[i];
    delete label;
}

std::int32_t SubMenuPhysics::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
void SubMenuPhysics::setUNIQUEIDENTIFIER(std::int32_t id) {
    uniqueidentifier = id;
}
float SubMenuPhysics::getXPos() { return x_pos; }
void SubMenuPhysics::setXPos(float new_xpos) { x_pos = new_xpos; }
float SubMenuPhysics::getYPos() { return y_pos; }
void SubMenuPhysics::setYPos(float new_ypos) { y_pos = new_ypos; }
float SubMenuPhysics::getZPos() { return z_pos; }
void SubMenuPhysics::setZPos(float new_zpos) { z_pos = new_zpos; }
float SubMenuPhysics::getRed() { return color[0]; }
void SubMenuPhysics::setRed(float red) { color[0] = red; }
float SubMenuPhysics::getGreen() { return color[1]; }
void SubMenuPhysics::setGreen(float green) { color[1] = green; }
float SubMenuPhysics::getBlue() { return color[2]; }
void SubMenuPhysics::setBlue(float blue) { color[2] = blue; }
std::int32_t SubMenuPhysics::getWidth() { return width; }
void SubMenuPhysics::setWdith(std::int32_t new_width) { width = new_width; }
std::int32_t SubMenuPhysics::getHeight() { return height; }
void SubMenuPhysics::setHeight(std::int32_t new_height) {
    height = new_height;
}
std::string SubMenuPhysics::getCaption() { return caption; }
void SubMenuPhysics::setCaption(const std::string& new_caption) {
    caption = new_caption;
}
float SubMenuPhysics::getPerecentBorder() { return percent_border; }
void SubMenuPhysics::setPercentBorder(float percent) {
    percent_border = percent;
}

void SubMenuPhysics::draw(render::RenderContext& context) {
    // The same raised 3-pixel bevel every button draws.
    if (frame_mesh.triangles().empty()) {
        vulkan_earth::appendBevel(
                frame_mesh,
                x_pos,
                y_pos,
                z_pos,
                width,
                height,
                math::Vec4<float>(color[0], color[1], color[2], color[3]),
                false);
    }
    context.draw(frame_mesh);
    label->draw(context);
    for (std::int32_t i = 0; i < num_control_items_phy; i++) {
        if (sub_menu_button[i]) {
            sub_menu_button[i]->draw(context);
        }
    }
}

std::string SubMenuPhysics::collectData() {
    std::string optionsarray = "/Physics/";
    for (std::int32_t x = 0; x < num_control_items_phy; x++) {
        if (sub_menu_button[x]) {
            optionsarray += sub_menu_button[x]->collectData();
            optionsarray += "/";
        }
    }
    return optionsarray;
}

void SubMenuPhysics::subMenuMouseTest(std::int32_t x,
                                      std::int32_t y,
                                      std::int32_t button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (std::int32_t button_i = 0; button_i < num_control_items_phy;
             button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                            // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                            // CLICKED A ARROW BUTTON???
            if ((x >= sub_menu_button[button_i]->getXPos()) &&
                (x <= (sub_menu_button[button_i]->getXPos() +
                       sub_menu_button[button_i]->getWidth())) &&
                (y <= sub_menu_button[button_i]->getYPos()) &&
                (y >= (sub_menu_button[button_i]->getYPos() -
                       sub_menu_button[button_i]->getHeight()))) {
                sub_menu_button[button_i]->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // YOU PRESSED OVER A ARROWBUTTON
                button_pressed = sub_menu_button[button_i];
            }
        }
    } else if (!button_down) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                                // GOES UP
        if (button_pressed !=
            nullptr) {  // IF YOU MANAGED TO CLICK INSIDE AN ARROW BUTTON
                        // CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if ((x >= button_pressed->getXPos()) &&
                (x <=
                 (button_pressed->getXPos() + button_pressed->getWidth())) &&
                (y <= button_pressed->getYPos()) &&
                (y >=
                 (button_pressed->getYPos() - button_pressed->getHeight()))) {
                button_pressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                // RELEASE THE MOUSE
            } else {
                button_pressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                button_pressed = nullptr;
            }
        }
    }
}

void SubMenuPhysics::updateMouse(std::int32_t x, std::int32_t y) {}