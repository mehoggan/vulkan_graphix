#ifndef MAINMENUBUTTON_H
#define MAINMENUBUTTON_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>
#include <string>
#include "vulkan_earth/Item.h"

class TextObject;
class SubMenu;

class MainMenuButton {
public:
    MainMenuButton();
    MainMenuButton(int id,
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
    void pressDraw();
    void draw();
    void pressButton();
    void activateSubMenu();
    void deactivateSubMenu();
    void depressButton();
    bool isPressed();
    bool isActive();
    void setLabel(const std::string& c);
    int getUNIQUEIDENTIFIER();
    float getXPos();
    float getYPos();
    float getHeight();
    float getWidth();
    float* getColor();
    void setColor(float r, float g, float b);
    SubMenu* getSubMenu();
    void printSelf(int i);

private:
    int uniqueidentifier;
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
};
#endif  // MAINMENUBUTTON_H