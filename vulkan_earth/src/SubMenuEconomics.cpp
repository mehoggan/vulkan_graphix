#include "SubMenuEconomics.h"
#include <cstring>
#include <string>
#include "ControlItem.h"
#include "ControlItemCheckBox.h"
#include "ControlItemSelectionBox.h"
#include "SubMenu.h"
#include "TextObject.h"
#include "MacroCrtdbg.h"

using namespace std;

SubMenuEconomics::SubMenuEconomics() = default;

SubMenuEconomics::SubMenuEconomics(int id,
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
                                        y_pos - (0.2 * height),
                                        z_pos + 1,
                                        0.5f,
                                        0.5f,
                                        0.5f,
                                        0.6f * width,
                                        0.06 * (height),
                                        "Interest Rate",
                                        "0.01/0.02/0.05/0.1/");
    sub_menu_button[1] =
            new ControlItemSelectionBox(x_pos + (width / 2) - (0.3 * width),
                                        y_pos - (0.27 * height),
                                        z_pos + 1,
                                        0.5f,
                                        0.5f,
                                        0.5f,
                                        0.6f * width,
                                        0.06 * (height),
                                        "Cash at Start",
                                        "1000/5000/10000/50000/100000/");
    sub_menu_button[2] =
            new ControlItemCheckBox(x_pos + (width / 2) - (0.3 * width),
                                    y_pos - (0.34 * height),
                                    z_pos + 1,
                                    0.5f,
                                    0.5f,
                                    0.5f,
                                    0.6f * width,
                                    0.06 * (height),
                                    "Computers Buy");
    sub_menu_button[3] =
            new ControlItemCheckBox(x_pos + (width / 2) - (0.3 * width),
                                    y_pos - (0.41 * height),
                                    z_pos + 1,
                                    0.5f,
                                    0.5f,
                                    0.5f,
                                    0.6f * width,
                                    0.06 * (height),
                                    "Free Market");
    sub_menu_button[4] =
            new ControlItemSelectionBox(x_pos + (width / 2) - (0.3 * width),
                                        y_pos - (0.48 * height),
                                        z_pos + 1,
                                        0.5f,
                                        0.5f,
                                        0.5f,
                                        0.6f * width,
                                        0.06 * (height),
                                        "Scoring Mode",
                                        "Weak Sauce/Standard/Mad Crazy/");
}

SubMenuEconomics::~SubMenuEconomics() {
    for (int i = 0; i < num_control_items_econ; i++) delete sub_menu_button[i];
    delete label;
}

int SubMenuEconomics::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
void SubMenuEconomics::setUNIQUEIDENTIFIER(int id) { uniqueidentifier = id; }
GLfloat SubMenuEconomics::getXPos() { return x_pos; }
void SubMenuEconomics::setXPos(GLfloat new_xpos) { x_pos = new_xpos; }
GLfloat SubMenuEconomics::getYPos() { return y_pos; }
void SubMenuEconomics::setYPos(GLfloat new_ypos) { y_pos = new_ypos; }
GLfloat SubMenuEconomics::getZPos() { return z_pos; }
void SubMenuEconomics::setZPos(GLfloat new_zpos) { z_pos = new_zpos; }
GLfloat SubMenuEconomics::getRed() { return color[0]; }
void SubMenuEconomics::setRed(GLfloat red) { color[0] = red; }
GLfloat SubMenuEconomics::getGreen() { return color[1]; }
void SubMenuEconomics::setGreen(GLfloat green) { color[1] = green; }
GLfloat SubMenuEconomics::getBlue() { return color[2]; }
void SubMenuEconomics::setBlue(GLfloat blue) { color[2] = blue; }
GLint SubMenuEconomics::getWidth() { return width; }
void SubMenuEconomics::setWdith(GLint new_width) { width = new_width; }
GLint SubMenuEconomics::getHeight() { return height; }
void SubMenuEconomics::setHeight(GLint new_height) { height = new_height; }
std::string SubMenuEconomics::getCaption() { return caption; }
void SubMenuEconomics::setCaption(const std::string& new_caption) {
    caption = new_caption;
}
GLfloat SubMenuEconomics::getPerecentBorder() { return percent_border; }
void SubMenuEconomics::setPercentBorder(GLfloat percent) {
    percent_border = percent;
}

void SubMenuEconomics::draw() {
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

    for (int i = 0; i < num_control_items_econ; i++) {
        if (sub_menu_button[i]) {
            sub_menu_button[i]->draw();
        }
    }
}

std::string SubMenuEconomics::collectData() {
    std::string optionsarray = "/Economics/";
    for (int x = 0; x < num_control_items_econ; x++) {
        if (sub_menu_button[x]) {
            optionsarray += sub_menu_button[x]->collectData();
            optionsarray += "/";
        }
    }
    return optionsarray;
}

void SubMenuEconomics::subMenuMouseTest(int x, int y, int button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < num_control_items_econ;
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

void SubMenuEconomics::updateMouse(int x, int y) {}