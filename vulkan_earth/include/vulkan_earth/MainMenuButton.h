#ifndef MAINMENUBUTTON_H
#define MAINMENUBUTTON_H

#include <cstdint>
#include <string>
#include "vulkan_earth/Item.h"
#include "vulkan_earth/render/Mesh.h"

class TextObject;
class SubMenu;

namespace vulkan_earth::render {
class RenderContext;
}

class MainMenuButton {
public:
    MainMenuButton();
    MainMenuButton(std::int32_t id,
                   float new_x_pos,
                   float new_y_pos,
                   float new_z_pos,
                   float red,
                   float green,
                   float blue,
                   std::int32_t new_width,
                   std::int32_t new_height,
                   const std::string& new_caption,
                   SubMenu* new_submenu);
    ~MainMenuButton();
    void draw(vulkan_earth::render::RenderContext& context);
    void pressButton();
    void activateSubMenu();
    void deactivateSubMenu();
    void depressButton();
    bool isPressed();
    bool isActive();
    void setLabel(const std::string& c);
    std::int32_t getUNIQUEIDENTIFIER();
    float getXPos();
    float getYPos();
    float getHeight();
    float getWidth();
    float* getColor();
    void setColor(float r, float g, float b);
    SubMenu* getSubMenu();
    void printSelf(std::int32_t i);

private:
    std::int32_t uniqueidentifier;
    float x_pos;
    float y_pos;
    float z_pos;
    float color[4];
    std::int32_t width;
    std::int32_t height;
    std::string caption;
    TextObject* label;
    bool pressed;
    bool active;
    SubMenu* submenu;
    vulkan_earth::render::UiMesh mesh;
    bool built_pressed = false;
    vulkan_earth::render::Vec4 built_color = vulkan_earth::render::Vec4(-1.0f);
};
#endif  // MAINMENUBUTTON_H