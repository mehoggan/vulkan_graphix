#ifndef SUB_MENU_SELECT_TANKS_H
#define SUB_MENU_SELECT_TANKS_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>
#include <string>
#include "vulkan_earth/SubMenu.h"

class TextObject;
class SubMenu;
class ControlItem;
class ControlItemCheckBox;
class ControlItemSelectionBox;

const int num_control_items_st = 1;  // uncomment the draw and mouse functions
                                     // in .cpp if you actually change this

class SubMenuSelectTanks : public SubMenu {
public:
    SubMenuSelectTanks();
    SubMenuSelectTanks(int id,
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
    ~SubMenuSelectTanks() override;
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
    void subMenuMouseTest(int x, int y, int button_down) override;
    void updateMouse(int x, int y) override;

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
    ControlItem* sub_menu_button[num_control_items_st];
    ControlItem* button_pressed;
};

#endif  //	SUB_MENU_SELECT_TANKS_H