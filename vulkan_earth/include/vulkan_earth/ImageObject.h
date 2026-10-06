#ifndef VULKAN_EARTH_IMAGEOBJECT_H
#define VULKAN_EARTH_IMAGEOBJECT_H

#include <cstdint>
#include <memory>
#include <string>
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"
#include "vulkan_graphix/Render/Texture.h"

// A textured rectangle (one of the game's headerless RGB .raw images, drawn
// GL_REPLACE - texels only) with an optional two-tone frame.
class ImageObject {
public:
    ImageObject();
    ImageObject(float new_x_pos,
      float new_y_pos,
      float new_z_pos,
      std::int32_t new_width,
      std::int32_t new_height,
      float border,
      std::int32_t i_width,
      std::int32_t i_height,
      const std::string& filename);
    ~ImageObject();
    float getXpos();
    float getYpos();
    float getZpos();
    std::int32_t getWidth();
    std::int32_t getHeight();
    void setXpos(float x);
    void setYpos(float y);
    void setZpos(float z);
    void setWidth(std::int32_t w);
    void setHeight(std::int32_t h);
    void draw(vulkan_graphix::Render::RenderContext& context);

private:
    void buildGeometry();

    float m_x_pos;
    float m_y_pos;
    float m_z_pos;
    std::int32_t m_width;
    std::int32_t m_height;
    float m_border_size;
    std::shared_ptr<vulkan_graphix::Render::Texture> m_texture;
    vulkan_graphix::Render::UiMesh m_image_mesh;
    vulkan_graphix::Render::UiMesh m_border_mesh;
    bool m_geometry_dirty = true;
};

#endif
