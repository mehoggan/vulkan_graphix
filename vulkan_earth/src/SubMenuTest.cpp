#include "SubMenuTest.h"
#include <cstring>
#include <string>
#include "SubMenu.h"
#include "TextObject.h"
#include "macro_crtdbg.h"

using namespace std;

SubMenuTest::SubMenuTest() = default;

SubMenuTest::SubMenuTest(int id,
                         GLfloat new_x_pos,
                         GLfloat new_y_pos,
                         GLfloat new_z_pos,
                         GLfloat red,
                         GLfloat green,
                         GLfloat blue,
                         GLint new_width,
                         GLint new_height,
                         const std::string& new_caption,
                         GLfloat new_percent_border) {
    uniqueidentifier = id;
    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    percent_border = new_percent_border;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    width = new_width;
    height = new_height;
    caption = new_caption;

    /*	BUTTON TEXT PLACEMENT	*/
    int real_length = 0;
    for (char ch : caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    GLfloat label_x_pos = x_pos + ((width) / 2) - (real_length / 2);
    GLfloat label_y_pos = y_pos - height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           (z_pos + 1),
                           GLUT_BITMAP_TIMES_ROMAN_24,
                           0.0f,
                           0.0f,
                           0.0f);
}

SubMenuTest::~SubMenuTest() = default;

int SubMenuTest::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
void SubMenuTest::setUNIQUEIDENTIFIER(int id) { uniqueidentifier = id; }
GLfloat SubMenuTest::getXPos() { return x_pos; }
void SubMenuTest::setXPos(GLfloat new_xpos) { x_pos = new_xpos; }
GLfloat SubMenuTest::getYPos() { return y_pos; }
void SubMenuTest::setYPos(GLfloat new_ypos) { y_pos = new_ypos; }
GLfloat SubMenuTest::getZPos() { return z_pos; }
void SubMenuTest::setZPos(GLfloat new_zpos) { z_pos = new_zpos; }
GLfloat SubMenuTest::getRed() { return color[0]; }
void SubMenuTest::setRed(GLfloat red) { color[0] = red; }
GLfloat SubMenuTest::getGreen() { return color[1]; }
void SubMenuTest::setGreen(GLfloat green) { color[1] = green; }
GLfloat SubMenuTest::getBlue() { return color[2]; }
void SubMenuTest::setBlue(GLfloat blue) { color[2] = blue; }
GLint SubMenuTest::getWidth() { return width; }
void SubMenuTest::setWdith(GLint new_width) { width = new_width; }
GLint SubMenuTest::getHeight() { return height; }
void SubMenuTest::setHeight(GLint new_height) { height = new_height; }
std::string SubMenuTest::getCaption() { return caption; }
void SubMenuTest::setCaption(const std::string& new_caption) {
    caption = new_caption;
}
GLfloat SubMenuTest::getPerecentBorder() { return percent_border; }
void SubMenuTest::setPercentBorder(GLfloat percent) {
    percent_border = percent;
}

void SubMenuTest::draw() {
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
    glVertex3f(x_pos, y_pos, z_pos);
    glVertex3f(x_pos - 3, y_pos + 3, z_pos);
    glVertex3f(x_pos + width + 3, y_pos + 3, z_pos);
    glVertex3f(x_pos + width, y_pos, z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] + .2, color[1] + .2, color[2] + .2, color[3]);
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
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(x_pos - 3, y_pos - height - 3, z_pos);
    glVertex3f(x_pos + width + 3, y_pos - height - 3, z_pos);
    glVertex3f(x_pos + width, y_pos - height, z_pos);
    glVertex3f(x_pos, y_pos - height, z_pos);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(color[0] - .4, color[1] - .4, color[2] - .4, color[3]);
    glVertex3f(x_pos + width, y_pos, z_pos);
    glVertex3f(x_pos + width + 3, y_pos + 3, z_pos);
    glVertex3f(x_pos + width + 3, y_pos - height - 3, z_pos);
    glVertex3f(x_pos + width, y_pos + -height, z_pos);
    glEnd();
    label->draw();
}

std::string SubMenuTest::collectData() { return "Test:"; }