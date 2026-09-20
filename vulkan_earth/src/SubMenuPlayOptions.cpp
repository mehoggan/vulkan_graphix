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

SubMenuPlayOptions::SubMenuPlayOptions(int ID,
                                       GLfloat xPos,
                                       GLfloat yPos,
                                       GLfloat zPos,
                                       GLfloat red,
                                       GLfloat green,
                                       GLfloat blue,
                                       GLint width,
                                       GLint height,
                                       const std::string& caption,
                                       GLfloat percentBorder) {
    UNIQUEIDENTIFIER = ID;
    this->xPos = xPos;
    this->yPos = yPos;
    this->zPos = zPos;
    this->percentBorder = percentBorder;
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
            "Teams",
            "Allowed/Random/Not Allowed/");
    subMenuButton[1] = new ControlItemCheckBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.27),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Status Bar");
    subMenuButton[2] = new ControlItemSelectionBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.34),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Play Order",
            "Sequential/Random/");
    subMenuButton[3] = new ControlItemCheckBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.41),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Fast Computers");
    subMenuButton[4] = new ControlItemSelectionBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.48),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Talking Tanks",
            "Yes/No/");
    subMenuButton[5] = new ControlItemSelectionBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.55),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Talk Probability",
            "0.1/0.2/0.3/0.4/0.5/0.6/0.7/0.8/0.9/1.0/");
}

SubMenuPlayOptions::~SubMenuPlayOptions() {
    for (int i = 0; i < NUM_CONTROL_ITEMS_PO; i++) delete subMenuButton[i];
    delete label;
}

int SubMenuPlayOptions::getUNIQUEIDENTIFIER() { return UNIQUEIDENTIFIER; }
void SubMenuPlayOptions::setUNIQUEIDENTIFIER(int ID) { UNIQUEIDENTIFIER = ID; }
GLfloat SubMenuPlayOptions::getXPos() { return this->xPos; }
void SubMenuPlayOptions::setXPos(GLfloat newXpos) { this->xPos = newXpos; }
GLfloat SubMenuPlayOptions::getYPos() { return this->yPos; }
void SubMenuPlayOptions::setYPos(GLfloat newYpos) { this->yPos = newYpos; }
GLfloat SubMenuPlayOptions::getZPos() { return this->zPos; }
void SubMenuPlayOptions::setZPos(GLfloat newZpos) { this->zPos = newZpos; }
GLfloat SubMenuPlayOptions::getRed() { return color[0]; }
void SubMenuPlayOptions::setRed(GLfloat red) { color[0] = red; }
GLfloat SubMenuPlayOptions::getGreen() { return color[1]; }
void SubMenuPlayOptions::setGreen(GLfloat green) { color[1] = green; }
GLfloat SubMenuPlayOptions::getBlue() { return color[2]; }
void SubMenuPlayOptions::setBlue(GLfloat blue) { color[2] = blue; }
GLint SubMenuPlayOptions::getWidth() { return this->width; }
void SubMenuPlayOptions::setWdith(GLint width) { this->width = width; }
GLint SubMenuPlayOptions::getHeight() { return this->height; }
void SubMenuPlayOptions::setHeight(GLint height) { this->height = height; }
std::string SubMenuPlayOptions::getCaption() { return this->caption; }
void SubMenuPlayOptions::setCaption(const std::string& caption) {
    this->caption = caption;
}
GLfloat SubMenuPlayOptions::getPerecentBorder() { return this->percentBorder; }
void SubMenuPlayOptions::setPercentBorder(GLfloat percent) {
    this->percentBorder = percentBorder;
}

void SubMenuPlayOptions::draw() {
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
    for (int i = 0; i < NUM_CONTROL_ITEMS_PO; i++) {
        if (subMenuButton[i]) {
            subMenuButton[i]->draw();
        }
    }
}

std::string SubMenuPlayOptions::collectData() {
    std::string optionsarray = "/Game Options/";
    for (int x = 0; x < NUM_CONTROL_ITEMS_PO; x++) {
        if (subMenuButton[x]) {
            optionsarray += subMenuButton[x]->collectData();
            optionsarray += "/";
        }
    }
    return optionsarray;
}

void SubMenuPlayOptions::subMenuMouseTest(int x, int y, int buttonDown) {
    if (buttonDown) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < NUM_CONTROL_ITEMS_PO;
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
                        buttonDown,
                        true);  // YOU PRESSED OVER A ARROWBUTTON
                buttonPressed = subMenuButton[button_i];
            }
        }
    } else if (!buttonDown) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
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
                        buttonDown,
                        true);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                // RELEASE THE MOUSE
            } else {
                buttonPressed->mouseClickEvent(
                        x,
                        y,
                        buttonDown,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                buttonPressed = nullptr;
            }
        }
    }
}

void SubMenuPlayOptions::updateMouse(int x, int y) {}