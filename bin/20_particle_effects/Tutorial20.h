#ifndef VULKAN_GRAPHIX_TUTORIAL20_H
#define VULKAN_GRAPHIX_TUTORIAL20_H

// Ported from vulkan_earth's Effects: `ParticleGenerator`/`Particle`
// (see ParticleGenerator.h/Particle*.h under
// vulkan_earth/include/vulkan_earth/ and their .cpp files under
// vulkan_earth/src/) and `Explosion`/`SpecialEffect` (Explosion.h/.cpp)
// are both real, wired-up code, and both are untextured, unlit,
// alpha-blended `glutSolidSphere` geometry - confirmed no
// particle/explosion texture asset exists
// anywhere in vulkan_earth/src/, and confirmed `GL_LIGHTING` in the real
// game is only ever enabled for the terrain (see Tutorial21's own header
// comment) - tanks, projectiles, particles, and explosions are all drawn
// unlit. This tutorial reuses this project's own `Math::Sphere` (an
// icosphere, same construction Tutorial08/14 already use) as the one
// mesh drawn many times with different push-constant model/color, in
// place of `glutSolidSphere()` - no lighting math at all, unlike
// Tutorial08/14's own Phong-lit spheres, since the real originals here
// are unlit too.
//
// Three status effects get a real visual on a real Tank:
//   - shield (Tank::draw()'s literal glutSolidSphere(300, 20, 20) at
//     alpha 0.2, translucent blue) - a single static sphere here.
//   - acid (Tank::setDurationAcid(), Tank.cpp:830-841) - a real
//     ParticleGenerator, type Acid: green, linear motion, no color-
//     over-time (ParticleAcid::update(), ParticleAcid.cpp:27-33).
//   - float (Tank::setDurationFloat(), Tank.cpp:844-855) - a real
//     ParticleGenerator, type Float: white, damped vertical drift
//     (ParticleFloat::update(), ParticleFloat.cpp:27-33).
// Plus a fourth, non-status-effect generator type already real in the
// source (Tank's own death effect): smoke - white fading through yellow/
// red to black over its lifetime, rising as it ages. All of it - each
// particle kind's update(), ParticleGenerator's 1000-slot pool and
// spawning, and Explosion::draw()'s growth and color timeline - runs on
// libvulkan_graphix's EffectSimulation, the same code the game itself
// now runs, with each generator's real constructor arguments (Tank.cpp:
// spawn 10, life 100, speed 1 for smoke and 2 for acid/float). Only the
// drawing scale is this tutorial's own (the simulation is in vulkan_earth's
// much larger world units): each cloud spans a couple of units around its
// emitter, and smoke rises a few units over a particle's life.
//
// The explosion uses a real weapon's real explosion colors and blast
// radius from the shared GameCatalog (WeaponBFB's, already used in
// Tutorial19), looping once it has faded out.
//
// The Vulkan scaffolding - render pass, pipelines, descriptors, buffers,
// texture uploads, and the frame loop - is the library's (VulkanCommon's
// ResourceContext/FrameLoop); this file is what's specific to the
// tutorial.

#include <array>
#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/EffectSimulation.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/OrbitCamera.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"
#include "vulkan_graphix/VulkanCommon/FrameLoop.h"
#include "vulkan_graphix/VulkanCommon/ResourceContext.h"

namespace vulkan_graphix {

// Position only - unlit, untextured, unlike Tutorial08/14's lit sphere.
struct Tutorial20VertexData {
  Math::Vec4<float> m_position;
};

struct Tutorial20UniformBufferData {
  Math::Mat4<float> m_view;
  Math::Mat4<float> m_projection;
};

// The active sphere instance's model matrix and flat color, set per
// draw call - same shape as Tutorial18's push constant.
struct Tutorial20PushConstants {
  Math::Mat4<float> m_model;
  Math::Vec4<float> m_color;
};

class Tutorial20 : public TutorialBase {
public:
  Tutorial20();
  ~Tutorial20() override;

  // Everything the tutorial draws with; call once after prepareVulkan().
  bool createResources();

  bool draw() override;
  void onMouseButton(
      std::int32_t button,
      bool pressed,
      std::int32_t pos_x,
      std::int32_t pos_y) override;
  void onMouseMove(std::int32_t pos_x, std::int32_t pos_y) override;

private:
  Tutorial20UniformBufferData getUniformBufferData() const;
  const std::vector<Tutorial20VertexData>& getVertexData() const;
  const std::vector<std::uint32_t>& getIndexData() const;

  void updateParticles();
  void updateExplosion();

  bool childOnWindowSizeChanged() override;
  void childClear() override;

  VulkanCommon::ResourceContext m_resources;
  VulkanCommon::FrameLoop m_frames;
  BufferParameters m_uniform_buffer;
  BufferParameters m_vertex_buffer;
  BufferParameters m_index_buffer;
  std::uint32_t m_index_count = 0;
  VkDescriptorSet m_descriptor_set = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  OrbitCamera m_camera;

  // Smoke, acid, and float, in that order.
  std::array<EffectSimulation::ParticleEmitter, 3> m_emitters;
  EffectSimulation::Explosion m_explosion;
  // The explosion's current color (the last one its timeline set).
  Math::Vec4<float> m_explosion_color;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL20_H
