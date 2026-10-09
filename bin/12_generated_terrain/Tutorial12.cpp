#include "Tutorial12.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ctime>

#include "vulkan_graphix/TerrainGenerator.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
constexpr VkFormat c_depth_format = VK_FORMAT_D32_SFLOAT;
}  // namespace

namespace vc = VulkanCommon;

// Pulled back to OrbitCamera's max distance (40) and pitched down
// moderately, showing the grid's full silhouette with sky around it.
Tutorial12::Tutorial12() :
    m_camera(0.5f, 0.35f, 40.0f) {
  std::srand(static_cast<std::uint32_t>(std::time(nullptr)));
}

Tutorial12::~Tutorial12() { childClear(); }

bool Tutorial12::createResources() {
  if (!m_resources.initialize(*this)) {
    return false;
  }

  // REPEAT, like Tutorial09's ground texture: texcoords tile across the
  // terrain rather than being clamped into one stretched copy.
  if (!m_resources.loadRawTexture(
          "Rocky.raw",
          2048,
          2048,
          VK_SAMPLER_ADDRESS_MODE_REPEAT,
          m_texture)) {
    Logging::error(LOG_TAG, "Could not create the terrain texture!");
    return false;
  }
  if (!m_resources.createUniformBuffer(
          sizeof(Tutorial12UniformBufferData), m_uniform_buffer) ||
      !m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    Logging::error(LOG_TAG, "Could not create uniform buffer!");
    return false;
  }

  // Binding numbers match Tutorial09's shaders exactly (binding 0 =
  // uniform buffer in vertex+fragment, binding 1 = sampler in the
  // fragment stage) since this tutorial reuses those compiled shaders.
  const std::vector<vc::DescriptorBinding> bindings = {
      {0,
       VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT},
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
  vc::writeImageDescriptor(getVkDevice(), m_descriptor_set, 1, m_texture);

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
          {set_layout}, {}, &m_pipeline_layout)) {
    Logging::error(LOG_TAG, "Could not create the render pass!");
    return false;
  }

  // Reused byte-for-byte from Tutorial09: a Phong-lit, textured, depth-
  // tested surface is exactly what this generated terrain needs, and its
  // vertex layout (position/normal/texcoord) matches Tutorial12VertexData
  // exactly. A non-indexed triangle list, six vertices per grid quad -
  // matches TerrainMaker::prepareData()'s own glDrawArrays(GL_TRIANGLES,
  // ...) layout (see getVertexData()) rather than Tutorial09's indexed
  // one. Culling is NONE: the winding of the ported v_i..v_z quad pattern
  // hasn't been verified against this project's usual CCW convention, and
  // the orbit camera can end up on either side of the grid - same
  // reasoning the skybox tutorial (11) uses for its own cull mode.
  if (!m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.12.vert.spv",
           .m_fragment_shader = "shader.12.frag.spv",
           .m_vertex_stride = sizeof(Tutorial12VertexData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial12VertexData, m_position)},
                {1,
                 0,
                 VK_FORMAT_R32G32B32_SFLOAT,
                 offsetof(Tutorial12VertexData, m_normal)},
                {2,
                 0,
                 VK_FORMAT_R32G32_SFLOAT,
                 offsetof(Tutorial12VertexData, m_texcoord)}},
           .m_depth = vc::DepthState{},
           .m_layout = m_pipeline_layout,
           .m_render_pass = render_pass},
          &m_pipeline)) {
    Logging::error(LOG_TAG, "Could not create graphics pipeline!");
    return false;
  }

  if (!m_resources.createDeviceLocalBuffer(
          getVertexData(),
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          m_vertex_buffer)) {
    Logging::error(LOG_TAG, "Could not create the vertex buffer!");
    return false;
  }

  if (!m_frames.create(*this, render_pass, m_depth_image.getVkImageView())) {
    Logging::error(LOG_TAG, "Could not create the frame resources!");
    return false;
  }
  return true;
}

Tutorial12UniformBufferData Tutorial12::getUniformBufferData() const {
  Tutorial12UniformBufferData data{};
  data.m_model = Math::Mat4<float>(1.0f);  // static terrain, no rotation
  data.m_view = m_camera.view();

  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  data.m_projection = Tools::getPerspectiveProjectionMatrix(
      width / height, 45.0f, 0.1f, 100.0f);

  data.m_light_position = Math::Vec4<float>(15.0f, 25.0f, 15.0f, 1.0f);
  data.m_light_color = Math::Vec4<float>(1.0f, 1.0f, 1.0f, 1.0f);
  data.m_view_position = Math::Vec4<float>(m_camera.eye(), 1.0f);

  return data;
}

const std::vector<Tutorial12VertexData>& Tutorial12::getVertexData() {
  if (!m_vertex_data.empty()) {
    return m_vertex_data;
  }

  TerrainGenerator generator(c_grid_size, c_grid_scale);
  generator.generate(
      c_gen_steps,
      c_gen_increase,
      c_gen_radius,
      c_gen_random_jump,
      c_smoothing_passes);

  const std::int32_t chunk_size = c_grid_size / 2;
  const float chunk_span = static_cast<float>(chunk_size - 1);
  const float half_extent =
      static_cast<float>(c_grid_size - 1) * c_grid_scale / 2.0f;

  m_vertex_data.reserve(
      static_cast<std::size_t>(c_grid_size - 1) *
      static_cast<std::size_t>(c_grid_size - 1) * 6);

  for (std::int32_t i = 0; i < c_grid_size - 1; ++i) {
    for (std::int32_t j = 0; j < c_grid_size - 1; ++j) {
      auto make_position = [&](std::int32_t grid_x, std::int32_t grid_z) {
        return Math::Vec4<float>(
            static_cast<float>(grid_x * c_grid_scale) - half_extent,
            static_cast<float>(generator.heightAt(grid_x, grid_z)),
            static_cast<float>(grid_z * c_grid_scale) - half_extent,
            1.0f);
      };

      // V_I
      Math::Vec2<float> t_i(
          static_cast<float>(i % (chunk_size - 1)) / chunk_span,
          static_cast<float>(j % (chunk_size - 1)) / chunk_span);
      Math::Vec3<float> n_i(0.0f, 0.0f, 0.0f);
      if (i != 0 && j != 0) {
        n_i = generator.normalAt(j, i);
      }
      m_vertex_data.push_back({make_position(j, i), n_i, t_i});

      // V_J
      Math::Vec2<float> t_j(
          (static_cast<float>(i % (chunk_size - 1)) + 1) / chunk_span,
          static_cast<float>(j % (chunk_size - 1)) / chunk_span);
      Math::Vec3<float> n_j(0.0f, 0.0f, 0.0f);
      if (j != 0 && i != c_grid_size - 2) {
        n_j = generator.normalAt(j, i + 1);
      }
      m_vertex_data.push_back({make_position(j, i + 1), n_j, t_j});

      // V_K
      Math::Vec2<float> t_k(
          static_cast<float>(i % (chunk_size - 1)) / chunk_span,
          (static_cast<float>(j % (chunk_size - 1)) + 1) / chunk_span);
      Math::Vec3<float> n_k(0.0f, 0.0f, 0.0f);
      if (i != 0 && j != c_grid_size - 2) {
        n_k = generator.normalAt(j + 1, i);
      }
      m_vertex_data.push_back({make_position(j + 1, i), n_k, t_k});

      // V_X (same position/UV pattern as V_J)
      m_vertex_data.push_back({make_position(j, i + 1), n_j, t_j});

      // V_Y
      Math::Vec2<float> t_y(
          (static_cast<float>(i % (chunk_size - 1)) + 1) / chunk_span,
          (static_cast<float>(j % (chunk_size - 1)) + 1) / chunk_span);
      Math::Vec3<float> n_y(0.0f, 0.0f, 0.0f);
      if (i != c_grid_size - 2 && j != c_grid_size - 2) {
        n_y = generator.normalAt(j + 1, i + 1);
      }
      m_vertex_data.push_back({make_position(j + 1, i + 1), n_y, t_y});

      // V_Z (same position/UV pattern as V_K)
      m_vertex_data.push_back({make_position(j + 1, i), n_k, t_k});
    }
  }

  return m_vertex_data;
}

bool Tutorial12::draw() {
  vkDeviceWaitIdle(getVkDevice());
  if (!m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    return false;
  }

  std::vector<VkClearValue> clear_values(2);
  clear_values[0].color = {{0.4f, 0.55f, 0.8f, 1.0f}};
  clear_values[1].depthStencil = {1.0f, 0};
  return m_frames.draw(
      *this, clear_values, [this](VkCommandBuffer command_buffer) {
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

void Tutorial12::onMouseButton(
    std::int32_t button,
    bool pressed,
    std::int32_t pos_x,
    std::int32_t pos_y) {
  m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial12::onMouseMove(std::int32_t pos_x, std::int32_t pos_y) {
  m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial12::childOnWindowSizeChanged() {
  if (getVkDevice() == VK_NULL_HANDLE) {
    return true;
  }
  return createResources();
}

void Tutorial12::childClear() {
  m_frames.destroy();
  m_resources.releaseAll();
}

}  // namespace vulkan_graphix
