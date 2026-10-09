#include "Tutorial22.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/HellfireTank.h"
#include "vulkan_graphix/OrbitCamera.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/UiGeometry.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
constexpr VkFormat c_depth_format = VK_FORMAT_D32_SFLOAT;

// Fixed preview camera - Tutorial16's own OrbitCamera values (yaw=0.6,
// pitch=-0.05, distance=650), never fed mouse input, since ReadyMenu's own
// preview camera is a static gluLookAt - only the tank itself spins.
const OrbitCamera c_preview_camera(0.6f, -0.05f, 650.0f);
}  // namespace

namespace vc = VulkanCommon;

Tutorial22::Tutorial22() :
    m_button_pressed(false),
    m_click_count(0),
    m_tank_angle(0.0f) {}

Tutorial22::~Tutorial22() { childClear(); }

bool Tutorial22::createResources() {
  if (!m_resources.initialize(*this)) {
    return false;
  }

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
          sizeof(Tutorial22TankUniformBufferData), m_tank_uniform_buffer) ||
      !m_resources.writeBuffer(
          m_tank_uniform_buffer, getTankUniformBufferData()) ||
      !m_resources.createUniformBuffer(
          sizeof(Math::Mat4<float>), m_panel_uniform_buffer) ||
      !m_resources.writeBuffer(
          m_panel_uniform_buffer, getPanelUniformBufferData())) {
    Logging::error(LOG_TAG, "Could not create the uniform buffers!");
    return false;
  }

  // The tank layout matches Tutorial16's own shader layout exactly (0 =
  // sampler in the fragment stage, 1 = UBO in the vertex stage); the
  // panel layout matches Tutorial15's (0 = UBO in the vertex stage, 1 =
  // sampler in the fragment stage).
  const std::vector<vc::DescriptorBinding> tank_bindings = {
      {0,
       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       VK_SHADER_STAGE_FRAGMENT_BIT},
      {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}};
  const std::vector<vc::DescriptorBinding> panel_bindings = {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
      {1,
       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       VK_SHADER_STAGE_FRAGMENT_BIT}};
  VkDescriptorSetLayout tank_layout = VK_NULL_HANDLE;
  VkDescriptorSetLayout panel_layout = VK_NULL_HANDLE;
  VkDescriptorPool pool = VK_NULL_HANDLE;
  if (!m_resources.createDescriptorSetLayout(tank_bindings, &tank_layout) ||
      !m_resources.createDescriptorSetLayout(panel_bindings, &panel_layout) ||
      !m_resources.createDescriptorPool(tank_bindings, 2, &pool) ||
      !vc::allocateDescriptorSet(
          getVkDevice(), pool, tank_layout, &m_tank_descriptor_set) ||
      !vc::allocateDescriptorSet(
          getVkDevice(), pool, panel_layout, &m_panel_descriptor_set)) {
    Logging::error(LOG_TAG, "Could not create the descriptor sets!");
    return false;
  }
  vc::writeImageDescriptor(
      getVkDevice(), m_tank_descriptor_set, 0, m_tank_texture);
  vc::writeUniformBufferDescriptor(
      getVkDevice(), m_tank_descriptor_set, 1, m_tank_uniform_buffer);
  vc::writeUniformBufferDescriptor(
      getVkDevice(), m_panel_descriptor_set, 0, m_panel_uniform_buffer);
  vc::writeImageDescriptor(
      getVkDevice(), m_panel_descriptor_set, 1, m_font_texture);

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
          {tank_layout},
          {{VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(Tutorial22TankPushConstants)}},
          &m_tank_pipeline_layout) ||
      !m_resources.createPipelineLayout(
          {panel_layout}, {}, &m_panel_pipeline_layout)) {
    Logging::error(LOG_TAG, "Could not create the render pass!");
    return false;
  }

  // Tank preview pipeline: unlit, textured, real depth test/write, with
  // its dynamic viewport/scissor confined to the preview sub-region at
  // draw time.
  if (!m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.22_3d.vert.spv",
           .m_fragment_shader = "shader.22_3d.frag.spv",
           .m_vertex_stride = sizeof(Tutorial22TankVertexData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial22TankVertexData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32_SFLOAT,
                 offsetof(Tutorial22TankVertexData, m_texcoord)}},
           .m_depth = vc::DepthState{},
           .m_layout = m_tank_pipeline_layout,
           .m_render_pass = render_pass},
          &m_tank_pipeline) ||
      // Panel pipeline: 2D, alpha-blended, full extent, depth disabled
      // entirely - the panel is drawn first, full-screen, and should
      // never be occluded by (or occlude) the 3D preview drawn on top of
      // it within its own sub-region.
      !m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.22_panel.vert.spv",
           .m_fragment_shader = "shader.22_panel.frag.spv",
           .m_vertex_stride = sizeof(Tutorial22PanelVertexData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial22PanelVertexData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32_SFLOAT,
                 offsetof(Tutorial22PanelVertexData, m_texcoord)},
                {2,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial22PanelVertexData, m_color)}},
           .m_depth = vc::DepthState{false, false, VK_COMPARE_OP_ALWAYS},
           .m_blend = vc::alphaBlend(),
           .m_layout = m_panel_pipeline_layout,
           .m_render_pass = render_pass},
          &m_panel_pipeline)) {
    Logging::error(LOG_TAG, "Could not create the pipelines!");
    return false;
  }

  const std::array<const char*, c_tutorial22_tank_part_count> tank_files = {
      {"Hellfire_Body.ogl", "Hellfire_Head.ogl", "Hellfire_Turret.ogl"}};
  for (std::size_t i = 0; i < c_tutorial22_tank_part_count; ++i) {
    const std::vector<Tutorial22TankVertexData> vertex_data =
        Tools::loadOglTexturedMesh<Tutorial22TankVertexData>(tank_files[i]);
    m_tank_vertex_counts[i] = static_cast<std::uint32_t>(vertex_data.size());
    if (vertex_data.empty() ||
        !m_resources.createDeviceLocalBuffer(
            vertex_data,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            m_tank_vertex_buffers[i])) {
      Logging::error(
          LOG_TAG, "Could not load mesh data from \"", tank_files[i], "\"!");
      return false;
    }
  }
  // Host-visible: rewritten every frame from the button's press state.
  if (!m_resources.createHostVisibleBuffer(
          static_cast<std::uint32_t>(
              c_max_panel_vertex_count * sizeof(Tutorial22PanelVertexData)),
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          m_panel_vertex_buffer) ||
      !updatePanelVertexBufferData()) {
    Logging::error(LOG_TAG, "Could not create the panel vertex buffer!");
    return false;
  }

  if (!m_frames.create(*this, render_pass, m_depth_image.getVkImageView())) {
    Logging::error(LOG_TAG, "Could not create the frame resources!");
    return false;
  }
  return true;
}

Math::Vec2<float> Tutorial22::getPreviewSize() const {
  return Math::Vec2<float>(280.0f, 200.0f);
}

Math::Vec2<float> Tutorial22::getPreviewTopLeft() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  const Math::Vec2<float> size = getPreviewSize();
  return Math::Vec2<float>(
      width * 0.5f - size.x * 0.5f, height * 0.5f - size.y * 0.5f - 40.0f);
}

Tutorial22TankUniformBufferData Tutorial22::getTankUniformBufferData() const {
  Tutorial22TankUniformBufferData data{};
  data.m_view = c_preview_camera.view();

  const Math::Vec2<float> preview_size = getPreviewSize();
  data.m_projection = Tools::getPerspectiveProjectionMatrix(
      preview_size.x / preview_size.y, 45.0f, 1.0f, 5000.0f);

  return data;
}

Math::Mat4<float> Tutorial22::getPanelUniformBufferData() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  return Tools::getOrthographicProjectionMatrix(
      0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

Math::Mat4<float> Tutorial22::getTankPartModelMatrix(
    const Math::Vec3<float>& part_translation) const {
  return glm::rotate(
             Math::Mat4<float>(1.0f),
             m_tank_angle,
             Math::Vec3<float>(0.0f, 1.0f, 0.0f)) *
      HellfireTank::buildPartMatrix(part_translation);
}

Math::Vec2<float> Tutorial22::getButtonSize() const {
  return Math::Vec2<float>(180.0f, 50.0f);
}

Math::Vec2<float> Tutorial22::getButtonTopLeft() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  const Math::Vec2<float> size = getButtonSize();
  return Math::Vec2<float>(width * 0.5f - size.x * 0.5f, height - 100.0f);
}

std::string Tutorial22::getButtonLabel() const {
  if (m_click_count == 0) {
    return "Ready";
  }
  return "Ready x" + std::to_string(m_click_count);
}

std::vector<Tutorial22PanelVertexData> Tutorial22::buildPanelVertexData()
    const {
  std::vector<Tutorial22PanelVertexData> vertex_data;
  vertex_data.reserve(c_max_panel_vertex_count);

  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  const Math::Vec4<float> text_color(0.05f, 0.05f, 0.05f, 1.0f);

  // Main menu panel background - the identical 5-quad bevel every real
  // vulkan_earth menu screen (MainMenu/ReadyMenu/ShopMenu/SubMenu*)
  // opens with, confirmed via research (see this header's own top
  // comment).
  const Math::Vec2<float> panel_top_left(
      width * 0.5f - 200.0f, height * 0.5f - 240.0f);
  const Math::Vec2<float> panel_size(400.0f, 480.0f);
  const Math::Vec4<float> panel_color(0.35f, 0.38f, 0.45f, 1.0f);
  for (const UiGeometry::ColoredQuad& quad : UiGeometry::buildButtonBevel(
           panel_top_left, panel_size, panel_color, false)) {
    UiGeometry::appendColoredQuad(
        vertex_data, quad.m_corners, quad.m_color, m_font.solidTexelUv());
  }

  const std::string title = "Ready?";
  const float title_width = m_font.textWidth(title);
  const Math::Vec2<float> title_origin(
      width * 0.5f - title_width * 0.5f, panel_top_left.y + 40.0f);
  UiGeometry::appendText(vertex_data, m_font, title, title_origin, text_color);

  // A sunken "well" frame around the live 3D preview - matches
  // ReadyMenu's own tank preview being composited inside its menu
  // panel, not floating over it unframed.
  const Math::Vec2<float> preview_top_left = getPreviewTopLeft();
  const Math::Vec2<float> preview_size = getPreviewSize();
  constexpr float c_frame_margin = 10.0f;
  const Math::Vec2<float> frame_top_left(
      preview_top_left.x - c_frame_margin,
      preview_top_left.y - c_frame_margin);
  const Math::Vec2<float> frame_size(
      preview_size.x + 2.0f * c_frame_margin,
      preview_size.y + 2.0f * c_frame_margin);
  const Math::Vec4<float> frame_color(0.15f, 0.16f, 0.2f, 1.0f);
  for (const UiGeometry::ColoredQuad& quad : UiGeometry::buildButtonBevel(
           frame_top_left, frame_size, frame_color, true)) {
    UiGeometry::appendColoredQuad(
        vertex_data, quad.m_corners, quad.m_color, m_font.solidTexelUv());
  }

  const Math::Vec2<float> button_top_left = getButtonTopLeft();
  const Math::Vec2<float> button_size = getButtonSize();
  const Math::Vec4<float> button_color(0.3f, 0.5f, 0.75f, 1.0f);
  for (const UiGeometry::ColoredQuad& quad : UiGeometry::buildButtonBevel(
           button_top_left, button_size, button_color, m_button_pressed)) {
    UiGeometry::appendColoredQuad(
        vertex_data, quad.m_corners, quad.m_color, m_font.solidTexelUv());
  }

  const std::string label = getButtonLabel();
  const float label_width = m_font.textWidth(label);
  const Math::Vec2<float> label_origin(
      button_top_left.x + button_size.x * 0.5f - label_width * 0.5f,
      button_top_left.y + button_size.y * 0.5f + c_font_pixel_height * 0.3f);
  UiGeometry::appendText(vertex_data, m_font, label, label_origin, text_color);

  return vertex_data;
}

bool Tutorial22::updatePanelVertexBufferData() {
  const std::vector<Tutorial22PanelVertexData> vertex_data =
      buildPanelVertexData();
  if (vertex_data.size() > c_max_panel_vertex_count) {
    Logging::error(
        LOG_TAG,
        "Panel vertex data (",
        vertex_data.size(),
        " vertices) exceeds c_max_panel_vertex_count (",
        c_max_panel_vertex_count,
        ")!");
    return false;
  }
  m_panel_vertex_count = static_cast<std::uint32_t>(vertex_data.size());
  return m_resources.writeBuffer(m_panel_vertex_buffer, vertex_data);
}

bool Tutorial22::draw() {
  // ReadyMenu's own tank_angle += 0.25f per frame - this preview's
  // continuous spin, independent of mouse/camera input.
  m_tank_angle += c_tank_spin_step_radians;

  vkDeviceWaitIdle(getVkDevice());
  if (!updatePanelVertexBufferData()) {
    return false;
  }

  std::vector<VkClearValue> clear_values(2);
  clear_values[0].color = {{0.78f, 0.8f, 0.83f, 1.0f}};
  clear_values[1].depthStencil = {1.0f, 0};
  return m_frames.draw(
      *this, clear_values, [this](VkCommandBuffer command_buffer) {
        const VkDeviceSize zero_offset = 0;

        // --- Pass 1: 2D panel, full swapchain extent (begin() set the
        // full viewport/scissor), depth disabled ---
        vkCmdBindPipeline(
            command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_panel_pipeline);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_panel_pipeline_layout,
            0,
            1,
            &m_panel_descriptor_set,
            0,
            nullptr);
        vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_panel_vertex_buffer.getVkBuffer(),
            &zero_offset);
        vkCmdDraw(command_buffer, m_panel_vertex_count, 1, 0, 0);

        // --- Pass 2: 3D tank preview, scissored to its own sub-region,
        // drawn on top of the panel so it appears composited inside it ---
        vkCmdBindPipeline(
            command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_tank_pipeline);
        const Math::Vec2<float> preview_top_left = getPreviewTopLeft();
        const Math::Vec2<float> preview_size = getPreviewSize();
        const VkViewport preview_viewport = {
            .x = preview_top_left.x,
            .y = preview_top_left.y,
            .width = preview_size.x,
            .height = preview_size.y,
            .minDepth = 0.0f,
            .maxDepth = 1.0f};
        const VkRect2D preview_scissor = {
            .offset =
                {.x = static_cast<std::int32_t>(preview_top_left.x),
                 .y = static_cast<std::int32_t>(preview_top_left.y)},
            .extent = {
                .width = static_cast<std::uint32_t>(preview_size.x),
                .height = static_cast<std::uint32_t>(preview_size.y)}};
        vkCmdSetViewport(command_buffer, 0, 1, &preview_viewport);
        vkCmdSetScissor(command_buffer, 0, 1, &preview_scissor);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_tank_pipeline_layout,
            0,
            1,
            &m_tank_descriptor_set,
            0,
            nullptr);

        const HellfireTank::PartTranslations tank_parts =
            HellfireTank::getPartTranslations(Math::Vec3<float>(0.0f));
        const std::array<Math::Vec3<float>, c_tutorial22_tank_part_count>
            part_translations = {
                {tank_parts.m_body, tank_parts.m_head, tank_parts.m_turret}};
        for (std::size_t i = 0; i < c_tutorial22_tank_part_count; ++i) {
          vkCmdBindVertexBuffers(
              command_buffer,
              0,
              1,
              &m_tank_vertex_buffers[i].getVkBuffer(),
              &zero_offset);
          const Tutorial22TankPushConstants push_constants{
              getTankPartModelMatrix(part_translations[i])};
          vkCmdPushConstants(
              command_buffer,
              m_tank_pipeline_layout,
              VK_SHADER_STAGE_VERTEX_BIT,
              0,
              sizeof(Tutorial22TankPushConstants),
              &push_constants);
          vkCmdDraw(command_buffer, m_tank_vertex_counts[i], 1, 0, 0);
        }
      });
}

void Tutorial22::onMouseButton(
    std::int32_t button,
    bool pressed,
    std::int32_t pos_x,
    std::int32_t pos_y) {
  constexpr std::int32_t c_left_button = 1;
  if (button != c_left_button) {
    return;
  }

  if (!pressed) {
    m_button_pressed = false;
    return;
  }

  const Math::Vec2<float> top_left = getButtonTopLeft();
  const Math::Vec2<float> size = getButtonSize();
  const float x = static_cast<float>(pos_x);
  const float y = static_cast<float>(pos_y);
  if (x >= top_left.x && x <= top_left.x + size.x && y >= top_left.y &&
      y <= top_left.y + size.y) {
    m_button_pressed = true;
    ++m_click_count;
  } else {
    m_button_pressed = false;
  }
}

bool Tutorial22::childOnWindowSizeChanged() {
  if (getVkDevice() == VK_NULL_HANDLE) {
    return true;
  }
  return createResources();
}

void Tutorial22::childClear() {
  m_frames.destroy();
  m_resources.releaseAll();
}

}  // namespace vulkan_graphix
