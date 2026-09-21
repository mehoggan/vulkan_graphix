#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>

#ifndef Control_ITEM_BUTTON_H
#define Control_ITEM_BUTTON_H

#include "ControlItem.h"

class TextObject;
class SubMenu;
class SubMenuLandscape;

class ControlItemButton : public ControlItem {
public:
    ControlItemButton();
    ControlItemButton(SubMenuLandscape* parent,
                      GLfloat x_pos,
                      GLfloat y_pos,
                      GLfloat z_pos,
                      GLfloat red,
                      GLfloat green,
                      GLfloat blue,
                      GLint width,
                      GLint height,
                      const std::string& caption);
    ~ControlItemButton() override;
    void draw() override;
    void mouseClickEvent(GLint x,
                         GLint y,
                         GLint state,
                         bool still_over_control_item_check_box) override;
    GLfloat getXPos() override;
    GLfloat getYPos() override;
    GLfloat getHeight() override;
    GLfloat getWidth() override;
    std::string collectData() override;
    void updateMouse(int x, int y) override;
    void setOptionText(int index) override;
    void setOptionText(const std::string& new_text) override;
    void updateButtonState();
    bool isToggled();
    void setToggled(bool t);

private:
    GLfloat x_pos;
    GLfloat y_pos;
    GLfloat z_pos;
    GLfloat color[4];
    GLint width;
    GLint height;
    TextObject* label;
    std::string caption;
    int menu_state;
    int button_state;  // 0 = no button pressed, 1 = up button pressed, 2 =
                       // down button pressed
    SubMenuLandscape* parent;
    bool toggled;
};
#endif  // Control_ITEM_BUTTON_H