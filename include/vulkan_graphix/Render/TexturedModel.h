#ifndef VULKAN_GRAPHIX_RENDER_TEXTUREDMODEL_H
#define VULKAN_GRAPHIX_RENDER_TEXTUREDMODEL_H

// A model and its color texture, drawn through whichever mesh pipeline the
// caller gives it - vulkan_earth's tank parts and projectiles (its old
// VBOShaderLibrary), loaded from .ogl mesh files and .raw images.

#include <cstdint>
#include <memory>
#include <string>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Renderer.h"
#include "vulkan_graphix/Render/Vertex.h"

namespace vulkan_graphix::Render {

class Texture;

// An .ogl mesh file (Tools::loadOglMeshData()) as MeshVertices; empty when
// it can't be read.
MeshVertices loadOglMesh(const std::string& filename);

class TexturedModel {
public:
  // An .ogl mesh file, uploaded once. False (and nothing drawn) when it
  // can't be read.
  bool loadOgl(const std::string& filename);
  // A headerless RGB .raw image as the color texture (shared through the
  // Renderer's texture cache), repeating by default.
  void loadRawTexture(
      const std::string& filename,
      std::uint32_t width,
      std::uint32_t height,
      VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

  bool loaded() const;

  // model is the model's transform; params is the pipeline's params push
  // constant (for vulkan_earth's mesh pipeline, a tint multiplying the
  // texel's rgb and setting its alpha).
  void draw(
      RenderContext& context,
      PipelineHandle pipeline,
      const Math::Mat4<float>& model,
      const Math::Vec4<float>& params = Math::Vec4<float>(1.0f)) const;

private:
  std::unique_ptr<Mesh> m_mesh;
  std::shared_ptr<Texture> m_texture;
};

}  // namespace vulkan_graphix::Render

#endif  // VULKAN_GRAPHIX_RENDER_TEXTUREDMODEL_H
