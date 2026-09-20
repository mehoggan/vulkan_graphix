#ifndef SUB_MENU_PHYSICS_H
#define SUB_MENU_PHYSICS_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "SubMenu.h"

class TextObject;
class SubMenu;
class ControlItem;
class ControlItemCheckBox;
class ControlItemSelectionBox;

const int NUM_CONTROL_ITEMS_PHY = 3;

class SubMenuPhysics : public SubMenu {
public:
    SubMenuPhysics();
    SubMenuPhysics(int ID,
                   GLfloat xPos,
                   GLfloat yPos,
                   GLfloat zPos,
                   GLfloat red,
                   GLfloat green,
                   GLfloat blue,
                   GLint width,
                   GLint height,
                   char* caption,
                   GLfloat percentBorder);
    ~SubMenuPhysics() override;
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
    char* getCaption() override;
    void setCaption(char* caption) override;
    GLfloat getPerecentBorder() override;
    void setPercentBorder(GLfloat percent) override;
    void draw() override;
    const char* collectData() override;
    void subMenuMouseTest(int x, int y, int buttonDown) override;
    void updateMouse(int x, int y) override;

private:
    int UNIQUEIDENTIFIER;
    GLfloat xPos;
    GLfloat yPos;
    GLfloat zPos;
    GLfloat color[4];
    GLint width;
    GLint height;
    char* caption;
    GLfloat percentBorder;
    TextObject* label;
    ControlItem* subMenuButton[NUM_CONTROL_ITEMS_PHY];
    ControlItem* buttonPressed;
};

#endif  //	SUB_MENU_PHYSICS_H