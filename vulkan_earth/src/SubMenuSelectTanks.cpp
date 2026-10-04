#include "vulkan_earth/SubMenuSelectTanks.h"
#include <cstdint>
#include <string>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/ControlItemCheckBox.h"
#include "vulkan_earth/ControlItemSelectionBox.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/SubMenu.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

SubMenuSelectTanks::SubMenuSelectTanks() = default;

SubMenuSelectTanks::SubMenuSelectTanks(std::int32_t id,
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
}

SubMenuSelectTanks::~SubMenuSelectTanks() { delete label; }

std::int32_t SubMenuSelectTanks::getUNIQUEIDENTIFIER() {
    return uniqueidentifier;
}
void SubMenuSelectTanks::setUNIQUEIDENTIFIER(std::int32_t id) {
    uniqueidentifier = id;
}
float SubMenuSelectTanks::getXPos() { return x_pos; }
void SubMenuSelectTanks::setXPos(float new_xpos) { x_pos = new_xpos; }
float SubMenuSelectTanks::getYPos() { return y_pos; }
void SubMenuSelectTanks::setYPos(float new_ypos) { y_pos = new_ypos; }
float SubMenuSelectTanks::getZPos() { return z_pos; }
void SubMenuSelectTanks::setZPos(float new_zpos) { z_pos = new_zpos; }
float SubMenuSelectTanks::getRed() { return color[0]; }
void SubMenuSelectTanks::setRed(float red) { color[0] = red; }
float SubMenuSelectTanks::getGreen() { return color[1]; }
void SubMenuSelectTanks::setGreen(float green) { color[1] = green; }
float SubMenuSelectTanks::getBlue() { return color[2]; }
void SubMenuSelectTanks::setBlue(float blue) { color[2] = blue; }
std::int32_t SubMenuSelectTanks::getWidth() { return width; }
void SubMenuSelectTanks::setWdith(std::int32_t new_width) {
    width = new_width;
}
std::int32_t SubMenuSelectTanks::getHeight() { return height; }
void SubMenuSelectTanks::setHeight(std::int32_t new_height) {
    height = new_height;
}
std::string SubMenuSelectTanks::getCaption() { return caption; }
void SubMenuSelectTanks::setCaption(const std::string& new_caption) {
    caption = new_caption;
}
float SubMenuSelectTanks::getPerecentBorder() { return percent_border; }
void SubMenuSelectTanks::setPercentBorder(float percent) {
    percent_border = percent;
}

void SubMenuSelectTanks::draw(render::RenderContext& context) {
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
    for (std::int32_t i = 0; i < num_control_items_st; i++) {
        if (sub_menu_button[i]) {
            // subMenuButton[i]->draw(context);
        }
    }
}

std::string SubMenuSelectTanks::collectData() { return "SelectTanks:"; }

void SubMenuSelectTanks::subMenuMouseTest(std::int32_t x,
                                          std::int32_t y,
                                          std::int32_t button_down) {
    /*
    if(buttonDown){
    //FIRST CONDITION IS LEFT MOUSE BUTTON DOWN for(int
    button_i=0;button_i<NUM_CONTROL_ITEMS_ST;button_i++){
    //SCAN ALL BUTTONS TO SEE IF ONE WAS CLICKED
                                                                                                //IF YOU DID NOT CLICK A BUTTON PERHAPS YOU CLICKED A ARROW BUTTON???
            if	((x>=subMenuButton[button_i]->getXPos())	&&
    (x<=(subMenuButton[button_i]->getXPos()+subMenuButton[button_i]->getWidth()))
                                                                            &&
                (y<=subMenuButton[button_i]->getYPos())	&&
    (y>=(subMenuButton[button_i]->getYPos()-subMenuButton[button_i]->getHeight()))){
                    subMenuButton[button_i]->mouseClickEvent(x,y,buttonDown,true);
    //YOU PRESSED OVER A ARROWBUTTON
                    buttonPressed=subMenuButton[button_i];
            }
        }
    }else if(!buttonDown){
    //IF BUTTON WENT DOWN 2nd CONDITION IS BUTTON GOES UP
        if(buttonPressed!=NULL){
    //IF YOU MANAGED TO CLICK INSIDE AN ARROW BUTTON
                                                                                        //CHECK TO MAKE SURE YOU ARE OVER THE SAME ONE
            if	((x>=buttonPressed->getXPos())		&&
    (x<=(buttonPressed->getXPos()+buttonPressed->getWidth()))
                                                                &&
                (y<=buttonPressed->getYPos())		&&
    (y>=(buttonPressed->getYPos()-buttonPressed->getHeight()))) {
                buttonPressed->mouseClickEvent(x,y,buttonDown,true);
    //IF YOU ARE THEN TELL THE ARROW BUTTON YOU RELEASE THE MOUSE }else {
                buttonPressed->mouseClickEvent(x,y,buttonDown,false);
    //IF YOU ARE THEN TELL THE ARROW BUTTON YOU RELEASE THE MOUSE
                buttonPressed=NULL;
            }
        }
    }
    */
}

void SubMenuSelectTanks::updateMouse(std::int32_t x, std::int32_t y) {}