#include <cstdint>
#include "vulkan_earth/SpecialEffect.h"
#include "vulkan_graphix/EffectSimulation.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Renderer.h"

#ifndef PARTICLEGENERATOR_H
#define PARTICLEGENERATOR_H

namespace vulkan_graphix::Render {
class RenderContext;
}

// A tank's smoke/acid/float particle stream: libvulkan_graphix's
// EffectSimulation::ParticleEmitter (the simulation, shared with
// Tutorial20), drawn as translucent spheres.
class ParticleGenerator {
public:
    ParticleGenerator(std::int32_t spawn,
                      std::int32_t rate,
                      std::int32_t speed,
                      std::int32_t life,
                      std::int32_t new_type);
    void update(float new_x, float new_y, float new_z);
    void draw(vulkan_graphix::Render::RenderContext& context,
              vulkan_graphix::Math::Mat4<float> const& model =
                      vulkan_graphix::Math::Mat4<float>(1.0f));
    void killGenerator();

private:
    vulkan_graphix::EffectSimulation::ParticleEmitter emitter;
};

#endif
