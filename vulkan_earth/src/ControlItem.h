#ifndef Control_ITEM_H
#define Control_ITEM_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>
#include <string>

class ControlItem {
public:
    ControlItem();
    virtual ~ControlItem();
    virtual void draw() = 0;
    virtual void mouseClickEvent(GLint x,
                                 GLint y,
                                 GLint state,
                                 bool stillOverArrowButton) = 0;
    virtual void updateMouse(int x, int y) = 0;
    virtual GLfloat getXPos() = 0;
    virtual GLfloat getYPos() = 0;
    virtual GLfloat getHeight() = 0;
    virtual GLfloat getWidth() = 0;
    virtual std::string collectData() = 0;
    virtual void setOptionText(int index) = 0;
    virtual void setOptionText(const std::string& newText) = 0;
};
#endif  // CONTROL_ITEM_H