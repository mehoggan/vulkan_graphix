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

ControlItemTextField::ControlItemTextField(GLfloat x_pos,
                                           GLfloat y_pos,
                                           GLfloat z_pos,
                                           GLfloat red,
                                           GLfloat green,
                                           GLfloat blue,
                                           GLint width,
                                           GLint height) {
    this->x_pos = x_pos;
    this->y_pos = y_pos;
    this->z_pos = z_pos;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    this->width = width;
    this->height = height;
    current_text = nullptr;
    text_field_active = false;
    clearTextBuffer();
    current_length = 0;
    number_of_frames = 0;
    text_cursor_on = 1;
    setOptionText("");
}

ControlItemTextField::~ControlItemTextField() { delete current_text; }

void ControlItemTextField::draw() {
    if (number_of_frames == 50) {
        text_cursor_on *= -1;  // toggle
        number_of_frames = 0;
    }
    number_of_frames++;

    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.6f, color[1] - 0.6f, color[2] - 0.6f, color[3]);
    glVertex3f(this->x_pos, this->y_pos, this->z_pos);
    glVertex3f(this->x_pos - 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.6f, color[1] - 0.6f, color[2] - 0.6f, color[3]);
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
    glColor4f(color[0] - 0.3f, color[1] - 0.3f, color[2] - 0.3f, color[3]);
    glVertex3f(this->x_pos - 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos);
    glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.3f, color[1] - 0.3f, color[2] - 0.3f, color[3]);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos + -height, this->z_pos);
    glEnd();

    if (text_field_active) {
        if (text_cursor_on == 1) {
            int real_length = 0;
            for (char ch : current_chars) {
                if (ch != ' ') {
                    real_length +=
                            glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
                }
            }
            glBegin(GL_QUADS);
            glColor3f(0, 0, 0);
            glVertex3f(this->x_pos + 0.02 * this->width + real_length,
                       this->y_pos - 0.15 * height,
                       this->z_pos + 0.1);
            glVertex3f(this->x_pos + 0.02 * this->width + real_length,
                       this->y_pos - this->height + 0.15 * height,
                       this->z_pos + 0.1);
            glVertex3f(this->x_pos + 0.02 * this->width + real_length + 2,
                       this->y_pos - this->height + 0.15 * height,
                       this->z_pos + 0.1);
            glVertex3f(this->x_pos + 0.02 * this->width + real_length + 2,
                       this->y_pos - 0.15 * height,
                       this->z_pos + 0.1);
            glEnd();
        }
    }

    if (current_text) current_text->draw();
}

GLfloat ControlItemTextField::getXPos() { return this->x_pos; }
GLfloat ControlItemTextField::getYPos() { return this->y_pos; }
GLfloat ControlItemTextField::getHeight() { return this->height; }
GLfloat ControlItemTextField::getWidth() { return this->width; }
bool ControlItemTextField::isTextFieldActive() { return text_field_active; }
std::string ControlItemTextField::collectData() {
    return current_text->getOutput();
}
void ControlItemTextField::deactivate() { text_field_active = false; }
void ControlItemTextField::setOptionText(int index) {}

void ControlItemTextField::setOptionText(const std::string& new_text) {
    delete current_text;

    int real_length = 0;
    for (char ch : new_text) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = this->x_pos + 0.02 * this->width;
    GLfloat label_y_pos = this->y_pos +
                          ((this->y_pos - (this->y_pos + this->height)) / 2) -
                          this->height / 4;
    current_text = new TextObject(new_text,
                                  label_x_pos,
                                  label_y_pos,
                                  this->z_pos + 0.1,
                                  GLUT_BITMAP_TIMES_ROMAN_24,
                                  0.0f,
                                  0.0f,
                                  0.0f);
}

void ControlItemTextField::mouseClickEvent(
        GLint x,
        GLint y,
        GLint state,
        bool still_over_control_item_text_field) {
    if (state == 0)
        if (still_over_control_item_text_field) text_field_active = true;
}

void ControlItemTextField::updateMouse(int x, int y) {}
void ControlItemTextField::keyHandler(unsigned char key) {
    if (text_field_active) {
        if ((((key >= 48) && (key <= 57)) || ((key >= 65) && (key < 90))) ||
            ((key >= 97) && (key <= 122))) {
            if (current_text == nullptr) {
                setOptionText("");
                current_length = 0;
            }
            if (current_length < max_chars) {
                playSFX(KEYTYPING);
                current_chars[current_length] = key;
                current_length++;
                setOptionText(current_chars);
            }

        } else if (key == 8) {
            if (current_text) {
                if (current_length != 0) {
                    playSFX(KEYTYPING);
                    current_chars[current_length - 1] = ' ';
                    current_length--;
                    setOptionText(current_chars);
                }
            }
        }
    }
}

void ControlItemTextField::clearTextBuffer() {
    current_chars.assign(max_chars, ' ');
    current_length = 0;
}

void ControlItemTextField::setTextBuffer(const std::string& new_text) {
    setOptionText(new_text);
    clearTextBuffer();
    int new_length = 0;
    for (size_t i = 0; i < new_text.size() && i < current_chars.size(); i++) {
        current_chars[i] = new_text[i];
        if (new_text[i] != ' ') new_length++;
    }
    current_length = new_length;
}