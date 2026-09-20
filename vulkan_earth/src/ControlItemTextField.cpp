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
    this->color[0] = red;
    this->color[1] = green;
    this->color[2] = blue;
    this->color[3] = 1.0;
    this->width = width;
    this->height = height;
    this->currentText = nullptr;
    this->textFieldActive = false;
    this->clearTextBuffer();
    this->currentLength = 0;
    this->numberOfFrames = 0;
    this->textCursorOn = 1;
    this->setOptionText("");
}

ControlItemTextField::~ControlItemTextField() { delete this->currentText; }

void ControlItemTextField::draw() {
    if (numberOfFrames == 50) {
        this->textCursorOn *= -1;  // toggle
        this->numberOfFrames = 0;
    }
    this->numberOfFrames++;

    glBegin(GL_QUADS);
    glColor4f(this->color[0] - 0.6f,
              this->color[1] - 0.6f,
              this->color[2] - 0.6f,
              this->color[3]);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] - 0.6f,
              this->color[1] - 0.6f,
              this->color[2] - 0.6f,
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
    glColor4f(this->color[0] - 0.3f,
              this->color[1] - 0.3f,
              this->color[2] - 0.3f,
              this->color[3]);
    glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(this->color[0] - 0.3f,
              this->color[1] - 0.3f,
              this->color[2] - 0.3f,
              this->color[3]);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
    glEnd();

    if (this->textFieldActive) {
        if (this->textCursorOn == 1) {
            int real_length = 0;
            for (char ch : this->currentChars) {
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

    if (this->currentText) this->currentText->draw();
}

GLfloat ControlItemTextField::getXPos() { return this->xPos; }
GLfloat ControlItemTextField::getYPos() { return this->yPos; }
GLfloat ControlItemTextField::getHeight() { return this->height; }
GLfloat ControlItemTextField::getWidth() { return this->width; }
bool ControlItemTextField::isTextFieldActive() {
    return this->textFieldActive;
}
std::string ControlItemTextField::collectData() {
    return this->currentText->getOutput();
}
void ControlItemTextField::deactivate() { textFieldActive = false; }
void ControlItemTextField::setOptionText(int index) {}

void ControlItemTextField::setOptionText(const std::string& newText) {
    delete this->currentText;

    int real_length = 0;
    for (char ch : newText) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = this->xPos + 0.02 * this->width;
    GLfloat label_y_pos = this->yPos +
                          ((this->yPos - (this->yPos + this->height)) / 2) -
                          this->height / 4;
    this->currentText = new TextObject(newText,
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
        if (stillOverControlItemTextField) this->textFieldActive = true;
}

void ControlItemTextField::updateMouse(int x, int y) {}
void ControlItemTextField::keyHandler(unsigned char key) {
    if (this->textFieldActive) {
        if ((((key >= 48) && (key <= 57)) || ((key >= 65) && (key < 90))) ||
            ((key >= 97) && (key <= 122))) {
            if (this->currentText == nullptr) {
                this->setOptionText("");
                this->currentLength = 0;
            }
            if (currentLength < max_chars) {
                playSFX(KEYTYPING);
                this->currentChars[currentLength] = key;
                this->currentLength++;
                this->setOptionText(currentChars);
            }

        } else if (key == 8) {
            if (this->currentText) {
                if (currentLength != 0) {
                    playSFX(KEYTYPING);
                    this->currentChars[currentLength - 1] = ' ';
                    this->currentLength--;
                    this->setOptionText(currentChars);
                }
            }
        }
    }
}

void ControlItemTextField::clearTextBuffer() {
    this->currentChars.assign(max_chars, ' ');
    this->currentLength = 0;
}

void ControlItemTextField::setTextBuffer(const std::string& newText) {
    this->setOptionText(newText);
    this->clearTextBuffer();
    int new_length = 0;
    for (size_t i = 0; i < newText.size() && i < this->currentChars.size();
         i++) {
        this->currentChars[i] = newText[i];
        if (newText[i] != ' ') new_length++;
    }
    this->currentLength = new_length;
}