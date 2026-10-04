#include "vulkan_earth/MainMenuButton.h"
#include <cstdint>
#include <iostream>
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

extern void playSFX(std::int32_t sfx);

MainMenuButton::MainMenuButton() = default;

MainMenuButton::MainMenuButton(std::int32_t id,
                               float new_x_pos,
                               float new_y_pos,
                               float new_z_pos,
                               float red,
                               float green,
                               float blue,
                               std::int32_t new_width,
                               std::int32_t new_height,
                               const std::string& new_caption,
                               SubMenu* new_submenu) {
    uniqueidentifier = id;
    pressed = false;
    active = false;
    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    width = new_width;
    height = new_height;
    caption = new_caption;
    /*	BUTTON TEXT PLACEMENT	*/
    std::int32_t real_length = 0;
    for (char ch : caption) {
        real_length += vulkan_earth::textAdvance(
                vulkan_earth::FontId::TimesRoman24, ch);
    }
    float label_x_pos = x_pos + ((width) / 2) - (real_length / 2);
    float label_y_pos = y_pos + ((y_pos - (y_pos + height)) / 2) - height / 4;
    /*	END OF BUTTON TEXT PLACEMENT	*/
    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           z_pos,
                           vulkan_earth::FontId::TimesRoman24,
                           0.0f,
                           0.0f,
                           0.0f);
    submenu = new_submenu;
}

MainMenuButton::~MainMenuButton() { delete label; }

void MainMenuButton::draw(render::RenderContext& context) {
    math::Vec4<float> const current_color(
            color[0], color[1], color[2], color[3]);
    if (mesh.triangles().empty() || built_pressed != pressed ||
        built_color != current_color) {
        mesh.clear();
        vulkan_earth::appendBevel(mesh,
                                  x_pos,
                                  y_pos,
                                  z_pos,
                                  width,
                                  height,
                                  current_color,
                                  pressed);
        built_pressed = pressed;
        built_color = current_color;
    }
    context.draw(mesh);
    label->draw(context);

    if (active) {
        if (submenu != nullptr) {
            submenu->draw(context);
        }
    }
}

std::int32_t MainMenuButton::getUNIQUEIDENTIFIER() { return uniqueidentifier; }
float MainMenuButton::getXPos() { return x_pos; }
float MainMenuButton::getYPos() { return y_pos; }
float MainMenuButton::getHeight() { return height; }
float MainMenuButton::getWidth() { return width; }
SubMenu* MainMenuButton::getSubMenu() { return submenu; }
float* MainMenuButton::getColor() { return &color[0]; }
void MainMenuButton::setColor(float r, float g, float b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

void MainMenuButton::setLabel(const std::string& c) {
    delete label;
    caption = c;

    std::int32_t real_length = 0;
    for (char ch : caption) {
        real_length += vulkan_earth::textAdvance(
                vulkan_earth::FontId::TimesRoman24, ch);
    }
    float label_x_pos = x_pos + ((width) / 2) - (real_length / 2);
    float label_y_pos = y_pos + ((y_pos - (y_pos + height)) / 2) - height / 4;

    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           z_pos,
                           vulkan_earth::FontId::TimesRoman24,
                           0.0f,
                           0.0f,
                           0.0f);
}

bool MainMenuButton::isPressed() { return pressed; }
bool MainMenuButton::isActive() { return active; }

void MainMenuButton::pressButton() {
    if (Mix_Playing(0) == 0) playSFX(BIG_CLICK);
    pressed = true;
}

void MainMenuButton::depressButton() { pressed = false; }

void MainMenuButton::activateSubMenu() { active = true; }

void MainMenuButton::deactivateSubMenu() { active = false; }

void MainMenuButton::printSelf(std::int32_t i) {
    cout << " Button[" << i << "].x=" << (getXPos()) << " Button[" << i
         << "].y=" << (getYPos()) << " Button[" << i
         << "].width=" << (getWidth()) << " Button[" << i
         << "].height=" << (getHeight()) << endl;
}
