#ifndef SUB_MENU_TEST_H
#define SUB_MENU_TEST_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "SubMenu.h"

class TextObject;
class SubMenu;

class SubMenuTest : public SubMenu {
public:
    SubMenuTest();
    SubMenuTest(int id,
                GLfloat x_pos,
                GLfloat y_pos,
                GLfloat z_pos,
                GLfloat red,
                GLfloat green,
                GLfloat blue,
                GLint width,
                GLint height,
                const std::string& caption,
                GLfloat percent_border);
    ~SubMenuTest() override;
    int getUNIQUEIDENTIFIER() override;
    void setUNIQUEIDENTIFIER(int id) override;
    GLfloat getXPos() override;
    void setXPos(GLfloat new_xpos) override;
    GLfloat getYPos() override;
    void setYPos(GLfloat new_ypos) override;
    GLfloat getZPos() override;
    void setZPos(GLfloat new_zpos) override;
    GLfloat getRed() override;
    void setRed(GLfloat red) override;
    GLfloat getGreen() override;
    void setGreen(GLfloat green) override;
    GLfloat getBlue() override;
    void setBlue(GLfloat blue) override;
    GLint getWidth() override;
    void setWdith(GLint width) override;
    GLint getHeight() override;
    void setHeight(GLint height) override;
    std::string getCaption() override;
    void setCaption(const std::string& caption) override;
    GLfloat getPerecentBorder() override;
    void setPercentBorder(GLfloat percent) override;
    void draw() override;
    std::string collectData() override;

private:
    int uniqueidentifier;
    GLfloat x_pos;
    GLfloat y_pos;
    GLfloat z_pos;
    GLfloat color[4];
    GLint width;
    GLint height;
    std::string caption;
    GLfloat percent_border;
    TextObject* label;
};

#endif  //	SUB_MENU_TEST_H