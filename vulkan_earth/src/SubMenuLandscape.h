#ifndef SUB_MENU_LANDSCAPE_H
#define SUB_MENU_LANDSCAPE_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "SubMenu.h"

class TextObject;
class SubMenu;
class ControlItem;
class ControlItemCheckBox;
class ControlItemSelectionBox;
class ControlItemSliderbar;
class TerrainMaker;

const int num_control_items_lnd = 4;
const int preview_button = 3;

class SubMenuLandscape : public SubMenu {
public:
    SubMenuLandscape();
    SubMenuLandscape(int id,
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
    ~SubMenuLandscape() override;
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
    TerrainMaker* tm;  // PUBLIC BECAUSE I AM TOO LAZY TO UPDATE ENTIRE
                       // INTERFACE FOR ONE GET FUNCTION
    ControlItem*
            sub_menu_button[num_control_items_lnd];  // BOTH THESE ITEMS NEED
                                                     // GETTERS AND SETTERS
                                                     // WHICH MEANS UPDATE TO
                                                     // INTERFACE
private:
    int uniqueidentifier;
    int old_mouse_x, old_mouse_y;
    GLfloat cam_x, cam_y, cam_z;
    GLfloat x_pos;
    GLfloat y_pos;
    GLfloat z_pos;
    GLfloat color[4];
    GLint width;
    GLint height;
    std::string caption;
    GLfloat percent_border;
    TextObject* label;
    ControlItem* button_pressed;
    int numberpressed;
};

#endif  //	SUB_MENU_LANDSCAPE_H