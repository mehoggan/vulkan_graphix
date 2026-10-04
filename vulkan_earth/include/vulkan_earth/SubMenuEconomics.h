#ifndef SUB_MENU_ECONOMICS_H
#define SUB_MENU_ECONOMICS_H

#include <cstdint>
#include <string>
#include "vulkan_earth/SubMenu.h"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

class TextObject;
class SubMenu;
class ControlItem;
class ControlItemCheckBox;
class ControlItemSelectionBox;

const std::int32_t num_control_items_econ = 5;

class SubMenuEconomics : public SubMenu {
public:
    SubMenuEconomics();
    SubMenuEconomics(std::int32_t id,
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
    ~SubMenuEconomics() override;
    std::int32_t getUNIQUEIDENTIFIER() override;
    void setUNIQUEIDENTIFIER(std::int32_t id) override;
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
    void draw(vulkan_graphix::Render::RenderContext& context) override;
    std::string collectData() override;
    void subMenuMouseTest(std::int32_t x,
                          std::int32_t y,
                          std::int32_t button_down) override;
    void updateMouse(std::int32_t x, std::int32_t y) override;

private:
    std::int32_t uniqueidentifier;
    float x_pos;
    float y_pos;
    float z_pos;
    float color[4];
    std::int32_t width;
    std::int32_t height;
    std::string caption;
    float percent_border;
    TextObject* label;
    ControlItem* sub_menu_button[num_control_items_econ];
    ControlItem* button_pressed;
    vulkan_graphix::Render::UiMesh frame_mesh;
};

#endif  //	SUB_MENU_ECONOMICS_H