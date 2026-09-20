#include "ControlItemSelectionBox.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "ControlItem.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

using namespace std;

extern void playSFX(int sfx);

ControlItemSelectionBox::ControlItemSelectionBox() = default;

ControlItemSelectionBox::ControlItemSelectionBox(
        GLfloat xPos,
        GLfloat yPos,
        GLfloat zPos,
        GLfloat red,
        GLfloat green,
        GLfloat blue,
        GLint width,
        GLint height,
        const std::string& caption,
        const std::string& menuString) {
    this->xPos = xPos;
    this->yPos = yPos;
    this->zPos = zPos;
    this->color[0] = red;
    this->color[1] = green;
    this->color[2] = blue;
    this->color[3] = 1.0;
    this->width = width;
    this->height = height;
    this->caption = caption;
    this->menuInfo = menuString;

    // split menuInfo on '/' into allOptions
    std::string current;
    for (char ch : this->menuInfo) {
        if (ch == '/') {
            this->allOptions.push_back(current);
            current.clear();
        } else {
            current += ch;
        }
    }
    this->numberOfOptions = static_cast<int>(this->allOptions.size());

    this->menuState = 0;
    this->buttonState = 0;
    this->optionText = nullptr;
    this->setOptionText(menuState);  // set option to first option
    /*	BUTTON TEXT PLACEMENT	*/
    int realLength = 0;
    for (char ch : this->caption) {
        realLength += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat labelXPos = this->xPos + (this->width / 2) - (realLength / 2);
    GLfloat labelYPos = this->yPos +
                        ((this->yPos - (this->yPos + this->height)) / 2) -
                        this->height / 4;
    /*	END OF BUTTON TEXT PLACEMENT	*/
    this->label = new TextObject(this->caption,
                                 labelXPos,
                                 labelYPos,
                                 this->zPos,
                                 GLUT_BITMAP_TIMES_ROMAN_24,
                                 0.0f,
                                 0.0f,
                                 0.0f);
}

ControlItemSelectionBox::~ControlItemSelectionBox() {
    delete optionText;
    delete label;
}

void ControlItemSelectionBox::draw() {
    // draw main button box
    glBegin(GL_QUADS);
    glColor4f(this->color[0] - 0.2f,
              this->color[1] - 0.2f,
              this->color[2] - 0.2f,
              this->color[3]);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] - 0.2f,
              this->color[1] - 0.2f,
              this->color[2] - 0.2f,
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
    glColor4f(this->color[0] + 0.4f,
              this->color[1] + 0.4f,
              this->color[2] + 0.4f,
              this->color[3]);
    glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] + 0.4f,
              this->color[1] + 0.4f,
              this->color[2] + 0.4f,
              this->color[3]);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
    glEnd();

    this->label->draw();
    this->optionText->draw();

    // draw up arrow
    glBegin(GL_TRIANGLES);
    glColor3f(this->color[0] + .2,
              this->color[1] + .2,
              this->color[2] + .2); /*		3		*/
    glVertex3f(xPos + 0.02 * width,
               (yPos - height / 2) + 0.05 * height,
               zPos + 1); /*	   / \		*/
    glVertex3f(xPos + 0.02 * width + height * 0.7,
               (yPos - height / 2) + 0.05 * height,
               zPos + 1); /*	  /   \		*/
    if (this->buttonState == 1)
        glColor3f(this->color[0] - .2,
                  this->color[1] - .2,
                  this->color[2] - .2); /*   1-----2	*/
    glVertex3f(xPos + 0.02 * (width) + height * 0.35,
               yPos - 0.05 * height,
               zPos + 1);
    glEnd();

    // draw down arrow
    glBegin(GL_TRIANGLES);
    glColor3f(this->color[0] + .2,
              this->color[1] + .2,
              this->color[2] + .2); /*	1-----2		*/
    glVertex3f(xPos + 0.02 * width,
               (yPos - height / 2) - 0.05 * height,
               zPos + 1); /*	 \   /		*/
    glVertex3f(xPos + 0.02 * width + height * 0.7,
               (yPos - height / 2) - 0.05 * height,
               zPos + 1); /*	  \ /		*/
    if (this->buttonState == 2)
        glColor3f(this->color[0] - .2,
                  this->color[1] - .2,
                  this->color[2] - .2); /*	   3		*/
    glVertex3f(xPos + 0.02 * width + height * 0.35,
               yPos - height + 0.05 * height,
               zPos + 1);
    glEnd();
}

GLfloat ControlItemSelectionBox::getXPos() { return this->xPos; }
GLfloat ControlItemSelectionBox::getYPos() { return this->yPos; }
GLfloat ControlItemSelectionBox::getHeight() { return this->height; }
GLfloat ControlItemSelectionBox::getWidth() { return this->width; }
std::string ControlItemSelectionBox::collectData() {
    return this->currentOption;
}

void ControlItemSelectionBox::setOptionText(int index) {
    this->currentOption = this->allOptions[index];
    int realLength = 0;
    for (char ch : this->currentOption) {
        realLength += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat labelXPos =
            this->xPos + this->width - realLength - this->width / 50;
    GLfloat labelYPos = this->yPos +
                        ((this->yPos - (this->yPos + this->height)) / 2) -
                        this->height / 4;
    delete optionText;
    this->optionText = new TextObject(this->currentOption,
                                      labelXPos,
                                      labelYPos,
                                      this->zPos,
                                      GLUT_BITMAP_TIMES_ROMAN_24,
                                      0.0f,
                                      0.0f,
                                      0.0f);
    this->menuState = index;
}
void ControlItemSelectionBox::setOptionText(const std::string& newText) {}

void ControlItemSelectionBox::mouseClickEvent(
        GLint x, GLint y, GLint state, bool stillOverControlItemSelectionBox) {
    // up arrow test
    if ((x >= (this->xPos + 0.02 * (this->width)) &&
         (x <= this->xPos + 0.02 * (this->width) + 0.1 * (this->width))) &&
        ((y <= this->yPos - 3) &&
         (y >= (this->yPos - height / 2) +
                       3))) {  // YOU HAVE CLICKED INSIDE THE UP ARROW
        if (state == 1) {  // IF MOUSE BUTTON DOWN (YOU ARE INSIDE UP ARROW)
            this->buttonState = 1;  // THEN UP ARROW HAS BEEN PRESSED
        } else if (state == 0) {
            if (stillOverControlItemSelectionBox) {  // ONCE YOU RELEASE MOUSE
                                                     // BUTTON
                playSFX(SMALL_CLICK);
                this->menuState++;
                if (this->menuState ==
                    this->numberOfOptions)  // wrap around check
                    this->menuState -= numberOfOptions;
                this->setOptionText(menuState);
            }
            this->buttonState = 0;
        }
    }
    if (((x >= this->xPos + 0.02 * (this->width)) &&
         (x <= this->xPos + 0.02 * (this->width) + 0.1 * (this->width))) &&
        ((y <= (this->yPos - height / 2) - 3) &&
         (y >= (this->yPos - height) +
                       3))) {  // YOU HAVE CLICKED INSIDE THE UP ARROW
        if (state == 1) {  // IF MOUSE BUTTON DOWN (YOU ARE INSIDE UP ARROW)
            this->buttonState = 2;  // THEN UP ARROW HAS BEEN PRESSED
        } else if (state == 0) {
            if (stillOverControlItemSelectionBox) {  // ONCE YOU RELEASE MOUSE
                                                     // BUTTON
                playSFX(SMALL_CLICK);
                this->menuState--;
                if (this->menuState < 0)  // wrap around check
                    this->menuState += numberOfOptions;
                this->setOptionText(menuState);
            }
            this->buttonState = 0;
        }
    }
    if (state == 0) {
        this->buttonState = 0;
    }
}

void ControlItemSelectionBox::updateMouse(int x, int y) {}