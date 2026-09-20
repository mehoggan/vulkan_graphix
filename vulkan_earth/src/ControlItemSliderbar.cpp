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
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    this->width = width;
    this->height = height;

    barWidth = width - width * 0.1;
    barXPos = xPos + (width - barWidth) / 2;
    barYPos = yPos - height / 1.5;
    barZPos = zPos + 0.5;

    sliderXPos = barXPos - barWidth * 0.012;
    sliderYPos = yPos - height / 1.7;
    sliderZPos = barZPos + 0.5;
    sliderWidth = width / 5 * 0.1;
    sliderHeight = height * 0.15;

    this->caption = caption;
    menuInfo = menuString;
    isSliderClicked = false;

    // split menuInfo on '/' into allOptions
    std::string current;
    for (char ch : menuInfo) {
        if (ch == '/') {
            allOptions.push_back(current);
            current.clear();
        } else {
            current += ch;
        }
    }
    numberOfOptions = static_cast<int>(allOptions.size());
    interval = barWidth / (numberOfOptions -
                           1.0);  // if it's divided by an integer, the whole
                                  // thing becomes an integer value???

    menuState = sliderStartingIndex;
    buttonState = 0;
    optionText = nullptr;
    setOptionText(menuState);  // set option to first option
    /*	BUTTON TEXT PLACEMENT	*/
    int real_length = 0;
    for (char ch : this->caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = barXPos;
    GLfloat label_y_pos = this->yPos - this->height * 0.45;
    /*	END OF BUTTON TEXT PLACEMENT	*/
    label = new TextObject(this->caption,
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
    glColor4f(color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
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
    glColor4f(color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
    glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
    glEnd();

    // draw bar lines
    glBegin(GL_LINES);
    glColor4f(0, 0, 0, 1);
    glVertex3f(barXPos, barYPos + 1, barZPos);
    glVertex3f(barXPos + barWidth, barYPos + 1, barZPos);
    glEnd();
    glBegin(GL_LINES);
    glColor4f(0, 0, 0, 1);
    glVertex3f(barXPos, barYPos, barZPos);
    glVertex3f(barXPos + barWidth, barYPos, barZPos);
    glEnd();
    glBegin(GL_LINES);
    glColor4f(0, 0, 0, 1);
    glVertex3f(barXPos, barYPos - 1, barZPos);
    glVertex3f(barXPos + barWidth, barYPos - 1, barZPos);
    glEnd();
    for (int i = 0; i < numberOfOptions; i++) {
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
        glColor4f(color[0] + 0.2f, color[1] + 0.2f, color[2] + 0.2f, color[3]);
        glVertex3f(sliderXPos, sliderYPos, sliderZPos);
        glVertex3f(sliderXPos - 3, sliderYPos + 3, sliderZPos);
        glVertex3f(sliderXPos + sliderWidth + 3, sliderYPos + 3, sliderZPos);
        glVertex3f(sliderXPos + sliderWidth, sliderYPos, sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] + 0.2f, color[1] + 0.2f, color[2] + 0.2f, color[3]);
        glVertex3f(sliderXPos - 3, sliderYPos + 3, sliderZPos);
        glVertex3f(sliderXPos - 3, sliderYPos - sliderHeight - 3, sliderZPos);
        glVertex3f(sliderXPos, sliderYPos - sliderHeight, sliderZPos);
        glVertex3f(sliderXPos, sliderYPos, sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0], color[1], color[2], color[3]);
        glVertex3f(sliderXPos, sliderYPos, sliderZPos);
        glVertex3f(sliderXPos, sliderYPos - sliderHeight, sliderZPos);
        glVertex3f(sliderXPos + sliderWidth,
                   sliderYPos - sliderHeight,
                   sliderZPos);
        glVertex3f(sliderXPos + sliderWidth, sliderYPos, sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - 0.4f, color[1] - 0.4f, color[2] - 0.4f, color[3]);
        glVertex3f(sliderXPos - 3, sliderYPos - sliderHeight - 3, sliderZPos);
        glVertex3f(sliderXPos + sliderWidth + 3,
                   sliderYPos - sliderHeight - 3,
                   sliderZPos);
        glVertex3f(sliderXPos + sliderWidth,
                   sliderYPos - sliderHeight,
                   sliderZPos);
        glVertex3f(sliderXPos, sliderYPos - sliderHeight, sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - 0.4f, color[1] - 0.4f, color[2] - 0.4f, color[3]);
        glVertex3f(sliderXPos + sliderWidth, sliderYPos, sliderZPos);
        glVertex3f(sliderXPos + sliderWidth + 3, sliderYPos + 3, sliderZPos);
        glVertex3f(sliderXPos + sliderWidth + 3,
                   sliderYPos - sliderHeight - 3,
                   sliderZPos);
        glVertex3f(sliderXPos + sliderWidth,
                   sliderYPos + -sliderHeight,
                   sliderZPos);
        glEnd();

    } else {
        glBegin(GL_QUADS);
        glColor4f(color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
        glVertex3f(sliderXPos, sliderYPos, sliderZPos);
        glVertex3f(sliderXPos - 3, sliderYPos + 3, sliderZPos);
        glVertex3f(sliderXPos + sliderWidth + 3, sliderYPos + 3, sliderZPos);
        glVertex3f(sliderXPos + sliderWidth, sliderYPos, sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
        glVertex3f(sliderXPos - 3, sliderYPos + 3, sliderZPos);
        glVertex3f(sliderXPos - 3, sliderYPos - sliderHeight - 3, sliderZPos);
        glVertex3f(sliderXPos, sliderYPos - sliderHeight, sliderZPos);
        glVertex3f(sliderXPos, sliderYPos, sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] + 0.2f, color[1] + 0.2f, color[2] + 0.2f, color[3]);
        glVertex3f(sliderXPos, sliderYPos, sliderZPos);
        glVertex3f(sliderXPos, sliderYPos - sliderHeight, sliderZPos);
        glVertex3f(sliderXPos + sliderWidth,
                   sliderYPos - sliderHeight,
                   sliderZPos);
        glVertex3f(sliderXPos + sliderWidth, sliderYPos, sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
        glVertex3f(sliderXPos - 3, sliderYPos - sliderHeight - 3, sliderZPos);
        glVertex3f(sliderXPos + sliderWidth + 3,
                   sliderYPos - sliderHeight - 3,
                   sliderZPos);
        glVertex3f(sliderXPos + sliderWidth,
                   sliderYPos - sliderHeight,
                   sliderZPos);
        glVertex3f(sliderXPos, sliderYPos - sliderHeight, sliderZPos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
        glVertex3f(sliderXPos + sliderWidth, sliderYPos, sliderZPos);
        glVertex3f(sliderXPos + sliderWidth + 3, sliderYPos + 3, sliderZPos);
        glVertex3f(sliderXPos + sliderWidth + 3,
                   sliderYPos - sliderHeight - 3,
                   sliderZPos);
        glVertex3f(sliderXPos + sliderWidth,
                   sliderYPos + -sliderHeight,
                   sliderZPos);
        glEnd();
    }

    label->draw();
    optionText->draw();
}

GLfloat ControlItemSliderbar::getXPos() { return this->xPos; }
GLfloat ControlItemSliderbar::getYPos() { return this->yPos; }
GLfloat ControlItemSliderbar::getHeight() { return this->height; }
GLfloat ControlItemSliderbar::getWidth() { return this->width; }
GLfloat ControlItemSliderbar::getBarXPos() { return barXPos; }
GLfloat ControlItemSliderbar::getInterval() { return interval; }
GLfloat ControlItemSliderbar::getSliderXPos() { return sliderXPos; }
void ControlItemSliderbar::setSliderXPos(GLfloat x) { sliderXPos = x; }
std::string ControlItemSliderbar::collectData() { return currentOption; }

void ControlItemSliderbar::setOptionText(const std::string& newText) {}

void ControlItemSliderbar::setOptionText(int index) {
    currentOption = allOptions[index];
    int real_length = 0;
    for (char ch : currentOption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = this->xPos + (this->width / 2) - (real_length / 2);
    GLfloat label_y_pos = this->yPos - this->height * 0.45;
    delete optionText;
    optionText = new TextObject(currentOption,
                                label_x_pos,
                                label_y_pos,
                                this->zPos,
                                GLUT_BITMAP_TIMES_ROMAN_24,
                                0.0f,
                                0.0f,
                                0.0f);
    menuState = index;
    sliderXPos = barXPos + interval * index;
}

void ControlItemSliderbar::mouseClickEvent(
        GLint x, GLint y, GLint state, bool stillOverControlItemSliderbar) {
    if (state == 1) {
        // check if the click is on the slider
        if ((sliderXPos < x && x < sliderXPos + sliderWidth) &&
            (sliderYPos - sliderHeight < y && y < sliderYPos)) {
            isSliderClicked = true;
        }
        // check if the click is either left or right side from the slider
        else if ((barXPos < x && x < sliderXPos) &&
                 (sliderYPos - sliderHeight - 5.5 < y &&
                  y < sliderYPos + 5.5)) {
            playSFX(SMALL_CLICK);
            sliderXPos -= interval;
            menuState--;
            if (menuState < 0)  // wrap around check
                menuState += numberOfOptions;
            setOptionText(menuState);
        } else if ((sliderXPos + sliderWidth < x && x < barXPos + barWidth) &&
                   (sliderYPos - sliderHeight - 5.5 < y &&
                    y < sliderYPos + 5.5)) {
            playSFX(SMALL_CLICK);
            sliderXPos += interval;
            menuState++;
            if (menuState == numberOfOptions)  // wrap around check
                menuState -= numberOfOptions;
            setOptionText(menuState);
        }
    } else {
        isSliderClicked = false;
    }
}

void ControlItemSliderbar::updateMouse(int x, int y) {
    if (isSliderClicked) {
        // check if the mouse pointer is either left or right side from the
        // slider
        if (barXPos < x && x < sliderXPos - interval / 2) {
            sliderXPos -= interval;
            menuState--;
            if (menuState < 0)  // wrap around check
                menuState += numberOfOptions;
            setOptionText(menuState);
            playSFX(SMALL_CLICK);
        } else if (sliderXPos + interval / 2 + sliderWidth < x &&
                   x < barXPos + barWidth) {
            sliderXPos += interval;
            menuState++;
            if (menuState == numberOfOptions)  // wrap around check
                menuState -= numberOfOptions;
            setOptionText(menuState);
            playSFX(SMALL_CLICK);
        }
    }
}
