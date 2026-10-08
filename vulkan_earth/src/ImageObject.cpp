#include "vulkan_earth/ImageObject.h"
#include <cstdint>
#include <string>

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

ImageObject::ImageObject() = default;

ImageObject::ImageObject(float new_x_pos,
    float new_y_pos,
    float new_z_pos,
    std::int32_t new_width,
    std::int32_t new_height,
    float border,
    std::int32_t i_width,
    std::int32_t i_height,
    const std::string& filename) {
  m_x_pos = new_x_pos;
  m_y_pos = new_y_pos;
  m_z_pos = new_z_pos;
  m_width = new_width;
  m_height = new_height;
  m_border_size = border;
  // GL_LINEAR filtering, GL_CLAMP wrapping (as draw() used to set).
  m_texture = render::Renderer::instance().loadRawTexture(filename,
      static_cast<std::uint32_t>(i_width),
      static_cast<std::uint32_t>(i_height),
      VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
  m_image_mesh.setTexture(m_texture.get());
  // GL_REPLACE: texels only (the UI shader's params.x).
  m_image_mesh.setParams(math::Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f));
}

ImageObject::~ImageObject() = default;

/*GETTERS & SETTERS*/
float ImageObject::getXpos() { return m_x_pos; }
float ImageObject::getYpos() { return m_y_pos; }
float ImageObject::getZpos() { return m_z_pos; }
std::int32_t ImageObject::getWidth() { return m_width; }
std::int32_t ImageObject::getHeight() { return m_height; }
void ImageObject::setXpos(float x) {
  m_x_pos = x;
  m_geometry_dirty = true;
}
void ImageObject::setYpos(float y) {
  m_y_pos = y;
  m_geometry_dirty = true;
}
void ImageObject::setZpos(float z) {
  m_z_pos = z;
  m_geometry_dirty = true;
}
void ImageObject::setWidth(std::int32_t w) {
  m_width = w;
  m_geometry_dirty = true;
}
void ImageObject::setHeight(std::int32_t h) {
  m_height = h;
  m_geometry_dirty = true;
}

void ImageObject::buildGeometry() {
  using Vec3 = math::Vec3<float>;
  using Vec4 = math::Vec4<float>;
  const float w = static_cast<float>(m_width);
  const float h = static_cast<float>(m_height);
  const float b = m_border_size;

  // image plane
  m_image_mesh.clear();
  m_image_mesh.addTexturedQuad({Vec3(m_x_pos, m_y_pos, m_z_pos),
                                   Vec3(m_x_pos, m_y_pos - h, m_z_pos),
                                   Vec3(m_x_pos + w, m_y_pos - h, m_z_pos),
                                   Vec3(m_x_pos + w, m_y_pos, m_z_pos)},
      {math::Vec2<float>(0, 1),
          math::Vec2<float>(0, 0),
          math::Vec2<float>(1, 0),
          math::Vec2<float>(1, 1)},
      Vec4(1.0f));

  m_border_mesh.clear();
  if (m_border_size != 0) {
    // top and right borders
    const Vec4 dark(0.45f, 0.45f, 0.45f, 1.0f);
    m_border_mesh.addQuad({Vec3(m_x_pos, m_y_pos, m_z_pos),
                              Vec3(m_x_pos - b, m_y_pos + b, m_z_pos),
                              Vec3(m_x_pos + w + b, m_y_pos + b, m_z_pos),
                              Vec3(m_x_pos + w, m_y_pos, m_z_pos)},
        dark);
    m_border_mesh.addQuad({Vec3(m_x_pos - b, m_y_pos + b, m_z_pos),
                              Vec3(m_x_pos - b, m_y_pos - h - b, m_z_pos),
                              Vec3(m_x_pos, m_y_pos - h, m_z_pos),
                              Vec3(m_x_pos, m_y_pos, m_z_pos)},
        dark);
    // bottom and left borders
    const Vec4 light(0.85f, 0.85f, 0.85f, 1.0f);
    m_border_mesh.addQuad({Vec3(m_x_pos - b, m_y_pos - h - b, m_z_pos),
                              Vec3(m_x_pos + w + b, m_y_pos - h - b, m_z_pos),
                              Vec3(m_x_pos + w, m_y_pos - h, m_z_pos),
                              Vec3(m_x_pos, m_y_pos - h, m_z_pos)},
        light);
    m_border_mesh.addQuad({Vec3(m_x_pos + w, m_y_pos, m_z_pos),
                              Vec3(m_x_pos + w + b, m_y_pos + b, m_z_pos),
                              Vec3(m_x_pos + w + b, m_y_pos - h - b, m_z_pos),
                              Vec3(m_x_pos + w, m_y_pos - h, m_z_pos)},
        light);
  }
  m_geometry_dirty = false;
}

void ImageObject::draw(render::RenderContext& context) {
  if (m_geometry_dirty) {
    buildGeometry();
  }
  context.draw(m_image_mesh);
  context.draw(m_border_mesh);
}
