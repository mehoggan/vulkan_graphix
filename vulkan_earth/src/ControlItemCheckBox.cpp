#include "ControlItemCheckBox.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "ControlItem.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

using namespace std;

extern void playSFX(int sfx);

ControlItemCheckBox::ControlItemCheckBox() = default;

ControlItemCheckBox::ControlItemCheckBox(GLfloat new_x_pos,
                                         GLfloat new_y_pos,
                                         GLfloat new_z_pos,
                                         GLfloat red,
                                         GLfloat green,
                                         GLfloat blue,
                                         GLint new_width,
                                         GLint new_height,
                                         const std::string& new_caption) {
    this->x_pos = new_x_pos;
    this->y_pos = new_y_pos;
    this->z_pos = new_z_pos;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    this->width = new_width;
    this->height = new_height;
    this->caption = new_caption;

    button_state = 0;
    menu_state = 0;

    /*	BUTTON TEXT PLACEMENT	*/
    int real_length = 0;
    for (char ch : this->caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }

    GLfloat label_x_pos = this->x_pos + (this->width / 2) - (real_length / 2);
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

ControlItemCheckBox::~ControlItemCheckBox() { delete label; }

void ControlItemCheckBox::draw() {
    // draw main button box
    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
    glVertex3f(this->x_pos, this->y_pos, this->z_pos);
    glVertex3f(this->x_pos - 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
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
    glColor4f(color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
    glVertex3f(this->x_pos - 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos);
    glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos + -height, this->z_pos);
    glEnd();

    // draw the actual check box itself
    // composed of 4 smallers squares, the inside vertex of each square gets
    // colored darker when pressed square order:    1 4 				   2 3
    // NOTE: Squares are actually composed of 2 triangles, drawn counter
    // clockwise
    //       You can identify the innermost (center) vertex by the if statement
    //       preceding it

    // square 1
    glBegin(GL_TRIANGLES);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    glVertex3f(this->x_pos + (width - height * 0.9),
               (this->y_pos - height * 0.1),
               this->z_pos + 1);
    glVertex3f(this->x_pos + (width - height * 0.9),
               (this->y_pos - height / 2),
               this->z_pos + 1);
    if (button_state == 1)
        glColor3f(color[0] - .2, color[1] - .2, color[2] - .2);
    glVertex3f(this->x_pos + (width - height / 2),
               (this->y_pos - height / 2),
               this->z_pos + 1);
    glEnd();
    glBegin(GL_TRIANGLES);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    glVertex3f(this->x_pos + (width - height * 0.9),
               (this->y_pos - height * 0.1),
               this->z_pos + 1);
    if (button_state == 1)
        glColor3f(color[0] - .2, color[1] - .2, color[2] - .2);
    glVertex3f(this->x_pos + (width - height / 2),
               (this->y_pos - height / 2),
               this->z_pos + 1);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    glVertex3f(this->x_pos + (width - height / 2),
               (this->y_pos - height * 0.1),
               this->z_pos + 1);
    glEnd();

    // square 2
    glBegin(GL_TRIANGLES);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    glVertex3f(this->x_pos + (width - height * 0.9),
               (this->y_pos - height / 2),
               this->z_pos + 1);
    glVertex3f(this->x_pos + (width - height * 0.9),
               (this->y_pos - height * 0.9),
               this->z_pos + 1);
    if (button_state == 1)
        glColor3f(color[0] - .2, color[1] - .2, color[2] - .2);
    glVertex3f(this->x_pos + (width - height / 2),
               (this->y_pos - height / 2),
               this->z_pos + 1);
    glEnd();
    glBegin(GL_TRIANGLES);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    glVertex3f(this->x_pos + (width - height * 0.9),
               (this->y_pos - height * 0.9),
               this->z_pos + 1);
    glVertex3f(this->x_pos + (width - height / 2),
               (this->y_pos - height * 0.9),
               this->z_pos + 1);
    if (button_state == 1)
        glColor3f(color[0] - .2, color[1] - .2, color[2] - .2);
    glVertex3f(this->x_pos + (width - height / 2),
               (this->y_pos - height / 2),
               this->z_pos + 1);
    glEnd();

    // square 3
    glBegin(GL_TRIANGLES);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    if (button_state == 1)
        glColor3f(color[0] - .2, color[1] - .2, color[2] - .2);
    glVertex3f(this->x_pos + (width - height / 2),
               (this->y_pos - height / 2),
               this->z_pos + 1);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    glVertex3f(this->x_pos + (width - height / 2),
               (this->y_pos - height * 0.9),
               this->z_pos + 1);
    glVertex3f(this->x_pos + (width - height) + height * 0.9,
               (this->y_pos - height * 0.9),
               this->z_pos + 1);
    glEnd();
    glBegin(GL_TRIANGLES);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    if (button_state == 1)
        glColor3f(color[0] - .2, color[1] - .2, color[2] - .2);
    glVertex3f(this->x_pos + (width - height / 2),
               (this->y_pos - height / 2),
               this->z_pos + 1);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    glVertex3f(this->x_pos + (width - height * 0.1),
               (this->y_pos - height * 0.9),
               this->z_pos + 1);
    glVertex3f(this->x_pos + (width - height * 0.1),
               (this->y_pos - height / 2),
               this->z_pos + 1);
    glEnd();

    // square 4
    glBegin(GL_TRIANGLES);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    glVertex3f(this->x_pos + (width - height / 2),
               (this->y_pos - height * 0.1),
               this->z_pos + 1);
    if (button_state == 1)
        glColor3f(color[0] - .2, color[1] - .2, color[2] - .2);
    glVertex3f(this->x_pos + (width - height) + (height / 2),
               (this->y_pos - height / 2),
               this->z_pos + 1);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    glVertex3f(this->x_pos + (width - height * 0.1),
               (this->y_pos - height * 0.1),
               this->z_pos + 1);
    glEnd();
    glBegin(GL_TRIANGLES);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    if (button_state == 1)
        glColor3f(color[0] - .2, color[1] - .2, color[2] - .2);
    glVertex3f(this->x_pos + (width - height) + (height / 2),
               (this->y_pos - height / 2),
               this->z_pos + 1);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2);
    glVertex3f(this->x_pos + (width - height * 0.1),
               (this->y_pos - height / 2),
               this->z_pos + 1);
    glVertex3f(this->x_pos + (width - height * 0.1),
               (this->y_pos - height * 0.1),
               this->z_pos + 1);
    glEnd();

    // draw check mark if it was toggled on, otherwise dont
    if (menu_state == 1) {
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex3f(this->x_pos + (width - height + (height * 0.2)),
                   (this->y_pos - height / 2) + (0.015 * width),
                   this->z_pos + 2);
        glVertex3f(this->x_pos + (width - height + (height * 0.2)),
                   (this->y_pos - height / 2) + (0.005 * width),
                   this->z_pos + 2);
        glVertex3f(this->x_pos + (width - height / 2),
                   (this->y_pos - height / 2) - (0.015 * width),
                   this->z_pos + 2);
        glVertex3f(this->x_pos + (width - height / 2),
                   (this->y_pos - height / 2),
                   this->z_pos + 2);
        glEnd();
        glBegin(GL_QUADS);
        glVertex3f(this->x_pos + (width - height / 2),
                   (this->y_pos - height / 2) - (0.015 * width),
                   this->z_pos + 2);
        glVertex3f(this->x_pos + (width - height / 2),
                   (this->y_pos - height / 2),
                   this->z_pos + 2);
        glVertex3f(this->x_pos + (width - height) + (0.95 * height),
                   (this->y_pos - height / 2) + (0.025 * width),
                   this->z_pos + 2);
        glVertex3f(this->x_pos + (width - height) + (0.95 * height),
                   (this->y_pos - height / 2) + (0.015 * width),
                   this->z_pos + 2);
        glEnd();
    }

    label->draw();
}

GLfloat ControlItemCheckBox::getXPos() { return this->x_pos; }
GLfloat ControlItemCheckBox::getYPos() { return this->y_pos; }
GLfloat ControlItemCheckBox::getHeight() { return this->height; }
GLfloat ControlItemCheckBox::getWidth() { return this->width; }
std::string ControlItemCheckBox::collectData() {
    if (menu_state == 0)
        return "false";
    else
        return "true";
}

void ControlItemCheckBox::setOptionText(int index) {}
void ControlItemCheckBox::setOptionText(const std::string& new_text) {}

// NOTE: I use this->height for the x value check, this is intentional to
// maintain a square
void ControlItemCheckBox::mouseClickEvent(
        GLint x,
        GLint y,
        GLint state,
        bool still_over_control_item_check_box) {
    if ((x >= (this->x_pos + this->width - (this->height * 0.9)) &&
         (x <= this->x_pos + this->width - (this->height * 0.1))) &&
        ((y <= (this->y_pos - (this->height * 0.1))) &&
         (y >= (this->y_pos - this->height * 0.9)))) {
        if (state == 1) {
            button_state = 1;
        } else if (state == 0) {
            if (still_over_control_item_check_box) {
                if (menu_state == 1)
                    menu_state = 0;
                else
                    menu_state = 1;
                playSFX(SMALL_CLICK);
            }

            button_state = 0;
        }
    }

    if (state == 0) {
        button_state = 0;
    }
}

void ControlItemCheckBox::updateMouse(int x, int y) {}