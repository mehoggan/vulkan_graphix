#include "Tutorial11.h"

#include <cstddef>
#include <cstdint>

#include "vulkan_graphix/SkyboxGeometry.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace vc = VulkanCommon;

Tutorial11::Tutorial11() :
    m_camera(0.6f, 0.4f, 6.0f) {}

Tutorial11::~Tutorial11() { childClear(); }

bool Tutorial11::createResources() {
  if (!m_resources.initialize(*this)) {
    return false;
  }

  // The same 1024x1024 sky texture vulkan_earth's SkyboxFactory.cpp
  // loads as SkyBox.raw, decodable directly via stb_image here.
  if (!m_resources.loadTexture(
          "SkyBox.jpg", VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, m_texture)) {
    Logging::error(LOG_TAG, "Could not create the skybox texture!");
    return false;
  }
  // Host-visible/coherent: rewritten every frame as the orbit camera
  // moves, same tradeoff Tutorial09 makes for the same reason (see
  // draw()).
  if (!m_resources.createUniformBuffer(
          sizeof(Math::Mat4<float>), m_uniform_buffer) ||
      !m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    Logging::error(LOG_TAG, "Could not create uniform buffer!");
    return false;
  }

  // Binding numbers match Tutorial07's shaders exactly (binding 0 =
  // sampler in the fragment stage, binding 1 = uniform buffer in the
  // vertex stage) since this tutorial reuses those compiled shaders as-is.
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

  // Reused byte-for-byte from Tutorial07: its vertex shader does
  // gl_Position = u_ProjectionMatrix * i_Position on a {vec4 position;
  // vec2 texcoord;} vertex, and its fragment shader is a single
  // texture(sampler, uv) lookup - exactly an unlit textured surface,
  // which is all a skybox is. Culling is NONE, not BACK_BIT: the camera
  // can end up either outside or inside this cube (drag the orbit camera
  // in past a face), and both need to render without fighting over the
  // original triangle winding. A real skybox is conventionally rendered
  // this way (or with inverted culling) for exactly this reason.
  if (!m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.11.vert.spv",
           .m_fragment_shader = "shader.11.frag.spv",
           .m_vertex_stride = sizeof(Tutorial11VertexData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial11VertexData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32_SFLOAT,
                 offsetof(Tutorial11VertexData, m_texcoord)}},
           .m_layout = m_pipeline_layout,
           .m_render_pass = render_pass},
          &m_pipeline)) {
    Logging::error(LOG_TAG, "Could not create graphics pipeline!");
    return false;
  }

  // Indexed triangle list (24 unique vertices + 36 indices), unlike
  // Tutorial07's 4-vertex triangle strip - see getVertexData()/
  // getIndexData().
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

Math::Mat4<float> Tutorial11::getUniformBufferData() const {
  // Tutorial07's shaders name this uniform "u_ProjectionMatrix" but just
  // do gl_Position = u_ProjectionMatrix * i_Position - it's really a full
  // model-view-projection slot, which is exactly what's needed here.
  const Math::Mat4<float> model(1.0f);
  const Math::Mat4<float> view = m_camera.view();

  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  const Math::Mat4<float> projection = Tools::getPerspectiveProjectionMatrix(
      width / height, 45.0f, 0.1f, 100.0f);

  return projection * view * model;
}

const std::vector<Tutorial11VertexData>& Tutorial11::getVertexData() const {
  // vulkan_earth's SkyboxFactory faces (SkyboxGeometry, shared with the
  // game and Tutorial21) at a small, sane scale instead of the game's *100
  // world units, as 24 unique indexed vertices instead of six independent
  // GL_QUADS draws. Y is asymmetric on purpose, matching the original: the
  // top is half as far from center as the bottom/sides.
  static const std::vector<Tutorial11VertexData> vertex_data = [] {
    std::vector<Tutorial11VertexData> data;
    for (const SkyboxGeometry::Face& face : SkyboxGeometry::buildFaces(
             Math::Vec3<float>(-2.0f, -2.0f, -2.0f),
             Math::Vec3<float>(2.0f, 1.0f, 2.0f))) {
      for (std::size_t corner = 0; corner < face.m_corners.size(); ++corner) {
        data.push_back(
            {Math::Vec4<float>(face.m_corners[corner], 1.0f),
             face.m_texcoords[corner]});
      }
    }
    return data;
  }();

  return vertex_data;
}

const std::vector<std::uint32_t>& Tutorial11::getIndexData() const {
  // Two triangles per face, matching GL_QUADS' implicit fan triangulation
  // of the same four corners.
  static const std::vector<std::uint32_t> index_data = [] {
    std::vector<std::uint32_t> data;
    for (std::uint32_t face = 0; face < 6; ++face) {
      for (const std::uint32_t corner :
           SkyboxGeometry::c_quad_triangle_indices) {
        data.push_back(face * 4 + corner);
      }
    }
    return data;
  }();

  return index_data;
}

bool Tutorial11::draw() {
  // The uniform buffer is a single, shared, host-visible allocation
  // rewritten every frame from the orbit camera's current state - the
  // same tradeoff Tutorial09 makes for the same reason.
  vkDeviceWaitIdle(getVkDevice());
  if (!m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    return false;
  }

  VkClearValue clear_value = {};
  clear_value.color = {{0.0f, 0.0f, 0.0f, 0.0f}};
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

void Tutorial11::onMouseButton(
    std::int32_t button,
    bool pressed,
    std::int32_t pos_x,
    std::int32_t pos_y) {
  m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial11::onMouseMove(std::int32_t pos_x, std::int32_t pos_y) {
  m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial11::childOnWindowSizeChanged() {
  if (getVkDevice() == VK_NULL_HANDLE) {
    return true;
  }
  return createResources();
}

void Tutorial11::childClear() {
  m_frames.destroy();
  m_resources.releaseAll();
}

}  // namespace vulkan_graphix
