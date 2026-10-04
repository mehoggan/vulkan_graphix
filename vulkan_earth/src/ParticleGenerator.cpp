#include "vulkan_earth/ParticleGenerator.h"
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vulkan_earth/GameRenderer.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;
namespace effects = vulkan_graphix::EffectSimulation;

ParticleGenerator::ParticleGenerator(std::int32_t spawn,
                                     std::int32_t rate,
                                     std::int32_t speed,
                                     std::int32_t life,
                                     std::int32_t new_type)
        : emitter(spawn,
                  rate,
                  speed,
                  life,
                  static_cast<effects::ParticleKind>(new_type)) {}

void ParticleGenerator::update(float new_x, float new_y, float new_z) {
    emitter.update(new_x, new_y, new_z);
}

void ParticleGenerator::draw(render::RenderContext& context,
                             const math::Mat4<float>& model) {
    // Each particle: glutSolidSphere(1, 10, 10) at its position, scaled by
    // its size.
    for (const auto& slot : emitter.slots()) {
        if (!slot) {
            continue;
        }
        const effects::Particle& particle = *slot;
        context.drawMesh(
                render::Renderer::instance().sphere(10, 10),
                vulkan_earth::pipelines().flat_color,
                nullptr,
                glm::scale(
                        glm::translate(
                                model,
                                math::Vec3<float>(
                                        particle.x, particle.y, particle.z)),
                        math::Vec3<float>(
                                particle.size, particle.size, particle.size)),
                math::Vec4<float>(particle.red,
                                  particle.green,
                                  particle.blue,
                                  effects::c_particle_alpha));
    }
}

void ParticleGenerator::killGenerator() { emitter.clear(); }
