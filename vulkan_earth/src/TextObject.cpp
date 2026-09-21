#include "TextObject.h"
#include <string>
#include "macro_crtdbg.h"

using namespace std;

TextObject::TextObject() = default;

TextObject::TextObject(const std::string& input,
                       GLfloat pos_x,
                       GLfloat pos_y,
                       GLfloat pos_z,
                       void* font_size,
                       GLfloat red,
                       GLfloat green,
                       GLfloat blue) {
    output = input;
    this->pos_x = pos_x;
    this->pos_y = pos_y;
    this->pos_z = pos_z + 1;
    this->font_size = font_size;
    if (glutGet(GLUT_WINDOW_WIDTH) < 1300) {
        this->font_size = GLUT_BITMAP_9_BY_15;
    }
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
}

TextObject::~TextObject() = default;

const std::string& TextObject::getOutput() { return output; }
void TextObject::setXpos(GLfloat x) { this->pos_x = x; }
void TextObject::setYpos(GLfloat y) { this->pos_y = y; }
void TextObject::setZpos(GLfloat z) { this->pos_z = z; }

GLvoid TextObject::draw() {
    GLfloat x_pos = this->pos_x;
    glColor3f(color[0], color[1], color[2]);
    for (char ch : output) {
        int step = glutBitmapWidth(this->font_size, ch);
        glRasterPos3f(x_pos, this->pos_y, this->pos_z);
        glutBitmapCharacter(this->font_size, ch);
        x_pos += step;
    }
}