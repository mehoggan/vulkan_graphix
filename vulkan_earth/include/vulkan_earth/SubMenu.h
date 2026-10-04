#ifndef VULKAN_EARTH_SUBMENU_H
#define VULKAN_EARTH_SUBMENU_H

#include <cstdint>
#include <string>
#include "vulkan_graphix/Render/Renderer.h"

class TextObject;

namespace vulkan_graphix::Render {
class RenderContext;
}

class SubMenu {
public:
    SubMenu();
    SubMenu(std::int32_t id,
            float x_pos,
            float y_pos,
            float z_pos,
            float red,
            float green,
            float blue,
            std::int32_t width,
            std::int32_t height,
            const std::string& caption,
            float percent_border);
    virtual ~SubMenu() = 0;
    /*	GETTERS AND SETTERS	*/
    virtual std::int32_t getUNIQUEIDENTIFIER() = 0;
    virtual void setUNIQUEIDENTIFIER(std::int32_t id) = 0;
    virtual float getXPos() = 0;
    virtual void setXPos(float new_xpos) = 0;
    virtual float getYPos() = 0;
    virtual void setYPos(float new_ypos) = 0;
    virtual float getZPos() = 0;
    virtual void setZPos(float new_zpos) = 0;
    virtual float getRed() = 0;
    virtual void setRed(float red) = 0;
    virtual float getGreen() = 0;
    virtual void setGreen(float green) = 0;
    virtual float getBlue() = 0;
    virtual void setBlue(float blue) = 0;
    virtual std::int32_t getWidth() = 0;
    virtual void setWdith(std::int32_t width) = 0;
    virtual std::int32_t getHeight() = 0;
    virtual void setHeight(std::int32_t height) = 0;
    virtual std::string getCaption() = 0;
    virtual void setCaption(const std::string& caption) = 0;
    virtual float getPerecentBorder() = 0;
    virtual void setPercentBorder(float percent) = 0;
    /*	END OF GETTERS AND SETTERS	*/
    /*	ACTUAL ACTIONS A SUBMENU CAN MAKE	*/
    virtual void draw(vulkan_graphix::Render::RenderContext& context) = 0;
    virtual std::string collectData() = 0;
    virtual void subMenuMouseTest(std::int32_t x,
                                  std::int32_t y,
                                  std::int32_t button_down) = 0;
    virtual void updateMouse(std::int32_t x, std::int32_t y) = 0;
    /*	END OF ACTIONS A SUBMENU CAN MAKE	*/
};

#endif  // VULKAN_EARTH_SUBMENU_H