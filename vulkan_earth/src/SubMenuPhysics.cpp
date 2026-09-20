#include "SubMenuPhysics.h"
#include <cstring>
#include <string>
#include "ControlItem.h"
#include "ControlItemCheckBox.h"
#include "ControlItemSelectionBox.h"
#include "SubMenu.h"
#include "TextObject.h"
#include "macro_crtdbg.h"

using namespace std;

SubMenuPhysics::SubMenuPhysics() = default;

SubMenuPhysics::SubMenuPhysics(int ID,
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
    this->UNIQUEIDENTIFIER = ID;
    this->xPos = xPos;
    this->yPos = yPos;
    this->zPos = zPos;
    this->percentBorder = percentBorder;
    this->color[0] = red;
    this->color[1] = green;
    this->color[2] = blue;
    this->color[3] = 1.0;
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

    this->label = new TextObject(this->caption,
                                 label_x_pos,
                                 label_y_pos,
                                 (this->zPos + 1),
                                 GLUT_BITMAP_TIMES_ROMAN_24,
                                 0.0f,
                                 0.0f,
                                 0.0f);
    this->buttonPressed = nullptr;
    this->subMenuButton[0] = new ControlItemSelectionBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.2),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Air Viscosity",
            "Low/Medium/High/");
    this->subMenuButton[1] = new ControlItemSelectionBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.27),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Gravity",
            "0.1/0.2/0.5/1.0/1.5/2.0/");
    this->subMenuButton[2] = new ControlItemCheckBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.34),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Tanks Fall");
}

SubMenuPhysics::~SubMenuPhysics() {
    for (int i = 0; i < NUM_CONTROL_ITEMS_PHY; i++) delete subMenuButton[i];
    delete label;
}

int SubMenuPhysics::getUNIQUEIDENTIFIER() { return this->UNIQUEIDENTIFIER; }
void SubMenuPhysics::setUNIQUEIDENTIFIER(int ID) {
    this->UNIQUEIDENTIFIER = ID;
}
GLfloat SubMenuPhysics::getXPos() { return this->xPos; }
void SubMenuPhysics::setXPos(GLfloat newXpos) { this->xPos = newXpos; }
GLfloat SubMenuPhysics::getYPos() { return this->yPos; }
void SubMenuPhysics::setYPos(GLfloat newYpos) { this->yPos = newYpos; }
GLfloat SubMenuPhysics::getZPos() { return this->zPos; }
void SubMenuPhysics::setZPos(GLfloat newZpos) { this->zPos = newZpos; }
GLfloat SubMenuPhysics::getRed() { return this->color[0]; }
void SubMenuPhysics::setRed(GLfloat red) { this->color[0] = red; }
GLfloat SubMenuPhysics::getGreen() { return this->color[1]; }
void SubMenuPhysics::setGreen(GLfloat green) { this->color[1] = green; }
GLfloat SubMenuPhysics::getBlue() { return this->color[2]; }
void SubMenuPhysics::setBlue(GLfloat blue) { this->color[2] = blue; }
GLint SubMenuPhysics::getWidth() { return this->width; }
void SubMenuPhysics::setWdith(GLint width) { this->width = width; }
GLint SubMenuPhysics::getHeight() { return this->height; }
void SubMenuPhysics::setHeight(GLint height) { this->height = height; }
std::string SubMenuPhysics::getCaption() { return this->caption; }
void SubMenuPhysics::setCaption(const std::string& caption) {
    this->caption = caption;
}
GLfloat SubMenuPhysics::getPerecentBorder() { return this->percentBorder; }
void SubMenuPhysics::setPercentBorder(GLfloat percent) {
    this->percentBorder = percentBorder;
}

void SubMenuPhysics::draw() {
    glBegin(GL_QUADS);
    glColor4f(this->color[0] + .2,
              this->color[1] + .2,
              this->color[2] + .2,
              this->color[3]);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] + .2,
              this->color[1] + .2,
              this->color[2] + .2,
              this->color[3]);
    glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0], this->color[1], this->color[2], this->color[3]);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] - .4,
              this->color[1] - .4,
              this->color[2] - .4,
              this->color[3]);
    glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] - .4,
              this->color[1] - .4,
              this->color[2] - .4,
              this->color[3]);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
    glEnd();
    this->label->draw();
    for (int i = 0; i < NUM_CONTROL_ITEMS_PHY; i++) {
        if (this->subMenuButton[i]) {
            this->subMenuButton[i]->draw();
        }
    }
}

std::string SubMenuPhysics::collectData() {
    std::string optionsarray = "/Physics/";
    for (int x = 0; x < NUM_CONTROL_ITEMS_PHY; x++) {
        if (this->subMenuButton[x]) {
            optionsarray += this->subMenuButton[x]->collectData();
            optionsarray += "/";
        }
    }
    return optionsarray;
}

void SubMenuPhysics::subMenuMouseTest(int x, int y, int buttonDown) {
    if (buttonDown) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < NUM_CONTROL_ITEMS_PHY;
             button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                            // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                            // CLICKED A ARROW BUTTON???
            if ((x >= this->subMenuButton[button_i]->getXPos()) &&
                (x <= (this->subMenuButton[button_i]->getXPos() +
                       this->subMenuButton[button_i]->getWidth())) &&
                (y <= this->subMenuButton[button_i]->getYPos()) &&
                (y >= (this->subMenuButton[button_i]->getYPos() -
                       this->subMenuButton[button_i]->getHeight()))) {
                this->subMenuButton[button_i]->mouseClickEvent(
                        x,
                        y,
                        buttonDown,
                        true);  // YOU PRESSED OVER A ARROWBUTTON
                this->buttonPressed = this->subMenuButton[button_i];
            }
        }
    } else if (!buttonDown) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                               // GOES UP
        if (this->buttonPressed !=
            nullptr) {  // IF YOU MANAGED TO CLICK INSIDE AN ARROW BUTTON
                        // CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if ((x >= this->buttonPressed->getXPos()) &&
                (x <= (this->buttonPressed->getXPos() +
                       this->buttonPressed->getWidth())) &&
                (y <= this->buttonPressed->getYPos()) &&
                (y >= (this->buttonPressed->getYPos() -
                       this->buttonPressed->getHeight()))) {
                this->buttonPressed->mouseClickEvent(
                        x,
                        y,
                        buttonDown,
                        true);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                // RELEASE THE MOUSE
            } else {
                this->buttonPressed->mouseClickEvent(
                        x,
                        y,
                        buttonDown,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                this->buttonPressed = nullptr;
            }
        }
    }
}

void SubMenuPhysics::updateMouse(int x, int y) {}