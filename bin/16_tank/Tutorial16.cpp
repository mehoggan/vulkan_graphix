#include "Tutorial16.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

#include "vulkan_graphix/HellfireTank.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace vc = VulkanCommon;

// The assembled tank spans roughly [-210,210]x[-19,120]x[-150,150]
// in world units (TankB's own offsets/scale are hundreds of units,
// unlike every other OrbitCamera tutorial's single/double-digit
// world) - 700 frames the whole thing; OrbitCamera's own zoom-out
// clamp (40, see OrbitCamera.cpp) is far smaller than that, same
// mismatch Tutorial12's terrain already runs into (it sets its own
// initial distance to that same 40 clamp) - scrolling out from here
// will clamp closer than this initial framing, but scrolling in to
// inspect the mesh works over the its full range.
Tutorial16::Tutorial16() :
    m_camera(0.6f, -0.05f, 650.0f) {}

Tutorial16::~Tutorial16() { childClear(); }

bool Tutorial16::createResources() {
  if (!m_resources.initialize(*this)) {
    return false;
  }

  // Shared by all three parts - TankB's own constructor loads
  // "TestImage.raw" for its body/head/turret VBOShaderLibrary instances
  // alike.
  if (!m_resources.loadRawTexture(
          "TestImage.raw",
          1024,
          1024,
          VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
          m_texture)) {
    Logging::error(LOG_TAG, "Could not create the tank texture!");
    return false;
  }
  if (!m_resources.createUniformBuffer(
          sizeof(Tutorial16UniformBufferData), m_uniform_buffer) ||
      !m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    Logging::error(LOG_TAG, "Could not create uniform buffer!");
    return false;
  }

  // shader.16.vert/frag: binding 0 = sampler in the fragment stage,
  // binding 1 = the shared view/projection uniform in the vertex stage.
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
          {set_layout},
          {{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Tutorial16PushConstants)}},
          &m_pipeline_layout)) {
    Logging::error(LOG_TAG, "Could not create the render pass!");
    return false;
  }

  // A non-indexed triangle list - matches VBOShaderLibrary::
  // drawClientData()'s own glDrawArrays(GL_TRIANGLES, ...); none of these
  // .ogl files carry index data. Culling is NONE: these hand-authored
  // assets' winding order hasn't been verified against this project's
  // usual CCW convention, same reasoning Tutorial11/13 use.
  if (!m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.16.vert.spv",
           .m_fragment_shader = "shader.16.frag.spv",
           .m_vertex_stride = sizeof(Tutorial16VertexData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial16VertexData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32_SFLOAT,
                 offsetof(Tutorial16VertexData, m_texcoord)}},
           .m_layout = m_pipeline_layout,
           .m_render_pass = render_pass},
          &m_pipeline)) {
    Logging::error(LOG_TAG, "Could not create graphics pipeline!");
    return false;
  }

  if (!createPart("Hellfire_Body.ogl", m_body) ||
      !createPart("Hellfire_Head.ogl", m_head) ||
      !createPart("Hellfire_Turret.ogl", m_turret)) {
    return false;
  }

  if (!m_frames.create(*this, render_pass)) {
    Logging::error(LOG_TAG, "Could not create the frame resources!");
    return false;
  }
  return true;
}

bool Tutorial16::createPart(const char* mesh_filename, Part& out) {
  // Normal is parsed by loadOglMeshData() (to stay honest about the
  // file's real layout) but dropped, same as VBOShaderLibrary::
  // drawClientData()'s real fragment shader effectively does (see
  // Tutorial13.h for the same reasoning).
  const std::vector<Tutorial16VertexData> vertex_data =
      Tools::loadOglTexturedMesh<Tutorial16VertexData>(mesh_filename);
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

Tutorial16UniformBufferData Tutorial16::getUniformBufferData() const {
  Tutorial16UniformBufferData data{};
  data.m_view = m_camera.view();

  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  // Near/far clip planes sized for this tutorial's much larger world
  // (hundreds of units, see the constructor's own comment) rather than
  // copied from another tutorial's 0.01-10 or 0.1-100 range.
  data.m_projection = Tools::getPerspectiveProjectionMatrix(
      width / height, 45.0f, 1.0f, 2000.0f);

  return data;
}

Math::Mat4<float> Tutorial16::getBodyModelMatrix() const {
  return HellfireTank::buildPartMatrix(
      HellfireTank::getPartTranslations(Math::Vec3<float>(0.0f)).m_body);
}

Math::Mat4<float> Tutorial16::getHeadModelMatrix() const {
  return HellfireTank::buildPartMatrix(
      HellfireTank::getPartTranslations(Math::Vec3<float>(0.0f)).m_head);
}

Math::Mat4<float> Tutorial16::getTurretModelMatrix() const {
  return HellfireTank::buildPartMatrix(
      HellfireTank::getPartTranslations(Math::Vec3<float>(0.0f)).m_turret);
}

bool Tutorial16::draw() {
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
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline_layout,
            0,
            1,
            &m_descriptor_set,
            0,
            nullptr);

        // Each part is bound, given its own model-matrix push constant,
        // and drawn independently - matches Tank::draw()'s own three
        // separate glPushMatrix()/glMultMatrixf()/drawClientData()/
        // glPopMatrix() blocks (see Tutorial16.h's top comment).
        const std::array<std::pair<const Part*, Math::Mat4<float>>, 3> parts =
            {{{&m_body, getBodyModelMatrix()},
              {&m_head, getHeadModelMatrix()},
              {&m_turret, getTurretModelMatrix()}}};
        for (const auto& [part, model] : parts) {
          const Tutorial16PushConstants push_constants{model};
          const VkDeviceSize offset = 0;
          vkCmdBindVertexBuffers(
              command_buffer,
              0,
              1,
              &part->m_vertex_buffer.getVkBuffer(),
              &offset);
          vkCmdPushConstants(
              command_buffer,
              m_pipeline_layout,
              VK_SHADER_STAGE_VERTEX_BIT,
              0,
              sizeof(Tutorial16PushConstants),
              &push_constants);
          vkCmdDraw(command_buffer, part->m_vertex_count, 1, 0, 0);
        }
      });
}

void Tutorial16::onMouseButton(
    std::int32_t button,
    bool pressed,
    std::int32_t pos_x,
    std::int32_t pos_y) {
  m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial16::onMouseMove(std::int32_t pos_x, std::int32_t pos_y) {
  m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial16::childOnWindowSizeChanged() {
  if (getVkDevice() == VK_NULL_HANDLE) {
    return true;
  }
  return createResources();
}

void Tutorial16::childClear() {
  m_frames.destroy();
  m_resources.releaseAll();
}

}  // namespace vulkan_graphix
