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

SubMenuWeapons::SubMenuWeapons(int ID,
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
            "Arms Level",
            "1/2/3/4/5/6/7/8/9/10/");
    this->subMenuButton[1] = new ControlItemSelectionBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.27),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Bomb Icon",
            "Small/Medium/Large/");
    this->subMenuButton[2] = new ControlItemCheckBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.34),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Tunneling");
    this->subMenuButton[3] = new ControlItemSelectionBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.41),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Scale",
            "Small/Medium/Large/");
    this->subMenuButton[4] = new ControlItemCheckBox(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.48),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.06 * (this->height),
            "Trace Path");
}

SubMenuWeapons::~SubMenuWeapons() {
    delete label;
    for (int i = 0; i < NUM_CONTROL_ITEMS_WPN; i++) delete subMenuButton[i];
}

int SubMenuWeapons::getUNIQUEIDENTIFIER() { return this->UNIQUEIDENTIFIER; }
void SubMenuWeapons::setUNIQUEIDENTIFIER(int ID) {
    this->UNIQUEIDENTIFIER = ID;
}
GLfloat SubMenuWeapons::getXPos() { return this->xPos; }
void SubMenuWeapons::setXPos(GLfloat newXpos) { this->xPos = newXpos; }
GLfloat SubMenuWeapons::getYPos() { return this->yPos; }
void SubMenuWeapons::setYPos(GLfloat newYpos) { this->yPos = newYpos; }
GLfloat SubMenuWeapons::getZPos() { return this->zPos; }
void SubMenuWeapons::setZPos(GLfloat newZpos) { this->zPos = newZpos; }
GLfloat SubMenuWeapons::getRed() { return this->color[0]; }
void SubMenuWeapons::setRed(GLfloat red) { this->color[0] = red; }
GLfloat SubMenuWeapons::getGreen() { return this->color[1]; }
void SubMenuWeapons::setGreen(GLfloat green) { this->color[1] = green; }
GLfloat SubMenuWeapons::getBlue() { return this->color[2]; }
void SubMenuWeapons::setBlue(GLfloat blue) { this->color[2] = blue; }
GLint SubMenuWeapons::getWidth() { return this->width; }
void SubMenuWeapons::setWdith(GLint width) { this->width = width; }
GLint SubMenuWeapons::getHeight() { return this->height; }
void SubMenuWeapons::setHeight(GLint height) { this->height = height; }
char* SubMenuWeapons::getCaption() { return this->caption; }
void SubMenuWeapons::setCaption(char* caption) { this->caption = caption; }
GLfloat SubMenuWeapons::getPerecentBorder() { return this->percentBorder; }
void SubMenuWeapons::setPercentBorder(GLfloat percent) {
    this->percentBorder = percentBorder;
}

void SubMenuWeapons::draw() {
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
    for (int i = 0; i < NUM_CONTROL_ITEMS_WPN; i++) {
        if (this->subMenuButton[i]) {
            this->subMenuButton[i]->draw();
        }
    }
}

const char* SubMenuWeapons::collectData() {
    char optionsarray[512];
    memset(optionsarray, 0, 512);
    strcat(optionsarray, "/Weapons/");
    for (int x = 0; x < NUM_CONTROL_ITEMS_WPN; x++) {
        if (this->subMenuButton[x]) {
            strcat(optionsarray, this->subMenuButton[x]->collectData());
            strcat(optionsarray, "/");
        }
    }
    const char* realretrn = optionsarray;
    return realretrn;
}

void SubMenuWeapons::subMenuMouseTest(int x, int y, int buttonDown) {
    if (buttonDown) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < NUM_CONTROL_ITEMS_WPN;
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

void SubMenuWeapons::updateMouse(int x, int y) {}