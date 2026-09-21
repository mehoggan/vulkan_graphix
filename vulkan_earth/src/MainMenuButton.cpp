#include "MainMenuButton.h"
#include <stdio.h>
#include <iostream>
#include "SubMenu.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

using namespace std;

extern void playSFX(int sfx);

MainMenuButton::MainMenuButton() = default;

MainMenuButton::MainMenuButton(int id,
                               GLfloat new_x_pos,
                               GLfloat new_y_pos,
                               GLfloat new_z_pos,
                               GLfloat red,
                               GLfloat green,
                               GLfloat blue,
                               GLint new_width,
                               GLint new_height,
                               const std::string& new_caption,
                               SubMenu* new_submenu) {
    uniqueidentifier = id;
    pressed = false;
    active = false;
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
    this->submenu = new_submenu;
}

MainMenuButton::~MainMenuButton() { delete label; }

void MainMenuButton::pressDraw() {
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
    glVertex3f(this->x_pos + width + 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos);
    glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos + -height, this->z_pos);
    glEnd();
    glPopMatrix();
}

void MainMenuButton::draw() {
    glPushMatrix();
    if (pressed)
        pressDraw();
    else {
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
    }
    label->draw();

    if (active) {
        if (this->submenu != nullptr) {
            this->submenu->draw();
        }
    }
    glPopMatrix();
}

int MainMenuButton::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
GLfloat MainMenuButton::getXPos() { return this->x_pos; }
GLfloat MainMenuButton::getYPos() { return this->y_pos; }
GLfloat MainMenuButton::getHeight() { return this->height; }
GLfloat MainMenuButton::getWidth() { return this->width; }
SubMenu* MainMenuButton::getSubMenu() { return this->submenu; }
GLfloat* MainMenuButton::getColor() { return &color[0]; }
void MainMenuButton::setColor(GLfloat r, GLfloat g, GLfloat b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

void MainMenuButton::setLabel(const std::string& c) {
    delete label;
    this->caption = c;

    int real_length = 0;
    for (char ch : this->caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos =
            this->x_pos + ((this->width) / 2) - (real_length / 2);
    GLfloat label_y_pos = this->y_pos +
                          ((this->y_pos - (this->y_pos + this->height)) / 2) -
                          this->height / 4;

    label = new TextObject(this->caption,
                           label_x_pos,
                           label_y_pos,
                           this->z_pos,
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
}

bool MainMenuButton::isPressed() { return pressed; }
bool MainMenuButton::isActive() { return active; }

void MainMenuButton::pressButton() {
    if (Mix_Playing(0) == 0) playSFX(BIG_CLICK);
    pressed = true;
}

void MainMenuButton::depressButton() { pressed = false; }

void MainMenuButton::activateSubMenu() { active = true; }

void MainMenuButton::deactivateSubMenu() { active = false; }

void MainMenuButton::printSelf(int i) {
    cout << " Button[" << i << "].x=" << (getXPos()) << " Button[" << i
         << "].y=" << (getYPos()) << " Button[" << i
         << "].width=" << (getWidth()) << " Button[" << i
         << "].height=" << (getHeight()) << endl;
}
