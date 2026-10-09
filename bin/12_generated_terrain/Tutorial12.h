#ifndef VULKAN_GRAPHIX_TUTORIAL12_H
#define VULKAN_GRAPHIX_TUTORIAL12_H

// Ported from vulkan_earth's OpenGL->Vulkan migration pilot
// (VulkanTerrainPilot, see this repo's git history) into a proper numbered
// tutorial: a small, pilot-sized diamond-square-style generated terrain,
// Phong-lit and textured via Tutorial09's shader/UBO/depth-resource shape
// (Tutorial09 is already a lit, textured, depth-tested terrain tutorial -
// a closer match than Tutorial07 was for the skybox). The actual height-
// field generation (random-walk accumulation + box-blur smoothing) lives
// in the shared vulkan_graphix::TerrainGenerator (see TerrainGenerator.h),
// not here, so a future real vulkan_earth Vulkan port can call the same
// code instead of keeping a separate copy.
//
// The Vulkan scaffolding - render pass, depth buffer, pipeline,
// descriptors, buffers, texture upload, and the frame loop - is the
// library's (VulkanCommon's ResourceContext/FrameLoop).

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/OrbitCamera.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"
#include "vulkan_graphix/VulkanCommon/FrameLoop.h"
#include "vulkan_graphix/VulkanCommon/ResourceContext.h"

namespace vulkan_graphix {

// Matches Tutorial09VertexData's shape on purpose - this tutorial reuses
// Tutorial09's compiled shaders byte-for-byte (see createResources()).
struct Tutorial12VertexData {
  Math::Vec4<float> m_position;
  Math::Vec3<float> m_normal;
  Math::Vec2<float> m_texcoord;
};

// Matches Tutorial09UniformBufferData's shape byte-for-byte - same reason.
struct Tutorial12UniformBufferData {
  Math::Mat4<float> m_model;
  Math::Mat4<float> m_view;
  Math::Mat4<float> m_projection;
  Math::Vec4<float> m_light_position;
  Math::Vec4<float> m_light_color;
  Math::Vec4<float> m_view_position;
};

class Tutorial12 : public TutorialBase {
public:
  Tutorial12();
  ~Tutorial12() override;

  // Everything the terrain draws with; call once after prepareVulkan().
  bool createResources();

  bool draw() override;
  void onMouseButton(
      std::int32_t button,
      bool pressed,
      std::int32_t pos_x,
      std::int32_t pos_y) override;
  void onMouseMove(std::int32_t pos_x, std::int32_t pos_y) override;

private:
  // Small pilot-sized stand-in for TerrainMaker's real size=256/scale=100
  // (390,150 vertices) - kept just large enough to show real diamond-
  // square-style relief while staying quick to build/render/verify.
  // Centered on the origin (OrbitCamera's fixed target) rather than
  // starting at (0,0) the way TerrainMaker's own world placement does,
  // since OrbitCamera can't be re-targeted away from the origin.
  static constexpr std::int32_t c_grid_size = 32;
  static constexpr std::int32_t c_grid_scale = 1;
  static constexpr std::int32_t c_gen_steps = 150;
  static constexpr std::int32_t c_gen_increase = 1;
  static constexpr float c_gen_radius = 4.0f;
  static constexpr std::int32_t c_gen_random_jump = 5;
  static constexpr std::int32_t c_smoothing_passes = 3;

  Tutorial12UniformBufferData getUniformBufferData() const;
  const std::vector<Tutorial12VertexData>& getVertexData();

  bool childOnWindowSizeChanged() override;
  void childClear() override;

  VulkanCommon::ResourceContext m_resources;
  VulkanCommon::FrameLoop m_frames;
  ImageParameters m_texture;
  ImageParameters m_depth_image;
  BufferParameters m_uniform_buffer;
  BufferParameters m_vertex_buffer;
  VkDescriptorSet m_descriptor_set = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  OrbitCamera m_camera;

  std::vector<Tutorial12VertexData> m_vertex_data;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL12_H
