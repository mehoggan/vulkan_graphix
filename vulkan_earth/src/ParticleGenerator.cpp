#include "vulkan_earth/ParticleGenerator.h"
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vulkan_earth/GameRenderer.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;
namespace effects = vulkan_graphix::EffectSimulation;

ParticleGenerator::ParticleGenerator(
    std::int32_t spawn,
    std::int32_t rate,
    std::int32_t speed,
    std::int32_t life,
    std::int32_t new_type) :
    m_emitter(
        spawn,
        rate,
        speed,
        life,
        static_cast<effects::ParticleKind>(new_type)) {}

void ParticleGenerator::update(float new_x, float new_y, float new_z) {
  m_emitter.update(new_x, new_y, new_z);
}

void ParticleGenerator::draw(
    render::RenderContext& context, const math::Mat4<float>& model) {
  // Each particle: glutSolidSphere(1, 10, 10) at its position, scaled by
  // its size.
  for (const auto& slot : m_emitter.slots()) {
    if (!slot) {
      continue;
    }
    const effects::Particle& particle = *slot;
    context.drawMesh(
        render::Renderer::instance().sphere(10, 10),
        vulkan_earth::pipelines().m_flat_color,
        nullptr,
        glm::scale(
            glm::translate(
                model,
                math::Vec3<float>(particle.m_x, particle.m_y, particle.m_z)),
            math::Vec3<float>(
                particle.m_size, particle.m_size, particle.m_size)),
        math::Vec4<float>(
            particle.m_red,
            particle.m_green,
            particle.m_blue,
            effects::c_particle_alpha));
  }
}

void ParticleGenerator::killGenerator() { m_emitter.clear(); }
