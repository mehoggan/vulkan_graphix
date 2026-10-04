#include "vulkan_earth/VBOShaderLibrary.h"
#include "vulkan_earth/GameRenderer.h"

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "vulkan_graphix/Tools.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

VBOShaderLibrary::VBOShaderLibrary() = default;

VBOShaderLibrary::~VBOShaderLibrary() = default;

void VBOShaderLibrary::draw(render::RenderContext& context,
                            const math::Mat4<float>& model,
                            const math::Vec4<float>& tint) {
    if (mesh) {
        context.drawMesh(*mesh,
                         vulkan_earth::pipelines().mesh,
                         color_texture.get(),
                         model,
                         tint);
    }
}

bool VBOShaderLibrary::loadClientData(const std::string& model_file) {
    const std::vector<vulkan_graphix::Tools::OglVertexData> data =
            vulkan_graphix::Tools::loadOglMeshData(model_file);
    if (data.empty()) {
        std::printf("ERROR: File %s not found\n", model_file.c_str());
        return false;
    }
    std::vector<render::MeshVertex> vertices;
    vertices.reserve(data.size());
    for (const auto& vertex : data) {
        vertices.push_back(
                {vertex.position,
                 vertex.normal,
                 math::Vec2<float>(vertex.texcoord.x, vertex.texcoord.y)});
    }
    mesh = render::Renderer::instance().createMesh(vertices);
    return mesh != nullptr;
}

void VBOShaderLibrary::swapTexture(const char* filename,
                                   std::int32_t width,
                                   std::int32_t height) {
    loadTexture(filename, width, height);
}

void VBOShaderLibrary::loadTexture(const char* filename,
                                   std::int32_t width,
                                   std::int32_t height) {
    // GL_LINEAR filtering, GL_REPEAT wrapping.
    color_texture = render::Renderer::instance().loadRawTexture(
            filename,
            static_cast<std::uint32_t>(width),
            static_cast<std::uint32_t>(height),
            VK_SAMPLER_ADDRESS_MODE_REPEAT);
}
