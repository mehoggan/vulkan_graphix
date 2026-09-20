#include "ControlItemSliderbar.h"
#include <stdio.h>
#include <iostream>
#include "ControlItem.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ControlItemSliderbar::ControlItemSliderbar() = default;
ControlItemSliderbar::ControlItemSliderbar(GLfloat xPos,
                                           GLfloat yPos,
                                           GLfloat zPos,
                                           GLfloat red,
                                           GLfloat green,
                                           GLfloat blue,
                                           GLint width,
                                           GLint height,
                                           const std::string& caption,
                                           const std::string& menuString,
                                           int sliderStartingIndex) {
    this->xPos = xPos;
    this->yPos = yPos;
    this->zPos = zPos;
    this->color[0] = red;
    this->color[1] = green;
    this->color[2] = blue;
    this->color[3] = 1.0;
    this->width = width;
    this->height = height;

    this->barWidth = width - width * 0.1;
    this->barXPos = xPos + (width - barWidth) / 2;
    this->barYPos = yPos - height / 1.5;
    this->barZPos = zPos + 0.5;

    this->sliderXPos = barXPos - barWidth * 0.012;
    this->sliderYPos = yPos - height / 1.7;
    this->sliderZPos = barZPos + 0.5;
    this->sliderWidth = width / 5 * 0.1;
    this->sliderHeight = height * 0.15;

    this->caption = caption;
    this->menuInfo = menuString;
    this->isSliderClicked = false;

    // split menuInfo on '/' into allOptions
    std::string current;
    for (char ch : this->menuInfo) {
        if (ch == '/') {
            this->allOptions.push_back(current);
            current.clear();
        } else {
            current += ch;
        }
    }
    this->numberOfOptions = static_cast<int>(this->allOptions.size());
    this->interval =
            barWidth / (numberOfOptions -
                        1.0);  // if it's divided by an integer, the whole
                               // thing becomes an integer value???

    this->menuState = sliderStartingIndex;
    this->buttonState = 0;
    this->optionText = nullptr;
    this->setOptionText(menuState);  // set option to first option
    /*	BUTTON TEXT PLACEMENT	*/
    int real_length = 0;
    for (char ch : this->caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = barXPos;
    GLfloat label_y_pos = this->yPos - this->height * 0.45;
    /*	END OF BUTTON TEXT PLACEMENT	*/
    this->label = new TextObject(this->caption,
                                 label_x_pos,
                                 label_y_pos,
                                 this->zPos,
                                 GLUT_BITMAP_TIMES_ROMAN_24,
                                 0.0f,
                                 0.0f,
                                 0.0f);
}
ControlItemSliderbar::~ControlItemSliderbar() {
    delete optionText;
    delete label;
}

void ControlItemSliderbar::draw() {
    // draw main button box
    glBegin(GL_QUADS);
    glColor4f(this->color[0] - 0.2f,
              this->color[1] - 0.2f,
              this->color[2] - 0.2f,
              this->color[3]);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] - 0.2f,
              this->color[1] - 0.2f,
              this->color[2] - 0.2f,
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
    glColor4f(this->color[0] + 0.4f,
              this->color[1] + 0.4f,
              this->color[2] + 0.4f,
              this->color[3]);
    glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] + 0.4f,
              this->color[1] + 0.4f,
              this->color[2] + 0.4f,
              this->color[3]);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
    glEnd();

    // draw bar lines
    glBegin(GL_LINES);
    glColor4f(0, 0, 0, 1);
    glVertex3f(this->barXPos, this->barYPos + 1, this->barZPos);
    glVertex3f(
            this->barXPos + this->barWidth, this->barYPos + 1, this->barZPos);
    glEnd();
    glBegin(GL_LINES);
    glColor4f(0, 0, 0, 1);
    glVertex3f(this->barXPos, this->barYPos, this->barZPos);
    glVertex3f(this->barXPos + this->barWidth, this->barYPos, this->barZPos);
    glEnd();
    glBegin(GL_LINES);
    glColor4f(0, 0, 0, 1);
    glVertex3f(this->barXPos, this->barYPos - 1, this->barZPos);
    glVertex3f(
            this->barXPos + this->barWidth, this->barYPos - 1, this->barZPos);
    glEnd();
    for (int i = 0; i < this->numberOfOptions; i++) {
        glBegin(GL_LINES);
        glColor3f(0, 0, 0);
        glVertex3f(barXPos + (interval * i),
                   barYPos + this->height * 0.07,
                   barZPos);
        glVertex3f(barXPos + (interval * i),
                   barYPos - this->height * 0.07,
                   barZPos);
        glEnd();
    }

    // draw slider
    if (!isSliderClicked) {
        glBegin(GL_QUADS);
        glColor4f(this->color[0] + 0.2f,
                  this->color[1] + 0.2f,
                  this->color[2] + 0.2f,
                  this->color[3]);
        glVertex3f(this->sliderXPos, this->sliderYPos, this->sliderZPos);
        glVertex3f(
                this->sliderXPos - 3, this->sliderYPos + 3, this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth + 3,
                   this->sliderYPos + 3,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth,
                   this->sliderYPos,
                   this->sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(this->color[0] + 0.2f,
                  this->color[1] + 0.2f,
                  this->color[2] + 0.2f,
                  this->color[3]);
        glVertex3f(
                this->sliderXPos - 3, this->sliderYPos + 3, this->sliderZPos);
        glVertex3f(this->sliderXPos - 3,
                   this->sliderYPos - sliderHeight - 3,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos,
                   this->sliderYPos - sliderHeight,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos, this->sliderYPos, this->sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(this->color[0],
                  this->color[1],
                  this->color[2],
                  this->color[3]);
        glVertex3f(this->sliderXPos, this->sliderYPos, this->sliderZPos);
        glVertex3f(this->sliderXPos,
                   this->sliderYPos - sliderHeight,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth,
                   this->sliderYPos - sliderHeight,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth,
                   this->sliderYPos,
                   this->sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(this->color[0] - 0.4f,
                  this->color[1] - 0.4f,
                  this->color[2] - 0.4f,
                  this->color[3]);
        glVertex3f(this->sliderXPos - 3,
                   this->sliderYPos - sliderHeight - 3,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth + 3,
                   this->sliderYPos - sliderHeight - 3,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth,
                   this->sliderYPos - sliderHeight,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos,
                   this->sliderYPos - sliderHeight,
                   this->sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(this->color[0] - 0.4f,
                  this->color[1] - 0.4f,
                  this->color[2] - 0.4f,
                  this->color[3]);
        glVertex3f(this->sliderXPos + sliderWidth,
                   this->sliderYPos,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth + 3,
                   this->sliderYPos + 3,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth + 3,
                   this->sliderYPos - sliderHeight - 3,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth,
                   this->sliderYPos + -sliderHeight,
                   this->sliderZPos);
        glEnd();

    } else {
        glBegin(GL_QUADS);
        glColor4f(this->color[0] + 0.4f,
                  this->color[1] + 0.4f,
                  this->color[2] + 0.4f,
                  this->color[3]);
        glVertex3f(this->sliderXPos, this->sliderYPos, this->sliderZPos);
        glVertex3f(
                this->sliderXPos - 3, this->sliderYPos + 3, this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth + 3,
                   this->sliderYPos + 3,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth,
                   this->sliderYPos,
                   this->sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(this->color[0] + 0.4f,
                  this->color[1] + 0.4f,
                  this->color[2] + 0.4f,
                  this->color[3]);
        glVertex3f(
                this->sliderXPos - 3, this->sliderYPos + 3, this->sliderZPos);
        glVertex3f(this->sliderXPos - 3,
                   this->sliderYPos - sliderHeight - 3,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos,
                   this->sliderYPos - sliderHeight,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos, this->sliderYPos, this->sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(this->color[0] + 0.2f,
                  this->color[1] + 0.2f,
                  this->color[2] + 0.2f,
                  this->color[3]);
        glVertex3f(this->sliderXPos, this->sliderYPos, this->sliderZPos);
        glVertex3f(this->sliderXPos,
                   this->sliderYPos - sliderHeight,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth,
                   this->sliderYPos - sliderHeight,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth,
                   this->sliderYPos,
                   this->sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(this->color[0] - 0.2f,
                  this->color[1] - 0.2f,
                  this->color[2] - 0.2f,
                  this->color[3]);
        glVertex3f(this->sliderXPos - 3,
                   this->sliderYPos - sliderHeight - 3,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth + 3,
                   this->sliderYPos - sliderHeight - 3,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth,
                   this->sliderYPos - sliderHeight,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos,
                   this->sliderYPos - sliderHeight,
                   this->sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(this->color[0] - 0.2f,
                  this->color[1] - 0.2f,
                  this->color[2] - 0.2f,
                  this->color[3]);
        glVertex3f(this->sliderXPos + sliderWidth,
                   this->sliderYPos,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth + 3,
                   this->sliderYPos + 3,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth + 3,
                   this->sliderYPos - sliderHeight - 3,
                   this->sliderZPos);
        glVertex3f(this->sliderXPos + sliderWidth,
                   this->sliderYPos + -sliderHeight,
                   this->sliderZPos);
        glEnd();
    }

    this->label->draw();
    this->optionText->draw();
}

GLfloat ControlItemSliderbar::getXPos() { return this->xPos; }
GLfloat ControlItemSliderbar::getYPos() { return this->yPos; }
GLfloat ControlItemSliderbar::getHeight() { return this->height; }
GLfloat ControlItemSliderbar::getWidth() { return this->width; }
GLfloat ControlItemSliderbar::getBarXPos() { return this->barXPos; }
GLfloat ControlItemSliderbar::getInterval() { return this->interval; }
GLfloat ControlItemSliderbar::getSliderXPos() { return this->sliderXPos; }
void ControlItemSliderbar::setSliderXPos(GLfloat x) { this->sliderXPos = x; }
std::string ControlItemSliderbar::collectData() { return this->currentOption; }

void ControlItemSliderbar::setOptionText(const std::string& newText) {}

void ControlItemSliderbar::setOptionText(int index) {
    this->currentOption = this->allOptions[index];
    int real_length = 0;
    for (char ch : this->currentOption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = this->xPos + (this->width / 2) - (real_length / 2);
    GLfloat label_y_pos = this->yPos - this->height * 0.45;
    delete optionText;
    this->optionText = new TextObject(this->currentOption,
                                      label_x_pos,
                                      label_y_pos,
                                      this->zPos,
                                      GLUT_BITMAP_TIMES_ROMAN_24,
                                      0.0f,
                                      0.0f,
                                      0.0f);
    this->menuState = index;
    this->sliderXPos = this->barXPos + interval * index;
}

void ControlItemSliderbar::mouseClickEvent(
        GLint x, GLint y, GLint state, bool stillOverControlItemSliderbar) {
    if (state == 1) {
        // check if the click is on the slider
        if ((sliderXPos < x && x < sliderXPos + sliderWidth) &&
            (sliderYPos - sliderHeight < y && y < sliderYPos)) {
            this->isSliderClicked = true;
        }
        // check if the click is either left or right side from the slider
        else if ((barXPos < x && x < sliderXPos) &&
                 (sliderYPos - sliderHeight - 5.5 < y &&
                  y < sliderYPos + 5.5)) {
            playSFX(SMALL_CLICK);
            sliderXPos -= interval;
            this->menuState--;
            if (this->menuState < 0)  // wrap around check
                this->menuState += numberOfOptions;
            this->setOptionText(menuState);
        } else if ((sliderXPos + sliderWidth < x && x < barXPos + barWidth) &&
                   (sliderYPos - sliderHeight - 5.5 < y &&
                    y < sliderYPos + 5.5)) {
            playSFX(SMALL_CLICK);
            sliderXPos += interval;
            this->menuState++;
            if (this->menuState == this->numberOfOptions)  // wrap around check
                this->menuState -= numberOfOptions;
            this->setOptionText(menuState);
        }
    } else {
        this->isSliderClicked = false;
    }
}

void ControlItemSliderbar::updateMouse(int x, int y) {
    if (isSliderClicked) {
        // check if the mouse pointer is either left or right side from the
        // slider
        if (barXPos < x && x < sliderXPos - interval / 2) {
            sliderXPos -= interval;
            this->menuState--;
            if (this->menuState < 0)  // wrap around check
                this->menuState += numberOfOptions;
            this->setOptionText(menuState);
            playSFX(SMALL_CLICK);
        } else if (sliderXPos + interval / 2 + sliderWidth < x &&
                   x < barXPos + barWidth) {
            sliderXPos += interval;
            this->menuState++;
            if (this->menuState == this->numberOfOptions)  // wrap around check
                this->menuState -= numberOfOptions;
            this->setOptionText(menuState);
            playSFX(SMALL_CLICK);
        }
    }
}
