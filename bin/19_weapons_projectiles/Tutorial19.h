#ifndef VULKAN_GRAPHIX_TUTORIAL19_H
#define VULKAN_GRAPHIX_TUTORIAL19_H

// Ported from vulkan_earth's Weapon: `Weapon` + its 10 concrete
// subclasses (see Weapon.h/WeaponXxx.h under
// vulkan_earth/include/vulkan_earth/ and their .cpp files under
// vulkan_earth/src/) are pure data - id, a real `description` string,
// `price`, `damage`, `radius`, `scale`, explosion colors, plus
// gameplay-effect virtuals - with no `draw()`, no `VBOShaderLibrary`, no
// `.ogl` reference anywhere
// (confirmed via grep across every subclass; `WeaponTest.h`'s
// `draw()` is a dead, unused abstract leftover with no concrete
// subclass). Structurally identical to `Item` - see Tutorial17.h's own
// top comment. The two real, renderable things adjacent to Weapon are:
//
//   - Projectile::draw() (vulkan_earth/src/Projectile.cpp:76-89) - looks
//     up a real 11-slot VBOShaderLibrary* mesh table by the equipped
//     weapon's uniqueidentifier and draws it scaled by that weapon's
//     real `scale` field. Most weapon ids reuse projectileDefault.ogl's
//     geometry with a swapped texture; Acid, BFB (and MFB, texture-
//     swapped from BFB), Thor, EMP, and Nuke have their own distinct
//     `.ogl` geometry. This tutorial shows three genuinely distinct
//     meshes - projectileDefault, projectileAcid, projectileBFB - each
//     scaled by its own real weapon's `scale` (60/60/100), reusing
//     Tutorial16's exact push-constant-model-matrix + shared-view/
//     projection-UBO pipeline shape. Each projectile has its own
//     dedicated texture (unlike Tutorial16's three tank parts, which
//     share one), so three descriptor sets are allocated from one
//     layout and bound in turn before each projectile's draw call -
//     the same "N sets from one layout, used in sequence" technique
//     Tutorial17 already established for its font/icon atlases.
//   - A weapon inventory grid - the same ControlItemGrid/Inventory
//     rendering Tutorial17 already ported for Item, fed the 10 real
//     shop-purchasable WeaponXxx subclasses' data instead (ids 0-9;
//     WeaponDefault/id 10 is an internal Projectile fallback, never
//     shop-purchasable - confirmed via ShopMenu.cpp only constructing
//     ids 0-9). Ten items fit a clean 5-column x 2-row grid, one more
//     column than Tutorial17's 4x2 Item grid.
//
// Both passes share one render pass (no depth buffer needed - unlike
// Tutorial18's two tanks, these three projectiles are placed side by
// side with no overlap risk under the tutorial's fixed-ish camera
// angle, the same simplification Tutorial16 itself originally used).
//
// The Vulkan scaffolding - render pass, pipelines, descriptors, buffers,
// texture uploads, and the frame loop - is the library's (VulkanCommon's
// ResourceContext/FrameLoop); this file is what's specific to the
// tutorial.

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/BitmapFont.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/OrbitCamera.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"
#include "vulkan_graphix/UiGeometry.h"
#include "vulkan_graphix/VulkanCommon/FrameLoop.h"
#include "vulkan_graphix/VulkanCommon/ResourceContext.h"

namespace vulkan_graphix {

// Same shape as Tutorial16VertexData.
struct Tutorial19Vertex3DData {
  Math::Vec4<float> m_position;
  Math::Vec2<float> m_texcoord;
};

// Same shape as Tutorial15/17/18-HUD VertexData.
struct Tutorial19VertexGridData {
  Math::Vec4<float> m_position;
  Math::Vec2<float> m_texcoord;
  Math::Vec4<float> m_color;
};

struct Tutorial19UniformBufferData3D {
  Math::Mat4<float> m_view;
  Math::Mat4<float> m_projection;
};

// Same shape as Tutorial16's push constant - just a model matrix, no
// color tint needed here.
struct Tutorial19PushConstants {
  Math::Mat4<float> m_model;
};

static constexpr std::size_t c_weapon_grid_item_count = 10;
static constexpr std::size_t c_projectile_mesh_count = 3;

class Tutorial19 : public TutorialBase {
public:
  Tutorial19();
  ~Tutorial19() override;

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
  static constexpr std::size_t c_max_grid_quads = 512;
  static constexpr std::size_t c_max_grid_vertex_count = c_max_grid_quads * 6;
  static constexpr float c_font_pixel_height = 18.0f;
  static constexpr const char* c_font_path =
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";

  static constexpr std::uint32_t c_icon_size = 256;
  static constexpr std::uint32_t c_icon_atlas_cols = 5;
  static constexpr std::uint32_t c_icon_atlas_rows = 2;
  static constexpr std::uint32_t c_icon_atlas_width =
      c_icon_size * c_icon_atlas_cols;
  static constexpr std::uint32_t c_icon_atlas_height =
      c_icon_size * c_icon_atlas_rows;

  bool createIconAtlas();
  Tutorial19UniformBufferData3D get3DUniformBufferData() const;
  Math::Mat4<float> getGridUniformBufferData() const;

  Math::Vec2<float> getPanelTopLeft() const;
  Math::Vec2<float> getPanelSize() const;
  Math::Vec2<float> getGridTopLeft() const;
  Math::Vec2<float> getGridSize() const;
  UiGeometry::GridLayout getGridLayout() const;
  float getDescriptionTop() const;

  std::vector<std::string> wrapText(
      const std::string& text, float max_width) const;

  std::vector<Tutorial19VertexGridData> buildTextPassVertexData() const;
  std::vector<Tutorial19VertexGridData> buildIconPassVertexData() const;
  bool updateGridVertexBufferData();

  bool childOnWindowSizeChanged() override;
  void childClear() override;

  VulkanCommon::ResourceContext m_resources;
  VulkanCommon::FrameLoop m_frames;
  ImageParameters m_font_texture;
  ImageParameters m_icon_texture;
  std::array<ImageParameters, c_projectile_mesh_count> m_projectile_textures;
  std::array<BufferParameters, c_projectile_mesh_count> m_projectile_buffers;
  std::array<std::uint32_t, c_projectile_mesh_count> m_projectile_counts{};
  BufferParameters m_uniform_buffer_3d;
  BufferParameters m_uniform_buffer_grid;
  BufferParameters m_grid_vertex_buffer;
  std::uint32_t m_text_vertex_count = 0;
  std::uint32_t m_icon_vertex_count = 0;
  // One 3D set per projectile (its own texture); the grid's font and icon
  // sets share the grid layout.
  std::array<VkDescriptorSet, c_projectile_mesh_count> m_descriptor_sets_3d{};
  VkDescriptorSet m_font_descriptor_set = VK_NULL_HANDLE;
  VkDescriptorSet m_icon_descriptor_set = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout_3d = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout_grid = VK_NULL_HANDLE;
  VkPipeline m_pipeline_3d = VK_NULL_HANDLE;
  VkPipeline m_pipeline_grid = VK_NULL_HANDLE;
  OrbitCamera m_camera;
  BitmapFont m_font;
  std::size_t m_selected_index;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL19_H
