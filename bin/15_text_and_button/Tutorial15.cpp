#include "Tutorial15.h"

#include <cstddef>
#include <cstdint>
#include <string>

#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/UiGeometry.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace vc = VulkanCommon;

Tutorial15::Tutorial15() :
    m_button_pressed(false),
    m_click_count(0) {}

Tutorial15::~Tutorial15() { childClear(); }

bool Tutorial15::createResources() {
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
          m_font_texture)) {
    Logging::error(LOG_TAG, "Could not create the font atlas texture!");
    return false;
  }
  if (!m_resources.createUniformBuffer(
          sizeof(Math::Mat4<float>), m_uniform_buffer) ||
      !m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    Logging::error(LOG_TAG, "Could not create uniform buffer!");
    return false;
  }

  const std::vector<vc::DescriptorBinding> bindings = {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
      {1,
       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       VK_SHADER_STAGE_FRAGMENT_BIT}};
  VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
  VkDescriptorPool pool = VK_NULL_HANDLE;
  if (!m_resources.createDescriptorSetLayout(bindings, &set_layout) ||
      !m_resources.createDescriptorPool(bindings, 1, &pool) ||
      !vc::allocateDescriptorSet(
          getVkDevice(), pool, set_layout, &m_descriptor_set)) {
    Logging::error(LOG_TAG, "Could not create the descriptor set!");
    return false;
  }
  vc::writeUniformBufferDescriptor(
      getVkDevice(), m_descriptor_set, 0, m_uniform_buffer);
  vc::writeImageDescriptor(getVkDevice(), m_descriptor_set, 1, m_font_texture);

  VkRenderPass render_pass = VK_NULL_HANDLE;
  if (!m_resources.createRenderPass(
          {.m_color_format = getSwapchainParameters().getVkFormat()},
          &render_pass) ||
      !m_resources.createPipelineLayout(
          {set_layout}, {}, &m_pipeline_layout)) {
    Logging::error(LOG_TAG, "Could not create the render pass!");
    return false;
  }

  // A non-indexed triangle list, 6 vertices/quad - matches Tutorial12's
  // non-indexed precedent; simplest fit for a small, per-frame-rebuilt CPU
  // vertex list (see updateVertexBufferData()). 2D UI, so winding doesn't
  // matter and there's no culling. Alpha blending on - glyph edges are
  // anti-aliased via the atlas's coverage alpha, same as Tutorial14's
  // translucent sphere.
  if (!m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.15.vert.spv",
           .m_fragment_shader = "shader.15.frag.spv",
           .m_vertex_stride = sizeof(Tutorial15VertexData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial15VertexData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32_SFLOAT,
                 offsetof(Tutorial15VertexData, m_texcoord)},
                {2,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial15VertexData, m_color)}},
           .m_blend = vc::alphaBlend(),
           .m_layout = m_pipeline_layout,
           .m_render_pass = render_pass},
          &m_pipeline)) {
    Logging::error(LOG_TAG, "Could not create graphics pipeline!");
    return false;
  }

  // Host-visible/coherent, not device-local+staged: rebuilt and
  // re-uploaded every frame from small CPU-side UI state (see
  // updateVertexBufferData()) - the same per-frame-map technique every
  // tutorial already uses for its uniform buffer, extended to vertex data
  // here since this content is small and dynamic rather than
  // static/upload-once.
  if (!m_resources.createHostVisibleBuffer(
          static_cast<std::uint32_t>(
              c_max_vertex_count * sizeof(Tutorial15VertexData)),
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

Math::Mat4<float> Tutorial15::getUniformBufferData() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  // Top-left-origin, y-down screen convention (matches the mouse
  // coordinates onMouseButton() receives): world (0,0) is the window's
  // top-left corner, world (width,height) is bottom-right. Vulkan's
  // own NDC is already y-down (unlike OpenGL's), so this maps directly
  // with no extra sign flip - top=0 -> NDC y=-1 (top), bottom=height ->
  // NDC y=+1 (bottom).
  return Tools::getOrthographicProjectionMatrix(
      0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

Math::Vec2<float> Tutorial15::getButtonSize() const {
  return Math::Vec2<float>(180.0f, 56.0f);
}

Math::Vec2<float> Tutorial15::getButtonTopLeft() const {
  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  const Math::Vec2<float> size = getButtonSize();
  return Math::Vec2<float>(
      width * 0.5f - size.x * 0.5f, height * 0.5f - size.y * 0.5f);
}

std::string Tutorial15::getButtonLabel() const {
  if (m_click_count == 0) {
    return "Click Me";
  }
  return "Clicked: " + std::to_string(m_click_count);
}

std::vector<Tutorial15VertexData> Tutorial15::buildUiVertexData() const {
  std::vector<Tutorial15VertexData> vertex_data;
  vertex_data.reserve(c_max_vertex_count);

  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const Math::Vec4<float> text_color(0.05f, 0.05f, 0.05f, 1.0f);

  const std::string title = "Tutorial 15 - Text & UI";
  const float title_width = m_font.textWidth(title);
  const Math::Vec2<float> title_origin(
      width * 0.5f - title_width * 0.5f, 140.0f);
  UiGeometry::appendText(vertex_data, m_font, title, title_origin, text_color);

  const Math::Vec2<float> button_top_left = getButtonTopLeft();
  const Math::Vec2<float> button_size = getButtonSize();
  const Math::Vec4<float> button_color(0.3f, 0.5f, 0.75f, 1.0f);
  const std::vector<UiGeometry::ColoredQuad> bevel =
      UiGeometry::buildButtonBevel(
          button_top_left, button_size, button_color, m_button_pressed);
  for (const UiGeometry::ColoredQuad& quad : bevel) {
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

bool Tutorial15::updateVertexBufferData() {
  const std::vector<Tutorial15VertexData> vertex_data = buildUiVertexData();
  if (vertex_data.size() > c_max_vertex_count) {
    Logging::error(
        LOG_TAG,
        "UI vertex data (",
        vertex_data.size(),
        " vertices) exceeds c_max_vertex_count (",
        c_max_vertex_count,
        ")!");
    return false;
  }
  m_vertex_count = static_cast<std::uint32_t>(vertex_data.size());
  return m_resources.writeBuffer(m_vertex_buffer, vertex_data);
}

bool Tutorial15::draw() {
  // The vertex buffer is a single, shared, host-visible allocation
  // rewritten every frame from current UI state (mouse clicks can change
  // it at any time) - vkDeviceWaitIdle() avoids racing a previous frame's
  // in-flight command buffer that might still be reading it, same
  // reasoning every camera-driven tutorial already applies to its own
  // per-frame uniform buffer write.
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
        const VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(
            command_buffer, 0, 1, &m_vertex_buffer.getVkBuffer(), &offset);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline_layout,
            0,
            1,
            &m_descriptor_set,
            0,
            nullptr);
        vkCmdDraw(command_buffer, m_vertex_count, 1, 0, 0);
      });
}

void Tutorial15::onMouseButton(
    std::int32_t button,
    bool pressed,
    std::int32_t pos_x,
    std::int32_t pos_y) {
  constexpr std::int32_t c_left_button = 1;
  if (button != c_left_button) {
    return;
  }

  if (!pressed) {
    // Matches ControlItemButton::mouseClickEvent()'s own behavior:
    // the visual press state always clears on release, regardless
    // of where the release happened.
    m_button_pressed = false;
    return;
  }

  const Math::Vec2<float> top_left = getButtonTopLeft();
  const Math::Vec2<float> size = getButtonSize();
  const float x = static_cast<float>(pos_x);
  const float y = static_cast<float>(pos_y);
  if (x >= top_left.x && x <= top_left.x + size.x && y >= top_left.y &&
      y <= top_left.y + size.y) {
    // Matches the original: the click registers on press, not on
    // release.
    m_button_pressed = true;
    ++m_click_count;
  } else {
    m_button_pressed = false;
  }
}

bool Tutorial15::childOnWindowSizeChanged() {
  if (getVkDevice() == VK_NULL_HANDLE) {
    return true;
  }
  return createResources();
}

void Tutorial15::childClear() {
  m_frames.destroy();
  m_resources.releaseAll();
}

}  // namespace vulkan_graphix
