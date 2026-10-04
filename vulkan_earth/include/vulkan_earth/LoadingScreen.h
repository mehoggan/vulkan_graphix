#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include <cstdint>

#include "vulkan_earth/ImageObject.h"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"

namespace vulkan_graphix::Render {
class RenderContext;
}

class LoadingScreen {
public:
    LoadingScreen();
    LoadingScreen(float x,
                  float y,
                  float z,
                  std::int32_t new_width,
                  std::int32_t new_height,
                  float red,
                  float green,
                  float blue,
                  float alpha);
    ~LoadingScreen();
    void draw(vulkan_graphix::Render::RenderContext& context);

private:
    float pos[3];
    float color[4];
    std::int32_t width, height;
    ImageObject* image;
    vulkan_graphix::Render::UiMesh frame_mesh;
};

#endif
