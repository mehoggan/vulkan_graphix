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
#include "MacroCrtdbg.h"
#include "Sound.h"

using namespace std;

extern void playSFX(int sfx);

ControlItemButton::ControlItemButton() = default;

ControlItemButton::ControlItemButton(SubMenuLandscape* new_parent,
                                     GLfloat new_x_pos,
                                     GLfloat new_y_pos,
                                     GLfloat new_z_pos,
                                     GLfloat red,
                                     GLfloat green,
                                     GLfloat blue,
                                     GLint new_width,
                                     GLint new_height,
                                     const std::string& new_caption) {
    parent = new_parent;

    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    width = new_width;
    height = new_height;
    caption = new_caption;

    toggled = false;
    button_state = 0;
    menu_state = 0;

    /*	BUTTON TEXT PLACEMENT	*/
    int real_length = 0;
    for (char ch : caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = x_pos + ((width) / 2) - (real_length / 2);
    GLfloat label_y_pos =
            y_pos + ((y_pos - (y_pos + height)) / 2) - height / 4;
    /*	END OF BUTTON TEXT PLACEMENT	*/
    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           z_pos,
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
        glVertex3f(x_pos, y_pos, z_pos);
        glVertex3f(x_pos - 3, y_pos + 3, z_pos);
        glVertex3f(x_pos + width + 3, y_pos + 3, z_pos);
        glVertex3f(x_pos + width, y_pos, z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
        glVertex3f(x_pos - 3, y_pos + 3, z_pos);
        glVertex3f(x_pos - 3, y_pos - height - 3, z_pos);
        glVertex3f(x_pos, y_pos - height, z_pos);
        glVertex3f(x_pos, y_pos, z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0], color[1], color[2], color[3]);
        glVertex3f(x_pos, y_pos, z_pos);
        glVertex3f(x_pos, y_pos - height, z_pos);
        glVertex3f(x_pos + width, y_pos - height, z_pos);
        glVertex3f(x_pos + width, y_pos, z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
        glVertex3f(x_pos - 3, y_pos - height - 3, z_pos);
        glVertex3f(x_pos + width + 3, y_pos - height - 3, z_pos);
        glVertex3f(x_pos + width, y_pos - height, z_pos);
        glVertex3f(x_pos, y_pos - height, z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
        glVertex3f(x_pos + width, y_pos, z_pos);
        glVertex3f(x_pos + width + 3, y_pos + 3, z_pos);
        glVertex3f(x_pos + width + 3, y_pos - height - 3, z_pos);
        glVertex3f(x_pos + width, y_pos + -height, z_pos);
        glEnd();
        glPopMatrix();
    } else {
        glPushMatrix();
        glBegin(GL_QUADS);
        glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
        glVertex3f(x_pos, y_pos, z_pos);
        glVertex3f(x_pos - 3, y_pos + 3, z_pos);
        glVertex3f(x_pos + width + 3, y_pos + 3, z_pos);
        glVertex3f(x_pos + width, y_pos, z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
        glVertex3f(x_pos - 3, y_pos + 3, z_pos);
        glVertex3f(x_pos - 3, y_pos - height - 3, z_pos);
        glVertex3f(x_pos, y_pos - height, z_pos);
        glVertex3f(x_pos, y_pos, z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0], color[1], color[2], color[3]);
        glVertex3f(x_pos, y_pos, z_pos);
        glVertex3f(x_pos, y_pos - height, z_pos);
        glVertex3f(x_pos + width, y_pos - height, z_pos);
        glVertex3f(x_pos + width, y_pos, z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
        glVertex3f(x_pos - 3, y_pos - height - 3, z_pos);
        glVertex3f(x_pos + width + 3, y_pos - height - 3, z_pos);
        glVertex3f(x_pos + width, y_pos - height, z_pos);
        glVertex3f(x_pos, y_pos - height, z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
        glVertex3f(x_pos + width, y_pos, z_pos);
        glVertex3f(x_pos + width + 3, y_pos + 3, z_pos);
        glVertex3f(x_pos + width + 3, y_pos - height - 3, z_pos);
        glVertex3f(x_pos + width, y_pos + -height, z_pos);
        glEnd();
        glPopMatrix();
    }
    label->draw();
    glPopMatrix();
}

GLfloat ControlItemButton::getXPos() { return x_pos; }
GLfloat ControlItemButton::getYPos() { return y_pos; }
GLfloat ControlItemButton::getHeight() { return height; }
GLfloat ControlItemButton::getWidth() { return width; }
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
        if ((x >= (x_pos) && x <= ((x_pos) + (width))) &&
            (y <= (y_pos) &&
             y >= ((y_pos) - (height)))) {  // This if statement -->
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
