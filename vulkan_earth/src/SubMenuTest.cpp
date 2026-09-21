#include "SubMenuTest.h"
#include <cstring>
#include <string>
#include "SubMenu.h"
#include "TextObject.h"
#include "macro_crtdbg.h"

using namespace std;

SubMenuTest::SubMenuTest() = default;

SubMenuTest::SubMenuTest(int id,
                         GLfloat x_pos,
                         GLfloat y_pos,
                         GLfloat z_pos,
                         GLfloat red,
                         GLfloat green,
                         GLfloat blue,
                         GLint width,
                         GLint height,
                         const std::string& caption,
                         GLfloat percent_border) {
    UNIQUEIDENTIFIER = id;
    this->xPos = x_pos;
    this->yPos = y_pos;
    this->zPos = z_pos;
    this->percentBorder = percent_border;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    this->width = width;
    this->height = height;
    this->caption = caption;

    /*	BUTTON TEXT PLACEMENT	*/
    int real_length = 0;
    for (char ch : this->caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = this->xPos + ((this->width) / 2) - (real_length / 2);
    GLfloat label_y_pos = this->yPos - this->height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    label = new TextObject(this->caption,
                           label_x_pos,
                           label_y_pos,
                           (this->zPos + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
}

SubMenuTest::~SubMenuTest() = default;

int SubMenuTest::getUNIQUEIDENTIFIER() { return UNIQUEIDENTIFIER; }
void SubMenuTest::setUNIQUEIDENTIFIER(int id) { UNIQUEIDENTIFIER = id; }
GLfloat SubMenuTest::getXPos() { return this->xPos; }
void SubMenuTest::setXPos(GLfloat new_xpos) { this->xPos = new_xpos; }
GLfloat SubMenuTest::getYPos() { return this->yPos; }
void SubMenuTest::setYPos(GLfloat new_ypos) { this->yPos = new_ypos; }
GLfloat SubMenuTest::getZPos() { return this->zPos; }
void SubMenuTest::setZPos(GLfloat new_zpos) { this->zPos = new_zpos; }
GLfloat SubMenuTest::getRed() { return color[0]; }
void SubMenuTest::setRed(GLfloat red) { color[0] = red; }
GLfloat SubMenuTest::getGreen() { return color[1]; }
void SubMenuTest::setGreen(GLfloat green) { color[1] = green; }
GLfloat SubMenuTest::getBlue() { return color[2]; }
void SubMenuTest::setBlue(GLfloat blue) { color[2] = blue; }
GLint SubMenuTest::getWidth() { return this->width; }
void SubMenuTest::setWdith(GLint width) { this->width = width; }
GLint SubMenuTest::getHeight() { return this->height; }
void SubMenuTest::setHeight(GLint height) { this->height = height; }
std::string SubMenuTest::getCaption() { return this->caption; }
void SubMenuTest::setCaption(const std::string& caption) {
    this->caption = caption;
}
GLfloat SubMenuTest::getPerecentBorder() { return this->percentBorder; }
void SubMenuTest::setPercentBorder(GLfloat percent) {
    this->percentBorder = percentBorder;
}

void SubMenuTest::draw() {
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
    glVertex3f(this->xPos, this->yPos, this->zPos);
    glVertex3f(this->xPos - 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
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
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(this->xPos - 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos - height, this->zPos);
    glVertex3f(this->xPos, this->yPos - height, this->zPos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(this->xPos + width, this->yPos, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos + 3, this->zPos);
    glVertex3f(this->xPos + width + 3, this->yPos - height - 3, this->zPos);
    glVertex3f(this->xPos + width, this->yPos + -height, this->zPos);
    glEnd();
    label->draw();
}

std::string SubMenuTest::collectData() { return "Test:"; }