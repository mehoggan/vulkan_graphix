#include "vulkan_earth/SkyboxFactory.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include "vulkan_earth/MacroCrtdbg.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

SkyboxFactory::SkyboxFactory() = default;

SkyboxFactory::SkyboxFactory(std::int32_t size_of_box) {
  m_size = size_of_box;
  // One 1024x1024 image on every face (the per-face images the original
  // had begun slicing out are all commented out there too); GL_LINEAR,
  // GL_CLAMP.
  m_texture = render::Renderer::instance().loadRawTexture(
      "SkyBox.raw", 1024, 1024, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
  if (!m_texture) {
    std::cerr << "ERROR: File Not Found" << std::endl;
    exit(0);
  }
  buildGeometry();
}

SkyboxFactory::~SkyboxFactory() = default;

void SkyboxFactory::buildGeometry() {
  std::int32_t start = -m_size / 2;
  std::int32_t bound = m_size / 2;
  std::int32_t scale = 100;
  m_mesh.clear();
  m_mesh.setTexture(m_texture.get());
  // GL_REPLACE: texels only (the UI shader's params.x).
  m_mesh.setParams(math::Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f));
  // front, right, back, left, top, bottom (the box's top is at half
  // height)
  m_mesh.addTexturedQuad(
      {math::Vec3<float>(start * scale, start * scale, start * scale),
          math::Vec3<float>(bound * scale, start * scale, start * scale),
          math::Vec3<float>(bound * scale, bound * scale / 2, start * scale),
          math::Vec3<float>(start * scale, bound * scale / 2, start * scale)},
      {math::Vec2<float>(0, 1),
          math::Vec2<float>(0, 0),
          math::Vec2<float>(1, 0),
          math::Vec2<float>(1, 1)},
      math::Vec4<float>(1.0f));
  m_mesh.addTexturedQuad(
      {math::Vec3<float>(bound * scale, start * scale, start * scale),
          math::Vec3<float>(bound * scale, start * scale, bound * scale),
          math::Vec3<float>(bound * scale, bound * scale / 2, bound * scale),
          math::Vec3<float>(bound * scale, bound * scale / 2, start * scale)},
      {math::Vec2<float>(0, 1),
          math::Vec2<float>(0, 0),
          math::Vec2<float>(1, 0),
          math::Vec2<float>(1, 1)},
      math::Vec4<float>(1.0f));
  m_mesh.addTexturedQuad(
      {math::Vec3<float>(bound * scale, start * scale, bound * scale),
          math::Vec3<float>(start * scale, start * scale, bound * scale),
          math::Vec3<float>(start * scale, bound * scale / 2, bound * scale),
          math::Vec3<float>(bound * scale, bound * scale / 2, bound * scale)},
      {math::Vec2<float>(0, 1),
          math::Vec2<float>(0, 0),
          math::Vec2<float>(1, 0),
          math::Vec2<float>(1, 1)},
      math::Vec4<float>(1.0f));
  m_mesh.addTexturedQuad(
      {math::Vec3<float>(start * scale, start * scale, bound * scale),
          math::Vec3<float>(start * scale, start * scale, start * scale),
          math::Vec3<float>(start * scale, bound * scale / 2, start * scale),
          math::Vec3<float>(start * scale, bound * scale / 2, bound * scale)},
      {math::Vec2<float>(0, 1),
          math::Vec2<float>(0, 0),
          math::Vec2<float>(1, 0),
          math::Vec2<float>(1, 1)},
      math::Vec4<float>(1.0f));
  m_mesh.addTexturedQuad(
      {math::Vec3<float>(start * scale, bound * scale / 2, start * scale),
          math::Vec3<float>(bound * scale, bound * scale / 2, start * scale),
          math::Vec3<float>(bound * scale, bound * scale / 2, bound * scale),
          math::Vec3<float>(start * scale, bound * scale / 2, bound * scale)},
      {math::Vec2<float>(0, 1),
          math::Vec2<float>(0, 0),
          math::Vec2<float>(1, 0),
          math::Vec2<float>(1, 1)},
      math::Vec4<float>(1.0f));
  m_mesh.addTexturedQuad(
      {math::Vec3<float>(start * scale, start * scale, start * scale),
          math::Vec3<float>(start * scale, start * scale, bound * scale),
          math::Vec3<float>(bound * scale, start * scale, bound * scale),
          math::Vec3<float>(bound * scale, start * scale, start * scale)},
      {math::Vec2<float>(0, 1),
          math::Vec2<float>(0, 0),
          math::Vec2<float>(1, 0),
          math::Vec2<float>(1, 1)},
      math::Vec4<float>(1.0f));
}

void SkyboxFactory::draw(render::RenderContext& context) {
  context.draw(m_mesh);
}
