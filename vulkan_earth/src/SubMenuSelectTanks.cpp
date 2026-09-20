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

SubMenuSelectTanks::SubMenuSelectTanks(int ID,
                                       GLfloat xPos,
                                       GLfloat yPos,
                                       GLfloat zPos,
                                       GLfloat red,
                                       GLfloat green,
                                       GLfloat blue,
                                       GLint width,
                                       GLint height,
                                       const std::string& caption,
                                       GLfloat percentBorder) {
    this->UNIQUEIDENTIFIER = ID;
    this->xPos = xPos;
    this->yPos = yPos;
    this->zPos = zPos;
    this->percentBorder = percentBorder;
    this->color[0] = red;
    this->color[1] = green;
    this->color[2] = blue;
    this->color[3] = 1.0;
    this->width = width;
    this->height = height;
    this->caption = caption;

    /*	BUTTON TEXT PLACEMENT	*/
    int realLength = 0;
    for (char ch : this->caption) {
        realLength += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat labelXPos = this->xPos + ((this->width) / 2) - (realLength / 2);
    GLfloat labelYPos = this->yPos - this->height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    this->label = new TextObject(this->caption,
                                 labelXPos,
                                 labelYPos,
                                 (this->zPos + 1),
                                 GLUT_BITMAP_TIMES_ROMAN_24,
                                 0.0f,
                                 0.0f,
                                 0.0f);
}

SubMenuSelectTanks::~SubMenuSelectTanks() { delete this->label; }

int SubMenuSelectTanks::getUNIQUEIDENTIFIER() {
    return this->UNIQUEIDENTIFIER;
}
void SubMenuSelectTanks::setUNIQUEIDENTIFIER(int ID) {
    this->UNIQUEIDENTIFIER = ID;
}
GLfloat SubMenuSelectTanks::getXPos() { return this->xPos; }
void SubMenuSelectTanks::setXPos(GLfloat newXpos) { this->xPos = newXpos; }
GLfloat SubMenuSelectTanks::getYPos() { return this->yPos; }
void SubMenuSelectTanks::setYPos(GLfloat newYpos) { this->yPos = newYpos; }
GLfloat SubMenuSelectTanks::getZPos() { return this->zPos; }
void SubMenuSelectTanks::setZPos(GLfloat newZpos) { this->zPos = newZpos; }
GLfloat SubMenuSelectTanks::getRed() { return this->color[0]; }
void SubMenuSelectTanks::setRed(GLfloat red) { this->color[0] = red; }
GLfloat SubMenuSelectTanks::getGreen() { return this->color[1]; }
void SubMenuSelectTanks::setGreen(GLfloat green) { this->color[1] = green; }
GLfloat SubMenuSelectTanks::getBlue() { return this->color[2]; }
void SubMenuSelectTanks::setBlue(GLfloat blue) { this->color[2] = blue; }
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
    glColor4f(this->color[0] + .2,
              this->color[1] + .2,
              this->color[2] + .2,
              this->color[3]);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] + .2,
              this->color[1] + .2,
              this->color[2] + .2,
              this->color[3]);
    glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0], this->color[1], this->color[2], this->color[3]);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] - .4,
              this->color[1] - .4,
              this->color[2] - .4,
              this->color[3]);
    glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] - .4,
              this->color[1] - .4,
              this->color[2] - .4,
              this->color[3]);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
    glEnd();
    this->label->draw();
    for (int i = 0; i < NUM_CONTROL_ITEMS_ST; i++) {
        if (this->subMenuButton[i]) {
            // this->subMenuButton[i]->draw();
        }
    }
}

std::string SubMenuSelectTanks::collectData() { return "SelectTanks:"; }

void SubMenuSelectTanks::subMenuMouseTest(int x, int y, int buttonDown) {
    /*
    if(buttonDown){
    //FIRST CONDITION IS LEFT MOUSE BUTTON DOWN for(int
    button_i=0;button_i<NUM_CONTROL_ITEMS_ST;button_i++){
    //SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                                                                                                //IF YOU DID NOT CLICK A BUTTON PERHAPS YOU CLICKED A ARROW BUTTON???
            if	((x>=this->subMenuButton[button_i]->getXPos())	&&
    (x<=(this->subMenuButton[button_i]->getXPos()+this->subMenuButton[button_i]->getWidth()))
                                                                            &&
                (y<=this->subMenuButton[button_i]->getYPos())	&&
    (y>=(this->subMenuButton[button_i]->getYPos()-this->subMenuButton[button_i]->getHeight()))){
                    this->subMenuButton[button_i]->mouseClickEvent(x,y,buttonDown,true);
    //YOU PRESSED OVER A ARROWBUTTON
                    this->buttonPressed=this->subMenuButton[button_i];
            }
        }
    }else if(!buttonDown){
    //IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON GOES UP
        if(this->buttonPressed!=NULL){
    //IF YOU MANAGED TO CLICK INSIDE AN ARROW BUTTON
                                                                                        //CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if	((x>=this->buttonPressed->getXPos())		&&
    (x<=(this->buttonPressed->getXPos()+this->buttonPressed->getWidth()))
                                                                &&
                (y<=this->buttonPressed->getYPos())		&&
    (y>=(this->buttonPressed->getYPos()-this->buttonPressed->getHeight()))) {
                this->buttonPressed->mouseClickEvent(x,y,buttonDown,true);
    //IF YOU ARE THEN TELL THE ARROW BUTTON YOU RELEASE THE MOUSE }else {
                this->buttonPressed->mouseClickEvent(x,y,buttonDown,false);
    //IF YOU ARE THEN TELL THE ARROW BUTTON YOU RELEASE THE MOUSE
                this->buttonPressed=NULL;
            }
        }
    }
    */
}

void SubMenuSelectTanks::updateMouse(int x, int y) {}