#ifndef CONTROL_ITEM_TEXT_FIELD_H
#define CONTROL_ITEM_TEXT_FIELD_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ControlItem.h"

class TextObject;

class ControlItemTextField : public ControlItem {
public:
    ControlItemTextField();
    ControlItemTextField(GLfloat xPos,
                         GLfloat yPos,
                         GLfloat zPos,
                         GLfloat red,
                         GLfloat green,
                         GLfloat blue,
                         GLint width,
                         GLint height);
    ~ControlItemTextField() override;
    void draw() override;
    void mouseClickEvent(GLint x,
                         GLint y,
                         GLint state,
                         bool stillOverControlItemTextField) override;
    GLfloat getXPos() override;
    GLfloat getYPos() override;
    GLfloat getHeight() override;
    GLfloat getWidth() override;
    bool isTextFieldActive();
    char* collectData() override;
    void updateMouse(int x, int y) override;
    void keyHandler(unsigned char key);
    void deactivate();
    void setOptionText(char* newText) override;
    void clearTextBuffer();
    void setTextBuffer(char* newText);

private:
    void setOptionText(int index) override;

    GLfloat xPos;
    GLfloat yPos;
    GLfloat zPos;
    GLfloat color[4];
    GLint width;
    GLint height;
    TextObject* currentText;
    bool textFieldActive;
    char* currentChars;
    int currentLength;
    int numberOfFrames;
    int textCursorOn;  // this is a toggle, -1 off, 1 on
};
#endif  // Control_ITEM_TEXT_FIELD_H