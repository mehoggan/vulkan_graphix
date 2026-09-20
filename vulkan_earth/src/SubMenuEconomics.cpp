#include "SubMenuEconomics.h"
#include <cstring>
#include <string>
#include "ControlItem.h"
#include "ControlItemCheckBox.h"
#include "ControlItemSelectionBox.h"
#include "SubMenu.h"
#include "TextObject.h"
#include "macro_crtdbg.h"

using namespace std;

SubMenuEconomics::SubMenuEconomics() = default;

SubMenuEconomics::SubMenuEconomics(int ID,
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
            this->yPos - (0.2 * this->height),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Interest Rate",
            "0.01/0.02/0.05/0.1/");
    subMenuButton[1] = new ControlItemSelectionBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (0.27 * this->height),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Cash at Start",
            "1000/5000/10000/50000/100000/");
    subMenuButton[2] = new ControlItemCheckBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (0.34 * this->height),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Computers Buy");
    subMenuButton[3] = new ControlItemCheckBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (0.41 * this->height),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Free Market");
    subMenuButton[4] = new ControlItemSelectionBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (0.48 * this->height),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Scoring Mode",
            "Weak Sauce/Standard/Mad Crazy/");
}

SubMenuEconomics::~SubMenuEconomics() {
    for (int i = 0; i < NUM_CONTROL_ITEMS_ECON; i++) delete subMenuButton[i];
    delete label;
}

int SubMenuEconomics::getUNIQUEIDENTIFIER() { return UNIQUEIDENTIFIER; }
void SubMenuEconomics::setUNIQUEIDENTIFIER(int ID) { UNIQUEIDENTIFIER = ID; }
GLfloat SubMenuEconomics::getXPos() { return this->xPos; }
void SubMenuEconomics::setXPos(GLfloat newXpos) { this->xPos = newXpos; }
GLfloat SubMenuEconomics::getYPos() { return this->yPos; }
void SubMenuEconomics::setYPos(GLfloat newYpos) { this->yPos = newYpos; }
GLfloat SubMenuEconomics::getZPos() { return this->zPos; }
void SubMenuEconomics::setZPos(GLfloat newZpos) { this->zPos = newZpos; }
GLfloat SubMenuEconomics::getRed() { return color[0]; }
void SubMenuEconomics::setRed(GLfloat red) { color[0] = red; }
GLfloat SubMenuEconomics::getGreen() { return color[1]; }
void SubMenuEconomics::setGreen(GLfloat green) { color[1] = green; }
GLfloat SubMenuEconomics::getBlue() { return color[2]; }
void SubMenuEconomics::setBlue(GLfloat blue) { color[2] = blue; }
GLint SubMenuEconomics::getWidth() { return this->width; }
void SubMenuEconomics::setWdith(GLint width) { this->width = width; }
GLint SubMenuEconomics::getHeight() { return this->height; }
void SubMenuEconomics::setHeight(GLint height) { this->height = height; }
std::string SubMenuEconomics::getCaption() { return this->caption; }
void SubMenuEconomics::setCaption(const std::string& caption) {
    this->caption = caption;
}
GLfloat SubMenuEconomics::getPerecentBorder() { return this->percentBorder; }
void SubMenuEconomics::setPercentBorder(GLfloat percent) {
    this->percentBorder = percentBorder;
}

void SubMenuEconomics::draw() {
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

    for (int i = 0; i < NUM_CONTROL_ITEMS_ECON; i++) {
        if (subMenuButton[i]) {
            subMenuButton[i]->draw();
        }
    }
}

std::string SubMenuEconomics::collectData() {
    std::string optionsarray = "/Economics/";
    for (int x = 0; x < NUM_CONTROL_ITEMS_ECON; x++) {
        if (subMenuButton[x]) {
            optionsarray += subMenuButton[x]->collectData();
            optionsarray += "/";
        }
    }
    return optionsarray;
}

void SubMenuEconomics::subMenuMouseTest(int x, int y, int buttonDown) {
    if (buttonDown) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < NUM_CONTROL_ITEMS_ECON;
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

void SubMenuEconomics::updateMouse(int x, int y) {}