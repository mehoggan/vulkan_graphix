#include "ControlItemSliderbar.h"
#include <stdio.h>
#include <iostream>
#include "ControlItem.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

extern void playSFX(int sfx);

ControlItemSliderbar::ControlItemSliderbar() = default;
ControlItemSliderbar::ControlItemSliderbar(GLfloat x_pos,
                                           GLfloat y_pos,
                                           GLfloat z_pos,
                                           GLfloat red,
                                           GLfloat green,
                                           GLfloat blue,
                                           GLint width,
                                           GLint height,
                                           const std::string& caption,
                                           const std::string& menu_string,
                                           int slider_starting_index) {
    this->x_pos = x_pos;
    this->y_pos = y_pos;
    this->z_pos = z_pos;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    this->width = width;
    this->height = height;

    bar_width = width - width * 0.1;
    bar_x_pos = x_pos + (width - bar_width) / 2;
    bar_y_pos = y_pos - height / 1.5;
    bar_z_pos = z_pos + 0.5;

    slider_x_pos = bar_x_pos - bar_width * 0.012;
    slider_y_pos = y_pos - height / 1.7;
    slider_z_pos = bar_z_pos + 0.5;
    slider_width = width / 5 * 0.1;
    slider_height = height * 0.15;

    this->caption = caption;
    menu_info = menu_string;
    is_slider_clicked = false;

    // split menuInfo on '/' into allOptions
    std::string current;
    for (char ch : menu_info) {
        if (ch == '/') {
            all_options.push_back(current);
            current.clear();
        } else {
            current += ch;
        }
    }
    number_of_options = static_cast<int>(all_options.size());
    interval = bar_width / (number_of_options -
                            1.0);  // if it's divided by an integer, the whole
                                   // thing becomes an integer value???

    menu_state = slider_starting_index;
    button_state = 0;
    option_text = nullptr;
    setOptionText(menu_state);  // set option to first option
    /*	BUTTON TEXT PLACEMENT	*/
    int real_length = 0;
    for (char ch : this->caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = bar_x_pos;
    GLfloat label_y_pos = this->y_pos - this->height * 0.45;
    /*	END OF BUTTON TEXT PLACEMENT	*/
    label = new TextObject(this->caption,
                           label_x_pos,
                           label_y_pos,
                           this->z_pos,
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
}
ControlItemSliderbar::~ControlItemSliderbar() {
    delete option_text;
    delete label;
}

void ControlItemSliderbar::draw() {
    // draw main button box
    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
    glVertex3f(this->x_pos, this->y_pos, this->z_pos);
    glVertex3f(this->x_pos - 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
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
    glColor4f(color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
    glVertex3f(this->x_pos - 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos - height, this->z_pos);
    glVertex3f(this->x_pos, this->y_pos - height, this->z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
    glVertex3f(this->x_pos + width, this->y_pos, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos + 3, this->z_pos);
    glVertex3f(this->x_pos + width + 3, this->y_pos - height - 3, this->z_pos);
    glVertex3f(this->x_pos + width, this->y_pos + -height, this->z_pos);
    glEnd();

    // draw bar lines
    glBegin(GL_LINES);
    glColor4f(0, 0, 0, 1);
    glVertex3f(bar_x_pos, bar_y_pos + 1, bar_z_pos);
    glVertex3f(bar_x_pos + bar_width, bar_y_pos + 1, bar_z_pos);
    glEnd();
    glBegin(GL_LINES);
    glColor4f(0, 0, 0, 1);
    glVertex3f(bar_x_pos, bar_y_pos, bar_z_pos);
    glVertex3f(bar_x_pos + bar_width, bar_y_pos, bar_z_pos);
    glEnd();
    glBegin(GL_LINES);
    glColor4f(0, 0, 0, 1);
    glVertex3f(bar_x_pos, bar_y_pos - 1, bar_z_pos);
    glVertex3f(bar_x_pos + bar_width, bar_y_pos - 1, bar_z_pos);
    glEnd();
    for (int i = 0; i < number_of_options; i++) {
        glBegin(GL_LINES);
        glColor3f(0, 0, 0);
        glVertex3f(bar_x_pos + (interval * i),
                   bar_y_pos + this->height * 0.07,
                   bar_z_pos);
        glVertex3f(bar_x_pos + (interval * i),
                   bar_y_pos - this->height * 0.07,
                   bar_z_pos);
        glEnd();
    }

    // draw slider
    if (!is_slider_clicked) {
        glBegin(GL_QUADS);
        glColor4f(color[0] + 0.2f, color[1] + 0.2f, color[2] + 0.2f, color[3]);
        glVertex3f(slider_x_pos, slider_y_pos, slider_z_pos);
        glVertex3f(slider_x_pos - 3, slider_y_pos + 3, slider_z_pos);
        glVertex3f(slider_x_pos + slider_width + 3,
                   slider_y_pos + 3,
                   slider_z_pos);
        glVertex3f(slider_x_pos + slider_width, slider_y_pos, slider_z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] + 0.2f, color[1] + 0.2f, color[2] + 0.2f, color[3]);
        glVertex3f(slider_x_pos - 3, slider_y_pos + 3, slider_z_pos);
        glVertex3f(slider_x_pos - 3,
                   slider_y_pos - slider_height - 3,
                   slider_z_pos);
        glVertex3f(slider_x_pos, slider_y_pos - slider_height, slider_z_pos);
        glVertex3f(slider_x_pos, slider_y_pos, slider_z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0], color[1], color[2], color[3]);
        glVertex3f(slider_x_pos, slider_y_pos, slider_z_pos);
        glVertex3f(slider_x_pos, slider_y_pos - slider_height, slider_z_pos);
        glVertex3f(slider_x_pos + slider_width,
                   slider_y_pos - slider_height,
                   slider_z_pos);
        glVertex3f(slider_x_pos + slider_width, slider_y_pos, slider_z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - 0.4f, color[1] - 0.4f, color[2] - 0.4f, color[3]);
        glVertex3f(slider_x_pos - 3,
                   slider_y_pos - slider_height - 3,
                   slider_z_pos);
        glVertex3f(slider_x_pos + slider_width + 3,
                   slider_y_pos - slider_height - 3,
                   slider_z_pos);
        glVertex3f(slider_x_pos + slider_width,
                   slider_y_pos - slider_height,
                   slider_z_pos);
        glVertex3f(slider_x_pos, slider_y_pos - slider_height, slider_z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - 0.4f, color[1] - 0.4f, color[2] - 0.4f, color[3]);
        glVertex3f(slider_x_pos + slider_width, slider_y_pos, slider_z_pos);
        glVertex3f(slider_x_pos + slider_width + 3,
                   slider_y_pos + 3,
                   slider_z_pos);
        glVertex3f(slider_x_pos + slider_width + 3,
                   slider_y_pos - slider_height - 3,
                   slider_z_pos);
        glVertex3f(slider_x_pos + slider_width,
                   slider_y_pos + -slider_height,
                   slider_z_pos);
        glEnd();

    } else {
        glBegin(GL_QUADS);
        glColor4f(color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
        glVertex3f(slider_x_pos, slider_y_pos, slider_z_pos);
        glVertex3f(slider_x_pos - 3, slider_y_pos + 3, slider_z_pos);
        glVertex3f(slider_x_pos + slider_width + 3,
                   slider_y_pos + 3,
                   slider_z_pos);
        glVertex3f(slider_x_pos + slider_width, slider_y_pos, slider_z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
        glVertex3f(slider_x_pos - 3, slider_y_pos + 3, slider_z_pos);
        glVertex3f(slider_x_pos - 3,
                   slider_y_pos - slider_height - 3,
                   slider_z_pos);
        glVertex3f(slider_x_pos, slider_y_pos - slider_height, slider_z_pos);
        glVertex3f(slider_x_pos, slider_y_pos, slider_z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] + 0.2f, color[1] + 0.2f, color[2] + 0.2f, color[3]);
        glVertex3f(slider_x_pos, slider_y_pos, slider_z_pos);
        glVertex3f(slider_x_pos, slider_y_pos - slider_height, slider_z_pos);
        glVertex3f(slider_x_pos + slider_width,
                   slider_y_pos - slider_height,
                   slider_z_pos);
        glVertex3f(slider_x_pos + slider_width, slider_y_pos, slider_z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
        glVertex3f(slider_x_pos - 3,
                   slider_y_pos - slider_height - 3,
                   slider_z_pos);
        glVertex3f(slider_x_pos + slider_width + 3,
                   slider_y_pos - slider_height - 3,
                   slider_z_pos);
        glVertex3f(slider_x_pos + slider_width,
                   slider_y_pos - slider_height,
                   slider_z_pos);
        glVertex3f(slider_x_pos, slider_y_pos - slider_height, slider_z_pos);
        glEnd();
        glBegin(GL_QUADS);
        glColor4f(color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
        glVertex3f(slider_x_pos + slider_width, slider_y_pos, slider_z_pos);
        glVertex3f(slider_x_pos + slider_width + 3,
                   slider_y_pos + 3,
                   slider_z_pos);
        glVertex3f(slider_x_pos + slider_width + 3,
                   slider_y_pos - slider_height - 3,
                   slider_z_pos);
        glVertex3f(slider_x_pos + slider_width,
                   slider_y_pos + -slider_height,
                   slider_z_pos);
        glEnd();
    }

    label->draw();
    option_text->draw();
}

GLfloat ControlItemSliderbar::getXPos() { return this->x_pos; }
GLfloat ControlItemSliderbar::getYPos() { return this->y_pos; }
GLfloat ControlItemSliderbar::getHeight() { return this->height; }
GLfloat ControlItemSliderbar::getWidth() { return this->width; }
GLfloat ControlItemSliderbar::getBarXPos() { return bar_x_pos; }
GLfloat ControlItemSliderbar::getInterval() { return interval; }
GLfloat ControlItemSliderbar::getSliderXPos() { return slider_x_pos; }
void ControlItemSliderbar::setSliderXPos(GLfloat x) { slider_x_pos = x; }
std::string ControlItemSliderbar::collectData() { return current_option; }

void ControlItemSliderbar::setOptionText(const std::string& new_text) {}

void ControlItemSliderbar::setOptionText(int index) {
    current_option = all_options[index];
    int real_length = 0;
    for (char ch : current_option) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = this->x_pos + (this->width / 2) - (real_length / 2);
    GLfloat label_y_pos = this->y_pos - this->height * 0.45;
    delete option_text;
    option_text = new TextObject(current_option,
                                 label_x_pos,
                                 label_y_pos,
                                 this->z_pos,
                                 GLUT_BITMAP_TIMES_ROMAN_24,
                                 0.0f,
                                 0.0f,
                                 0.0f);
    menu_state = index;
    slider_x_pos = bar_x_pos + interval * index;
}

void ControlItemSliderbar::mouseClickEvent(
        GLint x,
        GLint y,
        GLint state,
        bool still_over_control_item_sliderbar) {
    if (state == 1) {
        // check if the click is on the slider
        if ((slider_x_pos < x && x < slider_x_pos + slider_width) &&
            (slider_y_pos - slider_height < y && y < slider_y_pos)) {
            is_slider_clicked = true;
        }
        // check if the click is either left or right side from the slider
        else if ((bar_x_pos < x && x < slider_x_pos) &&
                 (slider_y_pos - slider_height - 5.5 < y &&
                  y < slider_y_pos + 5.5)) {
            playSFX(SMALL_CLICK);
            slider_x_pos -= interval;
            menu_state--;
            if (menu_state < 0)  // wrap around check
                menu_state += number_of_options;
            setOptionText(menu_state);
        } else if ((slider_x_pos + slider_width < x &&
                    x < bar_x_pos + bar_width) &&
                   (slider_y_pos - slider_height - 5.5 < y &&
                    y < slider_y_pos + 5.5)) {
            playSFX(SMALL_CLICK);
            slider_x_pos += interval;
            menu_state++;
            if (menu_state == number_of_options)  // wrap around check
                menu_state -= number_of_options;
            setOptionText(menu_state);
        }
    } else {
        is_slider_clicked = false;
    }
}

void ControlItemSliderbar::updateMouse(int x, int y) {
    if (is_slider_clicked) {
        // check if the mouse pointer is either left or right side from the
        // slider
        if (bar_x_pos < x && x < slider_x_pos - interval / 2) {
            slider_x_pos -= interval;
            menu_state--;
            if (menu_state < 0)  // wrap around check
                menu_state += number_of_options;
            setOptionText(menu_state);
            playSFX(SMALL_CLICK);
        } else if (slider_x_pos + interval / 2 + slider_width < x &&
                   x < bar_x_pos + bar_width) {
            slider_x_pos += interval;
            menu_state++;
            if (menu_state == number_of_options)  // wrap around check
                menu_state -= number_of_options;
            setOptionText(menu_state);
            playSFX(SMALL_CLICK);
        }
    }
}
