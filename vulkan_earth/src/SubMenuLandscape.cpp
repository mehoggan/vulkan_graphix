#include "SubMenuLandscape.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include "ControlItem.h"
#include "ControlItemButton.h"
#include "ControlItemCheckBox.h"
#include "ControlItemSelectionBox.h"
#include "ControlItemSliderbar.h"
#include "SubMenu.h"
#include "TerrainMaker.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "math.h"
#include "sound.h"

#define PI 3.1415926535898

using namespace std;
extern void playSFX(int sfx);

SubMenuLandscape::SubMenuLandscape() = default;

SubMenuLandscape::SubMenuLandscape(int ID,
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

    this->camX = -4000;
    this->camY = 10000;
    this->camZ = -4000;

    this->tm = new TerrainMaker(100, 256);
    this->tm->prepareData(0, 0, 0, 0, 0);

    this->oldMouseX = -1;
    this->oldMouseY = -1;

    /*	BUTTON TEXT PLACEMENT	*/
    int realLength = 0;
    for (char ch : this->caption) {
        realLength += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
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

    this->subMenuButton[0] = new ControlItemSliderbar(
            this->xPos + (this->width / 2) - (0.48 * this->width),
            this->yPos - (this->height * 0.7),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.085 * (this->height),
            "Smoothness",
            "0/1/2/3/4/5/",
            5);
    this->subMenuButton[1] = new ControlItemSliderbar(
            this->xPos + (this->width / 2) - (0.48 * this->width),
            this->yPos - (this->height * 0.8),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.085 * (this->height),
            "Hill Height",
            "0/1/2/3/4/5/",
            5);
    this->subMenuButton[2] = new ControlItemSliderbar(
            this->xPos + (this->width / 2) - (0.48 * this->width),
            this->yPos - (this->height * 0.9),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.085 * (this->height),
            "Terrain Selection",
            "Rock/Snow/Ice/Mars/Desert/Lava/",
            0);
    this->subMenuButton[3] =
            new ControlItemButton(this,
                                  this->xPos + (0.655 * this->width),
                                  this->yPos - (this->height * 0.91),
                                  this->zPos + 1,
                                  0.75f,
                                  0.0f,
                                  0.0f,
                                  0.3f * this->width,
                                  0.05 * (this->height),
                                  "Sample");
}

SubMenuLandscape::~SubMenuLandscape() {
    delete label;
    delete tm;
    for (int i = 0; i < NUM_CONTROL_ITEMS_LND; i++) delete subMenuButton[i];
}

int SubMenuLandscape::getUNIQUEIDENTIFIER() { return this->UNIQUEIDENTIFIER; }
void SubMenuLandscape::setUNIQUEIDENTIFIER(int ID) {
    this->UNIQUEIDENTIFIER = ID;
}
GLfloat SubMenuLandscape::getXPos() { return this->xPos; }
void SubMenuLandscape::setXPos(GLfloat newXpos) { this->xPos = newXpos; }
GLfloat SubMenuLandscape::getYPos() { return this->yPos; }
void SubMenuLandscape::setYPos(GLfloat newYpos) { this->yPos = newYpos; }
GLfloat SubMenuLandscape::getZPos() { return this->zPos; }
void SubMenuLandscape::setZPos(GLfloat newZpos) { this->zPos = newZpos; }
GLfloat SubMenuLandscape::getRed() { return this->color[0]; }
void SubMenuLandscape::setRed(GLfloat red) { this->color[0] = red; }
GLfloat SubMenuLandscape::getGreen() { return this->color[1]; }
void SubMenuLandscape::setGreen(GLfloat green) { this->color[1] = green; }
GLfloat SubMenuLandscape::getBlue() { return this->color[2]; }
void SubMenuLandscape::setBlue(GLfloat blue) { this->color[2] = blue; }
GLint SubMenuLandscape::getWidth() { return this->width; }
void SubMenuLandscape::setWdith(GLint width) { this->width = width; }
GLint SubMenuLandscape::getHeight() { return this->height; }
void SubMenuLandscape::setHeight(GLint height) { this->height = height; }
std::string SubMenuLandscape::getCaption() { return this->caption; }
void SubMenuLandscape::setCaption(const std::string& caption) {
    this->caption = caption;
}
GLfloat SubMenuLandscape::getPerecentBorder() { return this->percentBorder; }
void SubMenuLandscape::setPercentBorder(GLfloat percent) {
    this->percentBorder = percentBorder;
}

void SubMenuLandscape::draw() {
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
    for (int i = 0; i < NUM_CONTROL_ITEMS_LND; i++) {
        if (this->subMenuButton[i]) {
            this->subMenuButton[i]->draw();
        }
    }

    GLfloat borderX = this->xPos + 0.03 * this->width;
    GLfloat borderY = this->yPos - 0.07 * this->height;

    // top-left
    glBegin(GL_QUADS);
    glColor4f(this->color[0] - .2,
              this->color[1] - .2,
              this->color[2] - .2,
              this->color[3]);
    glVertex3f(borderX, borderY, this->zPos + 1);
    glVertex3f(borderX - 3, borderY + 3, this->zPos + 1);
    glVertex3f(borderX + 0.936 * width + 3, borderY + 3, this->zPos + 1);
    glVertex3f(borderX + 0.936 * width, borderY, this->zPos + 1);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] - .2,
              this->color[1] - .2,
              this->color[2] - .2,
              this->color[3]);
    glVertex3f(borderX - 3, borderY + 3, this->zPos + 1);
    glVertex3f(borderX - 3, borderY - 0.597 * height - 3, this->zPos + 1);
    glVertex3f(borderX, borderY - 0.597 * height, this->zPos + 1);
    glVertex3f(borderX, borderY, this->zPos + 1);
    glEnd();

    // bottom-right
    glBegin(GL_QUADS);
    glColor4f(this->color[0] + .4,
              this->color[1] + .4,
              this->color[2] + .4,
              this->color[3]);
    glVertex3f(borderX - 3, borderY - 0.597 * height - 3, this->zPos + 1);
    glVertex3f(borderX + 0.936 * width + 3,
               borderY - 0.597 * height - 3,
               this->zPos + 1);
    glVertex3f(
            borderX + 0.936 * width, borderY - 0.597 * height, this->zPos + 1);
    glVertex3f(borderX, borderY - 0.597 * height, this->zPos + 1);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] + .4,
              this->color[1] + .4,
              this->color[2] + .4,
              this->color[3]);
    glVertex3f(borderX + 0.936 * width, borderY, this->zPos + 1);
    glVertex3f(borderX + 0.936 * width + 3, borderY + 3, this->zPos + 1);
    glVertex3f(borderX + 0.936 * width + 3,
               borderY - 0.597 * height - 3,
               this->zPos + 1);
    glVertex3f(borderX + 0.936 * width,
               borderY + -0.597 * height,
               this->zPos + 1);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    // glPushMatrix();
    glLoadIdentity();
    glViewport(this->xPos + 0.8 * this->width,
               this->yPos,
               (0.9417 * width),
               (0.6 * height));
    gluPerspective(45.0,
                   ((0.9417 * this->width) / (0.6 * this->height)),
                   1,
                   199999999);
    // glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    // glPushMatrix();
    glLoadIdentity();
    glScissor(this->xPos + 0.8 * this->width,
              this->yPos,
              (0.9417 * width),
              (0.6 * height));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gluLookAt(camX,
              camY,
              camZ,
              (this->tm->getActualSize() / 2.0),
              0.0f,
              (this->tm->getActualSize() / 2.0),
              0.0f,
              1.0f,
              0.0f);
    this->tm->draw();
    // glPopMatrix();

    int winWidth = glutGet(GLUT_WINDOW_WIDTH);
    int winHeight = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, winWidth, winHeight);
    gluPerspective(
            60.0,
            static_cast<float>(winWidth) / static_cast<float>(winHeight),
            1.0,
            1000000.0);

    glMatrixMode(GL_MODELVIEW);
    glScissor(0, 0, winWidth, winHeight);
    glLoadIdentity();
}

std::string SubMenuLandscape::collectData() {
    std::string optionsarray = "/Landscape/";
    for (int x = 0; x < NUM_CONTROL_ITEMS_LND; x++) {
        if (this->subMenuButton[x]) {
            optionsarray += this->subMenuButton[x]->collectData();
            optionsarray += "/";
        }
    }
    return optionsarray;
}

void SubMenuLandscape::subMenuMouseTest(int x, int y, int buttonDown) {
    if (buttonDown) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < NUM_CONTROL_ITEMS_LND;
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
                this->numberpressed = button_i;
            }
        }
        this->oldMouseX = x;
        this->oldMouseY = y;
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
                if (this->numberpressed == PREVIEW_BUTTON) {
                    // RIGHT NOW THERE ARE ONLY 3 OPTIONS AND subMenuButton[3]
                    // IS THE BUTTON ITSELF
                    /********************************************************************************/
                    /*		Smoothness		--			out1,i1
                     */
                    /*		Hill Height		--			out2,i2
                     */
                    /*		Terrain Texture	--			out3,i3
                     */
                    /********************************************************************************/
                    stringstream ss1(this->subMenuButton[0]->collectData());
                    int i1;
                    if (!(ss1 >> i1)) i1 = 0;
                    stringstream ss2(this->subMenuButton[1]->collectData());
                    int i2;
                    if (!(ss2 >> i2)) i2 = 0;
                    this->tm->prepareData(2500,          // int steps
                                          i2 * i2 + 10,  // int increase
                                          30,            // float radius
                                          5,    // int randomJump % (1-100)
                                          i1);  // int smoothness
                    this->tm->selectTexture(
                            this->subMenuButton[2]->collectData());
                    this->buttonPressed->mouseClickEvent(
                            x, y, buttonDown, false);
                    this->numberpressed = -1;
                    this->buttonPressed = nullptr;
                    playSFX(SMALL_CLICK);
                } else {
                    this->buttonPressed->mouseClickEvent(
                            x,
                            y,
                            buttonDown,
                            true);  // IF YOU ARE THEN TELL THE ARROW BUTTON
                                    // YOU RELEASE THE MOUSE
                    this->numberpressed = -1;
                    this->buttonPressed = nullptr;
                }
            } else {
                this->buttonPressed->mouseClickEvent(
                        x,
                        y,
                        buttonDown,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                this->buttonPressed = nullptr;
                this->numberpressed = -1;
            }
        }
        this->oldMouseX = -1;
        this->oldMouseY = -1;
    }
}

void SubMenuLandscape::updateMouse(int x, int y) {
    if (((x >= this->xPos + 0.8 * this->width) &&
         (x <= this->xPos + 0.8 * this->width + (0.9417 * this->width))) &&
        ((y >= this->yPos - 0.15 * this->height) &&
         (y <= this->yPos - 0.15 * this->height + (0.6 * this->height)))) {
        float newCamX = camX, newCamY = camY, newCamZ = camZ;
        if (x < oldMouseX) {
            newCamX = (camX - (this->tm->getActualSize() / 2.0)) *
                              cos(-PI / 180) -
                      (camZ - (this->tm->getActualSize() / 2.0)) *
                              sin(-PI / 180) +
                      (this->tm->getActualSize() / 2.0);
            newCamZ = (camX - (this->tm->getActualSize() / 2.0)) *
                              sin(-PI / 180) +
                      (camZ - (this->tm->getActualSize() / 2.0)) *
                              cos(-PI / 180) +
                      (this->tm->getActualSize() / 2.0);
        }
        if (x > oldMouseX) {
            newCamX = (camX - (this->tm->getActualSize() / 2.0)) *
                              cos(PI / 180) -
                      (camZ - (this->tm->getActualSize() / 2.0)) *
                              sin(PI / 180) +
                      (this->tm->getActualSize() / 2.0);
            newCamZ = (camX - (this->tm->getActualSize() / 2.0)) *
                              sin(PI / 180) +
                      (camZ - (this->tm->getActualSize() / 2.0)) *
                              cos(PI / 180) +
                      (this->tm->getActualSize() / 2.0);
        }
        if (y < oldMouseY) {
        }
        if (y > oldMouseY) {
        }
        camX = newCamX;
        camY = newCamY;
        camZ = newCamZ;
        oldMouseX = x;
        oldMouseY = y;
    }
    int winWidth = glutGet(GLUT_WINDOW_WIDTH);
    int winHeight = glutGet(GLUT_WINDOW_HEIGHT);
    this->subMenuButton[0]->updateMouse(x - (winWidth / 2),
                                        (winHeight / 2) - y);
    this->subMenuButton[1]->updateMouse(x - (winWidth / 2),
                                        (winHeight / 2) - y);
    this->subMenuButton[2]->updateMouse(x - (winWidth / 2),
                                        (winHeight / 2) - y);
}
