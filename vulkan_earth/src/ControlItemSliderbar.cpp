#include "vulkan_earth/ControlItemSliderbar.h"
#include <cstdint>
#include "vulkan_earth/ControlItem.h"
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/Sound.h"
#include "vulkan_earth/TextObject.h"
#include "vulkan_earth/MacroCrtdbg.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

extern void playSFX(std::int32_t sfx);

ControlItemSliderbar::ControlItemSliderbar() = default;
ControlItemSliderbar::ControlItemSliderbar(
        float new_x_pos,
        float new_y_pos,
        float new_z_pos,
        float red,
        float green,
        float blue,
        std::int32_t new_width,
        std::int32_t new_height,
        const std::string& new_caption,
        const std::string& menu_string,
        std::int32_t slider_starting_index) {
    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = 1.0;
    width = new_width;
    height = new_height;

    bar_width = new_width - new_width * 0.1;
    bar_x_pos = new_x_pos + (new_width - bar_width) / 2;
    bar_y_pos = new_y_pos - new_height / 1.5;
    bar_z_pos = new_z_pos + 0.5;

    slider_x_pos = bar_x_pos - bar_width * 0.012;
    slider_y_pos = new_y_pos - new_height / 1.7;
    slider_z_pos = bar_z_pos + 0.5;
    slider_width = new_width / 5 * 0.1;
    slider_height = new_height * 0.15;

    caption = new_caption;
    menu_info = menu_string;
    is_slider_clicked = false;

    // split menuInfo on '/' into allOptions
    std::string current;
    for (char ch : menu_info) {
        if (ch == '/') {
            all_options.push_back(current);
            current.clear();
        } else {
            current += ch;
        }
    }
    number_of_options = static_cast<std::int32_t>(all_options.size());
    interval = bar_width / (number_of_options -
                            1.0);  // if it's divided by an integer, the whole
                                   // thing becomes an integer value???

    menu_state = slider_starting_index;
    button_state = 0;
    option_text = nullptr;
    setOptionText(menu_state);  // set option to first option
    /*	BUTTON TEXT PLACEMENT	*/
    std::int32_t real_length = 0;
    for (char ch : caption) {
        real_length += vulkan_earth::textAdvance(
                vulkan_earth::FontId::TimesRoman24, ch);
    }
    float label_x_pos = bar_x_pos;
    float label_y_pos = y_pos - height * 0.45;
    /*	END OF BUTTON TEXT PLACEMENT	*/
    label = new TextObject(caption,
                           label_x_pos,
                           label_y_pos,
                           z_pos,
                           vulkan_earth::FontId::TimesRoman24,
                           0.0f,
                           0.0f,
                           0.0f);
}
ControlItemSliderbar::~ControlItemSliderbar() {
    delete option_text;
    delete label;
}

void ControlItemSliderbar::draw(render::RenderContext& context) {
    using Vec3 = math::Vec3<float>;
    using Vec4 = math::Vec4<float>;
    if (frame_mesh.triangles().empty()) {
        // draw main button box (sunken bevel: -0.2 top/left, +0.4
        // bottom/right)
        vulkan_earth::appendFrame(frame_mesh,
                                  x_pos,
                                  y_pos,
                                  z_pos,
                                  width,
                                  height,
                                  Vec4(color[0] - 0.2f,
                                       color[1] - 0.2f,
                                       color[2] - 0.2f,
                                       color[3]),
                                  Vec4(color[0], color[1], color[2], color[3]),
                                  Vec4(color[0] + 0.4f,
                                       color[1] + 0.4f,
                                       color[2] + 0.4f,
                                       color[3]));
        // draw bar lines
        const Vec4 black(0, 0, 0, 1);
        frame_mesh.addLine(
                Vec3(bar_x_pos, bar_y_pos + 1, bar_z_pos),
                Vec3(bar_x_pos + bar_width, bar_y_pos + 1, bar_z_pos),
                black);
        frame_mesh.addLine(Vec3(bar_x_pos, bar_y_pos, bar_z_pos),
                           Vec3(bar_x_pos + bar_width, bar_y_pos, bar_z_pos),
                           black);
        frame_mesh.addLine(
                Vec3(bar_x_pos, bar_y_pos - 1, bar_z_pos),
                Vec3(bar_x_pos + bar_width, bar_y_pos - 1, bar_z_pos),
                black);
        for (std::int32_t i = 0; i < number_of_options; i++) {
            frame_mesh.addLine(Vec3(bar_x_pos + (interval * i),
                                    bar_y_pos + height * 0.07,
                                    bar_z_pos),
                               Vec3(bar_x_pos + (interval * i),
                                    bar_y_pos - height * 0.07,
                                    bar_z_pos),
                               black);
        }
    }
    context.draw(frame_mesh);

    // draw slider: raised when idle, highlighted while being dragged
    if (slider_built_x != slider_x_pos || slider_built_y != slider_y_pos ||
        slider_built_clicked != static_cast<std::int32_t>(is_slider_clicked)) {
        slider_mesh.clear();
        if (!is_slider_clicked) {
            vulkan_earth::appendBevel(
                    slider_mesh,
                    slider_x_pos,
                    slider_y_pos,
                    slider_z_pos,
                    slider_width,
                    slider_height,
                    Vec4(color[0], color[1], color[2], color[3]),
                    false);
        } else {
            vulkan_earth::appendFrame(slider_mesh,
                                      slider_x_pos,
                                      slider_y_pos,
                                      slider_z_pos,
                                      slider_width,
                                      slider_height,
                                      Vec4(color[0] + 0.4f,
                                           color[1] + 0.4f,
                                           color[2] + 0.4f,
                                           color[3]),
                                      Vec4(color[0] + 0.2f,
                                           color[1] + 0.2f,
                                           color[2] + 0.2f,
                                           color[3]),
                                      Vec4(color[0] - 0.2f,
                                           color[1] - 0.2f,
                                           color[2] - 0.2f,
                                           color[3]));
        }
        slider_built_x = slider_x_pos;
        slider_built_y = slider_y_pos;
        slider_built_clicked = static_cast<std::int32_t>(is_slider_clicked);
    }
    context.draw(slider_mesh);

    label->draw(context);
    option_text->draw(context);
}

float ControlItemSliderbar::getXPos() { return x_pos; }
float ControlItemSliderbar::getYPos() { return y_pos; }
float ControlItemSliderbar::getHeight() { return height; }
float ControlItemSliderbar::getWidth() { return width; }
float ControlItemSliderbar::getBarXPos() { return bar_x_pos; }
float ControlItemSliderbar::getInterval() { return interval; }
float ControlItemSliderbar::getSliderXPos() { return slider_x_pos; }
void ControlItemSliderbar::setSliderXPos(float x) { slider_x_pos = x; }
std::string ControlItemSliderbar::collectData() { return current_option; }

void ControlItemSliderbar::setOptionText(const std::string& new_text) {}

void ControlItemSliderbar::setOptionText(std::int32_t index) {
    current_option = all_options[index];
    std::int32_t real_length = 0;
    for (char ch : current_option) {
        real_length += vulkan_earth::textAdvance(
                vulkan_earth::FontId::TimesRoman24, ch);
    }
    float label_x_pos = x_pos + (width / 2) - (real_length / 2);
    float label_y_pos = y_pos - height * 0.45;
    delete option_text;
    option_text = new TextObject(current_option,
                                 label_x_pos,
                                 label_y_pos,
                                 z_pos,
                                 vulkan_earth::FontId::TimesRoman24,
                                 0.0f,
                                 0.0f,
                                 0.0f);
    menu_state = index;
    slider_x_pos = bar_x_pos + interval * index;
}

void ControlItemSliderbar::mouseClickEvent(
        std::int32_t x,
        std::int32_t y,
        std::int32_t state,
        bool /*still_over_control_item_sliderbar*/) {
    if (state == 1) {
        // check if the click is on the slider
        if ((slider_x_pos < x && x < slider_x_pos + slider_width) &&
            (slider_y_pos - slider_height < y && y < slider_y_pos)) {
            is_slider_clicked = true;
        }
        // check if the click is either left or right side from the slider
        else if ((bar_x_pos < x && x < slider_x_pos) &&
                 (slider_y_pos - slider_height - 5.5 < y &&
                  y < slider_y_pos + 5.5)) {
            playSFX(SMALL_CLICK);
            slider_x_pos -= interval;
            menu_state--;
            if (menu_state < 0)  // wrap around check
                menu_state += number_of_options;
            setOptionText(menu_state);
        } else if ((slider_x_pos + slider_width < x &&
                    x < bar_x_pos + bar_width) &&
                   (slider_y_pos - slider_height - 5.5 < y &&
                    y < slider_y_pos + 5.5)) {
            playSFX(SMALL_CLICK);
            slider_x_pos += interval;
            menu_state++;
            if (menu_state == number_of_options)  // wrap around check
                menu_state -= number_of_options;
            setOptionText(menu_state);
        }
    } else {
        is_slider_clicked = false;
    }
}

void ControlItemSliderbar::updateMouse(std::int32_t x, std::int32_t /*y*/) {
    if (is_slider_clicked) {
        // check if the mouse pointer is either left or right side from the
        // slider
        if (bar_x_pos < x && x < slider_x_pos - interval / 2) {
            slider_x_pos -= interval;
            menu_state--;
            if (menu_state < 0)  // wrap around check
                menu_state += number_of_options;
            setOptionText(menu_state);
            playSFX(SMALL_CLICK);
        } else if (slider_x_pos + interval / 2 + slider_width < x &&
                   x < bar_x_pos + bar_width) {
            slider_x_pos += interval;
            menu_state++;
            if (menu_state == number_of_options)  // wrap around check
                menu_state -= number_of_options;
            setOptionText(menu_state);
            playSFX(SMALL_CLICK);
        }
    }
}
