#include "Tutorial20.h"

#include <array>
#include <cstddef>
#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/GameCatalog.h"
#include "vulkan_graphix/Math/Sphere.hpp"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
// Three fixed emitter origins, one per real ParticleGenerator type
// (Tank's own smoke_gen/acid_gen/float_gen - see Tutorial20.h's top
// comment), laid out side by side.
const Math::Vec3<float> c_smoke_emitter(-2.5f, -1.0f, 0.0f);
const Math::Vec3<float> c_acid_emitter(0.0f, -1.0f, 0.0f);
const Math::Vec3<float> c_float_emitter(2.5f, -1.0f, 0.0f);

// The real Tank::draw() shield sphere: glutSolidSphere(300, 20, 20) at
// alpha 0.2, translucent blue.
const Math::Vec3<float> c_shield_position(0.0f, 1.8f, 0.0f);
constexpr float c_shield_radius = 0.9f;
const Math::Vec4<float> c_shield_color(0.1f, 0.25f, 0.95f, 0.2f);

// Particle positions and sizes, and the explosion's radius, are simulated
// in vulkan_earth's own world units; these bring them down to this
// tutorial's.
constexpr float c_particle_position_scale = 0.01f;
constexpr float c_particle_size_scale = 0.03f;
constexpr float c_explosion_scale = 0.0003f;
const Math::Vec3<float> c_explosion_position(0.0f, -1.0f, 2.5f);

// WeaponBFB - the weapon Tutorial19's "Big Force Bomb" projectile already
// shows - supplies the explosion's real colors and blast radius.
const GameCatalog::WeaponSpec& explosionWeapon() {
  return GameCatalog::weapon(GameCatalog::WeaponKind::BFB);
}

Math::Vec4<float> explosionColor(std::int32_t index, float alpha) {
  const auto& color = explosionWeapon().m_explosion_colors[index];
  return Math::Vec4<float>(
      static_cast<float>(color[0]),
      static_cast<float>(color[1]),
      static_cast<float>(color[2]),
      alpha);
}

Math::Mat4<float> buildInstanceMatrix(
    const Math::Vec3<float>& position, float radius) {
  return glm::translate(Math::Mat4<float>(1.0f), position) *
      glm::scale(Math::Mat4<float>(1.0f), Math::Vec3<float>(radius));
}
}  // namespace

namespace vc = VulkanCommon;

Tutorial20::Tutorial20() :
    m_camera(0.5f, 0.2f, 6.0f),
    // Tank.cpp's own smoke_gen/acid_gen/float_gen arguments.
    m_emitters{
        EffectSimulation::ParticleEmitter(
            10, 5, 1, 100, EffectSimulation::ParticleKind::Smoke),
        EffectSimulation::ParticleEmitter(
            10, 5, 2, 100, EffectSimulation::ParticleKind::Acid),
        EffectSimulation::ParticleEmitter(
            10, 5, 2, 100, EffectSimulation::ParticleKind::Float)},
    m_explosion_color(explosionColor(0, 1.0f)) {}

Tutorial20::~Tutorial20() { childClear(); }

bool Tutorial20::createResources() {
  if (!m_resources.initialize(*this)) {
    return false;
  }

  if (!m_resources.createUniformBuffer(
          sizeof(Tutorial20UniformBufferData), m_uniform_buffer) ||
      !m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    Logging::error(LOG_TAG, "Could not create uniform buffer!");
    return false;
  }

  const std::vector<vc::DescriptorBinding> bindings = {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}};
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
          {set_layout},
          {{VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(Tutorial20PushConstants)}},
          &m_pipeline_layout)) {
    Logging::error(LOG_TAG, "Could not create the render pass!");
    return false;
  }

  // BACK_BIT/CLOCKWISE - Math::Sphere winds clockwise as seen from
  // outside, same pairing Tutorial08/14 use. Alpha blending - every real
  // Particle::draw()/Explosion::draw() call is translucent
  // (glColor4f(..., 0.4) / a fading alpha). No depth attachment, same as
  // Tutorial14 - back-face culling alone handles each sphere's own
  // self-occlusion, and these small translucent spheres don't need
  // cross-instance depth sorting for a demo like this.
  if (!m_resources.createGraphicsPipeline(
          {.m_vertex_shader = "shader.20.vert.spv",
           .m_fragment_shader = "shader.20.frag.spv",
           .m_vertex_stride = sizeof(Tutorial20VertexData),
           .m_vertex_attributes =
               {{0,
                 0,
                 VK_FORMAT_R32G32B32A32_SFLOAT,
                 offsetof(Tutorial20VertexData, m_position)}},
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

void Tutorial20::updateParticles() {
  // Each generator simulates around the origin; draw() places it
  // at its own emitter.
  for (EffectSimulation::ParticleEmitter& emitter : m_emitters) {
    emitter.update(0.0f, 0.0f, 0.0f);
  }
}

void Tutorial20::updateExplosion() {
  const EffectSimulation::ExplosionFrame frame =
      EffectSimulation::advanceExplosion(m_explosion);
  if (frame.m_color_index >= 0) {
    m_explosion_color = explosionColor(frame.m_color_index, frame.m_alpha);
  }
  // Faded out: start over.
  if (m_explosion.m_timer >= 150.0f) {
    m_explosion = EffectSimulation::Explosion{};
  }
}

Tutorial20UniformBufferData Tutorial20::getUniformBufferData() const {
  Tutorial20UniformBufferData data{};
  data.m_view = m_camera.view();

  const float width =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
  const float height =
      static_cast<float>(getSwapchainParameters().getVkExtent2d().height);
  data.m_projection = Tools::getPerspectiveProjectionMatrix(
      width / height, 45.0f, 0.1f, 100.0f);

  return data;
}

const std::vector<Tutorial20VertexData>& Tutorial20::getVertexData() const {
  // Icosphere, radius 1.0, low subdivision level - these spheres are
  // small and numerous on screen, and unlit, so a coarser mesh than
  // Tutorial08/14's is plenty.
  static const std::vector<Tutorial20VertexData> vertex_data = [] {
    const Math::Sphere<float, std::uint32_t> sphere(
        1.0f, static_cast<std::uint8_t>(2));

    std::vector<Tutorial20VertexData> data;
    data.reserve(sphere.points().size());
    for (const Math::Vec3<float>& point : sphere.points()) {
      data.push_back({Math::Vec4<float>(point, 1.0f)});
    }
    return data;
  }();

  return vertex_data;
}

const std::vector<std::uint32_t>& Tutorial20::getIndexData() const {
  static const std::vector<std::uint32_t> index_data = [] {
    const Math::Sphere<float, std::uint32_t> sphere(
        1.0f, static_cast<std::uint8_t>(2));
    return sphere.indices();
  }();
  return index_data;
}

bool Tutorial20::draw() {
  updateParticles();
  updateExplosion();

  vkDeviceWaitIdle(getVkDevice());
  if (!m_resources.writeBuffer(m_uniform_buffer, getUniformBufferData())) {
    return false;
  }

  VkClearValue clear_value = {};
  clear_value.color = {{0.08f, 0.08f, 0.1f, 1.0f}};
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
        // One shared sphere mesh, drawn once per instance with its own
        // model matrix and color.
        auto draw_sphere = [&](const Math::Mat4<float>& model,
                               const Math::Vec4<float>& color) {
          const Tutorial20PushConstants push_constants{model, color};
          vkCmdPushConstants(
              command_buffer,
              m_pipeline_layout,
              VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
              0,
              sizeof(Tutorial20PushConstants),
              &push_constants);
          vkCmdDrawIndexed(command_buffer, m_index_count, 1, 0, 0, 0);
        };

        // Shield: one static translucent sphere.
        draw_sphere(
            buildInstanceMatrix(c_shield_position, c_shield_radius),
            c_shield_color);

        // Particles: every live particle of each generator, at its
        // emitter.
        const std::array<Math::Vec3<float>, 3> emitter_origins = {
            c_smoke_emitter, c_acid_emitter, c_float_emitter};
        for (std::size_t emitter = 0; emitter < m_emitters.size(); ++emitter) {
          for (const auto& slot : m_emitters[emitter].slots()) {
            if (!slot) {
              continue;
            }
            const EffectSimulation::Particle& particle = *slot;
            draw_sphere(
                buildInstanceMatrix(
                    emitter_origins[emitter] +
                        Math::Vec3<float>(
                            particle.m_x, particle.m_y, particle.m_z) *
                            c_particle_position_scale,
                    particle.m_size * c_particle_size_scale),
                Math::Vec4<float>(
                    particle.m_red,
                    particle.m_green,
                    particle.m_blue,
                    EffectSimulation::c_particle_alpha));
          }
        }

        // Explosion: one growing/fading/color-cycling sphere.
        draw_sphere(
            buildInstanceMatrix(
                c_explosion_position,
                EffectSimulation::explosionSphereRadius(
                    m_explosion,
                    static_cast<std::int32_t>(explosionWeapon().m_radius)) *
                    c_explosion_scale),
            m_explosion_color);
      });
}

void Tutorial20::onMouseButton(
    std::int32_t button,
    bool pressed,
    std::int32_t pos_x,
    std::int32_t pos_y) {
  m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial20::onMouseMove(std::int32_t pos_x, std::int32_t pos_y) {
  m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial20::childOnWindowSizeChanged() {
  if (getVkDevice() == VK_NULL_HANDLE) {
    return true;
  }
  return createResources();
}

void Tutorial20::childClear() {
  m_frames.destroy();
  m_resources.releaseAll();
}

}  // namespace vulkan_graphix
