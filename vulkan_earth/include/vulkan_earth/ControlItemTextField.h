#ifndef CONTROL_ITEM_TEXT_FIELD_H
#define CONTROL_ITEM_TEXT_FIELD_H

#include <cstdint>
#include <string>

#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/render/Mesh.h"

class TextObject;

class ControlItemTextField : public ControlItem {
public:
    ControlItemTextField();
    ControlItemTextField(float new_x_pos,
                         float new_y_pos,
                         float new_z_pos,
                         float red,
                         float green,
                         float blue,
                         std::int32_t new_width,
                         std::int32_t new_height);
    ~ControlItemTextField() override;
    void draw(vulkan_earth::render::RenderContext& context) override;
    void mouseClickEvent(std::int32_t x,
                         std::int32_t y,
                         std::int32_t state,
                         bool still_over_control_item_text_field) override;
    float getXPos() override;
    float getYPos() override;
    float getHeight() override;
    float getWidth() override;
    bool isTextFieldActive();
    std::string collectData() override;
    void updateMouse(std::int32_t x, std::int32_t y) override;
    void keyHandler(std::uint8_t key);
    void deactivate();
    void setOptionText(const std::string& new_text) override;
    void clearTextBuffer();
    void setTextBuffer(const std::string& new_text);

private:
    void setOptionText(std::int32_t index) override;

    float x_pos;
    float y_pos;
    float z_pos;
    float color[4];
    std::int32_t width;
    std::int32_t height;
    TextObject* current_text;
    bool text_field_active;
    std::string current_chars;
    std::int32_t current_length;
    std::int32_t number_of_frames;
    std::int32_t text_cursor_on;  // this is a toggle, -1 off, 1 on
    vulkan_earth::render::UiMesh frame_mesh;
    vulkan_earth::render::UiMesh cursor_mesh;
    bool cursor_built_visible = false;
    std::string cursor_built_chars;
};
#endif  // Control_ITEM_TEXT_FIELD_H