#include "MainMenuButton.h"
#include <stdio.h>
#include <iostream>
#include "SubMenu.h"
#include "TextObject.h"
#include "MacroCrtdbg.h"
#include "Sound.h"

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
    submenu = new_submenu;
}

MainMenuButton::~MainMenuButton() { delete label; }

void MainMenuButton::pressDraw() {
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
}

void MainMenuButton::draw() {
    glPushMatrix();
    if (pressed)
        pressDraw();
    else {
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
    }
    label->draw();

    if (active) {
        if (submenu != nullptr) {
            submenu->draw();
        }
    }
    glPopMatrix();
}

int MainMenuButton::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
GLfloat MainMenuButton::getXPos() { return x_pos; }
GLfloat MainMenuButton::getYPos() { return y_pos; }
GLfloat MainMenuButton::getHeight() { return height; }
GLfloat MainMenuButton::getWidth() { return width; }
SubMenu* MainMenuButton::getSubMenu() { return submenu; }
GLfloat* MainMenuButton::getColor() { return &color[0]; }
void MainMenuButton::setColor(GLfloat r, GLfloat g, GLfloat b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

void MainMenuButton::setLabel(const std::string& c) {
    delete label;
    caption = c;

    int real_length = 0;
    for (char ch : caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = x_pos + ((width) / 2) - (real_length / 2);
    GLfloat label_y_pos =
            y_pos + ((y_pos - (y_pos + height)) / 2) - height / 4;

    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           z_pos,
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
