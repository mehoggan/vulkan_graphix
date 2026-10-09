#include "Tutorial13.h"

#include <cstddef>
#include <cstdint>

#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace vc = VulkanCommon;

// The mesh itself spans roughly [-0.2, 0.2] on every axis (a real
// tank-shell's own modeling-tool units) - 1.5 is OrbitCamera's own
// minimum distance, close enough to see it clearly.
Tutorial13::Tutorial13() :
    m_camera(0.6f, 0.3f, 1.5f) {}

Tutorial13::~Tutorial13() { childClear(); }

bool Tutorial13::createResources() {
  if (!m_resources.initialize(*this)) {
    return false;
  }

  if (!m_resources.loadRawTexture(
          "projectileDefault.raw",
          512,
          512,
          VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
          m_texture)) {
    Logging::error(LOG_TAG, "Could not create the shell texture!");
    return false;
  }
  if (!m_resources.createUniformBuffer(
          sizeof(Math::Mat4<float>), m_uniform_buffer) ||
      !m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    Logging::error(LOG_TAG, "Could not create uniform buffer!");
    return false;
  }

  // Tutorial07's bindings: 0 = sampler (fragment), 1 = uniform buffer
  // (vertex).
  const std::vector<vc::DescriptorBinding> bindings = {
      {0,
       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       VK_SHADER_STAGE_FRAGMENT_BIT},
      {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}};
  VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
  VkDescriptorPool pool = VK_NULL_HANDLE;
  if (!m_resources.createDescriptorSetLayout(bindings, &set_layout) ||
      !m_resources.createDescriptorPool(bindings, 1, &pool) ||
      !vc::allocateDescriptorSet(
          getVkDevice(), pool, set_layout, &m_descriptor_set)) {
    Logging::error(LOG_TAG, "Could not create the descriptor set!");
    return false;
  }
  vc::writeImageDescriptor(getVkDevice(), m_descriptor_set, 0, m_texture);
  vc::writeUniformBufferDescriptor(
      getVkDevice(), m_descriptor_set, 1, m_uniform_buffer);

  VkRenderPass render_pass = VK_NULL_HANDLE;
  if (!m_resources.createRenderPass(
          {.m_color_format = getSwapchainParameters().getVkFormat()},
          &render_pass) ||
      !m_resources.createPipelineLayout(
          {set_layout}, {}, &m_pipeline_layout)) {
    Logging::error(LOG_TAG, "Could not create the render pass!");
    return false;
  }

  // Reused byte-for-byte from Tutorial07: an unlit textured surface on a
  // {position, texcoord} vertex is exactly what this mesh needs too. A
  // non-indexed triangle list (792 vertices, 264 triangles) - matches
  // VBOShaderLibrary::drawClientData()'s own glDrawArrays(GL_TRIANGLES,
  // ...); the .ogl file has no index data at all. Culling is NONE: this
  // hand-authored asset's winding order hasn't been verified against this
  // project's usual CCW convention, and the orbit camera can end up on
  // any side - same reasoning Tutorial11's skybox uses.
  if (!m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.13.vert.spv",
           .m_fragment_shader = "shader.13.frag.spv",
           .m_vertex_stride = sizeof(Tutorial13VertexData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial13VertexData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32_SFLOAT,
                 offsetof(Tutorial13VertexData, m_texcoord)}},
           .m_layout = m_pipeline_layout,
           .m_render_pass = render_pass},
          &m_pipeline)) {
    Logging::error(LOG_TAG, "Could not create graphics pipeline!");
    return false;
  }

  // The real tank-shell mesh. Normal is parsed by loadOglMeshData() (to
  // stay honest about the file's real layout) but dropped, same as
  // VBOShaderLibrary::drawClientData()'s real fragment shader effectively
  // does (see Tutorial13.h).
  if (m_vertex_data.empty()) {
    m_vertex_data = Tools::loadOglTexturedMesh<Tutorial13VertexData>(
        "projectileDefault.ogl");
  }
  if (m_vertex_data.empty() ||
      !m_resources.createDeviceLocalBuffer(
          m_vertex_data, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_vertex_buffer)) {
    Logging::error(LOG_TAG, "Could not create the vertex buffer!");
    return false;
  }

  if (!m_frames.create(*this, render_pass)) {
    Logging::error(LOG_TAG, "Could not create the frame resources!");
    return false;
  }
  return true;
}

Math::Mat4<float> Tutorial13::getUniformBufferData() const {
  // Reused Tutorial07 shaders name this uniform "u_ProjectionMatrix" but
  // just do gl_Position = u_ProjectionMatrix * i_Position - really a full
  // model-view-projection slot, exactly what's needed here.
  const Math::Mat4<float> model(1.0f);
  const Math::Mat4<float> view = m_camera.view();

  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  const Math::Mat4<float> projection = Tools::getPerspectiveProjectionMatrix(
      width / height, 45.0f, 0.01f, 10.0f);

  return projection * view * model;
}

bool Tutorial13::draw() {
  vkDeviceWaitIdle(getVkDevice());
  if (!m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    return false;
  }

  VkClearValue clear_value = {};
  clear_value.color = {{0.15f, 0.15f, 0.18f, 1.0f}};
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
        vkCmdDraw(
            command_buffer,
            static_cast<std::uint32_t>(m_vertex_data.size()),
            1,
            0,
            0);
      });
}

void Tutorial13::onMouseButton(
    std::int32_t button,
    bool pressed,
    std::int32_t pos_x,
    std::int32_t pos_y) {
  m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial13::onMouseMove(std::int32_t pos_x, std::int32_t pos_y) {
  m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial13::childOnWindowSizeChanged() {
  if (getVkDevice() == VK_NULL_HANDLE) {
    return true;
  }
  return createResources();
}

void Tutorial13::childClear() {
  m_frames.destroy();
  m_resources.releaseAll();
}

}  // namespace vulkan_graphix
