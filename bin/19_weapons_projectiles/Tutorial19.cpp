#include "Tutorial19.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/GameCatalog.h"
#include "vulkan_graphix/ImageAtlas.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/UiGeometry.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
struct ProjectileMeshInfo {
  const char* m_mesh_file;
  const char* m_texture_file;
  float m_scale;
  float m_world_x;
};

// Each scale is its weapon's real one (the shared GameCatalog); world_x
// (side-by-side placement) is this tutorial's own layout choice, tuned
// via screenshot.
const std::array<ProjectileMeshInfo, c_projectile_mesh_count>&
getProjectileMeshInfo() {
  using GameCatalog::weapon;
  using GameCatalog::WeaponKind;
  static const std::array<ProjectileMeshInfo, c_projectile_mesh_count> info = {
      {{"projectileDefault.ogl",
        "projectileDefault.raw",
        weapon(WeaponKind::Default).m_scale,
        -220.0f},
       {"projectileAcid.ogl",
        "projectileAcid.raw",
        weapon(WeaponKind::Acid).m_scale,
        0.0f},
       {"projectileBFB.ogl",
        "projectileBFB.raw",
        weapon(WeaponKind::BFB).m_scale,
        220.0f}}};
  return info;
}

Math::Mat4<float> buildProjectileMatrix(float world_x, float scale) {
  return glm::translate(
             Math::Mat4<float>(1.0f), Math::Vec3<float>(world_x, 0.0f, 0.0f)) *
      glm::scale(Math::Mat4<float>(1.0f), Math::Vec3<float>(scale));
}

// The 10 real shop-purchasable weapons - the shared GameCatalog, the same
// data the game's own Weapon subclasses load - in the shop's own order
// (ids 0-9); WeaponDefault (id 10) is an internal Projectile fallback,
// never shop-purchasable, and is not included here.
const std::array<GameCatalog::WeaponSpec, c_weapon_grid_item_count>&
getWeaponDisplayData() {
  static_assert(c_weapon_grid_item_count == GameCatalog::c_shop_weapon_count);
  static const std::array<GameCatalog::WeaponSpec, c_weapon_grid_item_count>
      data = [] {
        std::array<GameCatalog::WeaponSpec, c_weapon_grid_item_count> shop{};
        std::copy_n(GameCatalog::weapons().begin(), shop.size(), shop.begin());
        return shop;
      }();
  return data;
}
}  // namespace

namespace vc = VulkanCommon;

// Real weapon scales (60-100) applied to these meshes' own raw
// modeling units puts each projectile at dozens of world units
// across (BFB, the "Big Force Bomb", reads as noticeably larger
// than the other two, as its name suggests) - spacing/distance
// tuned via screenshot to keep all three clearly separated.
Tutorial19::Tutorial19() :
    m_camera(0.05f, 0.2f, 800.0f),
    m_selected_index(0) {}

Tutorial19::~Tutorial19() { childClear(); }

bool Tutorial19::createResources() {
  if (!m_resources.initialize(*this)) {
    return false;
  }

  const std::array<ProjectileMeshInfo, c_projectile_mesh_count>& mesh_info =
      getProjectileMeshInfo();
  for (std::size_t i = 0; i < mesh_info.size(); ++i) {
    if (!m_resources.loadRawTexture(
            mesh_info[i].m_texture_file,
            512,
            512,
            VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            m_projectile_textures[i])) {
      Logging::error(
          LOG_TAG,
          "Could not load texture \"",
          mesh_info[i].m_texture_file,
          "\"!");
      return false;
    }
    const std::vector<Tutorial19Vertex3DData> vertex_data =
        Tools::loadOglTexturedMesh<Tutorial19Vertex3DData>(
            mesh_info[i].m_mesh_file);
    m_projectile_counts[i] = static_cast<std::uint32_t>(vertex_data.size());
    if (vertex_data.empty() ||
        !m_resources.createDeviceLocalBuffer(
            vertex_data,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            m_projectile_buffers[i])) {
      Logging::error(
          LOG_TAG,
          "Could not load mesh data from \"",
          mesh_info[i].m_mesh_file,
          "\"!");
      return false;
    }
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
          sizeof(Tutorial19UniformBufferData3D), m_uniform_buffer_3d) ||
      !m_resources.writeBuffer(
          m_uniform_buffer_3d, get3DUniformBufferData()) ||
      !m_resources.createUniformBuffer(
          sizeof(Math::Mat4<float>), m_uniform_buffer_grid) ||
      !m_resources.writeBuffer(
          m_uniform_buffer_grid, getGridUniformBufferData())) {
    Logging::error(LOG_TAG, "Could not create the uniform buffers!");
    return false;
  }

  // 3D layout matches Tutorial16's shaders exactly; the grid layout
  // matches Tutorial15/17's.
  const std::vector<vc::DescriptorBinding> bindings_3d = {
      {0,
       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       VK_SHADER_STAGE_FRAGMENT_BIT},
      {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}};
  const std::vector<vc::DescriptorBinding> bindings_grid = {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
      {1,
       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       VK_SHADER_STAGE_FRAGMENT_BIT}};
  VkDescriptorSetLayout set_layout_3d = VK_NULL_HANDLE;
  VkDescriptorSetLayout set_layout_grid = VK_NULL_HANDLE;
  VkDescriptorPool pool = VK_NULL_HANDLE;
  if (!m_resources.createDescriptorSetLayout(bindings_3d, &set_layout_3d) ||
      !m_resources.createDescriptorSetLayout(
          bindings_grid, &set_layout_grid) ||
      !m_resources.createDescriptorPool(
          bindings_3d, c_projectile_mesh_count + 2, &pool)) {
    Logging::error(LOG_TAG, "Could not create the descriptor pool!");
    return false;
  }
  for (std::size_t i = 0; i < c_projectile_mesh_count; ++i) {
    if (!vc::allocateDescriptorSet(
            getVkDevice(), pool, set_layout_3d, &m_descriptor_sets_3d[i])) {
      Logging::error(LOG_TAG, "Could not allocate a descriptor set!");
      return false;
    }
    vc::writeImageDescriptor(
        getVkDevice(), m_descriptor_sets_3d[i], 0, m_projectile_textures[i]);
    vc::writeUniformBufferDescriptor(
        getVkDevice(), m_descriptor_sets_3d[i], 1, m_uniform_buffer_3d);
  }
  if (!vc::allocateDescriptorSet(
          getVkDevice(), pool, set_layout_grid, &m_font_descriptor_set) ||
      !vc::allocateDescriptorSet(
          getVkDevice(), pool, set_layout_grid, &m_icon_descriptor_set)) {
    Logging::error(LOG_TAG, "Could not allocate a descriptor set!");
    return false;
  }
  for (const auto& [descriptor_set, texture] :
       {std::pair{m_font_descriptor_set, &m_font_texture},
        std::pair{m_icon_descriptor_set, &m_icon_texture}}) {
    vc::writeUniformBufferDescriptor(
        getVkDevice(), descriptor_set, 0, m_uniform_buffer_grid);
    vc::writeImageDescriptor(getVkDevice(), descriptor_set, 1, *texture);
  }

  VkRenderPass render_pass = VK_NULL_HANDLE;
  if (!m_resources.createRenderPass(
          {.m_color_format = getSwapchainParameters().getVkFormat()},
          &render_pass) ||
      !m_resources.createPipelineLayout(
          {set_layout_3d},
          {{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Tutorial19PushConstants)}},
          &m_pipeline_layout_3d) ||
      !m_resources.createPipelineLayout(
          {set_layout_grid}, {}, &m_pipeline_layout_grid)) {
    Logging::error(LOG_TAG, "Could not create the render pass!");
    return false;
  }

  // 3D pipeline: the three projectile meshes, no depth buffer (three
  // separate, non-overlapping static meshes). Culling is NONE: these
  // hand-authored assets' winding order hasn't been verified against this
  // project's usual CCW convention, same reasoning Tutorial13/16 use.
  if (!m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.19_3d.vert.spv",
           .m_fragment_shader = "shader.19_3d.frag.spv",
           .m_vertex_stride = sizeof(Tutorial19Vertex3DData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial19Vertex3DData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32_SFLOAT,
                 offsetof(Tutorial19Vertex3DData, m_texcoord)}},
           .m_layout = m_pipeline_layout_3d,
           .m_render_pass = render_pass},
          &m_pipeline_3d) ||
      // Grid pipeline: the 2D weapon inventory grid, alpha-blended.
      !m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.19_grid.vert.spv",
           .m_fragment_shader = "shader.19_grid.frag.spv",
           .m_vertex_stride = sizeof(Tutorial19VertexGridData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial19VertexGridData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32_SFLOAT,
                 offsetof(Tutorial19VertexGridData, m_texcoord)},
                {2,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial19VertexGridData, m_color)}},
           .m_blend = vc::alphaBlend(),
           .m_layout = m_pipeline_layout_grid,
           .m_render_pass = render_pass},
          &m_pipeline_grid)) {
    Logging::error(LOG_TAG, "Could not create the pipelines!");
    return false;
  }

  if (!m_resources.createHostVisibleBuffer(
          static_cast<std::uint32_t>(
              c_max_grid_vertex_count * sizeof(Tutorial19VertexGridData)),
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          m_grid_vertex_buffer) ||
      !updateGridVertexBufferData()) {
    Logging::error(LOG_TAG, "Could not create the grid vertex buffer!");
    return false;
  }

  if (!m_frames.create(*this, render_pass)) {
    Logging::error(LOG_TAG, "Could not create the frame resources!");
    return false;
  }
  return true;
}

bool Tutorial19::createIconAtlas() {
  ImageAtlas atlas(c_icon_size, c_icon_atlas_cols, c_icon_atlas_rows);
  const std::array<GameCatalog::WeaponSpec, c_weapon_grid_item_count>&
      weapons = getWeaponDisplayData();
  for (std::size_t index = 0; index < weapons.size(); ++index) {
    if (!atlas.setTileFromRawFile(index, weapons[index].m_image_file)) {
      Logging::error(
          LOG_TAG,
          "Could not load icon \"",
          weapons[index].m_image_file,
          "\"!");
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

Tutorial19UniformBufferData3D Tutorial19::get3DUniformBufferData() const {
  Tutorial19UniformBufferData3D data{};
  data.m_view = m_camera.view();

  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  data.m_projection = Tools::getPerspectiveProjectionMatrix(
      width / height, 45.0f, 1.0f, 2000.0f);

  return data;
}

Math::Mat4<float> Tutorial19::getGridUniformBufferData() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  return Tools::getOrthographicProjectionMatrix(
      0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

UiGeometry::GridLayout Tutorial19::getGridLayout() const {
  constexpr float c_cell_gap = 8.0f;
  return {
      getGridTopLeft(),
      getGridSize(),
      c_icon_atlas_cols,
      c_icon_atlas_rows,
      c_cell_gap};
}

Math::Vec2<float> Tutorial19::getPanelTopLeft() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  return Math::Vec2<float>(width * 0.05f, height * 0.68f);
}

Math::Vec2<float> Tutorial19::getPanelSize() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  return Math::Vec2<float>(width * 0.9f, height * 0.3f);
}

Math::Vec2<float> Tutorial19::getGridTopLeft() const {
  const Math::Vec2<float> panel_top_left = getPanelTopLeft();
  return Math::Vec2<float>(panel_top_left.x + 16.0f, panel_top_left.y + 20.0f);
}

Math::Vec2<float> Tutorial19::getGridSize() const {
  const Math::Vec2<float> panel_size = getPanelSize();
  return Math::Vec2<float>(panel_size.x - 32.0f, panel_size.y * 0.55f);
}

float Tutorial19::getDescriptionTop() const {
  const Math::Vec2<float> grid_top_left = getGridTopLeft();
  const Math::Vec2<float> grid_size = getGridSize();
  return grid_top_left.y + grid_size.y + 24.0f;
}

std::vector<std::string> Tutorial19::wrapText(
    const std::string& text, float max_width) const {
  return m_font.wrapText(text, max_width);
}

std::vector<Tutorial19VertexGridData> Tutorial19::buildTextPassVertexData()
    const {
  std::vector<Tutorial19VertexGridData> vertex_data;
  vertex_data.reserve(c_max_grid_vertex_count);

  const Math::Vec2<float> panel_top_left = getPanelTopLeft();
  const Math::Vec2<float> panel_size = getPanelSize();
  const Math::Vec4<float> panel_color(0.75f, 0.75f, 0.75f, 1.0f);
  const std::vector<UiGeometry::ColoredQuad> panel_bevel =
      UiGeometry::buildButtonBevel(
          panel_top_left, panel_size, panel_color, false);
  for (const UiGeometry::ColoredQuad& quad : panel_bevel) {
    UiGeometry::appendColoredQuad(
        vertex_data, quad.m_corners, quad.m_color, m_font.solidTexelUv());
  }

  const Math::Vec4<float> text_color(0.05f, 0.05f, 0.05f, 1.0f);
  const std::string title = "Tutorial 19 - Weapons";
  UiGeometry::appendText(
      vertex_data,
      m_font,
      title,
      Math::Vec2<float>(panel_top_left.x + 16.0f, panel_top_left.y + 16.0f),
      text_color);

  const std::array<GameCatalog::WeaponSpec, c_weapon_grid_item_count>&
      weapons = getWeaponDisplayData();
  const Math::Vec2<float> cell_size = getGridLayout().cellSize();
  for (std::size_t index = 0; index < weapons.size(); ++index) {
    const Math::Vec2<float> cell_top_left = getGridLayout().cellTopLeft(index);

    if (index == m_selected_index) {
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

    const std::string label = "$" + std::to_string(weapons[index].m_price);
    const float label_width = m_font.textWidth(label);
    UiGeometry::appendText(
        vertex_data,
        m_font,
        label,
        Math::Vec2<float>(
            cell_top_left.x + cell_size.x * 0.5f - label_width * 0.5f,
            cell_top_left.y + cell_size.y - 4.0f),
        text_color);
  }

  const float description_top = getDescriptionTop();
  const float max_description_width = panel_size.x - 32.0f;
  const std::vector<std::string> description_lines =
      wrapText(weapons[m_selected_index].m_description, max_description_width);
  for (std::size_t line = 0; line < description_lines.size(); ++line) {
    UiGeometry::appendText(
        vertex_data,
        m_font,
        description_lines[line],
        Math::Vec2<float>(
            panel_top_left.x + 16.0f,
            description_top + static_cast<float>(line) * m_font.lineHeight()),
        text_color);
  }

  return vertex_data;
}

std::vector<Tutorial19VertexGridData> Tutorial19::buildIconPassVertexData()
    const {
  std::vector<Tutorial19VertexGridData> vertex_data;
  vertex_data.reserve(c_weapon_grid_item_count * 6);

  const Math::Vec2<float> cell_size = getGridLayout().cellSize();
  const float icon_size = std::min(cell_size.x, cell_size.y - 18.0f) * 0.8f;
  for (std::size_t index = 0; index < c_weapon_grid_item_count; ++index) {
    const Math::Vec2<float> cell_top_left = getGridLayout().cellTopLeft(index);
    const Math::Vec2<float> icon_top_left(
        cell_top_left.x + cell_size.x * 0.5f - icon_size * 0.5f,
        cell_top_left.y + 4.0f);
    UiGeometry::appendImageQuad(
        vertex_data,
        icon_top_left,
        Math::Vec2<float>(icon_size, icon_size),
        ImageAtlas::tileUv(index, c_icon_atlas_cols, c_icon_atlas_rows).m_min,
        ImageAtlas::tileUv(index, c_icon_atlas_cols, c_icon_atlas_rows).m_max);
  }

  return vertex_data;
}

bool Tutorial19::updateGridVertexBufferData() {
  std::vector<Tutorial19VertexGridData> vertex_data =
      buildTextPassVertexData();
  const std::vector<Tutorial19VertexGridData> icon_vertex_data =
      buildIconPassVertexData();
  if (vertex_data.size() + icon_vertex_data.size() > c_max_grid_vertex_count) {
    Logging::error(
        LOG_TAG,
        "Weapon grid vertex data (",
        vertex_data.size() + icon_vertex_data.size(),
        " vertices) exceeds c_max_grid_vertex_count (",
        c_max_grid_vertex_count,
        ")!");
    return false;
  }
  m_text_vertex_count = static_cast<std::uint32_t>(vertex_data.size());
  m_icon_vertex_count = static_cast<std::uint32_t>(icon_vertex_data.size());
  vertex_data.insert(
      vertex_data.end(), icon_vertex_data.begin(), icon_vertex_data.end());
  return m_resources.writeBuffer(m_grid_vertex_buffer, vertex_data);
}

bool Tutorial19::draw() {
  // Vertex buffer/UBO writes race a previous frame's in-flight command
  // buffer without this, same reasoning Tutorial15/17 use for their own
  // per-frame vertex buffer writes.
  vkDeviceWaitIdle(getVkDevice());
  if (!m_resources.writeBuffer(
          m_uniform_buffer_3d, get3DUniformBufferData()) ||
      !updateGridVertexBufferData()) {
    return false;
  }

  VkClearValue clear_value = {};
  clear_value.color = {{0.15f, 0.15f, 0.18f, 1.0f}};
  return m_frames.draw(
      *this, {clear_value}, [this](VkCommandBuffer command_buffer) {
        // --- 3D pass: three projectile meshes, each with its own texture
        // (descriptor set) and its own push-constant model matrix.
        vkCmdBindPipeline(
            command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_3d);
        const VkDeviceSize zero_offset = 0;
        const std::array<ProjectileMeshInfo, c_projectile_mesh_count>&
            mesh_info = getProjectileMeshInfo();
        for (std::size_t i = 0; i < c_projectile_mesh_count; ++i) {
          vkCmdBindDescriptorSets(
              command_buffer,
              VK_PIPELINE_BIND_POINT_GRAPHICS,
              m_pipeline_layout_3d,
              0,
              1,
              &m_descriptor_sets_3d[i],
              0,
              nullptr);
          vkCmdBindVertexBuffers(
              command_buffer,
              0,
              1,
              &m_projectile_buffers[i].getVkBuffer(),
              &zero_offset);
          const Tutorial19PushConstants push_constants{buildProjectileMatrix(
              mesh_info[i].m_world_x, mesh_info[i].m_scale)};
          vkCmdPushConstants(
              command_buffer,
              m_pipeline_layout_3d,
              VK_SHADER_STAGE_VERTEX_BIT,
              0,
              sizeof(Tutorial19PushConstants),
              &push_constants);
          vkCmdDraw(command_buffer, m_projectile_counts[i], 1, 0, 0);
        }

        // --- Grid pass: weapon labels (font set), then icons (icon set)
        // drawn on top from the second range of the same vertex buffer.
        vkCmdBindPipeline(
            command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_grid);
        const VkBuffer grid_buffer = m_grid_vertex_buffer.getVkBuffer();
        vkCmdBindVertexBuffers(
            command_buffer, 0, 1, &grid_buffer, &zero_offset);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline_layout_grid,
            0,
            1,
            &m_font_descriptor_set,
            0,
            nullptr);
        vkCmdDraw(command_buffer, m_text_vertex_count, 1, 0, 0);

        const VkDeviceSize icon_offset =
            static_cast<VkDeviceSize>(m_text_vertex_count) *
            sizeof(Tutorial19VertexGridData);
        vkCmdBindVertexBuffers(
            command_buffer, 0, 1, &grid_buffer, &icon_offset);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline_layout_grid,
            0,
            1,
            &m_icon_descriptor_set,
            0,
            nullptr);
        vkCmdDraw(command_buffer, m_icon_vertex_count, 1, 0, 0);
      });
}

void Tutorial19::onMouseButton(
    std::int32_t button,
    bool pressed,
    std::int32_t pos_x,
    std::int32_t pos_y) {
  constexpr std::int32_t c_left_button = 1;
  if (button == c_left_button && pressed) {
    const float x = static_cast<float>(pos_x);
    const float y = static_cast<float>(pos_y);
    const Math::Vec2<float> cell_size = getGridLayout().cellSize();
    for (std::size_t index = 0; index < c_weapon_grid_item_count; ++index) {
      const Math::Vec2<float> cell_top_left =
          getGridLayout().cellTopLeft(index);
      if (x >= cell_top_left.x && x <= cell_top_left.x + cell_size.x &&
          y >= cell_top_left.y && y <= cell_top_left.y + cell_size.y) {
        m_selected_index = index;
        return;
      }
    }
  }
  m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial19::onMouseMove(std::int32_t pos_x, std::int32_t pos_y) {
  m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial19::childOnWindowSizeChanged() {
  if (getVkDevice() == VK_NULL_HANDLE) {
    return true;
  }
  return createResources();
}

void Tutorial19::childClear() {
  m_frames.destroy();
  m_resources.releaseAll();
}

}  // namespace vulkan_graphix
