#ifndef VULKAN_GRAPHIX_TUTORIAL15_H
#define VULKAN_GRAPHIX_TUTORIAL15_H

// A minimal 2D UI: a text title and one clickable button with a text
// label, demonstrating the two primitives vulkan_earth's whole menu
// system (MainMenu/SubMenu*/ReadyMenu/ShopMenu/ControlItem*) is built
// from - text (BitmapFont.h) and a beveled button face
// (UiGeometry::buildButtonBevel(), ported from
// vulkan_earth/src/ControlItemButton.cpp) - without porting that entire
// class hierarchy. Both of those pieces are pure CPU geometry/layout
// logic with no Vulkan coupling, so this tutorial's own job is just:
// upload the font atlas as a texture, and every frame, rebuild a small
// CPU vertex list from current UI state (title + button bevel + button
// label) and re-upload it into a host-visible vertex buffer - the
// correct Vulkan pattern for small, per-frame-dynamic geometry, the same
// way every other tutorial already maps/memcpy's its uniform buffer each
// frame rather than staging it.
//
// The Vulkan scaffolding - render pass, pipelines, descriptors, buffers,
// texture uploads, and the frame loop - is the library's (VulkanCommon's
// ResourceContext/FrameLoop); this file is what's specific to the
// tutorial.

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

struct Tutorial15VertexData {
  Math::Vec4<float> m_position;
  Math::Vec2<float> m_texcoord;
  Math::Vec4<float> m_color;
};

class Tutorial15 : public TutorialBase {
public:
  Tutorial15();
  ~Tutorial15() override;

  // Everything the tutorial draws with; call once after prepareVulkan().
  bool createResources();

  bool draw() override;
  void onMouseButton(
      std::int32_t button,
      bool pressed,
      std::int32_t pos_x,
      std::int32_t pos_y) override;

private:
  // Generous fixed capacity - a title plus one short button label is a
  // few dozen quads at most.
  static constexpr std::size_t c_max_quads = 256;
  static constexpr std::size_t c_max_vertex_count = c_max_quads * 6;
  static constexpr float c_font_pixel_height = 28.0f;
  // fonts-dejavu-core (see CLAUDE.md's Initial Setup) provides this.
  static constexpr const char* c_font_path =
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";

  Math::Mat4<float> getUniformBufferData() const;

  // Layout: button top-left/size in the same screen-pixel, top-left-
  // origin, y-down convention the vertex/projection setup uses -
  // shared between rendering and onMouseButton()'s hit test so they
  // can never disagree.
  Math::Vec2<float> getButtonTopLeft() const;
  Math::Vec2<float> getButtonSize() const;
  std::string getButtonLabel() const;

  std::vector<Tutorial15VertexData> buildUiVertexData() const;
  bool updateVertexBufferData();

  bool childOnWindowSizeChanged() override;
  void childClear() override;

  VulkanCommon::ResourceContext m_resources;
  VulkanCommon::FrameLoop m_frames;
  ImageParameters m_font_texture;
  BufferParameters m_uniform_buffer;
  BufferParameters m_vertex_buffer;
  std::uint32_t m_vertex_count = 0;
  VkDescriptorSet m_descriptor_set = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  BitmapFont m_font;
  bool m_button_pressed;
  std::int32_t m_click_count;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL15_H
