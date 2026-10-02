#ifndef VBO_SHADER_LIBRARY_H
#define VBO_SHADER_LIBRARY_H

#include <cstdint>
#include <memory>
#include <string>

#include "vulkan_earth/render/Mesh.h"
#include "vulkan_earth/render/RenderTypes.h"
#include "vulkan_earth/render/Texture.h"

namespace vulkan_earth::render {
class RenderContext;
}

// One textured .ogl model (a tank part, a projectile) in GPU memory, drawn
// with the unlit textured mesh pipeline - what the original's tank shaders
// (VertexTank.vs/FragmentTank.vs) amounted to: the texture color, no
// lighting.
class VBOShaderLibrary {
public:
    VBOShaderLibrary();
    ~VBOShaderLibrary();
    // model is the part's transform relative to the current camera (the
    // original's glMultMatrixf()/glScalef() before drawClientData()); tint
    // multiplies the texel (rgb) and sets the alpha.
    void draw(vulkan_earth::render::RenderContext& context,
              vulkan_earth::render::Mat4 const& model,
              vulkan_earth::render::Vec4 const& tint =
                      vulkan_earth::render::Vec4(1.0f));
    // Loads a .ogl model (libvulkan_graphix's Tools::loadOglMeshData()).
    bool loadClientData(const std::string& model_file);
    // The model's color texture: a headerless RGB .raw image, repeated.
    void loadTexture(const char* filename,
                     std::int32_t width,
                     std::int32_t height);
    void swapTexture(const char* filename,
                     std::int32_t width,
                     std::int32_t height);

private:
    std::unique_ptr<vulkan_earth::render::StaticMesh> mesh;
    std::shared_ptr<vulkan_earth::render::Texture> color_texture;
};

#endif
