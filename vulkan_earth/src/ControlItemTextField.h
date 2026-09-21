#ifndef CONTROL_ITEM_TEXT_FIELD_H
#define CONTROL_ITEM_TEXT_FIELD_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>

#include "ControlItem.h"

class TextObject;

class ControlItemTextField : public ControlItem {
public:
    ControlItemTextField();
    ControlItemTextField(GLfloat new_x_pos,
                         GLfloat new_y_pos,
                         GLfloat new_z_pos,
                         GLfloat red,
                         GLfloat green,
                         GLfloat blue,
                         GLint new_width,
                         GLint new_height);
    ~ControlItemTextField() override;
    void draw() override;
    void mouseClickEvent(GLint x,
                         GLint y,
                         GLint state,
                         bool still_over_control_item_text_field) override;
    GLfloat getXPos() override;
    GLfloat getYPos() override;
    GLfloat getHeight() override;
    GLfloat getWidth() override;
    bool isTextFieldActive();
    std::string collectData() override;
    void updateMouse(int x, int y) override;
    void keyHandler(unsigned char key);
    void deactivate();
    void setOptionText(const std::string& new_text) override;
    void clearTextBuffer();
    void setTextBuffer(const std::string& new_text);

private:
    void setOptionText(int index) override;

    GLfloat x_pos;
    GLfloat y_pos;
    GLfloat z_pos;
    GLfloat color[4];
    GLint width;
    GLint height;
    TextObject* current_text;
    bool text_field_active;
    std::string current_chars;
    int current_length;
    int number_of_frames;
    int text_cursor_on;  // this is a toggle, -1 off, 1 on
};
#endif  // Control_ITEM_TEXT_FIELD_H