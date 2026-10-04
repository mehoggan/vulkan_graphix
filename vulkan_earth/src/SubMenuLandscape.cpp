#include "vulkan_earth/SubMenuLandscape.h"
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include "math.h"
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemButton.h"
#include "vulkan_earth/ControlItemCheckBox.h"
#include "vulkan_earth/ControlItemSelectionBox.h"
#include "vulkan_earth/ControlItemSliderbar.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/TerrainMaker.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_earth/MacroCrtdbg.h"

#define PI 3.1415926535898

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;
extern void playSFX(std::int32_t sfx);

SubMenuLandscape::SubMenuLandscape() = default;

SubMenuLandscape::SubMenuLandscape(std::int32_t id,
                                   float new_x_pos,
                                   float new_y_pos,
                                   float new_z_pos,
                                   float red,
                                   float green,
                                   float blue,
                                   std::int32_t new_width,
                                   std::int32_t new_height,
                                   const std::string& new_caption,
                                   float new_percent_border) {
    uniqueidentifier = id;
    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    percent_border = new_percent_border;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    width = new_width;
    height = new_height;
    caption = new_caption;

    cam_x = -4000;
    cam_y = 10000;
    cam_z = -4000;

    tm = new TerrainMaker(100, 256);
    tm->prepareData(0, 0, 0, 0, 0);

    old_mouse_x = -1;
    old_mouse_y = -1;

    /*	BUTTON TEXT PLACEMENT	*/
    std::int32_t real_length = 0;
    for (char ch : caption) {
        real_length += vulkan_earth::textAdvance(
                vulkan_earth::FontId::TimesRoman24, ch);
    }
    float label_x_pos = x_pos + ((width) / 2) - (real_length / 2);
    float label_y_pos = y_pos - height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           (z_pos + 1),
                           vulkan_earth::FontId::TimesRoman24,
                           0.0f,
                           0.0f,
                           0.0f);
    button_pressed = nullptr;

    sub_menu_button[0] =
            new ControlItemSliderbar(x_pos + (width / 2) - (0.48 * width),
                                     y_pos - (height * 0.7),
                                     z_pos + 1,
                                     0.5f,
                                     0.5f,
                                     0.5f,
                                     0.6f * width,
                                     0.085 * (height),
                                     "Smoothness",
                                     "0/1/2/3/4/5/",
                                     5);
    sub_menu_button[1] =
            new ControlItemSliderbar(x_pos + (width / 2) - (0.48 * width),
                                     y_pos - (height * 0.8),
                                     z_pos + 1,
                                     0.5f,
                                     0.5f,
                                     0.5f,
                                     0.6f * width,
                                     0.085 * (height),
                                     "Hill Height",
                                     "0/1/2/3/4/5/",
                                     5);
    sub_menu_button[2] =
            new ControlItemSliderbar(x_pos + (width / 2) - (0.48 * width),
                                     y_pos - (height * 0.9),
                                     z_pos + 1,
                                     0.5f,
                                     0.5f,
                                     0.5f,
                                     0.6f * width,
                                     0.085 * (height),
                                     "Terrain Selection",
                                     "Rock/Snow/Ice/Mars/Desert/Lava/",
                                     0);
    sub_menu_button[3] = new ControlItemButton(this,
                                               x_pos + (0.655 * width),
                                               y_pos - (height * 0.91),
                                               z_pos + 1,
                                               0.75f,
                                               0.0f,
                                               0.0f,
                                               0.3f * width,
                                               0.05 * (height),
                                               "Sample");
}

SubMenuLandscape::~SubMenuLandscape() {
    delete label;
    delete tm;
    for (std::int32_t i = 0; i < num_control_items_lnd; i++)
        delete sub_menu_button[i];
}

std::int32_t SubMenuLandscape::getUNIQUEIDENTIFIER() {
    return uniqueidentifier;
}
void SubMenuLandscape::setUNIQUEIDENTIFIER(std::int32_t id) {
    uniqueidentifier = id;
}
float SubMenuLandscape::getXPos() { return x_pos; }
void SubMenuLandscape::setXPos(float new_xpos) { x_pos = new_xpos; }
float SubMenuLandscape::getYPos() { return y_pos; }
void SubMenuLandscape::setYPos(float new_ypos) { y_pos = new_ypos; }
float SubMenuLandscape::getZPos() { return z_pos; }
void SubMenuLandscape::setZPos(float new_zpos) { z_pos = new_zpos; }
float SubMenuLandscape::getRed() { return color[0]; }
void SubMenuLandscape::setRed(float red) { color[0] = red; }
float SubMenuLandscape::getGreen() { return color[1]; }
void SubMenuLandscape::setGreen(float green) { color[1] = green; }
float SubMenuLandscape::getBlue() { return color[2]; }
void SubMenuLandscape::setBlue(float blue) { color[2] = blue; }
std::int32_t SubMenuLandscape::getWidth() { return width; }
void SubMenuLandscape::setWdith(std::int32_t new_width) { width = new_width; }
std::int32_t SubMenuLandscape::getHeight() { return height; }
void SubMenuLandscape::setHeight(std::int32_t new_height) {
    height = new_height;
}
std::string SubMenuLandscape::getCaption() { return caption; }
void SubMenuLandscape::setCaption(const std::string& new_caption) {
    caption = new_caption;
}
float SubMenuLandscape::getPerecentBorder() { return percent_border; }
void SubMenuLandscape::setPercentBorder(float percent) {
    percent_border = percent;
}

void SubMenuLandscape::draw(render::RenderContext& context) {
    using Vec3 = math::Vec3<float>;
    using Vec4 = math::Vec4<float>;
    // The same raised 3-pixel bevel every button draws.
    if (frame_mesh.triangles().empty()) {
        vulkan_earth::appendBevel(
                frame_mesh,
                x_pos,
                y_pos,
                z_pos,
                width,
                height,
                math::Vec4<float>(color[0], color[1], color[2], color[3]),
                false);
    }
    context.draw(frame_mesh);
    label->draw(context);
    for (std::int32_t i = 0; i < num_control_items_lnd; i++) {
        if (sub_menu_button[i]) {
            sub_menu_button[i]->draw(context);
        }
    }

    // The preview's sunken border (top/left -0.2, bottom/right +0.4).
    if (border_mesh.triangles().empty()) {
        float border_x = x_pos + 0.03 * width;
        float border_y = y_pos - 0.07 * height;
        float const z1 = z_pos + 1;
        Vec4 const dark(color[0] - .2, color[1] - .2, color[2] - .2, color[3]);
        Vec4 const light(
                color[0] + .4, color[1] + .4, color[2] + .4, color[3]);
        // top-left
        border_mesh.addQuad(
                {Vec3(border_x, border_y, z1),
                 Vec3(border_x - 3, border_y + 3, z1),
                 Vec3(border_x + 0.936 * width + 3, border_y + 3, z1),
                 Vec3(border_x + 0.936 * width, border_y, z1)},
                dark);
        border_mesh.addQuad(
                {Vec3(border_x - 3, border_y + 3, z1),
                 Vec3(border_x - 3, border_y - 0.597 * height - 3, z1),
                 Vec3(border_x, border_y - 0.597 * height, z1),
                 Vec3(border_x, border_y, z1)},
                dark);
        // bottom-right
        border_mesh.addQuad(
                {Vec3(border_x - 3, border_y - 0.597 * height - 3, z1),
                 Vec3(border_x + 0.936 * width + 3,
                      border_y - 0.597 * height - 3,
                      z1),
                 Vec3(border_x + 0.936 * width, border_y - 0.597 * height, z1),
                 Vec3(border_x, border_y - 0.597 * height, z1)},
                light);
        border_mesh.addQuad(
                {Vec3(border_x + 0.936 * width, border_y, z1),
                 Vec3(border_x + 0.936 * width + 3, border_y + 3, z1),
                 Vec3(border_x + 0.936 * width + 3,
                      border_y - 0.597 * height - 3,
                      z1),
                 Vec3(border_x + 0.936 * width,
                      border_y + -0.597 * height,
                      z1)},
                light);
    }
    context.draw(border_mesh);

    // The live terrain preview, in its own viewport (glViewport()'s float
    // -> int truncation kept), cleared to black, then the menu's own
    // viewport and camera restored.
    render::Rect const saved_viewport = context.viewport();
    math::Mat4<float> const saved_projection = context.projection();
    math::Mat4<float> const saved_view = context.view();
    render::Rect const preview = vulkan_earth::glRect(
            static_cast<std::int32_t>(x_pos + 0.8 * width),
            static_cast<std::int32_t>(y_pos),
            static_cast<std::int32_t>(0.9417 * width),
            static_cast<std::int32_t>(0.6 * height));
    context.setViewport(preview);
    context.clearColorAndDepth(Vec4(0, 0, 0, 0));
    context.setCamera(
            vulkan_graphix::Tools::getPerspectiveProjectionMatrix(
                    ((0.9417 * width) / (0.6 * height)), 45.0, 1, 2.0e8f),
            glm::lookAt(Vec3(cam_x, cam_y, cam_z),
                        Vec3((tm->getActualSize() / 2.0),
                             0.0f,
                             (tm->getActualSize() / 2.0)),
                        Vec3(0.0f, 1.0f, 0.0f)));
    tm->draw(context);
    context.setViewport(saved_viewport);
    context.setCamera(saved_projection, saved_view);
}

std::string SubMenuLandscape::collectData() {
    std::string optionsarray = "/Landscape/";
    for (std::int32_t x = 0; x < num_control_items_lnd; x++) {
        if (sub_menu_button[x]) {
            optionsarray += sub_menu_button[x]->collectData();
            optionsarray += "/";
        }
    }
    return optionsarray;
}

void SubMenuLandscape::subMenuMouseTest(std::int32_t x,
                                        std::int32_t y,
                                        std::int32_t button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (std::int32_t button_i = 0; button_i < num_control_items_lnd;
             button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                            // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                            // CLICKED A ARROW BUTTON???
            if ((x >= sub_menu_button[button_i]->getXPos()) &&
                (x <= (sub_menu_button[button_i]->getXPos() +
                       sub_menu_button[button_i]->getWidth())) &&
                (y <= sub_menu_button[button_i]->getYPos()) &&
                (y >= (sub_menu_button[button_i]->getYPos() -
                       sub_menu_button[button_i]->getHeight()))) {
                sub_menu_button[button_i]->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // YOU PRESSED OVER A ARROWBUTTON
                button_pressed = sub_menu_button[button_i];
                numberpressed = button_i;
            }
        }
        old_mouse_x = x;
        old_mouse_y = y;
    } else if (!button_down) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                                // GOES UP
        if (button_pressed !=
            nullptr) {  // IF YOU MANAGED TO CLICK INSIDE AN ARROW BUTTON
                        // CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if ((x >= button_pressed->getXPos()) &&
                (x <=
                 (button_pressed->getXPos() + button_pressed->getWidth())) &&
                (y <= button_pressed->getYPos()) &&
                (y >=
                 (button_pressed->getYPos() - button_pressed->getHeight()))) {
                if (numberpressed == preview_button) {
                    // RIGHT NOW THERE ARE ONLY 3 OPTIONS AND subMenuButton[3]
                    // IS THE BUTTON ITSELF
                    /********************************************************************************/
                    /*		Smoothness		--			out1,i1
                     */
                    /*		Hill Height		--			out2,i2
                     */
                    /*		Terrain Texture	--			out3,i3
                     */
                    /********************************************************************************/
                    stringstream ss1(sub_menu_button[0]->collectData());
                    std::int32_t i1;
                    if (!(ss1 >> i1)) i1 = 0;
                    stringstream ss2(sub_menu_button[1]->collectData());
                    std::int32_t i2;
                    if (!(ss2 >> i2)) i2 = 0;
                    tm->prepareData(2500,          // int steps
                                    i2 * i2 + 10,  // int increase
                                    30,            // float radius
                                    5,             // int randomJump % (1-100)
                                    i1);           // int smoothness
                    tm->selectTexture(sub_menu_button[2]->collectData());
                    button_pressed->mouseClickEvent(x, y, button_down, false);
                    numberpressed = -1;
                    button_pressed = nullptr;
                    playSFX(SMALL_CLICK);
                } else {
                    button_pressed->mouseClickEvent(
                            x,
                            y,
                            button_down,
                            true);  // IF YOU ARE THEN TELL THE ARROW BUTTON
                                    // YOU RELEASE THE MOUSE
                    numberpressed = -1;
                    button_pressed = nullptr;
                }
            } else {
                button_pressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                button_pressed = nullptr;
                numberpressed = -1;
            }
        }
        old_mouse_x = -1;
        old_mouse_y = -1;
    }
}

void SubMenuLandscape::updateMouse(std::int32_t x, std::int32_t y) {
    if (((x >= x_pos + 0.8 * width) &&
         (x <= x_pos + 0.8 * width + (0.9417 * width))) &&
        ((y >= y_pos - 0.15 * height) &&
         (y <= y_pos - 0.15 * height + (0.6 * height)))) {
        float new_cam_x = cam_x, new_cam_y = cam_y, new_cam_z = cam_z;
        if (x < old_mouse_x) {
            new_cam_x =
                    (cam_x - (tm->getActualSize() / 2.0)) * cos(-PI / 180) -
                    (cam_z - (tm->getActualSize() / 2.0)) * sin(-PI / 180) +
                    (tm->getActualSize() / 2.0);
            new_cam_z =
                    (cam_x - (tm->getActualSize() / 2.0)) * sin(-PI / 180) +
                    (cam_z - (tm->getActualSize() / 2.0)) * cos(-PI / 180) +
                    (tm->getActualSize() / 2.0);
        }
        if (x > old_mouse_x) {
            new_cam_x = (cam_x - (tm->getActualSize() / 2.0)) * cos(PI / 180) -
                        (cam_z - (tm->getActualSize() / 2.0)) * sin(PI / 180) +
                        (tm->getActualSize() / 2.0);
            new_cam_z = (cam_x - (tm->getActualSize() / 2.0)) * sin(PI / 180) +
                        (cam_z - (tm->getActualSize() / 2.0)) * cos(PI / 180) +
                        (tm->getActualSize() / 2.0);
        }
        if (y < old_mouse_y) {
        }
        if (y > old_mouse_y) {
        }
        cam_x = new_cam_x;
        cam_y = new_cam_y;
        cam_z = new_cam_z;
        old_mouse_x = x;
        old_mouse_y = y;
    }
    std::int32_t win_width = vulkan_earth::windowWidth();
    std::int32_t win_height = vulkan_earth::windowHeight();
    sub_menu_button[0]->updateMouse(x - (win_width / 2), (win_height / 2) - y);
    sub_menu_button[1]->updateMouse(x - (win_width / 2), (win_height / 2) - y);
    sub_menu_button[2]->updateMouse(x - (win_width / 2), (win_height / 2) - y);
}
