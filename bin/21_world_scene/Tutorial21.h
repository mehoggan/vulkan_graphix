#ifndef VULKAN_GRAPHIX_TUTORIAL21_H
#define VULKAN_GRAPHIX_TUTORIAL21_H

// Ported from vulkan_earth's World/Camera integration: `GameState::
// draw()`'s real per-frame order is skybox -> terrain -> tank (see
// GameState.cpp:403-514) - three previously-separate pilots in this
// project (Tutorial11's skybox, Tutorial12's terrain, Tutorial16's tank)
// combined into one real scene, the way the original actually composites
// them.
//
// Correcting an assumption from initial scoping: Tutorial12's terrain is
// already Phong-lit and textured (its own shader.12.frag has the
// identical ambient+diffuse+specular block Tutorial08/09/14 use) -
// lighting isn't missing, and is reused here byte-for-byte (shader.
// 21_terrain.{vert,frag} are textually identical to Tutorial12's own).
// What's genuinely new is combining it with a skybox in the same scene
// (Tutorial11's is a fully standalone tutorial, never composited with
// anything) and placing a real object on the terrain using a real height
// query - `TerrainGenerator::heightAt()` (already exists, and is exactly
// what `Tutorial12::getVertexData()` itself calls to build the terrain
// mesh) - so the tank sits on the actual generated ground, not a
// fabricated position.
//
// Tutorial11's skybox and Tutorial16's tank share the exact same unlit-
// textured vertex/push-constant shape (`Tutorial11VertexData`/
// `Tutorial16PushConstants` are both position+texcoord / {mat4 model} -
// confirmed identical) - so this tutorial reuses ONE pipeline
// (shader.21_object.{vert,frag}, textually identical to Tutorial16's
// own) for both, with two descriptor sets from one layout (Tutorial19's
// "N sets from one layout" technique) rather than two pipelines.
//
// Tutorial12's terrain is deliberately pilot-sized (32x32 grid, ~32
// world-unit extent) to stay small and centered on OrbitCamera's fixed
// origin target - far smaller than a real, correctly-scaled Hellfire
// tank (hundreds of units across). This tutorial uses its own larger
// grid/scale constants (same kind of tutorial-local choice Tutorial12's
// own header comment already makes) so the tank reads as sitting *on* a
// landscape instead of dwarfing it.
//
// The game's own cameras (WorldCamera's overview, ChaseCamera following
// a shell) live in the library beside OrbitCamera, all on the shared
// Camera base; this tutorial has no shell or player panning to drive
// them, so it orbits with OrbitCamera like every other 3D tutorial.
//
// The Vulkan scaffolding - render pass, pipelines, descriptors, buffers,
// texture uploads, and the frame loop - is the library's (VulkanCommon's
// ResourceContext/FrameLoop); this file is what's specific to the
// tutorial.

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/OrbitCamera.h"
#include "vulkan_graphix/TerrainGenerator.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"
#include "vulkan_graphix/VulkanCommon/FrameLoop.h"
#include "vulkan_graphix/VulkanCommon/ResourceContext.h"

namespace vulkan_graphix {

// Matches Tutorial12VertexData's shape byte-for-byte - the terrain
// reuses Tutorial12's compiled shaders and vertex layout unchanged.
struct Tutorial21TerrainVertexData {
  Math::Vec4<float> m_position;
  Math::Vec3<float> m_normal;
  Math::Vec2<float> m_texcoord;
};

// Matches Tutorial12UniformBufferData's shape byte-for-byte.
struct Tutorial21TerrainUniformBufferData {
  Math::Mat4<float> m_model;
  Math::Mat4<float> m_view;
  Math::Mat4<float> m_projection;
  Math::Vec4<float> m_light_position;
  Math::Vec4<float> m_light_color;
  Math::Vec4<float> m_view_position;
};

// Matches Tutorial11/16VertexData's shape byte-for-byte - shared by the
// skybox and the tank.
struct Tutorial21ObjectVertexData {
  Math::Vec4<float> m_position;
  Math::Vec2<float> m_texcoord;
};

struct Tutorial21ObjectUniformBufferData {
  Math::Mat4<float> m_view;
  Math::Mat4<float> m_projection;
};

// Matches Tutorial16's push constant shape - just a model matrix.
struct Tutorial21PushConstants {
  Math::Mat4<float> m_model;
};

static constexpr std::size_t c_tank_part_count = 3;

class Tutorial21 : public TutorialBase {
public:
  Tutorial21();
  ~Tutorial21() override;

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
  // Bigger than Tutorial12's own pilot-sized 32x32/scale-1 grid (see
  // this header's own top comment) - sized so a full-scale Hellfire
  // tank (hundreds of units across) reads as sitting on a landscape
  // rather than dwarfing it. Generation step constants reused as-is
  // from Tutorial12.
  static constexpr std::int32_t c_grid_size = 64;
  static constexpr std::int32_t c_grid_scale = 16;
  // Tutorial12 (32x32 grid) uses steps=150/increase=1/radius=4 - a total
  // "paint volume" (steps * increase * pi * radius^2) of ~7500 spread
  // over 1024 cells. This grid has 4x the cells (4096), so steps=300/
  // increase=1/radius=6 (~34000) keeps a comparable paint density
  // instead of quadrupling it, which produced unrealistically steep
  // spikes during screenshot verification.
  static constexpr std::int32_t c_gen_steps = 200;
  static constexpr std::int32_t c_gen_increase = 1;
  static constexpr float c_gen_radius = 5.0f;
  static constexpr std::int32_t c_gen_random_jump = 8;
  static constexpr std::int32_t c_smoothing_passes = 6;

  // Large enough to comfortably enclose the whole terrain+tank scene
  // (see getSkyboxVertexData()) - the skybox is drawn first with
  // depth write disabled, so its exact size doesn't need to match the
  // camera distance precisely, just stay bigger than it.
  static constexpr float c_skybox_half_extent = 3000.0f;

  Tutorial21TerrainUniformBufferData getTerrainUniformBufferData() const;
  Tutorial21ObjectUniformBufferData getObjectUniformBufferData() const;

  // Runs TerrainGenerator::generate() exactly once (guarded by
  // m_terrain_generated) - shared by getTerrainVertexData() and
  // getTankGroundHeight() so both read the same generated terrain
  // instead of two independently-generated ones.
  void ensureTerrainGenerated();
  // Same six-vertices-per-cell layout as Tutorial12::getVertexData(),
  // parameterized by this tutorial's own (larger) grid constants.
  const std::vector<Tutorial21TerrainVertexData>& getTerrainVertexData();
  // Real height query - TerrainGenerator::heightAt() on the grid cell
  // nearest the terrain's center, the same call getTerrainVertexData()
  // itself makes to build the mesh - not a fabricated Y position.
  float getTankGroundHeight();
  Math::Mat4<float> getTankPartModelMatrix(
      const Math::Vec3<float>& part_translation) const;

  const std::vector<Tutorial21ObjectVertexData>& getSkyboxVertexData() const;
  const std::vector<std::uint32_t>& getSkyboxIndexData() const;

  bool childOnWindowSizeChanged() override;
  void childClear() override;

  VulkanCommon::ResourceContext m_resources;
  VulkanCommon::FrameLoop m_frames;
  ImageParameters m_terrain_texture;
  ImageParameters m_tank_texture;
  ImageParameters m_skybox_texture;
  ImageParameters m_depth_image;
  BufferParameters m_terrain_uniform_buffer;
  BufferParameters m_object_uniform_buffer;
  BufferParameters m_terrain_vertex_buffer;
  BufferParameters m_skybox_vertex_buffer;
  BufferParameters m_skybox_index_buffer;
  std::uint32_t m_skybox_index_count = 0;
  std::array<BufferParameters, c_tank_part_count> m_tank_vertex_buffers;
  std::array<std::uint32_t, c_tank_part_count> m_tank_vertex_counts{};
  VkDescriptorSet m_terrain_descriptor_set = VK_NULL_HANDLE;
  VkDescriptorSet m_tank_descriptor_set = VK_NULL_HANDLE;
  VkDescriptorSet m_skybox_descriptor_set = VK_NULL_HANDLE;
  VkPipelineLayout m_terrain_pipeline_layout = VK_NULL_HANDLE;
  VkPipelineLayout m_object_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline m_terrain_pipeline = VK_NULL_HANDLE;
  VkPipeline m_object_pipeline = VK_NULL_HANDLE;
  VkPipeline m_skybox_pipeline = VK_NULL_HANDLE;
  OrbitCamera m_camera;

  std::vector<Tutorial21TerrainVertexData> m_terrain_vertex_data;
  TerrainGenerator m_terrain_generator;
  bool m_terrain_generated;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL21_H
