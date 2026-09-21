#include "ControlItemSelectionBox.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "ControlItem.h"
#include "TextObject.h"
#include "macro_crtdbg.h"
#include "sound.h"

using namespace std;

extern void playSFX(int sfx);

ControlItemSelectionBox::ControlItemSelectionBox() = default;

ControlItemSelectionBox::ControlItemSelectionBox(
        GLfloat new_x_pos,
        GLfloat new_y_pos,
        GLfloat new_z_pos,
        GLfloat red,
        GLfloat green,
        GLfloat blue,
        GLint new_width,
        GLint new_height,
        const std::string& new_caption,
        const std::string& menu_string) {
    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    width = new_width;
    height = new_height;
    caption = new_caption;
    menu_info = menu_string;

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

    menu_state = 0;
    button_state = 0;
    option_text = nullptr;
    setOptionText(menu_state);  // set option to first option
    /*	BUTTON TEXT PLACEMENT	*/
    int real_length = 0;
    for (char ch : caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = x_pos + (width / 2) - (real_length / 2);
    GLfloat label_y_pos =
            y_pos + ((y_pos - (y_pos + height)) / 2) - height / 4;
    /*	END OF BUTTON TEXT PLACEMENT	*/
    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           z_pos,
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
}

ControlItemSelectionBox::~ControlItemSelectionBox() {
    delete option_text;
    delete label;
}

void ControlItemSelectionBox::draw() {
    // draw main button box
    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
    glVertex3f(x_pos, y_pos, z_pos);
    glVertex3f(x_pos - 3, y_pos + 3, z_pos);
    glVertex3f(x_pos + width + 3, y_pos + 3, z_pos);
    glVertex3f(x_pos + width, y_pos, z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - 0.2f, color[1] - 0.2f, color[2] - 0.2f, color[3]);
    glVertex3f(x_pos - 3, y_pos + 3, z_pos);
    glVertex3f(x_pos - 3, y_pos - height - 3, z_pos);
    glVertex3f(x_pos, y_pos - height, z_pos);
    glVertex3f(x_pos, y_pos, z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0], color[1], color[2], color[3]);
    glVertex3f(x_pos, y_pos, z_pos);
    glVertex3f(x_pos, y_pos - height, z_pos);
    glVertex3f(x_pos + width, y_pos - height, z_pos);
    glVertex3f(x_pos + width, y_pos, z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
    glVertex3f(x_pos - 3, y_pos - height - 3, z_pos);
    glVertex3f(x_pos + width + 3, y_pos - height - 3, z_pos);
    glVertex3f(x_pos + width, y_pos - height, z_pos);
    glVertex3f(x_pos, y_pos - height, z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + 0.4f, color[1] + 0.4f, color[2] + 0.4f, color[3]);
    glVertex3f(x_pos + width, y_pos, z_pos);
    glVertex3f(x_pos + width + 3, y_pos + 3, z_pos);
    glVertex3f(x_pos + width + 3, y_pos - height - 3, z_pos);
    glVertex3f(x_pos + width, y_pos + -height, z_pos);
    glEnd();

    label->draw();
    option_text->draw();

    // draw up arrow
    glBegin(GL_TRIANGLES);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2); /*		3		*/
    glVertex3f(x_pos + 0.02 * width,
               (y_pos - height / 2) + 0.05 * height,
               z_pos + 1); /*	   / \		*/
    glVertex3f(x_pos + 0.02 * width + height * 0.7,
               (y_pos - height / 2) + 0.05 * height,
               z_pos + 1); /*	  /   \		*/
    if (button_state == 1)
        glColor3f(
                color[0] - .2, color[1] - .2, color[2] - .2); /*   1-----2	*/
    glVertex3f(x_pos + 0.02 * (width) + height * 0.35,
               y_pos - 0.05 * height,
               z_pos + 1);
    glEnd();

    // draw down arrow
    glBegin(GL_TRIANGLES);
    glColor3f(color[0] + .2, color[1] + .2, color[2] + .2); /*	1-----2		*/
    glVertex3f(x_pos + 0.02 * width,
               (y_pos - height / 2) - 0.05 * height,
               z_pos + 1); /*	 \   /		*/
    glVertex3f(x_pos + 0.02 * width + height * 0.7,
               (y_pos - height / 2) - 0.05 * height,
               z_pos + 1); /*	  \ /		*/
    if (button_state == 2)
        glColor3f(color[0] - .2,
                  color[1] - .2,
                  color[2] - .2); /*	   3		*/
    glVertex3f(x_pos + 0.02 * width + height * 0.35,
               y_pos - height + 0.05 * height,
               z_pos + 1);
    glEnd();
}

GLfloat ControlItemSelectionBox::getXPos() { return x_pos; }
GLfloat ControlItemSelectionBox::getYPos() { return y_pos; }
GLfloat ControlItemSelectionBox::getHeight() { return height; }
GLfloat ControlItemSelectionBox::getWidth() { return width; }
std::string ControlItemSelectionBox::collectData() { return current_option; }

void ControlItemSelectionBox::setOptionText(int index) {
    current_option = all_options[index];
    int real_length = 0;
    for (char ch : current_option) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = x_pos + width - real_length - width / 50;
    GLfloat label_y_pos =
            y_pos + ((y_pos - (y_pos + height)) / 2) - height / 4;
    delete option_text;
    option_text = new TextObject(current_option,
                                 label_x_pos,
                                 label_y_pos,
                                 z_pos,
                                 GLUT_BITMAP_TIMES_ROMAN_24,
                                 0.0f,
                                 0.0f,
                                 0.0f);
    menu_state = index;
}
void ControlItemSelectionBox::setOptionText(const std::string& new_text) {}

void ControlItemSelectionBox::mouseClickEvent(
        GLint x,
        GLint y,
        GLint state,
        bool still_over_control_item_selection_box) {
    // up arrow test
    if ((x >= (x_pos + 0.02 * (width)) &&
         (x <= x_pos + 0.02 * (width) + 0.1 * (width))) &&
        ((y <= y_pos - 3) &&
         (y >= (y_pos - height / 2) +
                       3))) {  // YOU HAVE CLICKED INSIDE THE UP ARROW
        if (state == 1) {  // IF MOUSE BUTTON DOWN (YOU ARE INSIDE UP ARROW)
            button_state = 1;  // THEN UP ARROW HAS BEEN PRESSED
        } else if (state == 0) {
            if (still_over_control_item_selection_box) {  // ONCE YOU RELEASE
                                                          // MOUSE
                                                          // BUTTON
                playSFX(SMALL_CLICK);
                menu_state++;
                if (menu_state == number_of_options)  // wrap around check
                    menu_state -= number_of_options;
                setOptionText(menu_state);
            }
            button_state = 0;
        }
    }
    if (((x >= x_pos + 0.02 * (width)) &&
         (x <= x_pos + 0.02 * (width) + 0.1 * (width))) &&
        ((y <= (y_pos - height / 2) - 3) &&
         (y >=
          (y_pos - height) + 3))) {  // YOU HAVE CLICKED INSIDE THE UP ARROW
        if (state == 1) {  // IF MOUSE BUTTON DOWN (YOU ARE INSIDE UP ARROW)
            button_state = 2;  // THEN UP ARROW HAS BEEN PRESSED
        } else if (state == 0) {
            if (still_over_control_item_selection_box) {  // ONCE YOU RELEASE
                                                          // MOUSE
                                                          // BUTTON
                playSFX(SMALL_CLICK);
                menu_state--;
                if (menu_state < 0)  // wrap around check
                    menu_state += number_of_options;
                setOptionText(menu_state);
            }
            button_state = 0;
        }
    }
    if (state == 0) {
        button_state = 0;
    }
}

void ControlItemSelectionBox::updateMouse(int x, int y) {}