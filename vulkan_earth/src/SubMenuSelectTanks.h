#ifndef SUB_MENU_SELECT_TANKS_H
#define SUB_MENU_SELECT_TANKS_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "SubMenu.h"

class TextObject;
class SubMenu;
class ControlItem;
class ControlItemCheckBox;
class ControlItemSelectionBox;

const int num_control_items_st = 1;  // uncomment the draw and mouse functions
                                     // in .cpp if you actually change this

class SubMenuSelectTanks : public SubMenu {
public:
    SubMenuSelectTanks();
    SubMenuSelectTanks(int id,
                       GLfloat new_x_pos,
                       GLfloat new_y_pos,
                       GLfloat new_z_pos,
                       GLfloat red,
                       GLfloat green,
                       GLfloat blue,
                       GLint new_width,
                       GLint new_height,
                       const std::string& new_caption,
                       GLfloat new_percent_border);
    ~SubMenuSelectTanks() override;
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
    void setWdith(GLint new_width) override;
    GLint getHeight() override;
    void setHeight(GLint new_height) override;
    std::string getCaption() override;
    void setCaption(const std::string& new_caption) override;
    GLfloat getPerecentBorder() override;
    void setPercentBorder(GLfloat percent) override;
    void draw() override;
    std::string collectData() override;
    void subMenuMouseTest(int x, int y, int button_down) override;
    void updateMouse(int x, int y) override;

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
    ControlItem* sub_menu_button[num_control_items_st];
    ControlItem* button_pressed;
};

#endif  //	SUB_MENU_SELECT_TANKS_H