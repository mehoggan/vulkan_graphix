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
    ControlItemSliderbar(GLfloat xPos,
                         GLfloat yPos,
                         GLfloat zPos,
                         GLfloat red,
                         GLfloat green,
                         GLfloat blue,
                         GLint width,
                         GLint height,
                         const std::string& caption,
                         const std::string& menuInfo,
                         int sliderStartingPos);
    ~ControlItemSliderbar() override;
    void draw() override;
    void mouseClickEvent(GLint x,
                         GLint y,
                         GLint state,
                         bool stillOverControlItemSliderbar) override;
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
    void setOptionText(const std::string& newText) override;
    GLfloat xPos;
    GLfloat yPos;
    GLfloat zPos;
    GLfloat barXPos;
    GLfloat barYPos;
    GLfloat barZPos;
    GLint barWidth;
    GLfloat sliderXPos;
    GLfloat sliderYPos;
    GLfloat sliderZPos;
    GLint sliderWidth;
    GLint sliderHeight;
    GLfloat color[4];
    GLint width;
    GLint height;
    GLfloat interval;
    TextObject* optionText;
    TextObject* label;
    std::string caption;
    std::string menuInfo;
    std::string currentOption;
    int menuState;
    int buttonState;  // 0 = no button pressed, 1 = up button pressed, 2 = down
                      // button pressed
    int numberOfOptions;
    std::vector<std::string> allOptions;
    bool isSliderClicked;
};

#endif