#ifndef VULKAN_GRAPHIX_TUTORIAL13_H
#define VULKAN_GRAPHIX_TUTORIAL13_H

// Ported from vulkan_earth's OpenGL->Vulkan migration pilot
// (VulkanProjectilePilot, see this repo's git history) into a proper
// numbered tutorial: the real tank-shell mesh (Projectiles/
// projectileDefault.ogl), parsed via the shared Tools::loadOglMeshData()
// (see Tools.h) - not a private parser here - and rendered unlit and
// textured with Tutorial07's shader/UBO shape, since the real game's own
// FragmentTank.vs samples a normal_texture but never actually uses it in
// the final color (so the parsed normal is dropped here too, same as the
// original pilot).
//
// The Vulkan scaffolding - render pass, pipelines, descriptors, buffers,
// texture uploads, and the frame loop - is the library's (VulkanCommon's
// ResourceContext/FrameLoop); this file is what's specific to the
// tutorial.

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
struct Tutorial13VertexData {
  Math::Vec4<float> m_position;
  Math::Vec2<float> m_texcoord;
};

class Tutorial13 : public TutorialBase {
public:
  Tutorial13();
  ~Tutorial13() override;

  // Everything the tutorial draws with; call once after prepareVulkan().
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

  bool childOnWindowSizeChanged() override;
  void childClear() override;

  VulkanCommon::ResourceContext m_resources;
  VulkanCommon::FrameLoop m_frames;
  ImageParameters m_texture;
  BufferParameters m_uniform_buffer;
  BufferParameters m_vertex_buffer;
  VkDescriptorSet m_descriptor_set = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  OrbitCamera m_camera;

  std::vector<Tutorial13VertexData> m_vertex_data;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL13_H
