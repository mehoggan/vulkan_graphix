#ifndef Control_ITEM_SLIDERBAR_H
#define Control_ITEM_SLIDERBAR_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <vector>

#include "ControlItem.h"

class TextObject;

class ControlItemSliderbar : public ControlItem {
public:
    ControlItemSliderbar();
    ControlItemSliderbar(GLfloat new_x_pos,
                         GLfloat new_y_pos,
                         GLfloat new_z_pos,
                         GLfloat red,
                         GLfloat green,
                         GLfloat blue,
                         GLint new_width,
                         GLint new_height,
                         const std::string& new_caption,
                         const std::string& menu_string,
                         int slider_starting_index);
    ~ControlItemSliderbar() override;
    void draw() override;
    void mouseClickEvent(GLint x,
                         GLint y,
                         GLint state,
                         bool still_over_control_item_sliderbar) override;
    GLfloat getXPos() override;
    GLfloat getYPos() override;
    GLfloat getHeight() override;
    GLfloat getWidth() override;
    GLfloat getBarXPos();
    GLfloat getSliderXPos();
    void setSliderXPos(GLfloat x);
    GLfloat getInterval();
    std::string collectData() override;
    void updateMouse(int x, int y) override;

private:
    void setOptionText(int index) override;
    void setOptionText(const std::string& new_text) override;
    GLfloat x_pos;
    GLfloat y_pos;
    GLfloat z_pos;
    GLfloat bar_x_pos;
    GLfloat bar_y_pos;
    GLfloat bar_z_pos;
    GLint bar_width;
    GLfloat slider_x_pos;
    GLfloat slider_y_pos;
    GLfloat slider_z_pos;
    GLint slider_width;
    GLint slider_height;
    GLfloat color[4];
    GLint width;
    GLint height;
    GLfloat interval;
    TextObject* option_text;
    TextObject* label;
    std::string caption;
    std::string menu_info;
    std::string current_option;
    int menu_state;
    int button_state;  // 0 = no button pressed, 1 = up button pressed, 2 =
                       // down button pressed
    int number_of_options;
    std::vector<std::string> all_options;
    bool is_slider_clicked;
};

#endif