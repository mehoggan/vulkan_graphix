#ifndef VULKAN_GRAPHIX_TUTORIAL18_H
#define VULKAN_GRAPHIX_TUTORIAL18_H

// Ported from vulkan_earth's Player: `Player`/`PlayerHuman`/`PlayerCPU`
// (see Player.h/PlayerHuman.h/PlayerCPU.h under
// vulkan_earth/include/vulkan_earth/ and their .cpp files under
// vulkan_earth/src/) have no draw()/rendering code of their own at all -
// confirmed by grep across all three, the only hits are a dead, empty
// `drawHUD(){}` free-function stub and a debug-only line/plane
// visualizer that never touches a Tank. The two real, renderable things
// Player is adjacent to are:
//
//   - Tank::setTankPos(x,y,z) (vulkan_earth/src/Tank.cpp:73-105) - the
//     real per-frame world-positioning algorithm, which this tutorial
//     ports faithfully (see the fix applied to Tutorial16 alongside
//     this tutorial: setTankPos() composes hierarchically - a child
//     part's offset is rotated through its *parent's* own basis columns
//     before being added to the parent's translation, not added
//     directly to a common origin).
//   - GameState::drawHUD() (vulkan_earth/src/GameState.cpp:516-772) -
//     the real per-player HUD: name text tinted by that player's real
//     `color[4]`, an HP number plus a health bar using the real
//     `glColor3f(1 - ratio, ratio, 0)` color ramp, and a power number
//     plus a power bar using the real (inverted) `glColor3f(ratio,
//     1 - ratio, 0)` ramp. Not ported: the wait-counter, weapon-slot
//     icon, "previous power/angle" ghost text, and status-effect
//     (shield/acid) name-color overrides - real but deep turn-state,
//     the same kind of cut Tutorial16 already made for live gameplay
//     reorientation. HP/power values here are static illustrative
//     snapshots (one near-full-health tank, one damaged, to show the
//     ramp) rather than a live-simulated turn - "resting pose, not
//     live simulation", same as Tank.
//
// Two Hellfire tanks (reusing Tutorial16's already-staged
// Hellfire_Body/Head/Turret.ogl + TestImage.raw - see bin/Makefile.am)
// are placed at two world positions via that real setTankPos()
// composition and tinted by an illustrative per-player team color
// (Player::color[4] is a real field with no fixed default in the
// source - the specific red/blue chosen here is this tutorial's own
// call). This needs a real depth buffer (unlike Tutorial16's single
// clustered object) since two independent 3D objects under a freely
// orbiting camera need real depth testing to composite correctly.
//
// The HUD is a second pipeline in the *same* render pass, with depth
// test/write disabled, drawn after the 3D pass - this mirrors
// GameState::drawHUD()'s own glDisable(GL_DEPTH_TEST)/draw HUD/
// glEnable(GL_DEPTH_TEST) bracketing, adapted to Vulkan's per-pipeline
// depth state instead of GL's global mode switch. It reuses
// BitmapFont/UiGeometry exactly as Tutorial15/17 do.
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
#include "vulkan_graphix/VulkanCommon/FrameLoop.h"
#include "vulkan_graphix/VulkanCommon/ResourceContext.h"

namespace vulkan_graphix {

// Same shape as Tutorial16VertexData - the two tanks reuse Tutorial16's
// exact unlit-textured mesh pipeline shape.
struct Tutorial18Vertex3DData {
  Math::Vec4<float> m_position;
  Math::Vec2<float> m_texcoord;
};

// Same shape as Tutorial15/17VertexData - the HUD reuses that exact 2D
// alpha-blended pipeline shape.
struct Tutorial18VertexHudData {
  Math::Vec4<float> m_position;
  Math::Vec2<float> m_texcoord;
  Math::Vec4<float> m_color;
};

struct Tutorial18UniformBufferData3D {
  Math::Mat4<float> m_view;
  Math::Mat4<float> m_projection;
};

// Per-tank-instance model matrix and team-color tint, set immediately
// before each part's draw call - extends Tutorial16's own Mat4-only
// push constant (Tutorial16PushConstants) with a Vec4 tint.
struct Tutorial18PushConstants {
  Math::Mat4<float> m_model;
  Math::Vec4<float> m_color;
};

// One player's real+illustrative HUD data (see this header's own top
// comment for which fields are real vs. this tutorial's own static
// snapshot choice).
struct Tutorial18PlayerInfo {
  Math::Vec3<float> m_world_position;
  Math::Vec4<float> m_team_color;
  std::string m_name;
  std::int32_t m_hp;
  std::int32_t m_max_hp;
  float m_power_ratio;
};

class Tutorial18 : public TutorialBase {
public:
  Tutorial18();
  ~Tutorial18() override;

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
  static constexpr std::size_t c_max_hud_quads = 256;
  static constexpr std::size_t c_max_hud_vertex_count = c_max_hud_quads * 6;
  static constexpr float c_font_pixel_height = 18.0f;
  static constexpr const char* c_font_path =
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";

  // One tank part's mesh, shared by both tank instances.
  struct Part {
    BufferParameters m_vertex_buffer;
    std::uint32_t m_vertex_count = 0;
  };

  Tutorial18UniformBufferData3D get3DUniformBufferData() const;
  Math::Mat4<float> getHudUniformBufferData() const;
  bool createPart(const char* mesh_filename, Part& out);

  // The two players' real+illustrative HUD/position data - see
  // Tutorial18.h's top comment.
  const std::array<Tutorial18PlayerInfo, 2>& getPlayers() const;

  // TankB's real body/head/turret offsets, composed hierarchically for
  // a tank placed at world_position exactly as Tank::setTankPos() does
  // (see HellfireTank::getPartTranslations()).
  Math::Mat4<float> getBodyModelMatrix(
      const Math::Vec3<float>& world_position) const;
  Math::Mat4<float> getHeadModelMatrix(
      const Math::Vec3<float>& world_position) const;
  Math::Mat4<float> getTurretModelMatrix(
      const Math::Vec3<float>& world_position) const;

  Math::Vec2<float> getPanelTopLeft(std::size_t player_index) const;
  Math::Vec2<float> getPanelSize() const;

  // Outline + ratio-filled bar, colored via get_bar_color(ratio) - the
  // real GameState::drawHUD() color-ramp formula, passed in so health
  // and power bars (which invert the ramp) can share this one helper.
  void appendBar(
      std::vector<Tutorial18VertexHudData>& vertex_data,
      Math::Vec2<float> top_left,
      Math::Vec2<float> size,
      float ratio,
      Math::Vec4<float> (*get_bar_color)(float ratio)) const;

  std::vector<Tutorial18VertexHudData> buildHudVertexData() const;

  bool childOnWindowSizeChanged() override;
  void childClear() override;

  VulkanCommon::ResourceContext m_resources;
  VulkanCommon::FrameLoop m_frames;
  ImageParameters m_tank_texture;
  ImageParameters m_font_texture;
  ImageParameters m_depth_image;
  BufferParameters m_uniform_buffer_3d;
  BufferParameters m_uniform_buffer_hud;
  BufferParameters m_hud_vertex_buffer;
  std::uint32_t m_hud_vertex_count = 0;
  Part m_body;
  Part m_head;
  Part m_turret;
  VkDescriptorSet m_descriptor_set_3d = VK_NULL_HANDLE;
  VkDescriptorSet m_descriptor_set_hud = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout_3d = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout_hud = VK_NULL_HANDLE;
  VkPipeline m_pipeline_3d = VK_NULL_HANDLE;
  VkPipeline m_pipeline_hud = VK_NULL_HANDLE;
  OrbitCamera m_camera;
  BitmapFont m_font;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL18_H
