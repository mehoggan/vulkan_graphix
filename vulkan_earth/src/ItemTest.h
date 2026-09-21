#ifndef ITEM_TEST_H
#define ITEM_TEST_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>

#include "Item.h"

class ImageObject;

class ItemTest : public Item {
public:
    ItemTest();
    ~ItemTest() override = 0;
    /*	GETTERS AND SETTERS	*/
    virtual int getUNIQUEIDENTIFIER() = 0;
    virtual void setUNIQUEIDENTIFIER(int id) = 0;
    virtual GLfloat getXPos() = 0;
    virtual void setXPos(GLfloat new_xpos) = 0;
    virtual GLfloat getYPos() = 0;
    virtual void setYPos(GLfloat new_ypos) = 0;
    virtual GLfloat getZPos() = 0;
    virtual void setZPos(GLfloat new_zpos) = 0;
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
    virtual std::string getImage() = 0;
    virtual void setImage(const std::string& image) = 0;
    /*	END OF GETTERS AND SETTERS	*/
    /*	ACTUAL ACTIONS A Weapon CAN MAKE	*/
    virtual void draw() = 0;
    /*	END OF ACTIONS A Weapon CAN MAKE	*/
};

#endif  //	ITEM_TEST_H