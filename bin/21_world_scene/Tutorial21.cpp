#include "Tutorial21.h"

#include <array>
#include <cstddef>
#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/HellfireTank.h"
#include "vulkan_graphix/SkyboxGeometry.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
constexpr VkFormat c_depth_format = VK_FORMAT_D32_SFLOAT;
}  // namespace

namespace vc = VulkanCommon;

// Terrain half-extent = (64-1)*16/2 = ~504 world units; the tank
// (hundreds of units tall/wide) plus the terrain together need a
// much larger view than any single previous tutorial's world -
// tuned via screenshot. A steep pitch (looking down onto the
// terrain rather than across it) is deliberate: TerrainGenerator::
// terrainSmoothe() zeroes the outermost ring of heights every pass
// (a real, faithfully-ported part of the original algorithm), which
// creates a real one-cell-wide cliff at the grid's edge - a shallow
// pitch grazes under that cliff and exposes its unlit backside.
Tutorial21::Tutorial21() :
    m_camera(0.5f, 0.85f, 1800.0f),
    m_terrain_generator(c_grid_size, c_grid_scale),
    m_terrain_generated(false) {}

Tutorial21::~Tutorial21() { childClear(); }

bool Tutorial21::createResources() {
  if (!m_resources.initialize(*this)) {
    return false;
  }

  // REPEAT for the ground, like Tutorial12's own: texcoords tile across
  // the terrain rather than being clamped into one stretched copy.
  if (!m_resources.loadRawTexture(
          "Rocky.raw",
          2048,
          2048,
          VK_SAMPLER_ADDRESS_MODE_REPEAT,
          m_terrain_texture) ||
      !m_resources.loadRawTexture(
          "TestImage.raw",
          1024,
          1024,
          VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
          m_tank_texture) ||
      !m_resources.loadTexture(
          "SkyBox.jpg",
          VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
          m_skybox_texture)) {
    Logging::error(LOG_TAG, "Could not create the textures!");
    return false;
  }
  if (!m_resources.createUniformBuffer(
          sizeof(Tutorial21TerrainUniformBufferData),
          m_terrain_uniform_buffer) ||
      !m_resources.writeBuffer(
          m_terrain_uniform_buffer, getTerrainUniformBufferData()) ||
      !m_resources.createUniformBuffer(
          sizeof(Tutorial21ObjectUniformBufferData),
          m_object_uniform_buffer) ||
      !m_resources.writeBuffer(
          m_object_uniform_buffer, getObjectUniformBufferData())) {
    Logging::error(LOG_TAG, "Could not create the uniform buffers!");
    return false;
  }

  // Terrain layout matches shader.21_terrain's own bindings exactly: 0 =
  // UBO (both stages reference it), 1 = sampler (fragment only). Object
  // layout matches Tutorial16's shaders exactly (0 = sampler in the
  // fragment stage, 1 = UBO in the vertex stage) - shared by both the tank
  // and the skybox.
  const std::vector<vc::DescriptorBinding> terrain_bindings = {
      {0,
       VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT},
      {1,
       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       VK_SHADER_STAGE_FRAGMENT_BIT}};
  const std::vector<vc::DescriptorBinding> object_bindings = {
      {0,
       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       VK_SHADER_STAGE_FRAGMENT_BIT},
      {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}};
  VkDescriptorSetLayout terrain_layout = VK_NULL_HANDLE;
  VkDescriptorSetLayout object_layout = VK_NULL_HANDLE;
  VkDescriptorPool pool = VK_NULL_HANDLE;
  if (!m_resources.createDescriptorSetLayout(
          terrain_bindings, &terrain_layout) ||
      !m_resources.createDescriptorSetLayout(
          object_bindings, &object_layout) ||
      !m_resources.createDescriptorPool(terrain_bindings, 3, &pool) ||
      !vc::allocateDescriptorSet(
          getVkDevice(), pool, terrain_layout, &m_terrain_descriptor_set) ||
      !vc::allocateDescriptorSet(
          getVkDevice(), pool, object_layout, &m_tank_descriptor_set) ||
      !vc::allocateDescriptorSet(
          getVkDevice(), pool, object_layout, &m_skybox_descriptor_set)) {
    Logging::error(LOG_TAG, "Could not create the descriptor sets!");
    return false;
  }
  vc::writeUniformBufferDescriptor(
      getVkDevice(), m_terrain_descriptor_set, 0, m_terrain_uniform_buffer);
  vc::writeImageDescriptor(
      getVkDevice(), m_terrain_descriptor_set, 1, m_terrain_texture);
  vc::writeImageDescriptor(
      getVkDevice(), m_tank_descriptor_set, 0, m_tank_texture);
  vc::writeUniformBufferDescriptor(
      getVkDevice(), m_tank_descriptor_set, 1, m_object_uniform_buffer);
  vc::writeImageDescriptor(
      getVkDevice(), m_skybox_descriptor_set, 0, m_skybox_texture);
  vc::writeUniformBufferDescriptor(
      getVkDevice(), m_skybox_descriptor_set, 1, m_object_uniform_buffer);

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
          {terrain_layout}, {}, &m_terrain_pipeline_layout) ||
      !m_resources.createPipelineLayout(
          {object_layout},
          {{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Tutorial21PushConstants)}},
          &m_object_pipeline_layout)) {
    Logging::error(LOG_TAG, "Could not create the render pass!");
    return false;
  }

  // Terrain pipeline: Phong-lit, depth-tested. Culling is NONE - same
  // reasoning Tutorial12 uses for its own cull mode.
  const vc::GraphicsPipelineDescription terrain = {
      .m_vertex_shader = "shader.21_terrain.vert.spv",
      .m_fragment_shader = "shader.21_terrain.frag.spv",
      .m_vertex_stride = sizeof(Tutorial21TerrainVertexData),
      .m_vertex_attributes =
          {{0,
            0,
            VK_FORMAT_R32G32B32A32_SFLOAT,
            offsetof(Tutorial21TerrainVertexData, m_position)},
           {1,
            0,
            VK_FORMAT_R32G32B32_SFLOAT,
            offsetof(Tutorial21TerrainVertexData, m_normal)},
           {2,
            0,
            VK_FORMAT_R32G32_SFLOAT,
            offsetof(Tutorial21TerrainVertexData, m_texcoord)}},
      .m_depth = vc::DepthState{},
      .m_layout = m_terrain_pipeline_layout,
      .m_render_pass = render_pass};
  // Object pipeline: unlit, textured - the tank, with a real depth
  // test/write against the terrain drawn just before it.
  vc::GraphicsPipelineDescription object = {
      .m_vertex_shader = "shader.21_object.vert.spv",
      .m_fragment_shader = "shader.21_object.frag.spv",
      .m_vertex_stride = sizeof(Tutorial21ObjectVertexData),
      .m_vertex_attributes =
          {{0,
            0,
            VK_FORMAT_R32G32B32A32_SFLOAT,
            offsetof(Tutorial21ObjectVertexData, m_position)},
           {1,
            0,
            VK_FORMAT_R32G32_SFLOAT,
            offsetof(Tutorial21ObjectVertexData, m_texcoord)}},
      .m_depth = vc::DepthState{},
      .m_layout = m_object_pipeline_layout,
      .m_render_pass = render_pass};
  if (!m_resources.createGraphicsPipeline(terrain, &m_terrain_pipeline) ||
      !m_resources.createGraphicsPipeline(object, &m_object_pipeline)) {
    Logging::error(LOG_TAG, "Could not create the pipelines!");
    return false;
  }

  // --- Skybox pipeline (same shaders/layout/vertex format as the tank's
  // object pipeline, but with depth testing off entirely) ---
  //
  // The skybox is drawn first, against a freshly-cleared depth buffer
  // (cleared to the far value, 1.0). Real depth testing here is both
  // unnecessary (nothing has been drawn yet to occlude against) and
  // actively harmful: at c_skybox_half_extent (3000 world units) against
  // this scene's far plane (6000), the skybox cube's projected depth
  // for some fragments rounds to exactly 1.0 at floating-point
  // precision limits, which fails a strict "less than" test against
  // the 1.0-cleared buffer and leaves the (near-black) clear color
  // showing through as a visible hole - confirmed via screenshot
  // (a dark wedge, unaffected by changing the terrain's own height-
  // generation constants, ruling out a terrain cause). Disabling the
  // skybox's own depth test/write entirely avoids the precision edge
  // case altogether, while the tank's own pipeline (just above) keeps
  // real depth test/write for correct terrain/tank compositing.
  object.m_depth = vc::DepthState{false, false, VK_COMPARE_OP_ALWAYS};
  if (!m_resources.createGraphicsPipeline(object, &m_skybox_pipeline)) {
    Logging::error(LOG_TAG, "Could not create the skybox pipeline!");
    return false;
  }

  m_skybox_index_count =
      static_cast<std::uint32_t>(getSkyboxIndexData().size());
  if (!m_resources.createDeviceLocalBuffer(
          getTerrainVertexData(),
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          m_terrain_vertex_buffer) ||
      !m_resources.createDeviceLocalBuffer(
          getSkyboxVertexData(),
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          m_skybox_vertex_buffer) ||
      !m_resources.createDeviceLocalBuffer(
          getSkyboxIndexData(),
          VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
          m_skybox_index_buffer)) {
    Logging::error(LOG_TAG, "Could not create the terrain/skybox buffers!");
    return false;
  }
  const std::array<const char*, c_tank_part_count> tank_files = {
      {"Hellfire_Body.ogl", "Hellfire_Head.ogl", "Hellfire_Turret.ogl"}};
  for (std::size_t i = 0; i < c_tank_part_count; ++i) {
    const std::vector<Tutorial21ObjectVertexData> vertex_data =
        Tools::loadOglTexturedMesh<Tutorial21ObjectVertexData>(tank_files[i]);
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

  if (!m_frames.create(*this, render_pass, m_depth_image.getVkImageView())) {
    Logging::error(LOG_TAG, "Could not create the frame resources!");
    return false;
  }
  return true;
}

Tutorial21TerrainUniformBufferData Tutorial21::getTerrainUniformBufferData()
    const {
  Tutorial21TerrainUniformBufferData data{};
  data.m_model = Math::Mat4<float>(1.0f);  // static terrain, no rotation
  data.m_view = m_camera.view();

  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  // Far plane needs headroom past the skybox's own farthest corner as
  // seen from the camera (c_skybox_half_extent*sqrt(3) + the camera's own
  // distance from the origin, worst case ~6996 world units here) - too
  // tight a far plane clips a skybox corner, which produced a real,
  // screenshot-confirmed solid-black triangle on one face.
  data.m_projection = Tools::getPerspectiveProjectionMatrix(
      width / height, 45.0f, 1.0f, 8000.0f);

  // Same real light Tutorial12 uses, just repositioned/rescaled for
  // this tutorial's much larger terrain.
  data.m_light_position = Math::Vec4<float>(400.0f, 600.0f, 400.0f, 1.0f);
  data.m_light_color = Math::Vec4<float>(1.0f, 1.0f, 1.0f, 1.0f);
  data.m_view_position = Math::Vec4<float>(m_camera.eye(), 1.0f);

  return data;
}

Tutorial21ObjectUniformBufferData Tutorial21::getObjectUniformBufferData()
    const {
  Tutorial21ObjectUniformBufferData data{};
  data.m_view = m_camera.view();

  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  data.m_projection = Tools::getPerspectiveProjectionMatrix(
      width / height, 45.0f, 1.0f, 8000.0f);

  return data;
}

void Tutorial21::ensureTerrainGenerated() {
  if (m_terrain_generated) {
    return;
  }
  m_terrain_generator.generate(
      c_gen_steps,
      c_gen_increase,
      c_gen_radius,
      c_gen_random_jump,
      c_smoothing_passes);
  m_terrain_generated = true;
}

const std::vector<Tutorial21TerrainVertexData>&
Tutorial21::getTerrainVertexData() {
  if (!m_terrain_vertex_data.empty()) {
    return m_terrain_vertex_data;
  }
  ensureTerrainGenerated();

  // Same six-vertices-per-cell construction as Tutorial12::
  // getVertexData() (see that tutorial's own comments for the
  // texcoord-tiling chunk_size trick) - just parameterized by this
  // tutorial's own (larger) grid constants. Unlike Tutorial12, this
  // calls TerrainGenerator::normalAt() unconditionally at every grid
  // cell instead of leaving a hard (0,0,0) normal at the i==0/j==0
  // boundary: normalAt()'s own calcNormal() helper already falls back
  // to a safe (0,1,0) for any out-of-range neighbor it needs, so the
  // extra zero-vector guard only ever replaces that safe fallback with
  // a genuinely degenerate normal - normalize((0,0,0)) is NaN, which
  // propagates through the Phong lighting math into a solid-black
  // fragment. That produced a real, screenshot-confirmed black wedge
  // along the grid's near edge (reproducible regardless of the actual
  // generated height values, confirming it was this normal bug and not
  // a height-generation or skybox depth issue).
  const std::int32_t chunk_size = c_grid_size / 2;
  const float chunk_span = static_cast<float>(chunk_size - 1);
  const float half_extent =
      static_cast<float>(c_grid_size - 1) * c_grid_scale / 2.0f;

  m_terrain_vertex_data.reserve(
      static_cast<std::size_t>(c_grid_size - 1) *
      static_cast<std::size_t>(c_grid_size - 1) * 6);

  for (std::int32_t i = 0; i < c_grid_size - 1; ++i) {
    for (std::int32_t j = 0; j < c_grid_size - 1; ++j) {
      auto make_position = [&](std::int32_t grid_x, std::int32_t grid_z) {
        return Math::Vec4<float>(
            static_cast<float>(grid_x * c_grid_scale) - half_extent,
            static_cast<float>(m_terrain_generator.heightAt(grid_x, grid_z)),
            static_cast<float>(grid_z * c_grid_scale) - half_extent,
            1.0f);
      };

      Math::Vec2<float> t_i(
          static_cast<float>(i % (chunk_size - 1)) / chunk_span,
          static_cast<float>(j % (chunk_size - 1)) / chunk_span);
      const Math::Vec3<float> n_i = m_terrain_generator.normalAt(j, i);
      m_terrain_vertex_data.push_back({make_position(j, i), n_i, t_i});

      Math::Vec2<float> t_j(
          (static_cast<float>(i % (chunk_size - 1)) + 1) / chunk_span,
          static_cast<float>(j % (chunk_size - 1)) / chunk_span);
      const Math::Vec3<float> n_j = m_terrain_generator.normalAt(j, i + 1);
      m_terrain_vertex_data.push_back({make_position(j, i + 1), n_j, t_j});

      Math::Vec2<float> t_k(
          static_cast<float>(i % (chunk_size - 1)) / chunk_span,
          (static_cast<float>(j % (chunk_size - 1)) + 1) / chunk_span);
      const Math::Vec3<float> n_k = m_terrain_generator.normalAt(j + 1, i);
      m_terrain_vertex_data.push_back({make_position(j + 1, i), n_k, t_k});

      m_terrain_vertex_data.push_back({make_position(j, i + 1), n_j, t_j});

      Math::Vec2<float> t_y(
          (static_cast<float>(i % (chunk_size - 1)) + 1) / chunk_span,
          (static_cast<float>(j % (chunk_size - 1)) + 1) / chunk_span);
      const Math::Vec3<float> n_y = m_terrain_generator.normalAt(j + 1, i + 1);
      m_terrain_vertex_data.push_back({make_position(j + 1, i + 1), n_y, t_y});

      m_terrain_vertex_data.push_back({make_position(j + 1, i), n_k, t_k});
    }
  }

  return m_terrain_vertex_data;
}

float Tutorial21::getTankGroundHeight() {
  ensureTerrainGenerated();
  return static_cast<float>(
      m_terrain_generator.heightAt(c_grid_size / 2, c_grid_size / 2));
}

Math::Mat4<float> Tutorial21::getTankPartModelMatrix(
    const Math::Vec3<float>& part_translation) const {
  return HellfireTank::buildPartMatrix(part_translation);
}

const std::vector<Tutorial21ObjectVertexData>&
Tutorial21::getSkyboxVertexData() const {
  // vulkan_earth's SkyboxFactory faces (SkyboxGeometry, shared with the
  // game and Tutorial11) at this tutorial's c_skybox_half_extent scale;
  // the top sits at half height, as in the original.
  static const std::vector<Tutorial21ObjectVertexData> vertex_data = [] {
    std::vector<Tutorial21ObjectVertexData> data;
    for (const SkyboxGeometry::Face& face : SkyboxGeometry::buildFaces(
             Math::Vec3<float>(-c_skybox_half_extent),
             Math::Vec3<float>(
                 c_skybox_half_extent,
                 c_skybox_half_extent * 0.5f,
                 c_skybox_half_extent))) {
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

const std::vector<std::uint32_t>& Tutorial21::getSkyboxIndexData() const {
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

bool Tutorial21::draw() {
  vkDeviceWaitIdle(getVkDevice());
  if (!m_resources.writeBuffer(
          m_terrain_uniform_buffer, getTerrainUniformBufferData()) ||
      !m_resources.writeBuffer(
          m_object_uniform_buffer, getObjectUniformBufferData())) {
    return false;
  }

  std::vector<VkClearValue> clear_values(2);
  clear_values[0].color = {{0.05f, 0.05f, 0.08f, 1.0f}};
  clear_values[1].depthStencil = {1.0f, 0};
  return m_frames.draw(
      *this, clear_values, [this](VkCommandBuffer command_buffer) {
        const VkDeviceSize zero_offset = 0;

        // --- Pass 1: skybox first, its own pipeline with depth testing
        // off entirely (see createResources() for why).
        vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_skybox_pipeline);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_object_pipeline_layout,
            0,
            1,
            &m_skybox_descriptor_set,
            0,
            nullptr);
        vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_skybox_vertex_buffer.getVkBuffer(),
            &zero_offset);
        vkCmdBindIndexBuffer(
            command_buffer,
            m_skybox_index_buffer.getVkBuffer(),
            0,
            VK_INDEX_TYPE_UINT32);
        const Tutorial21PushConstants skybox_push_constants{
            Math::Mat4<float>(1.0f)};
        vkCmdPushConstants(
            command_buffer,
            m_object_pipeline_layout,
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(Tutorial21PushConstants),
            &skybox_push_constants);
        vkCmdDrawIndexed(command_buffer, m_skybox_index_count, 1, 0, 0, 0);

        // --- Pass 2: terrain, Phong-lit, real depth test/write ---
        vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_terrain_pipeline);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_terrain_pipeline_layout,
            0,
            1,
            &m_terrain_descriptor_set,
            0,
            nullptr);
        vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_terrain_vertex_buffer.getVkBuffer(),
            &zero_offset);
        vkCmdDraw(
            command_buffer,
            static_cast<std::uint32_t>(m_terrain_vertex_data.size()),
            1,
            0,
            0);

        // --- Pass 3: tank, three parts, real hierarchical positioning
        // (Tank::setTankPos()) rooted at a real terrain height query ---
        vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_object_pipeline);
        vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_object_pipeline_layout,
            0,
            1,
            &m_tank_descriptor_set,
            0,
            nullptr);
        const Math::Vec3<float> tank_world_position(
            0.0f, getTankGroundHeight(), 0.0f);
        const HellfireTank::PartTranslations tank_parts =
            HellfireTank::getPartTranslations(tank_world_position);
        const std::array<Math::Vec3<float>, c_tank_part_count>
            part_translations = {
                {tank_parts.m_body, tank_parts.m_head, tank_parts.m_turret}};
        for (std::size_t i = 0; i < c_tank_part_count; ++i) {
          vkCmdBindVertexBuffers(
              command_buffer,
              0,
              1,
              &m_tank_vertex_buffers[i].getVkBuffer(),
              &zero_offset);
          const Tutorial21PushConstants tank_push_constants{
              getTankPartModelMatrix(part_translations[i])};
          vkCmdPushConstants(
              command_buffer,
              m_object_pipeline_layout,
              VK_SHADER_STAGE_VERTEX_BIT,
              0,
              sizeof(Tutorial21PushConstants),
              &tank_push_constants);
          vkCmdDraw(command_buffer, m_tank_vertex_counts[i], 1, 0, 0);
        }
      });
}

void Tutorial21::onMouseButton(
    std::int32_t button,
    bool pressed,
    std::int32_t pos_x,
    std::int32_t pos_y) {
  m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial21::onMouseMove(std::int32_t pos_x, std::int32_t pos_y) {
  m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial21::childOnWindowSizeChanged() {
  if (getVkDevice() == VK_NULL_HANDLE) {
    return true;
  }
  return createResources();
}

void Tutorial21::childClear() {
  m_frames.destroy();
  m_resources.releaseAll();
}

}  // namespace vulkan_graphix
