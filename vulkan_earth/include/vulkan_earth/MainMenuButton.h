#ifndef MAINMENUBUTTON_H
#define MAINMENUBUTTON_H

#include <cstdint>
#include <string>
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

class TextObject;
class SubMenu;

namespace vulkan_graphix::Render {
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
    void draw(vulkan_graphix::Render::RenderContext& context);
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
    vulkan_graphix::Render::UiMesh mesh;
    bool built_pressed = false;
    vulkan_graphix::Math::Vec4<float> built_color =
            vulkan_graphix::Math::Vec4<float>(-1.0f);
};
#endif  // MAINMENUBUTTON_H