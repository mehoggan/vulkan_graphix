#ifndef VULKAN_EARTH_VBOSHADERLIBRARY_H
#define VULKAN_EARTH_VBOSHADERLIBRARY_H

#include <cstdint>
#include <memory>
#include <string>
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"
#include "vulkan_graphix/Render/Texture.h"

namespace vulkan_graphix::Render {
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
    void draw(vulkan_graphix::Render::RenderContext& context,
      const vulkan_graphix::Math::Mat4<float>& model,
      const vulkan_graphix::Math::Vec4<float>& tint =
        vulkan_graphix::Math::Vec4<float>(1.0f));
    // Loads a .ogl model (libvulkan_graphix's Tools::loadOglMeshData()).
    bool loadClientData(const std::string& model_file);
    // The model's color texture: a headerless RGB .raw image, repeated.
    void loadTexture(
      const char* filename, std::int32_t width, std::int32_t height);
    void swapTexture(
      const char* filename, std::int32_t width, std::int32_t height);

private:
    std::unique_ptr<vulkan_graphix::Render::Mesh> m_mesh;
    std::shared_ptr<vulkan_graphix::Render::Texture> m_color_texture;
};

#endif
