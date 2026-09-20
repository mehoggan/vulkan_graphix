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

MainMenuButton::MainMenuButton(int ID,
                               GLfloat xPos,
                               GLfloat yPos,
                               GLfloat zPos,
                               GLfloat red,
                               GLfloat green,
                               GLfloat blue,
                               GLint width,
                               GLint height,
                               const std::string& caption,
                               SubMenu* submenu) {
    UNIQUEIDENTIFIER = ID;
    pressed = false;
    active = false;
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
    this->submenu = submenu;
}

MainMenuButton::~MainMenuButton() { delete label; }

void MainMenuButton::pressDraw() {
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
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
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
    }
    label->draw();

    if (active) {
        if (this->submenu != nullptr) {
            this->submenu->draw();
        }
    }
    glPopMatrix();
}

int MainMenuButton::getUNIQUEIDENTIFIER() { return UNIQUEIDENTIFIER; }
GLfloat MainMenuButton::getXPos() { return this->xPos; }
GLfloat MainMenuButton::getYPos() { return this->yPos; }
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
    GLfloat label_x_pos = this->xPos + ((this->width) / 2) - (real_length / 2);
    GLfloat label_y_pos = this->yPos +
                          ((this->yPos - (this->yPos + this->height)) / 2) -
                          this->height / 4;

    label = new TextObject(this->caption,
                           label_x_pos,
                           label_y_pos,
                           this->zPos,
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
