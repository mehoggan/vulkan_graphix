#include "SubMenuPlayOptions.h"
#include <cstring>
#include <string>
#include "ControlItem.h"
#include "ControlItemCheckBox.h"
#include "ControlItemSelectionBox.h"
#include "SubMenu.h"
#include "TextObject.h"
#include "macro_crtdbg.h"

using namespace std;

SubMenuPlayOptions::SubMenuPlayOptions() = default;

SubMenuPlayOptions::SubMenuPlayOptions(int id,
                                       GLfloat new_x_pos,
                                       GLfloat new_y_pos,
                                       GLfloat new_z_pos,
                                       GLfloat red,
                                       GLfloat green,
                                       GLfloat blue,
                                       GLint new_width,
                                       GLint new_height,
                                       const std::string& new_caption,
                                       GLfloat new_percent_border) {
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
    int real_length = 0;
    for (char ch : caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = x_pos + ((width) / 2) - (real_length / 2);
    GLfloat label_y_pos = y_pos - height / 20;
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
                                        "Teams",
                                        "Allowed/Random/Not Allowed/");
    sub_menu_button[1] =
            new ControlItemCheckBox(x_pos + (width / 2) - (0.3 * width),
                                    y_pos - (height * 0.27),
                                    z_pos + 1,
                                    0.5f,
                                    0.5f,
                                    0.5f,
                                    0.6f * width,
                                    0.06 * (height),
                                    "Status Bar");
    sub_menu_button[2] =
            new ControlItemSelectionBox(x_pos + (width / 2) - (0.3 * width),
                                        y_pos - (height * 0.34),
                                        z_pos + 1,
                                        0.5f,
                                        0.5f,
                                        0.5f,
                                        0.6f * width,
                                        0.06 * (height),
                                        "Play Order",
                                        "Sequential/Random/");
    sub_menu_button[3] =
            new ControlItemCheckBox(x_pos + (width / 2) - (0.3 * width),
                                    y_pos - (height * 0.41),
                                    z_pos + 1,
                                    0.5f,
                                    0.5f,
                                    0.5f,
                                    0.6f * width,
                                    0.06 * (height),
                                    "Fast Computers");
    sub_menu_button[4] =
            new ControlItemSelectionBox(x_pos + (width / 2) - (0.3 * width),
                                        y_pos - (height * 0.48),
                                        z_pos + 1,
                                        0.5f,
                                        0.5f,
                                        0.5f,
                                        0.6f * width,
                                        0.06 * (height),
                                        "Talking Tanks",
                                        "Yes/No/");
    sub_menu_button[5] = new ControlItemSelectionBox(
            x_pos + (width / 2) - (0.3 * width),
            y_pos - (height * 0.55),
            z_pos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * width,
            0.06 * (height),
            "Talk Probability",
            "0.1/0.2/0.3/0.4/0.5/0.6/0.7/0.8/0.9/1.0/");
}

SubMenuPlayOptions::~SubMenuPlayOptions() {
    for (int i = 0; i < num_control_items_po; i++) delete sub_menu_button[i];
    delete label;
}

int SubMenuPlayOptions::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
void SubMenuPlayOptions::setUNIQUEIDENTIFIER(int id) { uniqueidentifier = id; }
GLfloat SubMenuPlayOptions::getXPos() { return x_pos; }
void SubMenuPlayOptions::setXPos(GLfloat new_xpos) { x_pos = new_xpos; }
GLfloat SubMenuPlayOptions::getYPos() { return y_pos; }
void SubMenuPlayOptions::setYPos(GLfloat new_ypos) { y_pos = new_ypos; }
GLfloat SubMenuPlayOptions::getZPos() { return z_pos; }
void SubMenuPlayOptions::setZPos(GLfloat new_zpos) { z_pos = new_zpos; }
GLfloat SubMenuPlayOptions::getRed() { return color[0]; }
void SubMenuPlayOptions::setRed(GLfloat red) { color[0] = red; }
GLfloat SubMenuPlayOptions::getGreen() { return color[1]; }
void SubMenuPlayOptions::setGreen(GLfloat green) { color[1] = green; }
GLfloat SubMenuPlayOptions::getBlue() { return color[2]; }
void SubMenuPlayOptions::setBlue(GLfloat blue) { color[2] = blue; }
GLint SubMenuPlayOptions::getWidth() { return width; }
void SubMenuPlayOptions::setWdith(GLint new_width) { width = new_width; }
GLint SubMenuPlayOptions::getHeight() { return height; }
void SubMenuPlayOptions::setHeight(GLint new_height) { height = new_height; }
std::string SubMenuPlayOptions::getCaption() { return caption; }
void SubMenuPlayOptions::setCaption(const std::string& new_caption) {
    caption = new_caption;
}
GLfloat SubMenuPlayOptions::getPerecentBorder() { return percent_border; }
void SubMenuPlayOptions::setPercentBorder(GLfloat percent) {
    percent_border = percent;
}

void SubMenuPlayOptions::draw() {
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
    for (int i = 0; i < num_control_items_po; i++) {
        if (sub_menu_button[i]) {
            sub_menu_button[i]->draw();
        }
    }
}

std::string SubMenuPlayOptions::collectData() {
    std::string optionsarray = "/Game Options/";
    for (int x = 0; x < num_control_items_po; x++) {
        if (sub_menu_button[x]) {
            optionsarray += sub_menu_button[x]->collectData();
            optionsarray += "/";
        }
    }
    return optionsarray;
}

void SubMenuPlayOptions::subMenuMouseTest(int x, int y, int button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < num_control_items_po;
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

void SubMenuPlayOptions::updateMouse(int x, int y) {}