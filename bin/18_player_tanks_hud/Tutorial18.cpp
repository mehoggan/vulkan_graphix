#include "Tutorial18.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

#include "vulkan_graphix/HellfireTank.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/UiGeometry.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
constexpr VkFormat c_depth_format = VK_FORMAT_D32_SFLOAT;
}  // namespace

namespace vc = VulkanCommon;

// Two tanks 800 units apart (world X = +-400) span roughly
// [-610,610] in X once each tank's own ~210-unit body half-
// extent is added - a wider world than Tutorial16's single
// tank, so this needs a larger distance still; tuned via
// screenshot the same way.
Tutorial18::Tutorial18() :
    m_camera(0.5f, 0.08f, 1600.0f) {}

Tutorial18::~Tutorial18() { childClear(); }

bool Tutorial18::createResources() {
  if (!m_resources.initialize(*this)) {
    return false;
  }

  // Shared by both tank instances - TankB's own constructor loads
  // "TestImage.raw" for all three of its VBOShaderLibrary parts.
  if (!m_resources.loadRawTexture(
          "TestImage.raw",
          1024,
          1024,
          VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
          m_tank_texture)) {
    Logging::error(LOG_TAG, "Could not create the tank texture!");
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
          m_font_texture)) {
    Logging::error(LOG_TAG, "Could not create the font atlas texture!");
    return false;
  }
  if (!m_resources.createUniformBuffer(
          sizeof(Tutorial18UniformBufferData3D), m_uniform_buffer_3d) ||
      !m_resources.writeBuffer(
          m_uniform_buffer_3d, get3DUniformBufferData()) ||
      !m_resources.createUniformBuffer(
          sizeof(Math::Mat4<float>), m_uniform_buffer_hud) ||
      !m_resources.writeBuffer(
          m_uniform_buffer_hud, getHudUniformBufferData())) {
    Logging::error(LOG_TAG, "Could not create the uniform buffers!");
    return false;
  }

  // 3D layout matches Tutorial16's shaders exactly (0 = sampler in the
  // fragment stage, 1 = uniform buffer in the vertex stage); HUD layout
  // matches Tutorial15/17's (0 = uniform buffer in the vertex stage, 1 =
  // sampler in the fragment stage).
  const std::vector<vc::DescriptorBinding> bindings_3d = {
      {0,
       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       VK_SHADER_STAGE_FRAGMENT_BIT},
      {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}};
  const std::vector<vc::DescriptorBinding> bindings_hud = {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
      {1,
       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       VK_SHADER_STAGE_FRAGMENT_BIT}};
  VkDescriptorSetLayout set_layout_3d = VK_NULL_HANDLE;
  VkDescriptorSetLayout set_layout_hud = VK_NULL_HANDLE;
  VkDescriptorPool pool = VK_NULL_HANDLE;
  if (!m_resources.createDescriptorSetLayout(bindings_3d, &set_layout_3d) ||
      !m_resources.createDescriptorSetLayout(bindings_hud, &set_layout_hud) ||
      !m_resources.createDescriptorPool(bindings_3d, 2, &pool) ||
      !vc::allocateDescriptorSet(
          getVkDevice(), pool, set_layout_3d, &m_descriptor_set_3d) ||
      !vc::allocateDescriptorSet(
          getVkDevice(), pool, set_layout_hud, &m_descriptor_set_hud)) {
    Logging::error(LOG_TAG, "Could not create the descriptor sets!");
    return false;
  }
  vc::writeImageDescriptor(
      getVkDevice(), m_descriptor_set_3d, 0, m_tank_texture);
  vc::writeUniformBufferDescriptor(
      getVkDevice(), m_descriptor_set_3d, 1, m_uniform_buffer_3d);
  vc::writeUniformBufferDescriptor(
      getVkDevice(), m_descriptor_set_hud, 0, m_uniform_buffer_hud);
  vc::writeImageDescriptor(
      getVkDevice(), m_descriptor_set_hud, 1, m_font_texture);

  VkRenderPass render_pass = VK_NULL_HANDLE;
  if (!m_resources.createDepthImage(
          getSwapchainParameters().getVkExtent2d(),
          c_depth_format,
          m_depth_image) ||
      !m_resources.createRenderPass(
          {.m_color_format = getSwapchainParameters().getVkFormat(),
           .m_depth_format = c_depth_format},
          &render_pass) ||
      !m_resources.createPipelineLayout(
          {set_layout_3d},
          {{VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(Tutorial18PushConstants)}},
          &m_pipeline_layout_3d) ||
      !m_resources.createPipelineLayout(
          {set_layout_hud}, {}, &m_pipeline_layout_hud)) {
    Logging::error(LOG_TAG, "Could not create the render pass!");
    return false;
  }

  // 3D pipeline: two tanks, with real depth testing - two independent 3D
  // objects under a freely orbiting camera, unlike Tutorial16's single
  // clustered object. Culling is NONE: this hand-authored asset's winding
  // order hasn't been verified against this project's usual CCW
  // convention, same reasoning Tutorial16 uses.
  if (!m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.18_3d.vert.spv",
           .m_fragment_shader = "shader.18_3d.frag.spv",
           .m_vertex_stride = sizeof(Tutorial18Vertex3DData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial18Vertex3DData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32_SFLOAT,
                 offsetof(Tutorial18Vertex3DData, m_texcoord)}},
           .m_depth = vc::DepthState{},
           .m_layout = m_pipeline_layout_3d,
           .m_render_pass = render_pass},
          &m_pipeline_3d)) {
    Logging::error(LOG_TAG, "Could not create the 3D pipeline!");
    return false;
  }
  // HUD pipeline: depth test/write off - the render pass has a depth
  // attachment (the 3D pass uses it), but the HUD is a 2D overlay drawn
  // after - mirrors GameState::drawHUD()'s own glDisable(GL_DEPTH_TEST)
  // bracketing, adapted to Vulkan's per-pipeline depth state.
  if (!m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.18_hud.vert.spv",
           .m_fragment_shader = "shader.18_hud.frag.spv",
           .m_vertex_stride = sizeof(Tutorial18VertexHudData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial18VertexHudData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32_SFLOAT,
                 offsetof(Tutorial18VertexHudData, m_texcoord)},
                {2,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial18VertexHudData, m_color)}},
           .m_depth = vc::DepthState{false, false, VK_COMPARE_OP_ALWAYS},
           .m_blend = vc::alphaBlend(),
           .m_layout = m_pipeline_layout_hud,
           .m_render_pass = render_pass},
          &m_pipeline_hud)) {
    Logging::error(LOG_TAG, "Could not create the HUD pipeline!");
    return false;
  }

  if (!createPart("Hellfire_Body.ogl", m_body) ||
      !createPart("Hellfire_Head.ogl", m_head) ||
      !createPart("Hellfire_Turret.ogl", m_turret)) {
    return false;
  }
  // The HUD is a static snapshot (see Tutorial18.h), built once.
  const std::vector<Tutorial18VertexHudData> hud_vertex_data =
      buildHudVertexData();
  if (hud_vertex_data.size() > c_max_hud_vertex_count) {
    Logging::error(
        LOG_TAG,
        "HUD vertex data (",
        hud_vertex_data.size(),
        " vertices) exceeds c_max_hud_vertex_count (",
        c_max_hud_vertex_count,
        ")!");
    return false;
  }
  m_hud_vertex_count = static_cast<std::uint32_t>(hud_vertex_data.size());
  if (!m_resources.createDeviceLocalBuffer(
          hud_vertex_data,
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          m_hud_vertex_buffer)) {
    Logging::error(LOG_TAG, "Could not create the HUD vertex buffer!");
    return false;
  }

  if (!m_frames.create(*this, render_pass, m_depth_image.getVkImageView())) {
    Logging::error(LOG_TAG, "Could not create the frame resources!");
    return false;
  }
  return true;
}

bool Tutorial18::createPart(const char* mesh_filename, Part& out) {
  const std::vector<Tutorial18Vertex3DData> vertex_data =
      Tools::loadOglTexturedMesh<Tutorial18Vertex3DData>(mesh_filename);
  if (vertex_data.empty()) {
    Logging::error(
        LOG_TAG, "Could not load mesh data from \"", mesh_filename, "\"!");
    return false;
  }
  out.m_vertex_count = static_cast<std::uint32_t>(vertex_data.size());
  if (!m_resources.createDeviceLocalBuffer(
          vertex_data,
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          out.m_vertex_buffer)) {
    Logging::error(LOG_TAG, "Could not create vertex buffer!");
    return false;
  }
  return true;
}

Tutorial18UniformBufferData3D Tutorial18::get3DUniformBufferData() const {
  Tutorial18UniformBufferData3D data{};
  data.m_view = m_camera.view();

  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  // Near/far sized for this tutorial's wider two-tank world (see the
  // constructor's own comment), not copied from Tutorial16's range.
  data.m_projection = Tools::getPerspectiveProjectionMatrix(
      width / height, 45.0f, 1.0f, 4000.0f);

  return data;
}

Math::Mat4<float> Tutorial18::getHudUniformBufferData() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  return Tools::getOrthographicProjectionMatrix(
      0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

const std::array<Tutorial18PlayerInfo, 2>& Tutorial18::getPlayers() const {
  // Illustrative snapshot data (positions/colors/HP/power are this
  // tutorial's own reasonable choices, not extracted from a fixed
  // in-source default - see Tutorial18.h's top comment for exactly
  // which fields are real vs. illustrative). max_hp = armor*100 is
  // the real formula GameState::drawHUD() itself uses; TankB's real
  // armor is 5, so 500.
  static const std::array<Tutorial18PlayerInfo, 2> players = {
      {{Math::Vec3<float>(400.0f, 0.0f, 0.0f),
        Math::Vec4<float>(0.85f, 0.25f, 0.25f, 1.0f),
        "Player 1",
        460,
        500,
        0.65f},
       {Math::Vec3<float>(-400.0f, 0.0f, 0.0f),
        Math::Vec4<float>(0.3f, 0.45f, 0.9f, 1.0f),
        "Player 2",
        150,
        500,
        0.3f}}};
  return players;
}

Math::Mat4<float> Tutorial18::getBodyModelMatrix(
    const Math::Vec3<float>& world_position) const {
  return HellfireTank::buildPartMatrix(
      HellfireTank::getPartTranslations(world_position).m_body);
}

Math::Mat4<float> Tutorial18::getHeadModelMatrix(
    const Math::Vec3<float>& world_position) const {
  return HellfireTank::buildPartMatrix(
      HellfireTank::getPartTranslations(world_position).m_head);
}

Math::Mat4<float> Tutorial18::getTurretModelMatrix(
    const Math::Vec3<float>& world_position) const {
  return HellfireTank::buildPartMatrix(
      HellfireTank::getPartTranslations(world_position).m_turret);
}

Math::Vec2<float> Tutorial18::getPanelSize() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  return Math::Vec2<float>(width * 0.42f, height * 0.27f);
}

Math::Vec2<float> Tutorial18::getPanelTopLeft(std::size_t player_index) const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  const Math::Vec2<float> size = getPanelSize();
  const float margin = width * 0.03f;
  const float panel_top = height * 0.03f;
  if (player_index == 0) {
    return Math::Vec2<float>(margin, panel_top);
  }
  return Math::Vec2<float>(width - margin - size.x, panel_top);
}

void Tutorial18::appendBar(
    std::vector<Tutorial18VertexHudData>& vertex_data,
    Math::Vec2<float> top_left,
    Math::Vec2<float> size,
    float ratio,
    Math::Vec4<float> (*get_bar_color)(float)) const {
  const Math::Vec4<float> backing_color(0.25f, 0.25f, 0.25f, 1.0f);
  const std::array<Math::Vec2<float>, 4> backing_corners = {
      top_left,
      Math::Vec2<float>(top_left.x, top_left.y + size.y),
      Math::Vec2<float>(top_left.x + size.x, top_left.y + size.y),
      Math::Vec2<float>(top_left.x + size.x, top_left.y)};
  UiGeometry::appendColoredQuad(
      vertex_data, backing_corners, backing_color, m_font.solidTexelUv());

  const float fill_width = size.x * std::clamp(ratio, 0.0f, 1.0f);
  if (fill_width > 0.0f) {
    const Math::Vec4<float> fill_color = get_bar_color(ratio);
    const std::array<Math::Vec2<float>, 4> fill_corners = {
        top_left,
        Math::Vec2<float>(top_left.x, top_left.y + size.y),
        Math::Vec2<float>(top_left.x + fill_width, top_left.y + size.y),
        Math::Vec2<float>(top_left.x + fill_width, top_left.y)};
    UiGeometry::appendColoredQuad(
        vertex_data, fill_corners, fill_color, m_font.solidTexelUv());
  }
}

std::vector<Tutorial18VertexHudData> Tutorial18::buildHudVertexData() const {
  std::vector<Tutorial18VertexHudData> vertex_data;
  vertex_data.reserve(c_max_hud_vertex_count);

  const Math::Vec4<float> text_color(0.05f, 0.05f, 0.05f, 1.0f);
  const Math::Vec4<float> panel_color(0.75f, 0.75f, 0.75f, 1.0f);
  const Math::Vec2<float> panel_size = getPanelSize();
  const std::array<Tutorial18PlayerInfo, 2>& players = getPlayers();

  for (std::size_t i = 0; i < players.size(); ++i) {
    const Math::Vec2<float> panel_top_left = getPanelTopLeft(i);

    const std::vector<UiGeometry::ColoredQuad> bevel =
        UiGeometry::buildButtonBevel(
            panel_top_left, panel_size, panel_color, false);
    for (const UiGeometry::ColoredQuad& quad : bevel) {
      UiGeometry::appendColoredQuad(
          vertex_data, quad.m_corners, quad.m_color, m_font.solidTexelUv());
    }

    UiGeometry::appendText(
        vertex_data,
        m_font,
        players[i].m_name,
        Math::Vec2<float>(panel_top_left.x + 16.0f, panel_top_left.y + 26.0f),
        players[i].m_team_color);

    const std::string hp_text = "HP: " + std::to_string(players[i].m_hp) +
        " / " + std::to_string(players[i].m_max_hp);
    UiGeometry::appendText(
        vertex_data,
        m_font,
        hp_text,
        Math::Vec2<float>(panel_top_left.x + 16.0f, panel_top_left.y + 52.0f),
        text_color);

    const float health_ratio = static_cast<float>(players[i].m_hp) /
        static_cast<float>(players[i].m_max_hp);
    appendBar(
        vertex_data,
        Math::Vec2<float>(panel_top_left.x + 16.0f, panel_top_left.y + 60.0f),
        Math::Vec2<float>(panel_size.x - 32.0f, 14.0f),
        health_ratio,
        &UiGeometry::healthBarColor);

    const std::string power_text = "Power: " +
        std::to_string(static_cast<std::int32_t>(
            players[i].m_power_ratio * 1000.0f));
    UiGeometry::appendText(
        vertex_data,
        m_font,
        power_text,
        Math::Vec2<float>(panel_top_left.x + 16.0f, panel_top_left.y + 100.0f),
        text_color);

    appendBar(
        vertex_data,
        Math::Vec2<float>(panel_top_left.x + 16.0f, panel_top_left.y + 108.0f),
        Math::Vec2<float>(panel_size.x - 32.0f, 14.0f),
        players[i].m_power_ratio,
        &UiGeometry::powerBarColor);
  }

  return vertex_data;
}

bool Tutorial18::draw() {
  vkDeviceWaitIdle(getVkDevice());
  if (!m_resources.writeBuffer(
          m_uniform_buffer_3d, get3DUniformBufferData())) {
    return false;
  }

  std::vector<VkClearValue> clear_values(2);
  clear_values[0].color = {{0.15f, 0.15f, 0.18f, 1.0f}};
  clear_values[1].depthStencil = {1.0f, 0};
  return m_frames.draw(
      *this, clear_values, [this](VkCommandBuffer command_buffer) {
        // --- 3D pass: two tanks, three parts each, one push constant per
        // part carrying that tank's model matrix and team-color tint (see
        // Tutorial18.h's top comment). Vertex buffers are the same three
        // (body/head/turret) static buffers reused for both tank
        // instances.
        vkCmdBindPipeline(
            command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_3d);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline_layout_3d,
            0,
            1,
            &m_descriptor_set_3d,
            0,
            nullptr);
        const VkDeviceSize zero_offset = 0;
        for (const Tutorial18PlayerInfo& player : getPlayers()) {
          const std::array<std::pair<const Part*, Math::Mat4<float>>, 3>
              parts = {
                  {{&m_body, getBodyModelMatrix(player.m_world_position)},
                   {&m_head, getHeadModelMatrix(player.m_world_position)},
                   {&m_turret,
                    getTurretModelMatrix(player.m_world_position)}}};
          for (const auto& [part, model] : parts) {
            const Tutorial18PushConstants push_constants{
                model, player.m_team_color};
            vkCmdBindVertexBuffers(
                command_buffer,
                0,
                1,
                &part->m_vertex_buffer.getVkBuffer(),
                &zero_offset);
            vkCmdPushConstants(
                command_buffer,
                m_pipeline_layout_3d,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(Tutorial18PushConstants),
                &push_constants);
            vkCmdDraw(command_buffer, part->m_vertex_count, 1, 0, 0);
          }
        }

        // --- HUD pass: per-player name/HP/power panels, drawn on top ---
        vkCmdBindPipeline(
            command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_hud);
        vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_hud_vertex_buffer.getVkBuffer(),
            &zero_offset);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline_layout_hud,
            0,
            1,
            &m_descriptor_set_hud,
            0,
            nullptr);
        vkCmdDraw(command_buffer, m_hud_vertex_count, 1, 0, 0);
      });
}

void Tutorial18::onMouseButton(
    std::int32_t button,
    bool pressed,
    std::int32_t pos_x,
    std::int32_t pos_y) {
  m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial18::onMouseMove(std::int32_t pos_x, std::int32_t pos_y) {
  m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial18::childOnWindowSizeChanged() {
  if (getVkDevice() == VK_NULL_HANDLE) {
    return true;
  }
  return createResources();
}

void Tutorial18::childClear() {
  m_frames.destroy();
  m_resources.releaseAll();
}

}  // namespace vulkan_graphix
