#ifndef VULKAN_EARTH_LOADINGSCREEN_H
#define VULKAN_EARTH_LOADINGSCREEN_H

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
    float m_pos[3];
    float m_color[4];
    std::int32_t m_width, m_height;
    ImageObject* m_image;
    vulkan_graphix::Render::UiMesh m_frame_mesh;
};

#endif
