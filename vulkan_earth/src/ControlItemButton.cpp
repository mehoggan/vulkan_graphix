#include "ControlItemButton.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include "ControlItem.h"
#include "SubMenu.h"
#include "SubMenuLandscape.h"
#include "TerrainMaker.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

using namespace std;

extern void playSFX(int sfx);

ControlItemButton::ControlItemButton() = default;

ControlItemButton::ControlItemButton(SubMenuLandscape* parent,
                                     GLfloat xPos,
                                     GLfloat yPos,
                                     GLfloat zPos,
                                     GLfloat red,
                                     GLfloat green,
                                     GLfloat blue,
                                     GLint width,
                                     GLint height,
                                     const std::string& caption) {
    this->parent = parent;

    this->xPos = xPos;
    this->yPos = yPos;
    this->zPos = zPos;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    this->width = width;
    this->height = height;
    this->caption = caption;

    toggled = false;
    buttonState = 0;
    menuState = 0;

    /*	BUTTON TEXT PLACEMENT	*/
    int real_length = 0;
    for (char ch : this->caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = this->xPos + ((this->width) / 2) - (real_length / 2);
    GLfloat label_y_pos = this->yPos +
                          ((this->yPos - (this->yPos + this->height)) / 2) -
                          this->height / 4;
    /*	END OF BUTTON TEXT PLACEMENT	*/
    label = new TextObject(this->caption,
                           label_x_pos,
                           label_y_pos,
                           this->zPos,
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
}

ControlItemButton::~ControlItemButton() { delete label; }

void ControlItemButton::draw() {
    glPushMatrix();
    if (buttonState) {
        glPushMatrix();
        glBegin(GL_QUADS);
        glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
        glVertex3f(this->xPos, this->yPos, this->zPos);
        glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
        glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
        glVertex3f(this->xPos + width, this->yPos, this->zPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
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
        glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
        glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
        glVertex3f(
                this->xPos + width + 3, this->yPos - height - 3, this->zPos);
        glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
        glVertex3f(this->xPos, this->yPos - height, this->zPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
        glVertex3f(this->xPos + width, this->yPos, this->zPos);
        glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
        glVertex3f(
                this->xPos + width + 3, this->yPos - height - 3, this->zPos);
        glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
        glEnd();
        glPopMatrix();
    } else {
        glPushMatrix();
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
        glVertex3f(
                this->xPos + width + 3, this->yPos - height - 3, this->zPos);
        glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
        glVertex3f(this->xPos, this->yPos - height, this->zPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
        glVertex3f(this->xPos + width, this->yPos, this->zPos);
        glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
        glVertex3f(
                this->xPos + width + 3, this->yPos - height - 3, this->zPos);
        glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
        glEnd();
        glPopMatrix();
    }
    label->draw();
    glPopMatrix();
}

GLfloat ControlItemButton::getXPos() { return this->xPos; }
GLfloat ControlItemButton::getYPos() { return this->yPos; }
GLfloat ControlItemButton::getHeight() { return this->height; }
GLfloat ControlItemButton::getWidth() { return this->width; }
bool ControlItemButton::isToggled() { return toggled; }
void ControlItemButton::updateButtonState() {
    if (toggled)
        buttonState = 1;
    else
        buttonState = 0;
}
void ControlItemButton::setToggled(bool t) { toggled = t; }
void ControlItemButton::setOptionText(int index) {}
void ControlItemButton::setOptionText(const std::string& newText) {}
std::string ControlItemButton::collectData() { return "Button"; }

void ControlItemButton::mouseClickEvent(GLint x,
                                        GLint y,
                                        GLint state,
                                        bool stillOverControlItemButton) {
    if (state) {
        if ((x >= (this->xPos) && x <= ((this->xPos) + (this->width))) &&
            (y <= (this->yPos) &&
             y >= ((this->yPos) -
                   (this->height)))) {  // This if statement -->
                                        // stillOverControlItemButton
            buttonState = 1;
            toggled = true;
        } else {
            buttonState = 0;
        }
    } else {
        buttonState = 0;
    }
}

void ControlItemButton::updateMouse(int x, int y) {}
