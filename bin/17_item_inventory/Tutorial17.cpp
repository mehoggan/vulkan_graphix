#include "Tutorial17.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

#include "vulkan_graphix/GameCatalog.h"
#include "vulkan_graphix/ImageAtlas.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/UiGeometry.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
// All 8 real items - the shared GameCatalog, the same data the game's own
// Item subclasses load - in this tutorial's alphabetical grid order.
const std::array<GameCatalog::ItemSpec, c_inventory_item_count>&
getItemDisplayData() {
  static_assert(c_inventory_item_count == GameCatalog::c_item_count);
  using GameCatalog::item;
  using GameCatalog::ItemKind;
  static const std::array<GameCatalog::ItemSpec, c_inventory_item_count> data =
      {item(ItemKind::AntiAcid),
       item(ItemKind::BigRepair),
       item(ItemKind::Cloak),
       item(ItemKind::DoubleAction),
       item(ItemKind::ExtraBattery),
       item(ItemKind::Float),
       item(ItemKind::Shield),
       item(ItemKind::SmallRepair)};
  return data;
}
}  // namespace

namespace vc = VulkanCommon;

Tutorial17::Tutorial17() :
    m_selected_index(0) {}

Tutorial17::~Tutorial17() { childClear(); }

bool Tutorial17::createResources() {
  if (!m_resources.initialize(*this)) {
    return false;
  }

  if (!m_font.load(c_font_path, c_font_pixel_height)) {
    Logging::error(
        LOG_TAG,
        "Could not load font \"",
        c_font_path,
        "\" - is the fonts-dejavu-core package installed?");
    return false;
  }
  if (!m_resources.createTexture(
          m_font.atlasWidth(),
          m_font.atlasHeight(),
          m_font.atlasPixels(),
          VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
          m_font_texture) ||
      !createIconAtlas()) {
    Logging::error(LOG_TAG, "Could not create the atlas textures!");
    return false;
  }
  if (!m_resources.createUniformBuffer(
          sizeof(Math::Mat4<float>), m_uniform_buffer) ||
      !m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    Logging::error(LOG_TAG, "Could not create uniform buffer!");
    return false;
  }

  // Same binding shape as Tutorial15's shaders (0 = UBO in the vertex
  // stage, 1 = combined sampler in the fragment stage) - this tutorial's
  // shader.17.vert/frag are textually identical. One layout, two sets
  // allocated from it (see this tutorial's header comment).
  const std::vector<vc::DescriptorBinding> bindings = {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
      {1,
       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       VK_SHADER_STAGE_FRAGMENT_BIT}};
  VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
  VkDescriptorPool pool = VK_NULL_HANDLE;
  if (!m_resources.createDescriptorSetLayout(bindings, &set_layout) ||
      !m_resources.createDescriptorPool(bindings, 2, &pool) ||
      !vc::allocateDescriptorSet(
          getVkDevice(), pool, set_layout, &m_font_descriptor_set) ||
      !vc::allocateDescriptorSet(
          getVkDevice(), pool, set_layout, &m_icon_descriptor_set)) {
    Logging::error(LOG_TAG, "Could not create the descriptor sets!");
    return false;
  }
  for (const auto& [descriptor_set, texture] :
       {std::pair{m_font_descriptor_set, &m_font_texture},
        std::pair{m_icon_descriptor_set, &m_icon_texture}}) {
    vc::writeUniformBufferDescriptor(
        getVkDevice(), descriptor_set, 0, m_uniform_buffer);
    vc::writeImageDescriptor(getVkDevice(), descriptor_set, 1, *texture);
  }

  VkRenderPass render_pass = VK_NULL_HANDLE;
  if (!m_resources.createRenderPass(
          {.m_color_format = getSwapchainParameters().getVkFormat()},
          &render_pass) ||
      !m_resources.createPipelineLayout(
          {set_layout}, {}, &m_pipeline_layout)) {
    Logging::error(LOG_TAG, "Could not create the render pass!");
    return false;
  }

  // 2D UI - winding doesn't matter, so no culling. Alpha blending on -
  // glyph edges are anti-aliased via the font atlas's coverage alpha
  // (same as Tutorial15), and it costs nothing for the fully-opaque icon
  // quads drawn in the second pass.
  if (!m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.17.vert.spv",
           .m_fragment_shader = "shader.17.frag.spv",
           .m_vertex_stride = sizeof(Tutorial17VertexData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial17VertexData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32_SFLOAT,
                 offsetof(Tutorial17VertexData, m_texcoord)},
                {2,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial17VertexData, m_color)}},
           .m_blend = vc::alphaBlend(),
           .m_layout = m_pipeline_layout,
           .m_render_pass = render_pass},
          &m_pipeline)) {
    Logging::error(LOG_TAG, "Could not create graphics pipeline!");
    return false;
  }

  // Host-visible, rewritten every frame from the selection (see
  // updateVertexBufferData()), text pass first and icon pass after it.
  if (!m_resources.createHostVisibleBuffer(
          static_cast<std::uint32_t>(
              c_max_vertex_count * sizeof(Tutorial17VertexData)),
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          m_vertex_buffer) ||
      !updateVertexBufferData()) {
    Logging::error(LOG_TAG, "Could not create the vertex buffer!");
    return false;
  }

  if (!m_frames.create(*this, render_pass)) {
    Logging::error(LOG_TAG, "Could not create the frame resources!");
    return false;
  }
  return true;
}

bool Tutorial17::createIconAtlas() {
  ImageAtlas atlas(c_icon_size, c_icon_atlas_cols, c_icon_atlas_rows);
  const std::array<GameCatalog::ItemSpec, c_inventory_item_count>& items =
      getItemDisplayData();
  for (std::size_t index = 0; index < items.size(); ++index) {
    if (!atlas.setTileFromRawFile(index, items[index].m_image_file)) {
      Logging::error(
          LOG_TAG, "Could not load icon \"", items[index].m_image_file, "\"!");
      return false;
    }
  }
  return m_resources.createTexture(
      atlas.width(),
      atlas.height(),
      atlas.pixels(),
      VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      m_icon_texture);
}

Math::Mat4<float> Tutorial17::getUniformBufferData() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  // Top-left-origin, y-down screen convention, same as Tutorial15.
  return Tools::getOrthographicProjectionMatrix(
      0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

Math::Vec2<float> Tutorial17::getPanelTopLeft() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  return Math::Vec2<float>(width * 0.05f, height * 0.05f);
}

Math::Vec2<float> Tutorial17::getPanelSize() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  return Math::Vec2<float>(width * 0.9f, height * 0.9f);
}

Math::Vec2<float> Tutorial17::getGridTopLeft() const {
  const Math::Vec2<float> panel_top_left = getPanelTopLeft();
  return Math::Vec2<float>(panel_top_left.x + 20.0f, panel_top_left.y + 90.0f);
}

Math::Vec2<float> Tutorial17::getGridSize() const {
  const Math::Vec2<float> panel_size = getPanelSize();
  return Math::Vec2<float>(panel_size.x - 40.0f, 190.0f);
}

float Tutorial17::getDescriptionTop() const {
  const Math::Vec2<float> grid_top_left = getGridTopLeft();
  const Math::Vec2<float> grid_size = getGridSize();
  return grid_top_left.y + grid_size.y + 30.0f;
}

UiGeometry::GridLayout Tutorial17::getGridLayout() const {
  constexpr float c_cell_gap = 10.0f;
  return {
      getGridTopLeft(),
      getGridSize(),
      c_icon_atlas_cols,
      c_icon_atlas_rows,
      c_cell_gap};
}

std::vector<std::string> Tutorial17::wrapText(
    const std::string& text, float max_width) const {
  return m_font.wrapText(text, max_width);
}

std::vector<Tutorial17VertexData> Tutorial17::buildTextPassVertexData() const {
  std::vector<Tutorial17VertexData> vertex_data;
  vertex_data.reserve(c_max_vertex_count);

  const Math::Vec2<float> panel_top_left = getPanelTopLeft();
  const Math::Vec2<float> panel_size = getPanelSize();
  const Math::Vec4<float> panel_color(0.75f, 0.75f, 0.75f, 1.0f);
  // Same five-quad bevel ControlItemGrid::draw() itself uses for the
  // grid panel, tinted the same neutral gray.
  const std::vector<UiGeometry::ColoredQuad> panel_bevel =
      UiGeometry::buildButtonBevel(
          panel_top_left, panel_size, panel_color, false);
  for (const UiGeometry::ColoredQuad& quad : panel_bevel) {
    UiGeometry::appendColoredQuad(
        vertex_data, quad.m_corners, quad.m_color, m_font.solidTexelUv());
  }

  const Math::Vec4<float> text_color(0.05f, 0.05f, 0.05f, 1.0f);
  const std::string title = "Tutorial 17 - Inventory";
  UiGeometry::appendText(
      vertex_data,
      m_font,
      title,
      Math::Vec2<float>(panel_top_left.x + 20.0f, panel_top_left.y + 30.0f),
      text_color);

  const std::string explain = "(Click an item to see its description)";
  UiGeometry::appendText(
      vertex_data,
      m_font,
      explain,
      Math::Vec2<float>(panel_top_left.x + 20.0f, panel_top_left.y + 58.0f),
      text_color);

  const std::array<GameCatalog::ItemSpec, c_inventory_item_count>& items =
      getItemDisplayData();
  const Math::Vec2<float> cell_size = getGridLayout().cellSize();
  for (std::size_t index = 0; index < items.size(); ++index) {
    const Math::Vec2<float> cell_top_left = getGridLayout().cellTopLeft(index);

    if (index == m_selected_index) {
      // A highlight quad behind the icon/label - the icon pass
      // draws on top of this afterward, so it reads as a border
      // around the selected cell (matches ControlItemGrid's own
      // active-cell highlight).
      const Math::Vec4<float> highlight_color(0.95f, 0.82f, 0.25f, 1.0f);
      const std::array<Math::Vec2<float>, 4> highlight_corners = {
          cell_top_left,
          Math::Vec2<float>(cell_top_left.x, cell_top_left.y + cell_size.y),
          Math::Vec2<float>(
              cell_top_left.x + cell_size.x, cell_top_left.y + cell_size.y),
          Math::Vec2<float>(cell_top_left.x + cell_size.x, cell_top_left.y)};
      UiGeometry::appendColoredQuad(
          vertex_data,
          highlight_corners,
          highlight_color,
          m_font.solidTexelUv());
    }

    const std::string label = "x " + std::to_string(items[index].m_remaining);
    const float label_width = m_font.textWidth(label);
    UiGeometry::appendText(
        vertex_data,
        m_font,
        label,
        Math::Vec2<float>(
            cell_top_left.x + cell_size.x * 0.5f - label_width * 0.5f,
            cell_top_left.y + cell_size.y - 6.0f),
        text_color);
  }

  const float description_top = getDescriptionTop();
  const Math::Vec2<float> panel_bottom_right(
      panel_top_left.x + panel_size.x, panel_top_left.y + panel_size.y);
  const float max_description_width =
      panel_bottom_right.x - (panel_top_left.x + 20.0f) - 20.0f;
  const std::vector<std::string> description_lines =
      wrapText(items[m_selected_index].m_description, max_description_width);
  for (std::size_t line = 0; line < description_lines.size(); ++line) {
    UiGeometry::appendText(
        vertex_data,
        m_font,
        description_lines[line],
        Math::Vec2<float>(
            panel_top_left.x + 20.0f,
            description_top + static_cast<float>(line) * m_font.lineHeight()),
        text_color);
  }

  return vertex_data;
}

std::vector<Tutorial17VertexData> Tutorial17::buildIconPassVertexData() const {
  std::vector<Tutorial17VertexData> vertex_data;
  vertex_data.reserve(c_inventory_item_count * 6);

  const Math::Vec2<float> cell_size = getGridLayout().cellSize();
  // Icon inset within its cell, leaving room for the "x N" label
  // drawn along the cell's bottom edge in the text pass.
  const float icon_size = std::min(cell_size.x, cell_size.y - 24.0f) * 0.8f;
  for (std::size_t index = 0; index < c_inventory_item_count; ++index) {
    const Math::Vec2<float> cell_top_left = getGridLayout().cellTopLeft(index);
    const Math::Vec2<float> icon_top_left(
        cell_top_left.x + cell_size.x * 0.5f - icon_size * 0.5f,
        cell_top_left.y + 6.0f);
    UiGeometry::appendImageQuad(
        vertex_data,
        icon_top_left,
        Math::Vec2<float>(icon_size, icon_size),
        ImageAtlas::tileUv(index, c_icon_atlas_cols, c_icon_atlas_rows).m_min,
        ImageAtlas::tileUv(index, c_icon_atlas_cols, c_icon_atlas_rows).m_max);
  }

  return vertex_data;
}

bool Tutorial17::updateVertexBufferData() {
  std::vector<Tutorial17VertexData> vertex_data = buildTextPassVertexData();
  const std::vector<Tutorial17VertexData> icon_vertex_data =
      buildIconPassVertexData();
  if (vertex_data.size() + icon_vertex_data.size() > c_max_vertex_count) {
    Logging::error(
        LOG_TAG,
        "Inventory vertex data (",
        vertex_data.size() + icon_vertex_data.size(),
        " vertices) exceeds c_max_vertex_count (",
        c_max_vertex_count,
        ")!");
    return false;
  }
  m_text_vertex_count = static_cast<std::uint32_t>(vertex_data.size());
  m_icon_vertex_count = static_cast<std::uint32_t>(icon_vertex_data.size());
  vertex_data.insert(
      vertex_data.end(), icon_vertex_data.begin(), icon_vertex_data.end());
  return m_resources.writeBuffer(m_vertex_buffer, vertex_data);
}

bool Tutorial17::draw() {
  // The vertex buffer is a single, shared, host-visible allocation
  // rewritten every frame (mouse clicks can change the selection at any
  // time) - vkDeviceWaitIdle() avoids racing a previous frame's in-flight
  // command buffer, same reasoning Tutorial15 applies to its own
  // per-frame vertex buffer write.
  vkDeviceWaitIdle(getVkDevice());
  if (!updateVertexBufferData()) {
    return false;
  }

  VkClearValue clear_value = {};
  clear_value.color = {{0.92f, 0.92f, 0.9f, 1.0f}};
  return m_frames.draw(
      *this, {clear_value}, [this](VkCommandBuffer command_buffer) {
        vkCmdBindPipeline(
            command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
        const VkBuffer vertex_buffer = m_vertex_buffer.getVkBuffer();

        // Pass 1: every flat-color/text quad (panel bevel, title/explain/
        // descript, per-cell labels, selection highlight), bound against
        // the font atlas descriptor set.
        const VkDeviceSize text_offset = 0;
        vkCmdBindVertexBuffers(
            command_buffer, 0, 1, &vertex_buffer, &text_offset);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline_layout,
            0,
            1,
            &m_font_descriptor_set,
            0,
            nullptr);
        vkCmdDraw(command_buffer, m_text_vertex_count, 1, 0, 0);

        // Pass 2: the 8 icon quads, drawn on top against the icon atlas
        // descriptor set, from the second range of the same vertex buffer.
        const VkDeviceSize icon_offset =
            static_cast<VkDeviceSize>(m_text_vertex_count) *
            sizeof(Tutorial17VertexData);
        vkCmdBindVertexBuffers(
            command_buffer, 0, 1, &vertex_buffer, &icon_offset);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline_layout,
            0,
            1,
            &m_icon_descriptor_set,
            0,
            nullptr);
        vkCmdDraw(command_buffer, m_icon_vertex_count, 1, 0, 0);
      });
}

void Tutorial17::onMouseButton(
    std::int32_t button,
    bool pressed,
    std::int32_t pos_x,
    std::int32_t pos_y) {
  constexpr std::int32_t c_left_button = 1;
  if (button != c_left_button || !pressed) {
    return;
  }

  const float x = static_cast<float>(pos_x);
  const float y = static_cast<float>(pos_y);
  const Math::Vec2<float> cell_size = getGridLayout().cellSize();
  for (std::size_t index = 0; index < c_inventory_item_count; ++index) {
    const Math::Vec2<float> cell_top_left = getGridLayout().cellTopLeft(index);
    if (x >= cell_top_left.x && x <= cell_top_left.x + cell_size.x &&
        y >= cell_top_left.y && y <= cell_top_left.y + cell_size.y) {
      m_selected_index = index;
      return;
    }
  }
}

bool Tutorial17::childOnWindowSizeChanged() {
  if (getVkDevice() == VK_NULL_HANDLE) {
    return true;
  }
  return createResources();
}

void Tutorial17::childClear() {
  m_frames.destroy();
  m_resources.releaseAll();
}

}  // namespace vulkan_graphix
