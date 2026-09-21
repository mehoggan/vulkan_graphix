#ifndef SUB_MENU_SOUND_H
#define SUB_MENU_SOUND_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "SubMenu.h"

class TextObject;
class SubMenu;
class ControlItem;
class ControlItemCheckBox;
class ControlItemSelectionBox;

const int num_control_items_snd = 2;

class SubMenuSound : public SubMenu {
public:
    SubMenuSound();
    SubMenuSound(int id,
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
    ~SubMenuSound() override;
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
    void subMenuMouseTest(int x, int y, int button_down) override;
    void updateMouse(int x, int y) override;
    void changeVolumes(ControlItem* the_sub_menu_item);

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
    ControlItem* sub_menu_button[num_control_items_snd];
    ControlItem* button_pressed;
};

#endif  //	SUB_MENU_SOUND_H