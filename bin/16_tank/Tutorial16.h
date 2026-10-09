#ifndef VULKAN_GRAPHIX_TUTORIAL16_H
#define VULKAN_GRAPHIX_TUTORIAL16_H

// Ported from vulkan_earth's real multi-part tank renderer (see
// vulkan_earth/include/vulkan_earth/Tank.h, vulkan_earth/src/Tank.cpp
// and, for the concrete "Hellfire" tank this tutorial renders,
// vulkan_earth/src/TankB.cpp): a Tank::draw()
// independently positions and draws three mesh parts (body/head/turret
// - Tank tracks a fourth "wheel" part, but TankB never loads or draws
// one, so this tutorial doesn't either) through the same
// .ogl-mesh-plus-raw-texture pipeline Tutorial13 already ports (see
// Tools::loadOglMeshData()/Tools::getRawImageData()), all three parts
// sharing one texture (TestImage.raw) - just like TankB's own
// constructor loads it once per VBOShaderLibrary part.
//
// Tank::draw() applies each part's model matrix independently (no
// parent-child composition visible in Tank.cpp itself), so this
// tutorial renders a static "resting pose" built directly from TankB's
// own constructor data (body_offset/head_offset/turret_offset, a
// shared 50x scale, and the right/up/at basis Tank::initBody()/
// initHead()/initTurret() all set identically) rather than the live
// gameplay positioning (Player/GameState) that would normally combine
// those offsets with a moving tank's world position.
//
// Each part's model matrix is delivered as a vertex-stage push
// constant, set immediately before that part's own draw call -
// Tutorial10's LinePushConstants (a per-draw flat color) is this
// project's only other push-constant precedent; this is the first one
// carrying a model matrix.
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

// Same shape as Tutorial13VertexData on purpose: an unlit textured
// surface is exactly what these meshes need too.
struct Tutorial16VertexData {
  Math::Vec4<float> m_position;
  Math::Vec2<float> m_texcoord;
};

// Shared by all three parts, updated once per frame as the orbit
// camera moves - see Tutorial10UniformBufferData for the same split.
struct Tutorial16UniformBufferData {
  Math::Mat4<float> m_view;
  Math::Mat4<float> m_projection;
};

// The active part's model matrix, set per draw call.
struct Tutorial16PushConstants {
  Math::Mat4<float> m_model;
};

class Tutorial16 : public TutorialBase {
public:
  Tutorial16();
  ~Tutorial16() override;

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
  // One tank part: its mesh, uploaded once.
  struct Part {
    BufferParameters m_vertex_buffer;
    std::uint32_t m_vertex_count = 0;
  };

  Tutorial16UniformBufferData getUniformBufferData() const;
  bool createPart(const char* mesh_filename, Part& out);

  // TankB's own constructor values (body_offset/head_offset/
  // turret_offset, a shared 50x scale) combined with the right/up/at
  // basis Tank::initBody()/initHead()/initTurret() all set identically
  // - see this header's own top comment. Composed by the shared
  // HellfireTank module, for a tank placed at the world origin.
  Math::Mat4<float> getBodyModelMatrix() const;
  Math::Mat4<float> getHeadModelMatrix() const;
  Math::Mat4<float> getTurretModelMatrix() const;

  bool childOnWindowSizeChanged() override;
  void childClear() override;

  VulkanCommon::ResourceContext m_resources;
  VulkanCommon::FrameLoop m_frames;
  ImageParameters m_texture;
  BufferParameters m_uniform_buffer;
  Part m_body;
  Part m_head;
  Part m_turret;
  VkDescriptorSet m_descriptor_set = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  OrbitCamera m_camera;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL16_H
