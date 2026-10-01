#ifndef Control_ITEM_H
#define Control_ITEM_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <string>

class ControlItem {
public:
    ControlItem();
    virtual ~ControlItem();
    virtual void draw() = 0;
    virtual void mouseClickEvent(std::int32_t x,
                                 std::int32_t y,
                                 std::int32_t state,
                                 bool still_over_arrow_button) = 0;
    virtual void updateMouse(std::int32_t x, std::int32_t y) = 0;
    virtual float getXPos() = 0;
    virtual float getYPos() = 0;
    virtual float getHeight() = 0;
    virtual float getWidth() = 0;
    virtual std::string collectData() = 0;
    virtual void setOptionText(std::int32_t index) = 0;
    virtual void setOptionText(const std::string& new_text) = 0;
};
#endif  // CONTROL_ITEM_H