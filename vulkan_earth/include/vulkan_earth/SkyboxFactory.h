#ifndef VULKAN_EARTH_SKYBOXFACTORY_H
#define VULKAN_EARTH_SKYBOXFACTORY_H

#include <cstdint>
#include <memory>
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"
#include "vulkan_graphix/Render/Texture.h"

namespace vulkan_graphix::Render {
class RenderContext;
}

class SkyboxFactory {
public:
  SkyboxFactory();
  SkyboxFactory(std::int32_t size_of_box);
  ~SkyboxFactory();
  void draw(vulkan_graphix::Render::RenderContext& context);

private:
  void buildGeometry();

  float m_size;
  std::shared_ptr<vulkan_graphix::Render::Texture> m_texture;
  vulkan_graphix::Render::UiMesh m_mesh;
};

#endif  // VULKAN_EARTH_SKYBOXFACTORY_H