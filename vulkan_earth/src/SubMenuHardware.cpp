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

SubMenuHardware::SubMenuHardware(int ID,
                                 GLfloat xPos,
                                 GLfloat yPos,
                                 GLfloat zPos,
                                 GLfloat red,
                                 GLfloat green,
                                 GLfloat blue,
                                 GLint width,
                                 GLint height,
                                 char* caption,
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
    const char* cpchar = this->caption;
    int length = strlen(cpchar);
    char* pchar = this->caption;
    int realLength = 0;
    for (int i = 0; i < length; i++) {
        realLength += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, *(pchar));
        *(pchar)++;
    }
    GLfloat labelXPos = this->xPos + ((this->width) / 2) - (realLength / 2);
    GLfloat labelYPos = this->yPos - this->height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    this->label = new TextObject(this->caption,
                                 labelXPos,
                                 labelYPos,
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
            "Game Speed",
            "0.5x/0.6x/0.7x/0.8x/0.9x/1.0x/1.1x/1.2x/1.3x/1.4x/1.5x/");
}

SubMenuHardware::~SubMenuHardware() {
    delete label;
    for (int i = 0; i < NUM_CONTROL_ITEMS_HW; i++) delete subMenuButton[i];
}

int SubMenuHardware::getUNIQUEIDENTIFIER() { return this->UNIQUEIDENTIFIER; }
void SubMenuHardware::setUNIQUEIDENTIFIER(int ID) {
    this->UNIQUEIDENTIFIER = ID;
}
GLfloat SubMenuHardware::getXPos() { return this->xPos; }
void SubMenuHardware::setXPos(GLfloat newXpos) { this->xPos = newXpos; }
GLfloat SubMenuHardware::getYPos() { return this->yPos; }
void SubMenuHardware::setYPos(GLfloat newYpos) { this->yPos = newYpos; }
GLfloat SubMenuHardware::getZPos() { return this->zPos; }
void SubMenuHardware::setZPos(GLfloat newZpos) { this->zPos = newZpos; }
GLfloat SubMenuHardware::getRed() { return this->color[0]; }
void SubMenuHardware::setRed(GLfloat red) { this->color[0] = red; }
GLfloat SubMenuHardware::getGreen() { return this->color[1]; }
void SubMenuHardware::setGreen(GLfloat green) { this->color[1] = green; }
GLfloat SubMenuHardware::getBlue() { return this->color[2]; }
void SubMenuHardware::setBlue(GLfloat blue) { this->color[2] = blue; }
GLint SubMenuHardware::getWidth() { return this->width; }
void SubMenuHardware::setWdith(GLint width) { this->width = width; }
GLint SubMenuHardware::getHeight() { return this->height; }
void SubMenuHardware::setHeight(GLint height) { this->height = height; }
char* SubMenuHardware::getCaption() { return this->caption; }
void SubMenuHardware::setCaption(char* caption) { this->caption = caption; }
GLfloat SubMenuHardware::getPerecentBorder() { return this->percentBorder; }
void SubMenuHardware::setPercentBorder(GLfloat percent) {
    this->percentBorder = percentBorder;
}

void SubMenuHardware::draw() {
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
    for (int i = 0; i < NUM_CONTROL_ITEMS_HW; i++) {
        if (this->subMenuButton[i]) {
            this->subMenuButton[i]->draw();
        }
    }
}

const char* SubMenuHardware::collectData() {
    char optionsarray[512];
    memset(optionsarray, 0, 512);
    strcat(optionsarray, "/Hardware/");
    for (int x = 0; x < NUM_CONTROL_ITEMS_HW; x++) {
        if (this->subMenuButton[x]) {
            strcat(optionsarray, this->subMenuButton[x]->collectData());
            strcat(optionsarray, "/");
        }
    }
    const char* realretrn = optionsarray;
    return realretrn;
}

void SubMenuHardware::subMenuMouseTest(int x, int y, int buttonDown) {
    if (buttonDown) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < NUM_CONTROL_ITEMS_HW;
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
void SubMenuHardware::updateMouse(int x, int y) {}