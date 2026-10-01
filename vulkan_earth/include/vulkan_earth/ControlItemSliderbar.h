#ifndef Control_ITEM_SLIDERBAR_H
#define Control_ITEM_SLIDERBAR_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>
#include <string>
#include <vector>

#include "vulkan_earth/ControlItem.h"

class TextObject;

class ControlItemSliderbar : public ControlItem {
public:
    ControlItemSliderbar();
    ControlItemSliderbar(float new_x_pos,
                         float new_y_pos,
                         float new_z_pos,
                         float red,
                         float green,
                         float blue,
                         std::int32_t new_width,
                         std::int32_t new_height,
                         const std::string& new_caption,
                         const std::string& menu_string,
                         int slider_starting_index);
    ~ControlItemSliderbar() override;
    void draw() override;
    void mouseClickEvent(std::int32_t x,
                         std::int32_t y,
                         std::int32_t state,
                         bool still_over_control_item_sliderbar) override;
    float getXPos() override;
    float getYPos() override;
    float getHeight() override;
    float getWidth() override;
    float getBarXPos();
    float getSliderXPos();
    void setSliderXPos(float x);
    float getInterval();
    std::string collectData() override;
    void updateMouse(int x, int y) override;

private:
    void setOptionText(int index) override;
    void setOptionText(const std::string& new_text) override;
    float x_pos;
    float y_pos;
    float z_pos;
    float bar_x_pos;
    float bar_y_pos;
    float bar_z_pos;
    std::int32_t bar_width;
    float slider_x_pos;
    float slider_y_pos;
    float slider_z_pos;
    std::int32_t slider_width;
    std::int32_t slider_height;
    float color[4];
    std::int32_t width;
    std::int32_t height;
    float interval;
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