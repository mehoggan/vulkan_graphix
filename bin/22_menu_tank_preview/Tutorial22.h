#ifndef VULKAN_GRAPHIX_TUTORIAL22_H
#define VULKAN_GRAPHIX_TUTORIAL22_H

// Ported from vulkan_earth's GameState menu layer - GameState itself only
// covers in-match GAME_PLAY and owns no menus (confirmed via a full read
// of GameState.h); the menu screens (MainMenu/ReadyMenu/ShopMenu/
// SubMenu*) are separate top-level classes in VulkanEarth.cpp. Every one
// of them opens with the identical 5-quad bevel-panel background
// UiGeometry::buildButtonBevel() already implements generically -
// Tutorial17/19 already call it this exact way for their own panel
// backgrounds, so that piece needs no new code here either; this
// tutorial reuses Tutorial15's exact 2D pipeline/shader (shader.
// 22_panel.{vert,frag} are textually identical to Tutorial15's own) for
// the panel/title/button.
//
// What's genuinely new is ReadyMenu's own real technique
// (ReadyMenu.cpp:909-982): a live rotating 3D tank preview rendered into
// a scissored sub-region of the screen, via its own glViewport/
// gluPerspective/gluLookAt and a `tank_angle += 0.25f` per-frame spin -
// composited inside the 2D menu panel. This tutorial reuses Tutorial16's
// exact tank pipeline/push-constant shape (shader.22_3d.{vert,frag} are
// textually identical to Tutorial16's own, and the body/head/turret
// hierarchical positioning is TankB's same real offsets/basis/scale
// Tutorial16/21 already use) but binds it with a VkViewport/VkRect2D
// confined to a sub-region of the screen instead of the full swapchain
// extent - the first time this project's viewport doesn't cover the
// whole frame - and adds one extra Y-axis rotation to each part's model
// matrix that increments every draw() call, mirroring ReadyMenu's own
// tank_angle. Unlike Tutorial16 (OrbitCamera, mouse-orbitable), this
// preview's own view is fixed, matching ReadyMenu's own static
// gluLookAt - the only motion is the tank's own spin, not the camera.
//
// One render pass, two pipelines: the 2D panel pipeline (depth disabled,
// covers the full swapchain extent, drawn first) and the 3D preview
// pipeline (real depth test/write, scissored to the sub-region, drawn
// second so it appears composited inside the panel) - same multi-
// pipeline-one-render-pass technique Tutorial18 introduced, now paired
// with a viewport that doesn't cover the whole frame.
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
#include "vulkan_graphix/Tutorial/TutorialBase.h"
#include "vulkan_graphix/VulkanCommon/FrameLoop.h"
#include "vulkan_graphix/VulkanCommon/ResourceContext.h"

namespace vulkan_graphix {

// Matches Tutorial16VertexData's shape byte-for-byte.
struct Tutorial22TankVertexData {
  Math::Vec4<float> m_position;
  Math::Vec2<float> m_texcoord;
};

struct Tutorial22TankUniformBufferData {
  Math::Mat4<float> m_view;
  Math::Mat4<float> m_projection;
};

// The active part's model matrix (translation/basis/scale plus the
// preview's own continuous spin), set per draw call.
struct Tutorial22TankPushConstants {
  Math::Mat4<float> m_model;
};

// Matches Tutorial15VertexData's shape byte-for-byte.
struct Tutorial22PanelVertexData {
  Math::Vec4<float> m_position;
  Math::Vec2<float> m_texcoord;
  Math::Vec4<float> m_color;
};

static constexpr std::size_t c_tutorial22_tank_part_count = 3;

class Tutorial22 : public TutorialBase {
public:
  Tutorial22();
  ~Tutorial22() override;

  // Everything the tutorial draws with; call once after prepareVulkan().
  bool createResources();

  bool draw() override;
  void onMouseButton(
      std::int32_t button,
      bool pressed,
      std::int32_t pos_x,
      std::int32_t pos_y) override;

private:
  // Generous fixed capacity, same reasoning as Tutorial15's own
  // c_max_quads - a title, a framed preview border, and one button label.
  static constexpr std::size_t c_max_panel_quads = 256;
  static constexpr std::size_t c_max_panel_vertex_count =
      c_max_panel_quads * 6;
  static constexpr float c_font_pixel_height = 28.0f;
  static constexpr const char* c_font_path =
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
  // ReadyMenu.cpp's own tank_angle += 0.25f per frame.
  static constexpr float c_tank_spin_step_radians = 0.25f;

  Tutorial22TankUniformBufferData getTankUniformBufferData() const;
  Math::Mat4<float> getPanelUniformBufferData() const;

  // The sub-region of the screen the tank preview renders into, in the
  // same top-left-origin, y-down screen-pixel convention the 2D panel
  // uses - shared by draw()'s dynamic viewport/scissor and the
  // 2D panel's own frame-border bevel so they can never disagree.
  Math::Vec2<float> getPreviewTopLeft() const;
  Math::Vec2<float> getPreviewSize() const;

  // TankB's own real offsets/basis/scale (see HellfireTank.h) with one extra
  // Y-axis rotation - this preview's own continuous spin - applied around
  // the whole assembly before its real per-part translation, mirroring
  // ReadyMenu.cpp's own tank_angle-driven glRotatef call.
  Math::Mat4<float> getTankPartModelMatrix(
      const Math::Vec3<float>& part_translation) const;

  Math::Vec2<float> getButtonTopLeft() const;
  Math::Vec2<float> getButtonSize() const;
  std::string getButtonLabel() const;

  std::vector<Tutorial22PanelVertexData> buildPanelVertexData() const;
  bool updatePanelVertexBufferData();

  bool childOnWindowSizeChanged() override;
  void childClear() override;

  VulkanCommon::ResourceContext m_resources;
  VulkanCommon::FrameLoop m_frames;
  ImageParameters m_tank_texture;
  ImageParameters m_font_texture;
  ImageParameters m_depth_image;
  BufferParameters m_tank_uniform_buffer;
  BufferParameters m_panel_uniform_buffer;
  BufferParameters m_panel_vertex_buffer;
  std::uint32_t m_panel_vertex_count = 0;
  std::array<BufferParameters, c_tutorial22_tank_part_count>
      m_tank_vertex_buffers;
  std::array<std::uint32_t, c_tutorial22_tank_part_count>
      m_tank_vertex_counts{};
  VkDescriptorSet m_tank_descriptor_set = VK_NULL_HANDLE;
  VkDescriptorSet m_panel_descriptor_set = VK_NULL_HANDLE;
  VkPipelineLayout m_tank_pipeline_layout = VK_NULL_HANDLE;
  VkPipelineLayout m_panel_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline m_tank_pipeline = VK_NULL_HANDLE;
  VkPipeline m_panel_pipeline = VK_NULL_HANDLE;
  BitmapFont m_font;
  bool m_button_pressed;
  std::int32_t m_click_count;
  float m_tank_angle;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL22_H
