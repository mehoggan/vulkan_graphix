#ifndef VULKAN_GRAPHIX_TUTORIAL14_H
#define VULKAN_GRAPHIX_TUTORIAL14_H

// Ported from vulkan_earth's OpenGL->Vulkan migration pilot
// (VulkanParticlePilot, see this repo's git history) into a proper
// numbered tutorial: the shape Particle::render() actually draws for
// every tank-death/status effect (glPushMatrix(); glTranslatef(...);
// glColor4f(r,g,b,0.4); glutSolidSphere(size,10,10); glPopMatrix() - see
// vulkan_earth/src/Particle.cpp) as a flat, translucent, alpha-blended
// icosphere lit with the same Phong math Tutorial08 uses for its own
// Math::Sphere, via a small new fragment shader
// (resources/14/Data/shader.14.frag) since Tutorial08's own compiled one
// hardcodes an opaque, non-alpha-blended object color - its vertex shader
// is otherwise reused unchanged. This project's own Math::Sphere/
// Icosahedron (used exactly this way by Tutorial08 already - see
// bin/08_phong_sphere/Tutorial08.cpp's getVertexData()/getIndexData())
// replaces glutSolidSphere() - no texture, matching the original's
// flat-colored look.
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

// Matches Tutorial08VertexData's shape on purpose - this tutorial's
// Math::Sphere usage is otherwise identical to Tutorial08's own (see
// getVertexData()/getIndexData()).
struct Tutorial14VertexData {
  Math::Vec4<float> m_position;
  Math::Vec3<float> m_normal;
};

// Matches Tutorial08UniformBufferData's shape byte-for-byte - the new
// fragment shader (resources/14/Data/shader.14.frag) reads the same
// uniform block layout Tutorial08's own shader does.
struct Tutorial14UniformBufferData {
  Math::Mat4<float> m_model;
  Math::Mat4<float> m_view;
  Math::Mat4<float> m_projection;
  Math::Vec4<float> m_light_position;
  Math::Vec4<float> m_light_color;
  Math::Vec4<float> m_view_position;
};

class Tutorial14 : public TutorialBase {
public:
  Tutorial14();
  ~Tutorial14() override;

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
  Tutorial14UniformBufferData getUniformBufferData() const;
  const std::vector<Tutorial14VertexData>& getVertexData() const;
  const std::vector<std::uint32_t>& getIndexData() const;

  bool childOnWindowSizeChanged() override;
  void childClear() override;

  VulkanCommon::ResourceContext m_resources;
  VulkanCommon::FrameLoop m_frames;
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

#endif  // VULKAN_GRAPHIX_TUTORIAL14_H
