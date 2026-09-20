#ifndef SUB_MENU_SOUND_H
#define SUB_MENU_SOUND_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "SubMenu.h"

class TextObject;
class SubMenu;
class ControlItem;
class ControlItemCheckBox;
class ControlItemSelectionBox;

const int NUM_CONTROL_ITEMS_SND = 2;

class SubMenuSound : public SubMenu {
public:
    SubMenuSound();
    SubMenuSound(int ID,
                 GLfloat xPos,
                 GLfloat yPos,
                 GLfloat zPos,
                 GLfloat red,
                 GLfloat green,
                 GLfloat blue,
                 GLint width,
                 GLint height,
                 const std::string& caption,
                 GLfloat percentBorder);
    ~SubMenuSound() override;
    int getUNIQUEIDENTIFIER() override;
    void setUNIQUEIDENTIFIER(int ID) override;
    GLfloat getXPos() override;
    void setXPos(GLfloat newXpos) override;
    GLfloat getYPos() override;
    void setYPos(GLfloat newYpos) override;
    GLfloat getZPos() override;
    void setZPos(GLfloat newZpos) override;
    GLfloat getRed() override;
    void setRed(GLfloat red) override;
    GLfloat getGreen() override;
    void setGreen(GLfloat green) override;
    GLfloat getBlue() override;
    void setBlue(GLfloat blue) override;
    GLint getWidth() override;
    void setWdith(GLint width) override;
    GLint getHeight() override;
    void setHeight(GLint height) override;
    std::string getCaption() override;
    void setCaption(const std::string& caption) override;
    GLfloat getPerecentBorder() override;
    void setPercentBorder(GLfloat percent) override;
    void draw() override;
    std::string collectData() override;
    void subMenuMouseTest(int x, int y, int buttonDown) override;
    void updateMouse(int x, int y) override;
    void changeVolumes(ControlItem* TheSubMenuItem);

private:
    int UNIQUEIDENTIFIER;
    GLfloat xPos;
    GLfloat yPos;
    GLfloat zPos;
    GLfloat color[4];
    GLint width;
    GLint height;
    std::string caption;
    GLfloat percentBorder;
    TextObject* label;
    ControlItem* subMenuButton[NUM_CONTROL_ITEMS_SND];
    ControlItem* buttonPressed;
};

#endif  //	SUB_MENU_SOUND_H