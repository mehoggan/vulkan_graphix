#include "vulkan_earth/SubMenuSound.h"
#include <SDL/SDL_mixer.h>
#include <cstdint>
#include <string>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemCheckBox.h"
#include "vulkan_earth/ControlItemSelectionBox.h"
#include "vulkan_earth/ControlItemSliderbar.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

SubMenuSound::SubMenuSound() = default;

SubMenuSound::SubMenuSound(std::int32_t id,
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
    m_uniqueidentifier = id;
    m_x_pos = new_x_pos;
    m_y_pos = new_y_pos;
    m_z_pos = new_z_pos;
    m_percent_border = new_percent_border;
    m_color[0] = red;
    m_color[1] = green;
    m_color[2] = blue;
    m_color[3] = 1.0;
    m_width = new_width;
    m_height = new_height;
    m_caption = new_caption;

    /*	BUTTON TEXT PLACEMENT	*/
    std::int32_t real_length = 0;
    for (char ch : m_caption) {
        real_length += vulkan_earth::textAdvance(
                vulkan_earth::FontId::TimesRoman24, ch);
    }
    float label_x_pos = m_x_pos + ((m_width) / 2) - (real_length / 2);
    float label_y_pos = m_y_pos - m_height / 20;
    /*	END OF BUTTON TEXT PLACEMENT	*/

    m_label = new TextObject(m_caption,
                             label_x_pos,
                             label_y_pos,
                             (m_z_pos + 1),
                             vulkan_earth::FontId::TimesRoman24,
                             0.0f,
                             0.0f,
                             0.0f);
    m_button_pressed = nullptr;
    /*
    subMenuButton[0] = new
    ControlItemSelectionBox(xPos+(width/2)-(0.3*width),yPos-(height*0.2),
    zPos+1, 0.5f, 0.5f, 0.5f, 0.6f*width,0.06*(height), "SFX
    Volume","100/0/10/20/30/40/50/60/70/80/90/"); subMenuButton[1] = new
    ControlItemSelectionBox(
    xPos+(width/2)-(0.3*width),yPos-(height*0.27),
    zPos+1, 0.5f, 0.5f, 0.5f, 0.6f*width,0.06*(height),
                                                        "Music
    Volume","100/0/10/20/30/40/50/60/70/80/90/");
    //*/
    m_sub_menu_button[0] =
            new ControlItemSliderbar(m_x_pos + (m_width / 2) - (0.3 * m_width),
                                     m_y_pos - (m_height * 0.2),
                                     m_z_pos + 1,
                                     0.5f,
                                     0.5f,
                                     0.5f,
                                     0.6f * m_width,
                                     0.07 * (m_height),
                                     "SFX Volume",
                                     "0/10/20/30/40/50/60/70/80/90/100/",
                                     10);
    m_sub_menu_button[1] =
            new ControlItemSliderbar(m_x_pos + (m_width / 2) - (0.3 * m_width),
                                     m_y_pos - (m_height * 0.3),
                                     m_z_pos + 1,
                                     0.5f,
                                     0.5f,
                                     0.5f,
                                     0.6f * m_width,
                                     0.07 * (m_height),
                                     "Music Volume",
                                     "0/10/20/30/40/50/60/70/80/90/100/",
                                     10);
}

SubMenuSound::~SubMenuSound() {
    for (std::int32_t i = 0; i < num_control_items_snd; i++)
        delete m_sub_menu_button[i];
    delete m_label;
}

std::int32_t SubMenuSound::getUNIQUEIDENTIFIER() { return m_uniqueidentifier; }
void SubMenuSound::setUNIQUEIDENTIFIER(std::int32_t id) {
    m_uniqueidentifier = id;
}
float SubMenuSound::getXPos() { return m_x_pos; }
void SubMenuSound::setXPos(float new_xpos) { m_x_pos = new_xpos; }
float SubMenuSound::getYPos() { return m_y_pos; }
void SubMenuSound::setYPos(float new_ypos) { m_y_pos = new_ypos; }
float SubMenuSound::getZPos() { return m_z_pos; }
void SubMenuSound::setZPos(float new_zpos) { m_z_pos = new_zpos; }
float SubMenuSound::getRed() { return m_color[0]; }
void SubMenuSound::setRed(float red) { m_color[0] = red; }
float SubMenuSound::getGreen() { return m_color[1]; }
void SubMenuSound::setGreen(float green) { m_color[1] = green; }
float SubMenuSound::getBlue() { return m_color[2]; }
void SubMenuSound::setBlue(float blue) { m_color[2] = blue; }
std::int32_t SubMenuSound::getWidth() { return m_width; }
void SubMenuSound::setWdith(std::int32_t new_width) { m_width = new_width; }
std::int32_t SubMenuSound::getHeight() { return m_height; }
void SubMenuSound::setHeight(std::int32_t new_height) {
    m_height = new_height;
}
std::string SubMenuSound::getCaption() { return m_caption; }
void SubMenuSound::setCaption(const std::string& new_caption) {
    m_caption = new_caption;
}
float SubMenuSound::getPerecentBorder() { return m_percent_border; }
void SubMenuSound::setPercentBorder(float percent) {
    m_percent_border = percent;
}

void SubMenuSound::draw(render::RenderContext& context) {
    // The same raised 3-pixel bevel every button draws.
    if (m_frame_mesh.triangles().empty()) {
        vulkan_earth::appendBevel(
                m_frame_mesh,
                m_x_pos,
                m_y_pos,
                m_z_pos,
                m_width,
                m_height,
                math::Vec4<float>(
                        m_color[0], m_color[1], m_color[2], m_color[3]),
                false);
    }
    context.draw(m_frame_mesh);
    m_label->draw(context);
    for (std::int32_t i = 0; i < num_control_items_snd; i++) {
        if (m_sub_menu_button[i]) {
            m_sub_menu_button[i]->draw(context);
        }
    }
}

std::string SubMenuSound::collectData() { return "Sound:"; }

void SubMenuSound::subMenuMouseTest(std::int32_t x,
                                    std::int32_t y,
                                    std::int32_t button_down) {
    if (button_down) {  // FIRST CONDITION IS LEFT MOUSE BUTTON DOWN
        for (std::int32_t button_i = 0; button_i < num_control_items_snd;
             button_i++) {  // SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                            // IF YOU DID NOT CLICK A BUTTON PERHAPS YOU
                            // CLICKED A ARROW BUTTON???
            if ((x >= m_sub_menu_button[button_i]->getXPos()) &&
                (x <= (m_sub_menu_button[button_i]->getXPos() +
                       m_sub_menu_button[button_i]->getWidth())) &&
                (y <= m_sub_menu_button[button_i]->getYPos()) &&
                (y >= (m_sub_menu_button[button_i]->getYPos() -
                       m_sub_menu_button[button_i]->getHeight()))) {
                m_sub_menu_button[button_i]->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // YOU PRESSED OVER A ARROWBUTTON
                m_button_pressed = m_sub_menu_button[button_i];
            }
        }
    } else if (!button_down) {  // IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON
                                // GOES UP
        if (m_button_pressed !=
            nullptr) {  // IF YOU MANAGED TO CLICK INSIDE AN ARROW BUTTON
                        // CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if ((x >= m_button_pressed->getXPos()) &&
                (x <= (m_button_pressed->getXPos() +
                       m_button_pressed->getWidth())) &&
                (y <= m_button_pressed->getYPos()) &&
                (y >= (m_button_pressed->getYPos() -
                       m_button_pressed->getHeight()))) {
                m_button_pressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        true);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                // RELEASE THE MOUSE
                changeVolumes(m_button_pressed);
            } else {
                m_button_pressed->mouseClickEvent(
                        x,
                        y,
                        button_down,
                        false);  // IF YOU ARE THEN TELL THE ARROW BUTTON YOU
                                 // RELEASE THE MOUSE
                m_button_pressed = nullptr;
            }
        }
    }
}

void SubMenuSound::updateMouse(std::int32_t x, std::int32_t y) {
    m_sub_menu_button[0]->updateMouse(x, y);
    m_sub_menu_button[1]->updateMouse(x, y);
    changeVolumes(m_button_pressed);
}

void SubMenuSound::changeVolumes(ControlItem* the_sub_menu_button) {
    // SFX volume handler
    if (the_sub_menu_button == m_sub_menu_button[0]) {
        std::int32_t new_volume =
                atoi(the_sub_menu_button->collectData().c_str());
        Mix_Volume(-1,
                   128 / 100 *
                           new_volume);  //-1 is to apply to all allocated
                                         // channels, 128 is the maximum volume
    }
    // Music volume handler
    else if (the_sub_menu_button == m_sub_menu_button[1]) {
        std::int32_t new_volume =
                atoi(the_sub_menu_button->collectData().c_str());
        Mix_VolumeMusic(128 / 100 *
                        new_volume);  // music has its special channel, so
                                      // don't need to specify which channel.
    }
}