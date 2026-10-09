#ifndef VULKAN_GRAPHIX_TUTORIAL17_H
#define VULKAN_GRAPHIX_TUTORIAL17_H

// Ported from vulkan_earth's Inventory (see
// vulkan_earth/include/vulkan_earth/Inventory.h and
// vulkan_earth/src/Inventory.cpp): a titled bevel-panel grid of item
// icons with per-slot remaining-count labels and a click-to-select
// description, using the real data from all 8 concrete Item subclasses
// (vulkan_earth/src/ItemXxx.cpp) - name/description/price/remaining come
// straight from those constructors, not fabricated. Item itself (see
// Item.h) has no draw() method at all -
// it's a pure data record - so there is nothing to port for Item beyond
// supplying that real data here; Inventory's ControlItemGrid panel (a
// bevel-bordered box of cells - the same five-quad raised/pressed bevel
// UiGeometry::buildButtonBevel() already ports, confirmed against
// ControlItemGrid::draw()'s own 0.75-based colors) plus its title/
// explain/descript text (BitmapFont, exactly like Tutorial15) is the
// real rendering target.
//
// Two textures are needed - a BitmapFont glyph atlas (all the panel's
// text) and a combined icon atlas (all 8 item icons, stitched from the
// real ItemXxx.raw 256x256 assets by the library's ImageAtlas, which
// also gives each icon's texture coordinates) - kept behind ONE
// pipeline/descriptor-set-layout shape (matching every other tutorial's
// one-texture-per-descriptor-set precedent) by using TWO descriptor sets from
// that one layout and drawing in two passes: bind the font set, draw every
// flat-color/text quad (panel bevel, title/explain/descript, per-cell labels,
// the selected-cell highlight); bind the icon set, draw the 8 icon quads on
// top of that (see draw()).

#include <array>
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
#include "vulkan_graphix/UiGeometry.h"
#include "vulkan_graphix/VulkanCommon/FrameLoop.h"
#include "vulkan_graphix/VulkanCommon/ResourceContext.h"

namespace vulkan_graphix {

// Same shape as Tutorial15VertexData on purpose: reuses that tutorial's
// exact 2D alpha-blended pipeline shape (new resources/17/Data/shader.17.
// {vert,frag}, textually identical to Tutorial15's).
struct Tutorial17VertexData {
  Math::Vec4<float> m_position;
  Math::Vec2<float> m_texcoord;
  Math::Vec4<float> m_color;
};

static constexpr std::size_t c_inventory_item_count = 8;

class Tutorial17 : public TutorialBase {
public:
  Tutorial17();
  ~Tutorial17() override;

  // Everything the tutorial draws with; call once after prepareVulkan().
  bool createResources();

  bool draw() override;
  void onMouseButton(
      std::int32_t button,
      bool pressed,
      std::int32_t pos_x,
      std::int32_t pos_y) override;

private:
  // A title/explain line, a (possibly multi-line, wrapped) description,
  // an 8-cell grid's labels, a panel bevel, and a selection highlight
  // is comfortably under this - generous headroom kept anyway, same
  // reasoning as Tutorial15's own c_max_quads.
  static constexpr std::size_t c_max_quads = 512;
  static constexpr std::size_t c_max_vertex_count = c_max_quads * 6;
  static constexpr float c_font_pixel_height = 20.0f;
  static constexpr const char* c_font_path =
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";

  static constexpr std::uint32_t c_icon_size = 256;
  static constexpr std::uint32_t c_icon_atlas_cols = 4;
  static constexpr std::uint32_t c_icon_atlas_rows = 2;
  static constexpr std::uint32_t c_icon_atlas_width =
      c_icon_size * c_icon_atlas_cols;
  static constexpr std::uint32_t c_icon_atlas_height =
      c_icon_size * c_icon_atlas_rows;

  bool createIconAtlas();
  Math::Mat4<float> getUniformBufferData() const;

  // Layout - shared between rendering and onMouseButton()'s hit test,
  // same top-left-origin/y-down screen-pixel convention Tutorial15
  // uses, so the two can never disagree.
  Math::Vec2<float> getPanelTopLeft() const;
  Math::Vec2<float> getPanelSize() const;
  Math::Vec2<float> getGridTopLeft() const;
  Math::Vec2<float> getGridSize() const;
  UiGeometry::GridLayout getGridLayout() const;
  float getDescriptionTop() const;

  std::vector<std::string> wrapText(
      const std::string& text, float max_width) const;

  std::vector<Tutorial17VertexData> buildTextPassVertexData() const;
  std::vector<Tutorial17VertexData> buildIconPassVertexData() const;
  bool updateVertexBufferData();

  bool childOnWindowSizeChanged() override;
  void childClear() override;

  VulkanCommon::ResourceContext m_resources;
  VulkanCommon::FrameLoop m_frames;
  ImageParameters m_font_texture;
  ImageParameters m_icon_texture;
  BufferParameters m_uniform_buffer;
  BufferParameters m_vertex_buffer;
  std::uint32_t m_text_vertex_count = 0;
  std::uint32_t m_icon_vertex_count = 0;
  // One layout, two sets: the font atlas's and the icon atlas's.
  VkDescriptorSet m_font_descriptor_set = VK_NULL_HANDLE;
  VkDescriptorSet m_icon_descriptor_set = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  BitmapFont m_font;
  std::size_t m_selected_index;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL17_H
