#include "vulkan_earth/SubMenuWeapons.h"
#include <cstdint>
#include <cstring>
#include <string>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemCheckBox.h"
#include "vulkan_earth/ControlItemSelectionBox.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

SubMenuWeapons::SubMenuWeapons() = default;

SubMenuWeapons::SubMenuWeapons(std::int32_t id,
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
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    float label_x_pos = x_pos + ((width) / 2) - (real_length / 2);
    float label_y_pos = y_pos - height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           (z_pos + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
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
                                        "Arms Level",
                                        "1/2/3/4/5/6/7/8/9/10/");
    sub_menu_button[1] =
            new ControlItemSelectionBox(x_pos + (width / 2) - (0.3 * width),
                                        y_pos - (height * 0.27),
                                        z_pos + 1,
                                        0.5f,
                                        0.5f,
                                        0.5f,
                                        0.6f * width,
                                        0.06 * (height),
                                        "Bomb Icon",
                                        "Small/Medium/Large/");
    sub_menu_button[2] =
            new ControlItemCheckBox(x_pos + (width / 2) - (0.3 * width),
                                    y_pos - (height * 0.34),
                                    z_pos + 1,
                                    0.5f,
                                    0.5f,
                                    0.5f,
                                    0.6f * width,
                                    0.06 * (height),
                                    "Tunneling");
    sub_menu_button[3] =
            new ControlItemSelectionBox(x_pos + (width / 2) - (0.3 * width),
                                        y_pos - (height * 0.41),
                                        z_pos + 1,
                                        0.5f,
                                        0.5f,
                                        0.5f,
                                        0.6f * width,
                                        0.06 * (height),
                                        "Scale",
                                        "Small/Medium/Large/");
    sub_menu_button[4] =
            new ControlItemCheckBox(x_pos + (width / 2) - (0.3 * width),
                                    y_pos - (height * 0.48),
                                    z_pos + 1,
                                    0.5f,
                                    0.5f,
                                    0.5f,
                                    0.6f * width,
                                    0.06 * (height),
                                    "Trace Path");
}

SubMenuWeapons::~SubMenuWeapons() {
    delete label;
    for (std::int32_t i = 0; i < num_control_items_wpn; i++)
        delete sub_menu_button[i];
}

std::int32_t SubMenuWeapons::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
void SubMenuWeapons::setUNIQUEIDENTIFIER(std::int32_t id) {
    uniqueidentifier = id;
}
float SubMenuWeapons::getXPos() { return x_pos; }
void SubMenuWeapons::setXPos(float new_xpos) { x_pos = new_xpos; }
float SubMenuWeapons::getYPos() { return y_pos; }
void SubMenuWeapons::setYPos(float new_ypos) { y_pos = new_ypos; }
float SubMenuWeapons::getZPos() { return z_pos; }
void SubMenuWeapons::setZPos(float new_zpos) { z_pos = new_zpos; }
float SubMenuWeapons::getRed() { return color[0]; }
void SubMenuWeapons::setRed(float red) { color[0] = red; }
float SubMenuWeapons::getGreen() { return color[1]; }
void SubMenuWeapons::setGreen(float green) { color[1] = green; }
float SubMenuWeapons::getBlue() { return color[2]; }
void SubMenuWeapons::setBlue(float blue) { color[2] = blue; }
std::int32_t SubMenuWeapons::getWidth() { return width; }
void SubMenuWeapons::setWdith(std::int32_t new_width) { width = new_width; }
std::int32_t SubMenuWeapons::getHeight() { return height; }
void SubMenuWeapons::setHeight(std::int32_t new_height) {
    height = new_height;
}
std::string SubMenuWeapons::getCaption() { return caption; }
void SubMenuWeapons::setCaption(const std::string& new_caption) {
    caption = new_caption;
}
float SubMenuWeapons::getPerecentBorder() { return percent_border; }
void SubMenuWeapons::setPercentBorder(float percent) {
    percent_border = percent;
}

void SubMenuWeapons::draw() {
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
    glVertex3f(x_pos, y_pos, z_pos);
    glVertex3f(x_pos - 3, y_pos + 3, z_pos);
    glVertex3f(x_pos + width + 3, y_pos + 3, z_pos);
    glVertex3f(x_pos + width, y_pos, z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
    glVertex3f(x_pos - 3, y_pos + 3, z_pos);
    glVertex3f(x_pos - 3, y_pos - height - 3, z_pos);
    glVertex3f(x_pos, y_pos - height, z_pos);
    glVertex3f(x_pos, y_pos, z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0], color[1], color[2], color[3]);
    glVertex3f(x_pos, y_pos, z_pos);
    glVertex3f(x_pos, y_pos - height, z_pos);
    glVertex3f(x_pos + width, y_pos - height, z_pos);
    glVertex3f(x_pos + width, y_pos, z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(x_pos - 3, y_pos - height - 3, z_pos);
    glVertex3f(x_pos + width + 3, y_pos - height - 3, z_pos);
    glVertex3f(x_pos + width, y_pos - height, z_pos);
    glVertex3f(x_pos, y_pos - height, z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(x_pos + width, y_pos, z_pos);
    glVertex3f(x_pos + width + 3, y_pos + 3, z_pos);
    glVertex3f(x_pos + width + 3, y_pos - height - 3, z_pos);
    glVertex3f(x_pos + width, y_pos + -height, z_pos);
    glEnd();
    label->draw();
    for (std::int32_t i = 0; i < num_control_items_wpn; i++) {
        if (sub_menu_button[i]) {
            sub_menu_button[i]->draw();
        }
    }
}

std::string SubMenuWeapons::collectData() {
    std::string optionsarray = "/Weapons/";
    for (std::int32_t x = 0; x < num_control_items_wpn; x++) {
        if (sub_menu_button[x]) {
            optionsarray += sub_menu_button[x]->collectData();
            optionsarray += "/";
        }
    }
    return optionsarray;
}

void SubMenuWeapons::subMenuMouseTest(std::int32_t x,
                                      std::int32_t y,
                                      std::int32_t button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (std::int32_t button_i = 0; button_i < num_control_items_wpn;
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

void SubMenuWeapons::updateMouse(std::int32_t x, std::int32_t y) {}