#ifndef SUB_MENU_TEST_H
#define SUB_MENU_TEST_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>
#include <string>
#include "vulkan_earth/SubMenu.h"

class TextObject;
class SubMenu;

class SubMenuTest : public SubMenu {
public:
    SubMenuTest();
    SubMenuTest(int id,
                float new_x_pos,
                float new_y_pos,
                float new_z_pos,
                float red,
                float green,
                float blue,
                std::int32_t new_width,
                std::int32_t new_height,
                const std::string& new_caption,
                float new_percent_border);
    ~SubMenuTest() override;
    int getUNIQUEIDENTIFIER() override;
    void setUNIQUEIDENTIFIER(int id) override;
    float getXPos() override;
    void setXPos(float new_xpos) override;
    float getYPos() override;
    void setYPos(float new_ypos) override;
    float getZPos() override;
    void setZPos(float new_zpos) override;
    float getRed() override;
    void setRed(float red) override;
    float getGreen() override;
    void setGreen(float green) override;
    float getBlue() override;
    void setBlue(float blue) override;
    std::int32_t getWidth() override;
    void setWdith(std::int32_t new_width) override;
    std::int32_t getHeight() override;
    void setHeight(std::int32_t new_height) override;
    std::string getCaption() override;
    void setCaption(const std::string& new_caption) override;
    float getPerecentBorder() override;
    void setPercentBorder(float percent) override;
    void draw() override;
    std::string collectData() override;

private:
    int uniqueidentifier;
    float x_pos;
    float y_pos;
    float z_pos;
    float color[4];
    std::int32_t width;
    std::int32_t height;
    std::string caption;
    float percent_border;
    TextObject* label;
};

#endif  //	SUB_MENU_TEST_H