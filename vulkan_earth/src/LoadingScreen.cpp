#include "vulkan_earth/LoadingScreen.h"
#include <cstdint>
#include "vulkan_earth/ImageObject.h"
#include "vulkan_earth/render/Renderer.h"
#include "vulkan_earth/render/UiBuilders.h"
#include "vulkan_earth/MacroCrtdbg.h"

LoadingScreen::LoadingScreen() = default;
LoadingScreen::LoadingScreen(float x,
                             float y,
                             float z,
                             std::int32_t new_width,
                             std::int32_t new_height,
                             float red,
                             float green,
                             float blue,
                             float alpha) {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    width = new_width;
    height = new_height;
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;
    image = new ImageObject(pos[0] + (new_width * 0.03),
                            pos[1] - (new_height * 0.03),
                            pos[2] + 2,
                            new_width * 0.94,
                            new_height * 0.94,
                            .006 * new_width,
                            1024,
                            1024,
                            "loading_screen.raw");
}
LoadingScreen::~LoadingScreen() { delete image; }

void LoadingScreen::draw(vulkan_earth::render::RenderContext& context) {
    if (frame_mesh.triangles().empty()) {
        // The same raised 3-pixel bevel every button draws.
        vulkan_earth::render::appendBevel(
                frame_mesh,
                pos[0],
                pos[1],
                pos[2],
                static_cast<float>(width),
                static_cast<float>(height),
                vulkan_earth::render::Vec4(
                        color[0], color[1], color[2], color[3]),
                false);
    }
    context.draw(frame_mesh);
    image->draw(context);
}
