#ifndef VULKAN_GRAPHIX_TUTORIAL11_H
#define VULKAN_GRAPHIX_TUTORIAL11_H

// Ported from vulkan_earth's OpenGL->Vulkan migration pilot
// (VulkanSkyboxPilot, see this repo's git history) into a proper numbered
// tutorial: an indexed, textured cube replicating vulkan_earth's
// SkyboxFactory::draw() six-quad geometry/UV mapping at a small, sane
// scale instead of the game's *100 world units. Reuses Tutorial07's
// unlit-textured shader pair unchanged (a full model-view-projection
// uniform, one combined-image-sampler texture) since a skybox is exactly
// that - just drawn indexed instead of Tutorial07's 4-vertex strip, and
// mouse-orbitable instead of static.
//
// The Vulkan scaffolding - render pass, pipeline, descriptors, buffers,
// texture upload, and the acquire/submit/present loop - is the library's
// (VulkanCommon's ResourceContext/FrameLoop); this file is what's specific
// to the skybox.

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/OrbitCamera.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"
#include "vulkan_graphix/VulkanCommon/FrameLoop.h"
#include "vulkan_graphix/VulkanCommon/ResourceContext.h"

namespace vulkan_graphix {

// Same shape as Tutorial07VertexData on purpose: this tutorial reuses
// Tutorial07's compiled shaders byte-for-byte (see createResources()).
struct Tutorial11VertexData {
  Math::Vec4<float> m_position;
  Math::Vec2<float> m_texcoord;
};

class Tutorial11 : public TutorialBase {
public:
  Tutorial11();
  ~Tutorial11() override;

  // Everything the skybox draws with; call once after prepareVulkan().
  bool createResources();

  bool draw() override;
  void onMouseButton(
      std::int32_t button,
      bool pressed,
      std::int32_t pos_x,
      std::int32_t pos_y) override;
  void onMouseMove(std::int32_t pos_x, std::int32_t pos_y) override;

private:
  Math::Mat4<float> getUniformBufferData() const;
  const std::vector<Tutorial11VertexData>& getVertexData() const;
  const std::vector<std::uint32_t>& getIndexData() const;

  bool childOnWindowSizeChanged() override;
  void childClear() override;

  VulkanCommon::ResourceContext m_resources;
  VulkanCommon::FrameLoop m_frames;
  ImageParameters m_texture;
  BufferParameters m_uniform_buffer;
  BufferParameters m_vertex_buffer;
  BufferParameters m_index_buffer;
  std::uint32_t m_index_count = 0;
  VkDescriptorSet m_descriptor_set = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  OrbitCamera m_camera;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL11_H
