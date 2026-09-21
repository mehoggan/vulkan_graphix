#include "SubMenuWeapons.h"
#include <cstring>
#include <string>
#include "ControlItem.h"
#include "ControlItemCheckBox.h"
#include "ControlItemSelectionBox.h"
#include "SubMenu.h"
#include "TextObject.h"
#include "macro_crtdbg.h"

using namespace std;

SubMenuWeapons::SubMenuWeapons() = default;

SubMenuWeapons::SubMenuWeapons(int id,
                               GLfloat x_pos,
                               GLfloat y_pos,
                               GLfloat z_pos,
                               GLfloat red,
                               GLfloat green,
                               GLfloat blue,
                               GLint width,
                               GLint height,
                               const std::string& caption,
                               GLfloat percent_border) {
    uniqueidentifier = id;
    this->x_pos = x_pos;
    this->y_pos = y_pos;
    this->z_pos = z_pos;
    this->percent_border = percent_border;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    this->width = width;
    this->height = height;
    this->caption = caption;

    /*	BUTTON TEXT PLACEMENT	*/
    int real_length = 0;
    for (char ch : this->caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos =
            this->x_pos + ((this->width) / 2) - (real_length / 2);
    GLfloat label_y_pos = this->y_pos - this->height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    label = new TextObject(this->caption,
                           label_x_pos,
                           label_y_pos,
                           (this->z_pos + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
    button_pressed = nullptr;
    sub_menu_button[0] = new ControlItemSelectionBox(
            this->x_pos + (this->width / 2) - (0.3 * this->width),
            this->y_pos - (this->height * 0.2),
            this->z_pos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Arms Level",
            "1/2/3/4/5/6/7/8/9/10/");
    sub_menu_button[1] = new ControlItemSelectionBox(
            this->x_pos + (this->width / 2) - (0.3 * this->width),
            this->y_pos - (this->height * 0.27),
            this->z_pos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Bomb Icon",
            "Small/Medium/Large/");
    sub_menu_button[2] = new ControlItemCheckBox(
            this->x_pos + (this->width / 2) - (0.3 * this->width),
            this->y_pos - (this->height * 0.34),
            this->z_pos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Tunneling");
    sub_menu_button[3] = new ControlItemSelectionBox(
            this->x_pos + (this->width / 2) - (0.3 * this->width),
            this->y_pos - (this->height * 0.41),
            this->z_pos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Scale",
            "Small/Medium/Large/");
    sub_menu_button[4] = new ControlItemCheckBox(
            this->x_pos + (this->width / 2) - (0.3 * this->width),
            this->y_pos - (this->height * 0.48),
            this->z_pos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Trace Path");
}

SubMenuWeapons::~SubMenuWeapons() {
    delete label;
    for (int i = 0; i < num_control_items_wpn; i++) delete sub_menu_button[i];
}

int SubMenuWeapons::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
void SubMenuWeapons::setUNIQUEIDENTIFIER(int id) { uniqueidentifier = id; }
GLfloat SubMenuWeapons::getXPos() { return this->x_pos; }
void SubMenuWeapons::setXPos(GLfloat new_xpos) { this->x_pos = new_xpos; }
GLfloat SubMenuWeapons::getYPos() { return this->y_pos; }
void SubMenuWeapons::setYPos(GLfloat new_ypos) { this->y_pos = new_ypos; }
GLfloat SubMenuWeapons::getZPos() { return this->z_pos; }
void SubMenuWeapons::setZPos(GLfloat new_zpos) { this->z_pos = new_zpos; }
GLfloat SubMenuWeapons::getRed() { return color[0]; }
void SubMenuWeapons::setRed(GLfloat red) { color[0] = red; }
GLfloat SubMenuWeapons::getGreen() { return color[1]; }
void SubMenuWeapons::setGreen(GLfloat green) { color[1] = green; }
GLfloat SubMenuWeapons::getBlue() { return color[2]; }
void SubMenuWeapons::setBlue(GLfloat blue) { color[2] = blue; }
GLint SubMenuWeapons::getWidth() { return this->width; }
void SubMenuWeapons::setWdith(GLint width) { this->width = width; }
GLint SubMenuWeapons::getHeight() { return this->height; }
void SubMenuWeapons::setHeight(GLint height) { this->height = height; }
std::string SubMenuWeapons::getCaption() { return this->caption; }
void SubMenuWeapons::setCaption(const std::string& caption) {
    this->caption = caption;
}
GLfloat SubMenuWeapons::getPerecentBorder() { return this->percent_border; }
void SubMenuWeapons::setPercentBorder(GLfloat percent) {
    this->percent_border = percent_border;
}

void SubMenuWeapons::draw() {
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
    glVertex3f(this->x_pos, this->y_pos, this->z_pos);
    glVertex3f(this->x_pos - 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
    glVertex3f(this->x_pos - 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos - 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);
    glVertex3f(this->x_pos, this->y_pos, this->z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0], color[1], color[2], color[3]);
    glVertex3f(this->x_pos, this->y_pos, this->z_pos);
    glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(this->x_pos - 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos);
    glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos + -height, this->z_pos);
    glEnd();
    label->draw();
    for (int i = 0; i < num_control_items_wpn; i++) {
        if (sub_menu_button[i]) {
            sub_menu_button[i]->draw();
        }
    }
}

std::string SubMenuWeapons::collectData() {
    std::string optionsarray = "/Weapons/";
    for (int x = 0; x < num_control_items_wpn; x++) {
        if (sub_menu_button[x]) {
            optionsarray += sub_menu_button[x]->collectData();
            optionsarray += "/";
        }
    }
    return optionsarray;
}

void SubMenuWeapons::subMenuMouseTest(int x, int y, int button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < num_control_items_wpn;
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

void SubMenuWeapons::updateMouse(int x, int y) {}