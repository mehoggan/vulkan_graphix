#ifndef SUB_MENU_H
#define SUB_MENU_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class TextObject;

class SubMenu {
public:
    SubMenu();
    SubMenu(int ID,
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
    virtual ~SubMenu() = 0;
    /*	GETTERS AND SETTERS	*/
    virtual int getUNIQUEIDENTIFIER() = 0;
    virtual void setUNIQUEIDENTIFIER(int ID) = 0;
    virtual GLfloat getXPos() = 0;
    virtual void setXPos(GLfloat newXpos) = 0;
    virtual GLfloat getYPos() = 0;
    virtual void setYPos(GLfloat newYpos) = 0;
    virtual GLfloat getZPos() = 0;
    virtual void setZPos(GLfloat newZpos) = 0;
    virtual GLfloat getRed() = 0;
    virtual void setRed(GLfloat red) = 0;
    virtual GLfloat getGreen() = 0;
    virtual void setGreen(GLfloat green) = 0;
    virtual GLfloat getBlue() = 0;
    virtual void setBlue(GLfloat blue) = 0;
    virtual GLint getWidth() = 0;
    virtual void setWdith(GLint width) = 0;
    virtual GLint getHeight() = 0;
    virtual void setHeight(GLint height) = 0;
    virtual char* getCaption() = 0;
    virtual void setCaption(char* caption) = 0;
    virtual GLfloat getPerecentBorder() = 0;
    virtual void setPercentBorder(GLfloat percent) = 0;
    /*	END OF GETTERS AND SETTERS	*/
    /*	ACTUAL ACTIONS A SUBMENU CAN MAKE	*/
    virtual void draw() = 0;
    virtual const char* collectData() = 0;
    virtual void subMenuMouseTest(int x, int y, int buttonDown) = 0;
    virtual void updateMouse(int x, int y) = 0;
    /*	END OF ACTIONS A SUBMENU CAN MAKE	*/
};

#endif  //	SUB_MENU_H