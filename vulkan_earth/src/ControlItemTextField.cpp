#include "ControlItemTextField.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "ControlItem.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

using namespace std;

const int max_chars = 15;

extern void playSFX(int sfx);

ControlItemTextField::ControlItemTextField() = default;

ControlItemTextField::ControlItemTextField(GLfloat xPos,
                                           GLfloat yPos,
                                           GLfloat zPos,
                                           GLfloat red,
                                           GLfloat green,
                                           GLfloat blue,
                                           GLint width,
                                           GLint height) {
    this->xPos = xPos;
    this->yPos = yPos;
    this->zPos = zPos;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    this->width = width;
    this->height = height;
    currentText = nullptr;
    textFieldActive = false;
    clearTextBuffer();
    currentLength = 0;
    numberOfFrames = 0;
    textCursorOn = 1;
    setOptionText("");
}

ControlItemTextField::~ControlItemTextField() { delete currentText; }

void ControlItemTextField::draw() {
    if (numberOfFrames == 50) {
        textCursorOn *= -1;  // toggle
        numberOfFrames = 0;
    }
    numberOfFrames++;

    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.6f, color[1] - 0.6f, color[2] - 0.6f, color[3]);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.6f, color[1] - 0.6f, color[2] - 0.6f, color[3]);
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
    glColor4f(color[0] - 0.3f, color[1] - 0.3f, color[2] - 0.3f, color[3]);
    glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.3f, color[1] - 0.3f, color[2] - 0.3f, color[3]);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
    glEnd();

    if (textFieldActive) {
        if (textCursorOn == 1) {
            int real_length = 0;
            for (char ch : currentChars) {
                if (ch != ' ') {
                    real_length +=
                            glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
                }
            }
            glBegin(GL_QUADS);
            glColor3f(0, 0, 0);
            glVertex3f(this->xPos + 0.02 * this->width + real_length,
                       this->yPos - 0.15 * height,
                       this->zPos + 0.1);
            glVertex3f(this->xPos + 0.02 * this->width + real_length,
                       this->yPos - this->height + 0.15 * height,
                       this->zPos + 0.1);
            glVertex3f(this->xPos + 0.02 * this->width + real_length + 2,
                       this->yPos - this->height + 0.15 * height,
                       this->zPos + 0.1);
            glVertex3f(this->xPos + 0.02 * this->width + real_length + 2,
                       this->yPos - 0.15 * height,
                       this->zPos + 0.1);
            glEnd();
        }
    }

    if (currentText) currentText->draw();
}

GLfloat ControlItemTextField::getXPos() { return this->xPos; }
GLfloat ControlItemTextField::getYPos() { return this->yPos; }
GLfloat ControlItemTextField::getHeight() { return this->height; }
GLfloat ControlItemTextField::getWidth() { return this->width; }
bool ControlItemTextField::isTextFieldActive() { return textFieldActive; }
std::string ControlItemTextField::collectData() {
    return currentText->getOutput();
}
void ControlItemTextField::deactivate() { textFieldActive = false; }
void ControlItemTextField::setOptionText(int index) {}

void ControlItemTextField::setOptionText(const std::string& newText) {
    delete currentText;

    int real_length = 0;
    for (char ch : newText) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = this->xPos + 0.02 * this->width;
    GLfloat label_y_pos = this->yPos +
                          ((this->yPos - (this->yPos + this->height)) / 2) -
                          this->height / 4;
    currentText = new TextObject(newText,
                                 label_x_pos,
                                 label_y_pos,
                                 this->zPos + 0.1,
                                 GLUT_BITMAP_TIMES_ROMAN_24,
                                 0.0f,
                                 0.0f,
                                 0.0f);
}

void ControlItemTextField::mouseClickEvent(
        GLint x, GLint y, GLint state, bool stillOverControlItemTextField) {
    if (state == 0)
        if (stillOverControlItemTextField) textFieldActive = true;
}

void ControlItemTextField::updateMouse(int x, int y) {}
void ControlItemTextField::keyHandler(unsigned char key) {
    if (textFieldActive) {
        if ((((key >= 48) && (key <= 57)) || ((key >= 65) && (key < 90))) ||
            ((key >= 97) && (key <= 122))) {
            if (currentText == nullptr) {
                setOptionText("");
                currentLength = 0;
            }
            if (currentLength < max_chars) {
                playSFX(KEYTYPING);
                currentChars[currentLength] = key;
                currentLength++;
                setOptionText(currentChars);
            }

        } else if (key == 8) {
            if (currentText) {
                if (currentLength != 0) {
                    playSFX(KEYTYPING);
                    currentChars[currentLength - 1] = ' ';
                    currentLength--;
                    setOptionText(currentChars);
                }
            }
        }
    }
}

void ControlItemTextField::clearTextBuffer() {
    currentChars.assign(max_chars, ' ');
    currentLength = 0;
}

void ControlItemTextField::setTextBuffer(const std::string& newText) {
    setOptionText(newText);
    clearTextBuffer();
    int new_length = 0;
    for (size_t i = 0; i < newText.size() && i < currentChars.size(); i++) {
        currentChars[i] = newText[i];
        if (newText[i] != ' ') new_length++;
    }
    currentLength = new_length;
}