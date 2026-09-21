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
                           GLfloat x_pos,
                           GLfloat y_pos,
                           GLfloat z_pos,
                           GLfloat red,
                           GLfloat green,
                           GLfloat blue,
                           GLint width,
                           GLint height,
                           const std::string& caption,
                           GLfloat percent_border) {
    UNIQUEIDENTIFIER = id;
    this->xPos = x_pos;
    this->yPos = y_pos;
    this->zPos = z_pos;
    this->percentBorder = percent_border;
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
    GLfloat label_y_pos = this->yPos - this->height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    label = new TextObject(this->caption,
                           label_x_pos,
                           label_y_pos,
                           (this->zPos + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
    buttonPressed = nullptr;
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
    subMenuButton[0] = new ControlItemSliderbar(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.2),
            this->zPos + 1,
            0.5f,
            0.5f,
            0.5f,
            0.6f * this->width,
            0.07 * (this->height),
            "SFX Volume",
            "0/10/20/30/40/50/60/70/80/90/100/",
            10);
    subMenuButton[1] = new ControlItemSliderbar(
            this->xPos + (this->width / 2) - (0.3 * this->width),
            this->yPos - (this->height * 0.3),
            this->zPos + 1,
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
    for (int i = 0; i < NUM_CONTROL_ITEMS_SND; i++) delete subMenuButton[i];
    delete label;
}

int SubMenuSound::getUNIQUEIDENTIFIER() { return UNIQUEIDENTIFIER; }
void SubMenuSound::setUNIQUEIDENTIFIER(int id) { UNIQUEIDENTIFIER = id; }
GLfloat SubMenuSound::getXPos() { return this->xPos; }
void SubMenuSound::setXPos(GLfloat new_xpos) { this->xPos = new_xpos; }
GLfloat SubMenuSound::getYPos() { return this->yPos; }
void SubMenuSound::setYPos(GLfloat new_ypos) { this->yPos = new_ypos; }
GLfloat SubMenuSound::getZPos() { return this->zPos; }
void SubMenuSound::setZPos(GLfloat new_zpos) { this->zPos = new_zpos; }
GLfloat SubMenuSound::getRed() { return color[0]; }
void SubMenuSound::setRed(GLfloat red) { color[0] = red; }
GLfloat SubMenuSound::getGreen() { return color[1]; }
void SubMenuSound::setGreen(GLfloat green) { color[1] = green; }
GLfloat SubMenuSound::getBlue() { return color[2]; }
void SubMenuSound::setBlue(GLfloat blue) { color[2] = blue; }
GLint SubMenuSound::getWidth() { return this->width; }
void SubMenuSound::setWdith(GLint width) { this->width = width; }
GLint SubMenuSound::getHeight() { return this->height; }
void SubMenuSound::setHeight(GLint height) { this->height = height; }
std::string SubMenuSound::getCaption() { return this->caption; }
void SubMenuSound::setCaption(const std::string& caption) {
    this->caption = caption;
}
GLfloat SubMenuSound::getPerecentBorder() { return this->percentBorder; }
void SubMenuSound::setPercentBorder(GLfloat percent) {
    this->percentBorder = percentBorder;
}

void SubMenuSound::draw() {
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
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
    glEnd();
    label->draw();
    for (int i = 0; i < NUM_CONTROL_ITEMS_SND; i++) {
        if (subMenuButton[i]) {
            subMenuButton[i]->draw();
        }
    }
}

std::string SubMenuSound::collectData() { return "Sound:"; }

void SubMenuSound::subMenuMouseTest(int x, int y, int button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (int button_i = 0; button_i < NUM_CONTROL_ITEMS_SND;
             button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                            // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                            // CLICKED A ARROW BUTTON???
            if ((x >= subMenuButton[button_i]->getXPos()) &&
                (x <= (subMenuButton[button_i]->getXPos() +
                       subMenuButton[button_i]->getWidth())) &&
                (y <= subMenuButton[button_i]->getYPos()) &&
                (y >= (subMenuButton[button_i]->getYPos() -
                       subMenuButton[button_i]->getHeight()))) {
                subMenuButton[button_i]->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // YOU PRESSED OVER A ARROWBUTTON
                buttonPressed = subMenuButton[button_i];
            }
        }
    } else if (!button_down) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                                // GOES UP
        if (buttonPressed !=
            nullptr) {  // IF YOU MANAGED TO CLICK INSIDE AN ARROW BUTTON
                        // CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if ((x >= buttonPressed->getXPos()) &&
                (x <=
                 (buttonPressed->getXPos() + buttonPressed->getWidth())) &&
                (y <= buttonPressed->getYPos()) &&
                (y >=
                 (buttonPressed->getYPos() - buttonPressed->getHeight()))) {
                buttonPressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                // RELEASE THE MOUSE
                changeVolumes(buttonPressed);
            } else {
                buttonPressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                buttonPressed = nullptr;
            }
        }
    }
}

void SubMenuSound::updateMouse(int x, int y) {
    subMenuButton[0]->updateMouse(x, y);
    subMenuButton[1]->updateMouse(x, y);
    changeVolumes(buttonPressed);
}

void SubMenuSound::changeVolumes(ControlItem* the_sub_menu_button) {
    // SFX volume handler
    if (the_sub_menu_button == subMenuButton[0]) {
        int new_volume = atoi(the_sub_menu_button->collectData().c_str());
        Mix_Volume(-1,
                   128 / 100 *
                           new_volume);  //-1 is to apply to all allocated
                                         // channels, 128 is the maximum volume
    }
    // Music volume handler
    else if (the_sub_menu_button == subMenuButton[1]) {
        int new_volume = atoi(the_sub_menu_button->collectData().c_str());
        Mix_VolumeMusic(128 / 100 *
                        new_volume);  // music has its special channel, so
                                      // don't need to specify which channel.
    }
}