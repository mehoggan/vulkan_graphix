#ifndef ITEM_TEST_H
#define ITEM_TEST_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>
#include <string>

#include "vulkan_earth/Item.h"

class ImageObject;

class ItemTest : public Item {
public:
    ItemTest();
    ~ItemTest() override = 0;
    /*	GETTERS AND SETTERS	*/
    virtual std::int32_t getUNIQUEIDENTIFIER() = 0;
    virtual void setUNIQUEIDENTIFIER(std::int32_t id) = 0;
    virtual float getXPos() = 0;
    virtual void setXPos(float new_xpos) = 0;
    virtual float getYPos() = 0;
    virtual void setYPos(float new_ypos) = 0;
    virtual float getZPos() = 0;
    virtual void setZPos(float new_zpos) = 0;
    virtual float getRed() = 0;
    virtual void setRed(float red) = 0;
    virtual float getGreen() = 0;
    virtual void setGreen(float green) = 0;
    virtual float getBlue() = 0;
    virtual void setBlue(float blue) = 0;
    virtual std::int32_t getWidth() = 0;
    virtual void setWdith(std::int32_t width) = 0;
    virtual std::int32_t getHeight() = 0;
    virtual void setHeight(std::int32_t height) = 0;
    virtual std::string getImage() = 0;
    virtual void setImage(const std::string& image) = 0;
    /*	END OF GETTERS AND SETTERS	*/
    /*	ACTUAL ACTIONS A Weapon CAN MAKE	*/
    virtual void draw() = 0;
    /*	END OF ACTIONS A Weapon CAN MAKE	*/
};

#endif  //	ITEM_TEST_H