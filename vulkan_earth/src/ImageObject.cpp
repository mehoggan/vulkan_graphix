#include "vulkan_earth/ImageObject.h"
#include <cstdint>
#include <string>
#include "vulkan_earth/render/Renderer.h"
#include "vulkan_earth/render/UiBuilders.h"
#include "vulkan_earth/MacroCrtdbg.h"

namespace render = vulkan_earth::render;

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
    x_pos = new_x_pos;
    y_pos = new_y_pos;
    z_pos = new_z_pos;
    width = new_width;
    height = new_height;
    border_size = border;
    // GL_LINEAR filtering, GL_CLAMP wrapping (as draw() used to set).
    texture = render::Renderer::instance().loadRawTexture(
            filename,
            static_cast<std::uint32_t>(i_width),
            static_cast<std::uint32_t>(i_height),
            VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
    image_mesh.setTexture(texture.get());
    image_mesh.setReplaceTexEnv(true);
}

ImageObject::~ImageObject() = default;

/*GETTERS & SETTERS*/
float ImageObject::getXpos() { return x_pos; }
float ImageObject::getYpos() { return y_pos; }
float ImageObject::getZpos() { return z_pos; }
std::int32_t ImageObject::getWidth() { return width; }
std::int32_t ImageObject::getHeight() { return height; }
void ImageObject::setXpos(float x) {
    x_pos = x;
    geometry_dirty = true;
}
void ImageObject::setYpos(float y) {
    y_pos = y;
    geometry_dirty = true;
}
void ImageObject::setZpos(float z) {
    z_pos = z;
    geometry_dirty = true;
}
void ImageObject::setWidth(std::int32_t w) {
    width = w;
    geometry_dirty = true;
}
void ImageObject::setHeight(std::int32_t h) {
    height = h;
    geometry_dirty = true;
}

void ImageObject::buildGeometry() {
    using render::Vec3;
    using render::Vec4;
    float const w = static_cast<float>(width);
    float const h = static_cast<float>(height);
    float const b = border_size;

    // image plane
    image_mesh.clear();
    image_mesh.addTexturedQuad({Vec3(x_pos, y_pos, z_pos),
                                Vec3(x_pos, y_pos - h, z_pos),
                                Vec3(x_pos + w, y_pos - h, z_pos),
                                Vec3(x_pos + w, y_pos, z_pos)},
                               {render::Vec2(0, 1),
                                render::Vec2(0, 0),
                                render::Vec2(1, 0),
                                render::Vec2(1, 1)},
                               Vec4(1.0f));

    border_mesh.clear();
    if (border_size != 0) {
        // top and right borders
        Vec4 const dark(0.45f, 0.45f, 0.45f, 1.0f);
        render::appendQuad(border_mesh,
                           Vec3(x_pos, y_pos, z_pos),
                           Vec3(x_pos - b, y_pos + b, z_pos),
                           Vec3(x_pos + w + b, y_pos + b, z_pos),
                           Vec3(x_pos + w, y_pos, z_pos),
                           dark);
        render::appendQuad(border_mesh,
                           Vec3(x_pos - b, y_pos + b, z_pos),
                           Vec3(x_pos - b, y_pos - h - b, z_pos),
                           Vec3(x_pos, y_pos - h, z_pos),
                           Vec3(x_pos, y_pos, z_pos),
                           dark);
        // bottom and left borders
        Vec4 const light(0.85f, 0.85f, 0.85f, 1.0f);
        render::appendQuad(border_mesh,
                           Vec3(x_pos - b, y_pos - h - b, z_pos),
                           Vec3(x_pos + w + b, y_pos - h - b, z_pos),
                           Vec3(x_pos + w, y_pos - h, z_pos),
                           Vec3(x_pos, y_pos - h, z_pos),
                           light);
        render::appendQuad(border_mesh,
                           Vec3(x_pos + w, y_pos, z_pos),
                           Vec3(x_pos + w + b, y_pos + b, z_pos),
                           Vec3(x_pos + w + b, y_pos - h - b, z_pos),
                           Vec3(x_pos + w, y_pos - h, z_pos),
                           light);
    }
    geometry_dirty = false;
}

void ImageObject::draw(render::RenderContext& context) {
    if (geometry_dirty) {
        buildGeometry();
    }
    context.draw(image_mesh);
    context.draw(border_mesh);
}
