#include "vulkan_earth/LoadingScreen.h"
#include <cstdint>
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/ImageObject.h"
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
    m_pos[0] = x;
    m_pos[1] = y;
    m_pos[2] = z;
    m_width = new_width;
    m_height = new_height;
    m_color[0] = red;
    m_color[1] = green;
    m_color[2] = blue;
    m_color[3] = alpha;
    m_image = new ImageObject(m_pos[0] + (new_width * 0.03),
      m_pos[1] - (new_height * 0.03),
      m_pos[2] + 2,
      new_width * 0.94,
      new_height * 0.94,
      .006 * new_width,
      1024,
      1024,
      "loading_screen.raw");
}
LoadingScreen::~LoadingScreen() { delete m_image; }

void LoadingScreen::draw(vulkan_graphix::Render::RenderContext& context) {
    if (m_frame_mesh.triangles().empty()) {
        // The same raised 3-pixel bevel every button draws.
        vulkan_earth::appendBevel(m_frame_mesh,
          m_pos[0],
          m_pos[1],
          m_pos[2],
          static_cast<float>(m_width),
          static_cast<float>(m_height),
          vulkan_graphix::Math::Vec4<float>(
            m_color[0], m_color[1], m_color[2], m_color[3]),
          false);
    }
    context.draw(m_frame_mesh);
    m_image->draw(context);
}
