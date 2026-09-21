#include "SubMenuSound.h"
#include <string>
#include "ControlItem.h"
#include "ControlItemCheckBox.h"
#include "ControlItemSelectionBox.h"
#include "ControlItemSliderbar.h"
#include "SubMenu.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

using namespace std;

SubMenuSound::SubMenuSound() = default;

SubMenuSound::SubMenuSound(int id,
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
    this->x_pos = new_x_pos;
    this->y_pos = new_y_pos;
    this->z_pos = new_z_pos;
    this->percent_border = new_percent_border;
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
    GLfloat label_y_pos = this->y_pos - this->height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    label = new TextObject(this->caption,
                           label_x_pos,
                           label_y_pos,
                           (this->z_pos + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
    button_pressed = nullptr;
    /*
    subMenuButton[0] = new
    ControlItemSelectionBox(this->xPos+(this->width/2)-(0.3*this->width),this->yPos-(this->height*0.2),
    this->zPos+1, 0.5f, 0.5f, 0.5f, 0.6f*this->width,0.06*(this->height), "SFX
    Volume","100/0/10/20/30/40/50/60/70/80/90/"); subMenuButton[1] = new
    ControlItemSelectionBox(
    this->xPos+(this->width/2)-(0.3*this->width),this->yPos-(this->height*0.27),
    this->zPos+1, 0.5f, 0.5f, 0.5f, 0.6f*this->width,0.06*(this->height),
                                                        "Music
    Volume","100/0/10/20/30/40/50/60/70/80/90/");
    //*/
    sub_menu_button[0] = new ControlItemSliderbar(
            this->x_pos + (this->width / 2) - (0.3 * this->width),
            this->y_pos - (this->height * 0.2),
            this->z_pos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.07 * (this->height),
            "SFX Volume",
            "0/10/20/30/40/50/60/70/80/90/100/",
            10);
    sub_menu_button[1] = new ControlItemSliderbar(
            this->x_pos + (this->width / 2) - (0.3 * this->width),
            this->y_pos - (this->height * 0.3),
            this->z_pos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.07 * (this->height),
            "Music Volume",
            "0/10/20/30/40/50/60/70/80/90/100/",
            10);
}

SubMenuSound::~SubMenuSound() {
    for (int i = 0; i < num_control_items_snd; i++) delete sub_menu_button[i];
    delete label;
}

int SubMenuSound::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
void SubMenuSound::setUNIQUEIDENTIFIER(int id) { uniqueidentifier = id; }
GLfloat SubMenuSound::getXPos() { return this->x_pos; }
void SubMenuSound::setXPos(GLfloat new_xpos) { this->x_pos = new_xpos; }
GLfloat SubMenuSound::getYPos() { return this->y_pos; }
void SubMenuSound::setYPos(GLfloat new_ypos) { this->y_pos = new_ypos; }
GLfloat SubMenuSound::getZPos() { return this->z_pos; }
void SubMenuSound::setZPos(GLfloat new_zpos) { this->z_pos = new_zpos; }
GLfloat SubMenuSound::getRed() { return color[0]; }
void SubMenuSound::setRed(GLfloat red) { color[0] = red; }
GLfloat SubMenuSound::getGreen() { return color[1]; }
void SubMenuSound::setGreen(GLfloat green) { color[1] = green; }
GLfloat SubMenuSound::getBlue() { return color[2]; }
void SubMenuSound::setBlue(GLfloat blue) { color[2] = blue; }
GLint SubMenuSound::getWidth() { return this->width; }
void SubMenuSound::setWdith(GLint new_width) { this->width = new_width; }
GLint SubMenuSound::getHeight() { return this->height; }
void SubMenuSound::setHeight(GLint new_height) { this->height = new_height; }
std::string SubMenuSound::getCaption() { return this->caption; }
void SubMenuSound::setCaption(const std::string& new_caption) {
    this->caption = new_caption;
}
GLfloat SubMenuSound::getPerecentBorder() { return this->percent_border; }
void SubMenuSound::setPercentBorder(GLfloat percent) {
    this->percent_border = percent_border;
}

void SubMenuSound::draw() {
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
    glVertex3f(this->x_pos + width + 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos);
    glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos + -height, this->z_pos);
    glEnd();
    label->draw();
    for (int i = 0; i < num_control_items_snd; i++) {
        if (sub_menu_button[i]) {
            sub_menu_button[i]->draw();
        }
    }
}

std::string SubMenuSound::collectData() { return "Sound:"; }

void SubMenuSound::subMenuMouseTest(int x, int y, int button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < num_control_items_snd;
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
            }
        }
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
                button_pressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                // RELEASE THE MOUSE
                changeVolumes(button_pressed);
            } else {
                button_pressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                button_pressed = nullptr;
            }
        }
    }
}

void SubMenuSound::updateMouse(int x, int y) {
    sub_menu_button[0]->updateMouse(x, y);
    sub_menu_button[1]->updateMouse(x, y);
    changeVolumes(button_pressed);
}

void SubMenuSound::changeVolumes(ControlItem* the_sub_menu_button) {
    // SFX volume handler
    if (the_sub_menu_button == sub_menu_button[0]) {
        int new_volume = atoi(the_sub_menu_button->collectData().c_str());
        Mix_Volume(-1,
                   128 / 100 *
                           new_volume);  //-1 is to apply to all allocated
                                         // channels, 128 is the maximum volume
    }
    // Music volume handler
    else if (the_sub_menu_button == sub_menu_button[1]) {
        int new_volume = atoi(the_sub_menu_button->collectData().c_str());
        Mix_VolumeMusic(128 / 100 *
                        new_volume);  // music has its special channel, so
                                      // don't need to specify which channel.
    }
}