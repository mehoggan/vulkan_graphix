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
    ControlItemSelectionBox(GLfloat xPos,
                            GLfloat yPos,
                            GLfloat zPos,
                            GLfloat red,
                            GLfloat green,
                            GLfloat blue,
                            GLint width,
                            GLint height,
                            const std::string& caption,
                            const std::string& menuInfo);
    ~ControlItemSelectionBox() override;
    void draw() override;
    void mouseClickEvent(GLint x,
                         GLint y,
                         GLint state,
                         bool stillOverControlItemSelectionBox) override;
    GLfloat getXPos() override;
    GLfloat getYPos() override;
    GLfloat getHeight() override;
    GLfloat getWidth() override;
    std::string collectData() override;
    void updateMouse(int x, int y) override;

private:
    void setOptionText(int index) override;
    void setOptionText(const std::string& newText) override;
    GLfloat xPos;
    GLfloat yPos;
    GLfloat zPos;
    GLfloat color[4];
    GLint width;
    GLint height;
    TextObject* label;
    TextObject* optionText;
    std::string caption;
    std::string menuInfo;
    std::string currentOption;
    int menuState;
    int buttonState;  // 0 = no button pressed, 1 = up button pressed, 2 = down
                      // button pressed
    int numberOfOptions;
    std::vector<std::string> allOptions;
};
#endif  // Control_ITEM_SELECTION_BOX_H