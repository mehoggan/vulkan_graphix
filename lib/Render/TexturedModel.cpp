#include "vulkan_graphix/Render/TexturedModel.h"

#include <cstdio>
#include <vector>

#include "vulkan_graphix/Tools.h"

namespace vulkan_graphix::Render {

MeshVertices loadOglMesh(const std::string& filename) {
  const std::vector<Tools::OglVertexData> data =
      Tools::loadOglMeshData(filename);
  MeshVertices vertices;
  vertices.reserve(data.size());
  for (const Tools::OglVertexData& vertex : data) {
    vertices.add(
        {vertex.m_position,
         vertex.m_normal,
         Math::Vec2<float>(vertex.m_texcoord.x, vertex.m_texcoord.y)});
  }
  return vertices;
}

bool TexturedModel::loadOgl(const std::string& filename) {
  const MeshVertices vertices = loadOglMesh(filename);
  if (vertices.empty()) {
    std::fprintf(
        stderr, "vulkan_graphix: could not load model %s\n", filename.c_str());
    m_mesh.reset();
    return false;
  }
  m_mesh = Renderer::instance().createMesh(vertices);
  return m_mesh != nullptr;
}

void TexturedModel::loadRawTexture(
    const std::string& filename,
    std::uint32_t width,
    std::uint32_t height,
    VkSamplerAddressMode address_mode) {
  m_texture = Renderer::instance().loadRawTexture(
      filename, width, height, address_mode);
}

bool TexturedModel::loaded() const { return m_mesh != nullptr; }

void TexturedModel::draw(
    RenderContext& context,
    PipelineHandle pipeline,
    const Math::Mat4<float>& model,
    const Math::Vec4<float>& params) const {
  if (m_mesh) {
    context.drawMesh(*m_mesh, pipeline, m_texture.get(), model, params);
  }
}

}  // namespace vulkan_graphix::Render
