#include "vulkan_earth/ParticleGenerator.h"
#include <cstdint>
#include "vulkan_earth/render/GlMatrix.h"
#include "vulkan_earth/render/Renderer.h"

namespace render = vulkan_earth::render;
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
                             render::Mat4 const& model) {
    // Each particle: glutSolidSphere(1, 10, 10) at its position, scaled by
    // its size.
    for (auto const& slot : emitter.slots()) {
        if (!slot) {
            continue;
        }
        effects::Particle const& particle = *slot;
        context.drawMesh(
                render::Renderer::instance().sphere(10, 10),
                render::PipelineId::FlatColor,
                nullptr,
                render::glmatrix::scaled(
                        render::glmatrix::translated(
                                model, particle.x, particle.y, particle.z),
                        particle.size,
                        particle.size,
                        particle.size),
                render::Vec4(particle.red,
                             particle.green,
                             particle.blue,
                             effects::c_particle_alpha));
    }
}

void ParticleGenerator::killGenerator() { emitter.clear(); }
