#include "SubMenuHardware.h"
#include <cstring>
#include <string>
#include "ControlItem.h"
#include "ControlItemCheckBox.h"
#include "ControlItemSelectionBox.h"
#include "SubMenu.h"
#include "TextObject.h"
#include "macro_crtdbg.h"

using namespace std;

SubMenuHardware::SubMenuHardware() = default;

SubMenuHardware::SubMenuHardware(int id,
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
    UNIQUEIDENTIFIER = id;
    this->xPos = x_pos;
    this->yPos = y_pos;
    this->zPos = z_pos;
    this->percentBorder = percent_border;
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
    GLfloat label_x_pos = this->xPos + ((this->width) / 2) - (real_length / 2);
    GLfloat label_y_pos = this->yPos - this->height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    label = new TextObject(this->caption,
                           label_x_pos,
                           label_y_pos,
                           (this->zPos + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
    buttonPressed = nullptr;
    subMenuButton[0] = new ControlItemSelectionBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.2),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Game Speed",
            "0.5x/0.6x/0.7x/0.8x/0.9x/1.0x/1.1x/1.2x/1.3x/1.4x/1.5x/");
}

SubMenuHardware::~SubMenuHardware() {
    delete label;
    for (int i = 0; i < NUM_CONTROL_ITEMS_HW; i++) delete subMenuButton[i];
}

int SubMenuHardware::getUNIQUEIDENTIFIER() { return UNIQUEIDENTIFIER; }
void SubMenuHardware::setUNIQUEIDENTIFIER(int id) { UNIQUEIDENTIFIER = id; }
GLfloat SubMenuHardware::getXPos() { return this->xPos; }
void SubMenuHardware::setXPos(GLfloat new_xpos) { this->xPos = new_xpos; }
GLfloat SubMenuHardware::getYPos() { return this->yPos; }
void SubMenuHardware::setYPos(GLfloat new_ypos) { this->yPos = new_ypos; }
GLfloat SubMenuHardware::getZPos() { return this->zPos; }
void SubMenuHardware::setZPos(GLfloat new_zpos) { this->zPos = new_zpos; }
GLfloat SubMenuHardware::getRed() { return color[0]; }
void SubMenuHardware::setRed(GLfloat red) { color[0] = red; }
GLfloat SubMenuHardware::getGreen() { return color[1]; }
void SubMenuHardware::setGreen(GLfloat green) { color[1] = green; }
GLfloat SubMenuHardware::getBlue() { return color[2]; }
void SubMenuHardware::setBlue(GLfloat blue) { color[2] = blue; }
GLint SubMenuHardware::getWidth() { return this->width; }
void SubMenuHardware::setWdith(GLint width) { this->width = width; }
GLint SubMenuHardware::getHeight() { return this->height; }
void SubMenuHardware::setHeight(GLint height) { this->height = height; }
std::string SubMenuHardware::getCaption() { return this->caption; }
void SubMenuHardware::setCaption(const std::string& caption) {
    this->caption = caption;
}
GLfloat SubMenuHardware::getPerecentBorder() { return this->percentBorder; }
void SubMenuHardware::setPercentBorder(GLfloat percent) {
    this->percentBorder = percentBorder;
}

void SubMenuHardware::draw() {
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
    glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0], color[1], color[2], color[3]);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
    glEnd();
    label->draw();
    for (int i = 0; i < NUM_CONTROL_ITEMS_HW; i++) {
        if (subMenuButton[i]) {
            subMenuButton[i]->draw();
        }
    }
}

std::string SubMenuHardware::collectData() {
    std::string optionsarray = "/Hardware/";
    for (int x = 0; x < NUM_CONTROL_ITEMS_HW; x++) {
        if (subMenuButton[x]) {
            optionsarray += subMenuButton[x]->collectData();
            optionsarray += "/";
        }
    }
    return optionsarray;
}

void SubMenuHardware::subMenuMouseTest(int x, int y, int button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < NUM_CONTROL_ITEMS_HW;
             button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                            // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                            // CLICKED A ARROW BUTTON???
            if ((x >= subMenuButton[button_i]->getXPos()) &&
                (x <= (subMenuButton[button_i]->getXPos() +
                       subMenuButton[button_i]->getWidth())) &&
                (y <= subMenuButton[button_i]->getYPos()) &&
                (y >= (subMenuButton[button_i]->getYPos() -
                       subMenuButton[button_i]->getHeight()))) {
                subMenuButton[button_i]->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // YOU PRESSED OVER A ARROWBUTTON
                buttonPressed = subMenuButton[button_i];
            }
        }
    } else if (!button_down) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                                // GOES UP
        if (buttonPressed !=
            nullptr) {  // IF YOU MANAGED TO CLICK INSIDE AN ARROW BUTTON
                        // CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if ((x >= buttonPressed->getXPos()) &&
                (x <=
                 (buttonPressed->getXPos() + buttonPressed->getWidth())) &&
                (y <= buttonPressed->getYPos()) &&
                (y >=
                 (buttonPressed->getYPos() - buttonPressed->getHeight()))) {
                buttonPressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                // RELEASE THE MOUSE
            } else {
                buttonPressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                buttonPressed = nullptr;
            }
        }
    }
}
void SubMenuHardware::updateMouse(int x, int y) {}