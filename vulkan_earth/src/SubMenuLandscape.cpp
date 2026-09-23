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
#include "MacroCrtdbg.h"
#include "math.h"
#include "Sound.h"

#define PI 3.1415926535898

using namespace std;
extern void playSFX(int sfx);

SubMenuLandscape::SubMenuLandscape() = default;

SubMenuLandscape::SubMenuLandscape(int id,
                                   GLfloat new_x_pos,
                                   GLfloat new_y_pos,
                                   GLfloat new_z_pos,
                                   GLfloat red,
                                   GLfloat green,
                                   GLfloat blue,
                                   GLint new_width,
                                   GLint new_height,
                                   const std::string& new_caption,
                                   GLfloat new_percent_border) {
    uniqueidentifier = id;
    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    percent_border = new_percent_border;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    width = new_width;
    height = new_height;
    caption = new_caption;

    cam_x = -4000;
    cam_y = 10000;
    cam_z = -4000;

    tm = new TerrainMaker(100, 256);
    tm->prepareData(0, 0, 0, 0, 0);

    old_mouse_x = -1;
    old_mouse_y = -1;

    /*	BUTTON TEXT PLACEMENT	*/
    int real_length = 0;
    for (char ch : caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = x_pos + ((width) / 2) - (real_length / 2);
    GLfloat label_y_pos = y_pos - height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           (z_pos + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
    button_pressed = nullptr;

    sub_menu_button[0] =
            new ControlItemSliderbar(x_pos + (width / 2) - (0.48 * width),
                                     y_pos - (height * 0.7),
                                     z_pos + 1,
                                     0.5f,
                                     0.5f,
                                     0.5f,
                                     0.6f * width,
                                     0.085 * (height),
                                     "Smoothness",
                                     "0/1/2/3/4/5/",
                                     5);
    sub_menu_button[1] =
            new ControlItemSliderbar(x_pos + (width / 2) - (0.48 * width),
                                     y_pos - (height * 0.8),
                                     z_pos + 1,
                                     0.5f,
                                     0.5f,
                                     0.5f,
                                     0.6f * width,
                                     0.085 * (height),
                                     "Hill Height",
                                     "0/1/2/3/4/5/",
                                     5);
    sub_menu_button[2] =
            new ControlItemSliderbar(x_pos + (width / 2) - (0.48 * width),
                                     y_pos - (height * 0.9),
                                     z_pos + 1,
                                     0.5f,
                                     0.5f,
                                     0.5f,
                                     0.6f * width,
                                     0.085 * (height),
                                     "Terrain Selection",
                                     "Rock/Snow/Ice/Mars/Desert/Lava/",
                                     0);
    sub_menu_button[3] = new ControlItemButton(this,
                                               x_pos + (0.655 * width),
                                               y_pos - (height * 0.91),
                                               z_pos + 1,
                                               0.75f,
                                               0.0f,
                                               0.0f,
                                               0.3f * width,
                                               0.05 * (height),
                                               "Sample");
}

SubMenuLandscape::~SubMenuLandscape() {
    delete label;
    delete tm;
    for (int i = 0; i < num_control_items_lnd; i++) delete sub_menu_button[i];
}

int SubMenuLandscape::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
void SubMenuLandscape::setUNIQUEIDENTIFIER(int id) { uniqueidentifier = id; }
GLfloat SubMenuLandscape::getXPos() { return x_pos; }
void SubMenuLandscape::setXPos(GLfloat new_xpos) { x_pos = new_xpos; }
GLfloat SubMenuLandscape::getYPos() { return y_pos; }
void SubMenuLandscape::setYPos(GLfloat new_ypos) { y_pos = new_ypos; }
GLfloat SubMenuLandscape::getZPos() { return z_pos; }
void SubMenuLandscape::setZPos(GLfloat new_zpos) { z_pos = new_zpos; }
GLfloat SubMenuLandscape::getRed() { return color[0]; }
void SubMenuLandscape::setRed(GLfloat red) { color[0] = red; }
GLfloat SubMenuLandscape::getGreen() { return color[1]; }
void SubMenuLandscape::setGreen(GLfloat green) { color[1] = green; }
GLfloat SubMenuLandscape::getBlue() { return color[2]; }
void SubMenuLandscape::setBlue(GLfloat blue) { color[2] = blue; }
GLint SubMenuLandscape::getWidth() { return width; }
void SubMenuLandscape::setWdith(GLint new_width) { width = new_width; }
GLint SubMenuLandscape::getHeight() { return height; }
void SubMenuLandscape::setHeight(GLint new_height) { height = new_height; }
std::string SubMenuLandscape::getCaption() { return caption; }
void SubMenuLandscape::setCaption(const std::string& new_caption) {
    caption = new_caption;
}
GLfloat SubMenuLandscape::getPerecentBorder() { return percent_border; }
void SubMenuLandscape::setPercentBorder(GLfloat percent) {
    percent_border = percent;
}

void SubMenuLandscape::draw() {
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
    label->draw();
    for (int i = 0; i < num_control_items_lnd; i++) {
        if (sub_menu_button[i]) {
            sub_menu_button[i]->draw();
        }
    }

    GLfloat border_x = x_pos + 0.03 * width;
    GLfloat border_y = y_pos - 0.07 * height;

    // top-left
    glBegin(GL_QUADS);
    glColor4f(color[0] - .2, color[1] - .2, color[2] - .2, color[3]);
    glVertex3f(border_x, border_y, z_pos + 1);
    glVertex3f(border_x - 3, border_y + 3, z_pos + 1);
    glVertex3f(border_x + 0.936 * width + 3, border_y + 3, z_pos + 1);
    glVertex3f(border_x + 0.936 * width, border_y, z_pos + 1);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .2, color[1] - .2, color[2] - .2, color[3]);
    glVertex3f(border_x - 3, border_y + 3, z_pos + 1);
    glVertex3f(border_x - 3, border_y - 0.597 * height - 3, z_pos + 1);
    glVertex3f(border_x, border_y - 0.597 * height, z_pos + 1);
    glVertex3f(border_x, border_y, z_pos + 1);
    glEnd();

    // bottom-right
    glBegin(GL_QUADS);
    glColor4f(color[0] + .4, color[1] + .4, color[2] + .4, color[3]);
    glVertex3f(border_x - 3, border_y - 0.597 * height - 3, z_pos + 1);
    glVertex3f(border_x + 0.936 * width + 3,
               border_y - 0.597 * height - 3,
               z_pos + 1);
    glVertex3f(border_x + 0.936 * width, border_y - 0.597 * height, z_pos + 1);
    glVertex3f(border_x, border_y - 0.597 * height, z_pos + 1);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + .4, color[1] + .4, color[2] + .4, color[3]);
    glVertex3f(border_x + 0.936 * width, border_y, z_pos + 1);
    glVertex3f(border_x + 0.936 * width + 3, border_y + 3, z_pos + 1);
    glVertex3f(border_x + 0.936 * width + 3,
               border_y - 0.597 * height - 3,
               z_pos + 1);
    glVertex3f(
            border_x + 0.936 * width, border_y + -0.597 * height, z_pos + 1);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    // glPushMatrix();
    glLoadIdentity();
    glViewport(x_pos + 0.8 * width, y_pos, (0.9417 * width), (0.6 * height));
    gluPerspective(45.0, ((0.9417 * width) / (0.6 * height)), 1, 199999999);
    // glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    // glPushMatrix();
    glLoadIdentity();
    glScissor(x_pos + 0.8 * width, y_pos, (0.9417 * width), (0.6 * height));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gluLookAt(cam_x,
              cam_y,
              cam_z,
              (tm->getActualSize() / 2.0),
              0.0f,
              (tm->getActualSize() / 2.0),
              0.0f,
              1.0f,
              0.0f);
    tm->draw();
    // glPopMatrix();

    int win_width = glutGet(GLUT_WINDOW_WIDTH);
    int win_height = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, win_width, win_height);
    gluPerspective(
            60.0,
            static_cast<float>(win_width) / static_cast<float>(win_height),
            1.0,
            1000000.0);

    glMatrixMode(GL_MODELVIEW);
    glScissor(0, 0, win_width, win_height);
    glLoadIdentity();
}

std::string SubMenuLandscape::collectData() {
    std::string optionsarray = "/Landscape/";
    for (int x = 0; x < num_control_items_lnd; x++) {
        if (sub_menu_button[x]) {
            optionsarray += sub_menu_button[x]->collectData();
            optionsarray += "/";
        }
    }
    return optionsarray;
}

void SubMenuLandscape::subMenuMouseTest(int x, int y, int button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < num_control_items_lnd;
             button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                            // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                            // CLICKED A ARROW BUTTON???
            if ((x >= sub_menu_button[button_i]->getXPos()) &&
                (x <= (sub_menu_button[button_i]->getXPos() +
                       sub_menu_button[button_i]->getWidth())) &&
                (y <= sub_menu_button[button_i]->getYPos()) &&
                (y >= (sub_menu_button[button_i]->getYPos() -
                       sub_menu_button[button_i]->getHeight()))) {
                sub_menu_button[button_i]->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // YOU PRESSED OVER A ARROWBUTTON
                button_pressed = sub_menu_button[button_i];
                numberpressed = button_i;
            }
        }
        old_mouse_x = x;
        old_mouse_y = y;
    } else if (!button_down) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                                // GOES UP
        if (button_pressed !=
            nullptr) {  // IF YOU MANAGED TO CLICK INSIDE AN ARROW BUTTON
                        // CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if ((x >= button_pressed->getXPos()) &&
                (x <=
                 (button_pressed->getXPos() + button_pressed->getWidth())) &&
                (y <= button_pressed->getYPos()) &&
                (y >=
                 (button_pressed->getYPos() - button_pressed->getHeight()))) {
                if (numberpressed == preview_button) {
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
                    stringstream ss1(sub_menu_button[0]->collectData());
                    int i1;
                    if (!(ss1 >> i1)) i1 = 0;
                    stringstream ss2(sub_menu_button[1]->collectData());
                    int i2;
                    if (!(ss2 >> i2)) i2 = 0;
                    tm->prepareData(2500,          // int steps
                                    i2 * i2 + 10,  // int increase
                                    30,            // float radius
                                    5,             // int randomJump % (1-100)
                                    i1);           // int smoothness
                    tm->selectTexture(sub_menu_button[2]->collectData());
                    button_pressed->mouseClickEvent(x, y, button_down, false);
                    numberpressed = -1;
                    button_pressed = nullptr;
                    playSFX(SMALL_CLICK);
                } else {
                    button_pressed->mouseClickEvent(
                            x,
                            y,
                            button_down,
                            true);  // IF YOU ARE THEN TELL THE ARROW BUTTON
                                    // YOU RELEASE THE MOUSE
                    numberpressed = -1;
                    button_pressed = nullptr;
                }
            } else {
                button_pressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                button_pressed = nullptr;
                numberpressed = -1;
            }
        }
        old_mouse_x = -1;
        old_mouse_y = -1;
    }
}

void SubMenuLandscape::updateMouse(int x, int y) {
    if (((x >= x_pos + 0.8 * width) &&
         (x <= x_pos + 0.8 * width + (0.9417 * width))) &&
        ((y >= y_pos - 0.15 * height) &&
         (y <= y_pos - 0.15 * height + (0.6 * height)))) {
        float new_cam_x = cam_x, new_cam_y = cam_y, new_cam_z = cam_z;
        if (x < old_mouse_x) {
            new_cam_x =
                    (cam_x - (tm->getActualSize() / 2.0)) * cos(-PI / 180) -
                    (cam_z - (tm->getActualSize() / 2.0)) * sin(-PI / 180) +
                    (tm->getActualSize() / 2.0);
            new_cam_z =
                    (cam_x - (tm->getActualSize() / 2.0)) * sin(-PI / 180) +
                    (cam_z - (tm->getActualSize() / 2.0)) * cos(-PI / 180) +
                    (tm->getActualSize() / 2.0);
        }
        if (x > old_mouse_x) {
            new_cam_x = (cam_x - (tm->getActualSize() / 2.0)) * cos(PI / 180) -
                        (cam_z - (tm->getActualSize() / 2.0)) * sin(PI / 180) +
                        (tm->getActualSize() / 2.0);
            new_cam_z = (cam_x - (tm->getActualSize() / 2.0)) * sin(PI / 180) +
                        (cam_z - (tm->getActualSize() / 2.0)) * cos(PI / 180) +
                        (tm->getActualSize() / 2.0);
        }
        if (y < old_mouse_y) {
        }
        if (y > old_mouse_y) {
        }
        cam_x = new_cam_x;
        cam_y = new_cam_y;
        cam_z = new_cam_z;
        old_mouse_x = x;
        old_mouse_y = y;
    }
    int win_width = glutGet(GLUT_WINDOW_WIDTH);
    int win_height = glutGet(GLUT_WINDOW_HEIGHT);
    sub_menu_button[0]->updateMouse(x - (win_width / 2), (win_height / 2) - y);
    sub_menu_button[1]->updateMouse(x - (win_width / 2), (win_height / 2) - y);
    sub_menu_button[2]->updateMouse(x - (win_width / 2), (win_height / 2) - y);
}
