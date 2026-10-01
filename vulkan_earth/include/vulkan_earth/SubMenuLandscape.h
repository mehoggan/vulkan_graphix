#ifndef SUB_MENU_LANDSCAPE_H
#define SUB_MENU_LANDSCAPE_H

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
class ControlItemSliderbar;
class TerrainMaker;

const int num_control_items_lnd = 4;
const int preview_button = 3;

class SubMenuLandscape : public SubMenu {
public:
    SubMenuLandscape();
    SubMenuLandscape(int id,
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
    ~SubMenuLandscape() override;
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
    TerrainMaker* tm;  // PUBLIC BECAUSE I AM TOO LAZY TO UPDATE ENTIRE
                       // INTERFACE FOR ONE GET FUNCTION
    ControlItem*
            sub_menu_button[num_control_items_lnd];  // BOTH THESE ITEMS NEED
                                                     // GETTERS AND SETTERS
                                                     // WHICH MEANS UPDATE TO
                                                     // INTERFACE
private:
    int uniqueidentifier;
    int old_mouse_x, old_mouse_y;
    float cam_x, cam_y, cam_z;
    float x_pos;
    float y_pos;
    float z_pos;
    float color[4];
    std::int32_t width;
    std::int32_t height;
    std::string caption;
    float percent_border;
    TextObject* label;
    ControlItem* button_pressed;
    int numberpressed;
};

#endif  //	SUB_MENU_LANDSCAPE_H