#ifndef WEAPON_TEST_H
#define WEAPON_TEST_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Weapon.h"

class ImageObject;

class WeaponTest : public Weapon {
public:
    WeaponTest();
    ~WeaponTest() override = 0;
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
    virtual char* getImage() = 0;
    virtual void setImage(char* image) = 0;
    /*	END OF GETTERS AND SETTERS	*/
    /*	ACTUAL ACTIONS A Weapon CAN MAKE	*/
    virtual void draw() = 0;
    /*	END OF ACTIONS A Weapon CAN MAKE	*/
};

#endif  //	WEAPON_TEST_H