#include "SubMenuSelectTanks.h"
#include <cstring>
#include <string>
#include "ControlItem.h"
#include "ControlItemCheckBox.h"
#include "ControlItemSelectionBox.h"
#include "SubMenu.h"
#include "TextObject.h"
#include "macro_crtdbg.h"

using namespace std;

SubMenuSelectTanks::SubMenuSelectTanks() = default;

SubMenuSelectTanks::SubMenuSelectTanks(int id,
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
}

SubMenuSelectTanks::~SubMenuSelectTanks() { delete label; }

int SubMenuSelectTanks::getUNIQUEIDENTIFIER() { return UNIQUEIDENTIFIER; }
void SubMenuSelectTanks::setUNIQUEIDENTIFIER(int id) { UNIQUEIDENTIFIER = id; }
GLfloat SubMenuSelectTanks::getXPos() { return this->xPos; }
void SubMenuSelectTanks::setXPos(GLfloat new_xpos) { this->xPos = new_xpos; }
GLfloat SubMenuSelectTanks::getYPos() { return this->yPos; }
void SubMenuSelectTanks::setYPos(GLfloat new_ypos) { this->yPos = new_ypos; }
GLfloat SubMenuSelectTanks::getZPos() { return this->zPos; }
void SubMenuSelectTanks::setZPos(GLfloat new_zpos) { this->zPos = new_zpos; }
GLfloat SubMenuSelectTanks::getRed() { return color[0]; }
void SubMenuSelectTanks::setRed(GLfloat red) { color[0] = red; }
GLfloat SubMenuSelectTanks::getGreen() { return color[1]; }
void SubMenuSelectTanks::setGreen(GLfloat green) { color[1] = green; }
GLfloat SubMenuSelectTanks::getBlue() { return color[2]; }
void SubMenuSelectTanks::setBlue(GLfloat blue) { color[2] = blue; }
GLint SubMenuSelectTanks::getWidth() { return this->width; }
void SubMenuSelectTanks::setWdith(GLint width) { this->width = width; }
GLint SubMenuSelectTanks::getHeight() { return this->height; }
void SubMenuSelectTanks::setHeight(GLint height) { this->height = height; }
std::string SubMenuSelectTanks::getCaption() { return this->caption; }
void SubMenuSelectTanks::setCaption(const std::string& caption) {
    this->caption = caption;
}
GLfloat SubMenuSelectTanks::getPerecentBorder() { return this->percentBorder; }
void SubMenuSelectTanks::setPercentBorder(GLfloat percent) {
    this->percentBorder = percentBorder;
}

void SubMenuSelectTanks::draw() {
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
    for (int i = 0; i < NUM_CONTROL_ITEMS_ST; i++) {
        if (subMenuButton[i]) {
            // subMenuButton[i]->draw();
        }
    }
}

std::string SubMenuSelectTanks::collectData() { return "SelectTanks:"; }

void SubMenuSelectTanks::subMenuMouseTest(int x, int y, int button_down) {
    /*
    if(buttonDown){
    //FIRST CONDITION IS LEFT MOUSE BUTTON DOWN for(int
    button_i=0;button_i<NUM_CONTROL_ITEMS_ST;button_i++){
    //SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                                                                                                //IF YOU DID NOT CLICK A BUTTON PERHAPS YOU CLICKED A ARROW BUTTON???
            if	((x>=subMenuButton[button_i]->getXPos())	&&
    (x<=(subMenuButton[button_i]->getXPos()+subMenuButton[button_i]->getWidth()))
                                                                            &&
                (y<=subMenuButton[button_i]->getYPos())	&&
    (y>=(subMenuButton[button_i]->getYPos()-subMenuButton[button_i]->getHeight()))){
                    subMenuButton[button_i]->mouseClickEvent(x,y,buttonDown,true);
    //YOU PRESSED OVER A ARROWBUTTON
                    buttonPressed=subMenuButton[button_i];
            }
        }
    }else if(!buttonDown){
    //IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON GOES UP
        if(buttonPressed!=NULL){
    //IF YOU MANAGED TO CLICK INSIDE AN ARROW BUTTON
                                                                                        //CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if	((x>=buttonPressed->getXPos())		&&
    (x<=(buttonPressed->getXPos()+buttonPressed->getWidth()))
                                                                &&
                (y<=buttonPressed->getYPos())		&&
    (y>=(buttonPressed->getYPos()-buttonPressed->getHeight()))) {
                buttonPressed->mouseClickEvent(x,y,buttonDown,true);
    //IF YOU ARE THEN TELL THE ARROW BUTTON YOU RELEASE THE MOUSE }else {
                buttonPressed->mouseClickEvent(x,y,buttonDown,false);
    //IF YOU ARE THEN TELL THE ARROW BUTTON YOU RELEASE THE MOUSE
                buttonPressed=NULL;
            }
        }
    }
    */
}

void SubMenuSelectTanks::updateMouse(int x, int y) {}