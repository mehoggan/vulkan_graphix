#include "Tutorial14.h"

#include <cstddef>
#include <cstdint>

#include "vulkan_graphix/Math/Sphere.hpp"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace vc = VulkanCommon;

// The sphere itself has radius 1.0 (see getVertexData()); 3.5 gives
// a clear view of it and its translucency against the clear color.
Tutorial14::Tutorial14() :
    m_camera(0.6f, 0.3f, 3.5f) {}

Tutorial14::~Tutorial14() { childClear(); }

bool Tutorial14::createResources() {
  if (!m_resources.initialize(*this)) {
    return false;
  }

  if (!m_resources.createUniformBuffer(
          sizeof(Tutorial14UniformBufferData), m_uniform_buffer) ||
      !m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    Logging::error(LOG_TAG, "Could not create uniform buffer!");
    return false;
  }

  // Tutorial08's one binding: the uniform buffer, read by both stages.
  const std::vector<vc::DescriptorBinding> bindings = {
      {0,
       VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT}};
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

  VkRenderPass render_pass = VK_NULL_HANDLE;
  if (!m_resources.createRenderPass(
          {.m_color_format = getSwapchainParameters().getVkFormat()},
          &render_pass) ||
      !m_resources.createPipelineLayout(
          {set_layout}, {}, &m_pipeline_layout)) {
    Logging::error(LOG_TAG, "Could not create the render pass!");
    return false;
  }

  // Vertex shader reused verbatim from Tutorial08 (position/normal ->
  // world position/normal, same UBO shape); fragment shader is new
  // (resources/14/Data/shader.14.frag) since Tutorial08's own hardcodes
  // an opaque, non-alpha-blended color - see Tutorial14.h. BACK_BIT/
  // CLOCKWISE, not this project's usual CCW convention - Math::Sphere/
  // Icosahedron wind their triangles clockwise as seen from outside;
  // Tutorial08 uses the exact same pairing for the exact same reason.
  // The only one of the four migration tutorials that needs blending -
  // matches Particle::render()'s own glColor4f(r, g, b, 0.4)
  // translucency; no depth attachment (see Tutorial14.h - back-face
  // culling alone is enough for a single convex icosphere, so there's no
  // self-occlusion to resolve).
  if (!m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.14.vert.spv",
           .m_fragment_shader = "shader.14.frag.spv",
           .m_vertex_stride = sizeof(Tutorial14VertexData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial14VertexData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32B32_SFLOAT,
                 offsetof(Tutorial14VertexData, m_normal)}},
           .m_cull_mode = VK_CULL_MODE_BACK_BIT,
           .m_front_face = VK_FRONT_FACE_CLOCKWISE,
           .m_blend = vc::alphaBlend(),
           .m_layout = m_pipeline_layout,
           .m_render_pass = render_pass},
          &m_pipeline)) {
    Logging::error(LOG_TAG, "Could not create graphics pipeline!");
    return false;
  }

  m_index_count = static_cast<std::uint32_t>(getIndexData().size());
  if (!m_resources.createDeviceLocalBuffer(
          getVertexData(),
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          m_vertex_buffer) ||
      !m_resources.createDeviceLocalBuffer(
          getIndexData(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_index_buffer)) {
    Logging::error(LOG_TAG, "Could not create the vertex/index buffers!");
    return false;
  }

  if (!m_frames.create(*this, render_pass)) {
    Logging::error(LOG_TAG, "Could not create the frame resources!");
    return false;
  }
  return true;
}

Tutorial14UniformBufferData Tutorial14::getUniformBufferData() const {
  Tutorial14UniformBufferData data{};
  data.m_model = Math::Mat4<float>(1.0f);  // static particle, no rotation
  data.m_view = m_camera.view();

  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  data.m_projection = Tools::getPerspectiveProjectionMatrix(
      width / height, 45.0f, 0.1f, 100.0f);

  data.m_light_position = Math::Vec4<float>(5.0f, 8.0f, 5.0f, 1.0f);
  data.m_light_color = Math::Vec4<float>(1.0f, 1.0f, 1.0f, 1.0f);
  data.m_view_position = Math::Vec4<float>(m_camera.eye(), 1.0f);

  return data;
}

const std::vector<Tutorial14VertexData>& Tutorial14::getVertexData() const {
  // Same Math::Sphere usage as Tutorial08::getVertexData() - an icosphere
  // (radius 1.0, subdivision level 3), no texcoords since the original
  // glutSolidSphere() draw is untextured too.
  static const std::vector<Tutorial14VertexData> vertex_data = [] {
    const Math::Sphere<float, std::uint32_t> sphere(
        1.0f, static_cast<std::uint8_t>(3));

    std::vector<Tutorial14VertexData> data;
    data.reserve(sphere.points().size());
    for (std::size_t i = 0; i < sphere.points().size(); ++i) {
      const Math::Vec3<float>& point = sphere.points()[i];
      const Math::Vec3<float>& normal = sphere.normals()[i];
      data.push_back({Math::Vec4<float>(point, 1.0f), normal});
    }
    return data;
  }();

  return vertex_data;
}

const std::vector<std::uint32_t>& Tutorial14::getIndexData() const {
  static const std::vector<std::uint32_t> index_data = [] {
    const Math::Sphere<float, std::uint32_t> sphere(
        1.0f, static_cast<std::uint8_t>(3));
    return sphere.indices();
  }();
  return index_data;
}

bool Tutorial14::draw() {
  vkDeviceWaitIdle(getVkDevice());
  if (!m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    return false;
  }

  VkClearValue clear_value = {};
  clear_value.color = {{0.35f, 0.35f, 0.4f, 1.0f}};
  return m_frames.draw(
      *this, {clear_value}, [this](VkCommandBuffer command_buffer) {
        vkCmdBindPipeline(
            command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
        const VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(
            command_buffer, 0, 1, &m_vertex_buffer.getVkBuffer(), &offset);
        vkCmdBindIndexBuffer(
            command_buffer,
            m_index_buffer.getVkBuffer(),
            0,
            VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline_layout,
            0,
            1,
            &m_descriptor_set,
            0,
            nullptr);
        vkCmdDrawIndexed(command_buffer, m_index_count, 1, 0, 0, 0);
      });
}

void Tutorial14::onMouseButton(
    std::int32_t button,
    bool pressed,
    std::int32_t pos_x,
    std::int32_t pos_y) {
  m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial14::onMouseMove(std::int32_t pos_x, std::int32_t pos_y) {
  m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial14::childOnWindowSizeChanged() {
  if (getVkDevice() == VK_NULL_HANDLE) {
    return true;
  }
  return createResources();
}

void Tutorial14::childClear() {
  m_frames.destroy();
  m_resources.releaseAll();
}

}  // namespace vulkan_graphix
