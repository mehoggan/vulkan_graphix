#include "vulkan_earth/SubMenuTest.h"
#include <cstdint>
#include <cstring>
#include <string>
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

SubMenuTest::SubMenuTest() = default;

SubMenuTest::SubMenuTest(std::int32_t id,
                         float new_x_pos,
                         float new_y_pos,
                         float new_z_pos,
                         float red,
                         float green,
                         float blue,
                         std::int32_t new_width,
                         std::int32_t new_height,
                         const std::string& new_caption,
                         float new_percent_border) {
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
    std::int32_t real_length = 0;
    for (char ch : caption) {
        real_length += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
    float label_x_pos = x_pos + ((width) / 2) - (real_length / 2);
    float label_y_pos = y_pos - height / 20;
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

std::int32_t SubMenuTest::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
void SubMenuTest::setUNIQUEIDENTIFIER(std::int32_t id) {
    uniqueidentifier = id;
}
float SubMenuTest::getXPos() { return x_pos; }
void SubMenuTest::setXPos(float new_xpos) { x_pos = new_xpos; }
float SubMenuTest::getYPos() { return y_pos; }
void SubMenuTest::setYPos(float new_ypos) { y_pos = new_ypos; }
float SubMenuTest::getZPos() { return z_pos; }
void SubMenuTest::setZPos(float new_zpos) { z_pos = new_zpos; }
float SubMenuTest::getRed() { return color[0]; }
void SubMenuTest::setRed(float red) { color[0] = red; }
float SubMenuTest::getGreen() { return color[1]; }
void SubMenuTest::setGreen(float green) { color[1] = green; }
float SubMenuTest::getBlue() { return color[2]; }
void SubMenuTest::setBlue(float blue) { color[2] = blue; }
std::int32_t SubMenuTest::getWidth() { return width; }
void SubMenuTest::setWdith(std::int32_t new_width) { width = new_width; }
std::int32_t SubMenuTest::getHeight() { return height; }
void SubMenuTest::setHeight(std::int32_t new_height) { height = new_height; }
std::string SubMenuTest::getCaption() { return caption; }
void SubMenuTest::setCaption(const std::string& new_caption) {
    caption = new_caption;
}
float SubMenuTest::getPerecentBorder() { return percent_border; }
void SubMenuTest::setPercentBorder(float percent) { percent_border = percent; }

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