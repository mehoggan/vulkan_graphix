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
                                     GLfloat x_pos,
                                     GLfloat y_pos,
                                     GLfloat z_pos,
                                     GLfloat red,
                                     GLfloat green,
                                     GLfloat blue,
                                     GLint width,
                                     GLint height,
                                     const std::string& caption) {
    this->parent = parent;

    this->x_pos = x_pos;
    this->y_pos = y_pos;
    this->z_pos = z_pos;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    this->width = width;
    this->height = height;
    this->caption = caption;

    toggled = false;
    button_state = 0;
    menu_state = 0;

    /*	BUTTON TEXT PLACEMENT	*/
    int real_length = 0;
    for (char ch : this->caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos =
            this->x_pos + ((this->width) / 2) - (real_length / 2);
    GLfloat label_y_pos = this->y_pos +
                          ((this->y_pos - (this->y_pos + this->height)) / 2) -
                          this->height / 4;
    /*	END OF BUTTON TEXT PLACEMENT	*/
    label = new TextObject(this->caption,
                           label_x_pos,
                           label_y_pos,
                           this->z_pos,
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
}

ControlItemButton::~ControlItemButton() { delete label; }

void ControlItemButton::draw() {
    glPushMatrix();
    if (button_state) {
        glPushMatrix();
        glBegin(GL_QUADS);
        glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
        glVertex3f(this->x_pos, this->y_pos, this->z_pos);
        glVertex3f(this->x_pos - 3, this->y_pos + 3, this->z_pos);
        glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
        glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
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
        glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
        glVertex3f(this->x_pos - 3, this->y_pos - height - 3, this->z_pos);
        glVertex3f(this->x_pos + width + 3,
                   this->y_pos - height - 3,
                   this->z_pos);
        glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos);
        glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
        glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
        glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
        glVertex3f(this->x_pos + width + 3,
                   this->y_pos - height - 3,
                   this->z_pos);
        glVertex3f(this->x_pos + width, this->y_pos + -height, this->z_pos);
        glEnd();
        glPopMatrix();
    } else {
        glPushMatrix();
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
        glVertex3f(this->x_pos + width + 3,
                   this->y_pos - height - 3,
                   this->z_pos);
        glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos);
        glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
        glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
        glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
        glVertex3f(this->x_pos + width + 3,
                   this->y_pos - height - 3,
                   this->z_pos);
        glVertex3f(this->x_pos + width, this->y_pos + -height, this->z_pos);
        glEnd();
        glPopMatrix();
    }
    label->draw();
    glPopMatrix();
}

GLfloat ControlItemButton::getXPos() { return this->x_pos; }
GLfloat ControlItemButton::getYPos() { return this->y_pos; }
GLfloat ControlItemButton::getHeight() { return this->height; }
GLfloat ControlItemButton::getWidth() { return this->width; }
bool ControlItemButton::isToggled() { return toggled; }
void ControlItemButton::updateButtonState() {
    if (toggled)
        button_state = 1;
    else
        button_state = 0;
}
void ControlItemButton::setToggled(bool t) { toggled = t; }
void ControlItemButton::setOptionText(int index) {}
void ControlItemButton::setOptionText(const std::string& new_text) {}
std::string ControlItemButton::collectData() { return "Button"; }

void ControlItemButton::mouseClickEvent(GLint x,
                                        GLint y,
                                        GLint state,
                                        bool still_over_control_item_button) {
    if (state) {
        if ((x >= (this->x_pos) && x <= ((this->x_pos) + (this->width))) &&
            (y <= (this->y_pos) &&
             y >= ((this->y_pos) -
                   (this->height)))) {  // This if statement -->
                                        // stillOverControlItemButton
            button_state = 1;
            toggled = true;
        } else {
            button_state = 0;
        }
    } else {
        button_state = 0;
    }
}

void ControlItemButton::updateMouse(int x, int y) {}
