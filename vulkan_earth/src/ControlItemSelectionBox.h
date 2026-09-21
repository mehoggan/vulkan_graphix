#ifndef Control_ITEM_SELECTION_BOX_H
#define Control_ITEM_SELECTION_BOX_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <vector>

#include "ControlItem.h"

class TextObject;

class ControlItemSelectionBox : public ControlItem {
public:
    ControlItemSelectionBox();
    ControlItemSelectionBox(GLfloat x_pos,
                            GLfloat y_pos,
                            GLfloat z_pos,
                            GLfloat red,
                            GLfloat green,
                            GLfloat blue,
                            GLint width,
                            GLint height,
                            const std::string& caption,
                            const std::string& menu_info);
    ~ControlItemSelectionBox() override;
    void draw() override;
    void mouseClickEvent(GLint x,
                         GLint y,
                         GLint state,
                         bool still_over_control_item_selection_box) override;
    GLfloat getXPos() override;
    GLfloat getYPos() override;
    GLfloat getHeight() override;
    GLfloat getWidth() override;
    std::string collectData() override;
    void updateMouse(int x, int y) override;

private:
    void setOptionText(int index) override;
    void setOptionText(const std::string& new_text) override;
    GLfloat x_pos;
    GLfloat y_pos;
    GLfloat z_pos;
    GLfloat color[4];
    GLint width;
    GLint height;
    TextObject* label;
    TextObject* option_text;
    std::string caption;
    std::string menu_info;
    std::string current_option;
    int menu_state;
    int button_state;  // 0 = no button pressed, 1 = up button pressed, 2 =
                       // down button pressed
    int number_of_options;
    std::vector<std::string> all_options;
};
#endif  // Control_ITEM_SELECTION_BOX_H