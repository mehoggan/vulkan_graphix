#ifndef IMAGE_OBJECT_H
#define IMAGE_OBJECT_H

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

    float x_pos;
    float y_pos;
    float z_pos;
    std::int32_t width;
    std::int32_t height;
    float border_size;
    std::shared_ptr<vulkan_graphix::Render::Texture> texture;
    vulkan_graphix::Render::UiMesh image_mesh;
    vulkan_graphix::Render::UiMesh border_mesh;
    bool geometry_dirty = true;
};

#endif
