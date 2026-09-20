#include "TextObject.h"
#include <string>
#include "macro_crtdbg.h"

using namespace std;

TextObject::TextObject() = default;

TextObject::TextObject(const std::string& input,
                       GLfloat posX,
                       GLfloat posY,
                       GLfloat posZ,
                       void* font_size,
                       GLfloat red,
                       GLfloat green,
                       GLfloat blue) {
    this->output = input;
    this->posX = posX;
    this->posY = posY;
    this->posZ = posZ + 1;
    this->font_size = font_size;
    if (glutGet(GLUT_WINDOW_WIDTH) < 1300) {
        this->font_size = GLUT_BITMAP_9_BY_15;
    }
    this->color[0] = red;
    this->color[1] = green;
    this->color[2] = blue;
    this->color[3] = 1.0;
}

TextObject::~TextObject() = default;

const std::string& TextObject::getOutput() { return this->output; }
void TextObject::setXpos(GLfloat x) { this->posX = x; }
void TextObject::setYpos(GLfloat y) { this->posY = y; }
void TextObject::setZpos(GLfloat z) { this->posZ = z; }

GLvoid TextObject::draw() {
    GLfloat x_pos = this->posX;
    glColor3f(this->color[0], this->color[1], this->color[2]);
    for (char ch : this->output) {
        int step = glutBitmapWidth(this->font_size, ch);
        glRasterPos3f(x_pos, this->posY, this->posZ);
        glutBitmapCharacter(this->font_size, ch);
        x_pos += step;
    }
}