#ifndef VULKAN_GRAPHIX_TUTORIAL20_H
#define VULKAN_GRAPHIX_TUTORIAL20_H

// Ported from vulkan_earth's Effects: `ParticleGenerator`/`Particle`
// (see vulkan_earth/src/ParticleGenerator.h/.cpp, Particle*.h/.cpp) and
// `Explosion`/`SpecialEffect` (Explosion.h/.cpp) are both real, wired-up
// code, and both are untextured, unlit, alpha-blended `glutSolidSphere`
// geometry - confirmed no particle/explosion texture asset exists
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
// source (Tank's own death effect, Tank.cpp:350): smoke - white fading
// through yellow/red to black over its lifetime, rising as it ages
// (ParticleSmoke::update(), ParticleSmoke.cpp:27-45). All four are
// live, per-frame-updated CPU simulations using the real per-type
// update() formulas (position/velocity/lifetime, and smoke's real
// color-over-time fade), respawning at end of life exactly like
// ParticleGenerator::addParticles() refills a dead slot - not a static
// snapshot. Absolute position/speed/size scale is this tutorial's own
// choice (the originals are calibrated to vulkan_earth's own much
// larger world units), tuned via screenshot like every other
// tutorial's world scale.
//
// Explosion::draw() (Explosion.cpp:53-74) is ported the same way: one
// sphere whose radius grows and whose color cycles through a real
// weapon's real explosion_color1..4 (WeaponBFB's Yellow/Orange/Red/
// White palette, already used in Tutorial19), fading out and looping -
// same real formula structure (radius += rate; timer advances; color
// picked by timer thresholds; alpha = 1 - timer/limit), rescaled to
// this tutorial's own small world.

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/OrbitCamera.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/TutorialBase.h"
#include "vulkan_graphix/VertexTypes/AttributeTraits.hpp"

namespace vulkan_graphix {

// Position only - unlit, untextured, unlike Tutorial08/14's lit sphere.
struct Tutorial20VertexData {
    Math::Vec4<float> position;
};

using Tutorial20VertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>>;

struct Tutorial20UniformBufferData {
    Math::Mat4<float> view;
    Math::Mat4<float> projection;
};

// The active sphere instance's model matrix and flat color, set per
// draw call - same shape as Tutorial18's push constant.
struct Tutorial20PushConstants {
    Math::Mat4<float> model;
    Math::Vec4<float> color;
};

enum class ParticleKind { kSmoke, kAcid, kFloat };

// One live particle - mirrors vulkan_earth's own Particle base fields
// (position/direction/speed/lifetime) plus a kind tag standing in for
// the separate ParticleSmoke/ParticleAcid/ParticleFloat subclasses.
struct EffectParticle {
    ParticleKind kind;
    Math::Vec3<float> position;
    Math::Vec3<float> direction;
    Math::Vec4<float> color;
    float speed = 0.0f;
    int current_frame = 0;
    int active_frames = 0;
};

// ************************************************************ //
// VulkanTutorial20Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
struct VulkanTutorial20Parameters {
public:
    static const std::size_t resources_count = 3;

    VulkanTutorial20Parameters();

    const VkRenderPass& getVkRenderPass() const;
    VkRenderPass& getVkRenderPass();
    void setVkRenderPass(const VkRenderPass& vk_render_pass);

    const BufferParameters& getUniformBufferParameters() const;
    BufferParameters& getUniformBufferParameters();
    void setUniformBufferParameters(const BufferParameters& uniform_buffer);

    const DescriptorSetParameters& getDescriptorSetParameters() const;
    DescriptorSetParameters& getDescriptorSetParameters();
    void setDescriptorSetParameters(
            const DescriptorSetParameters& descriptor_set_parameters);

    const VkPipelineLayout& getVkPipelineLayout() const;
    VkPipelineLayout& getVkPipelineLayout();
    void setVkPipelineLayout(const VkPipelineLayout& vk_pipeline_layout);

    const VkPipeline& getVkGraphicsPipeline() const;
    VkPipeline& getVkGraphicsPipeline();
    void setVkGraphicsPipeline(const VkPipeline& vk_graphics_pipeline);

    const BufferParameters& getVertexBufferParameters() const;
    BufferParameters& getVertexBufferParameters();
    void setVertexBufferParameters(const BufferParameters& vertex_buffer);

    const BufferParameters& getIndexBufferParameters() const;
    BufferParameters& getIndexBufferParameters();
    void setIndexBufferParameters(const BufferParameters& index_buffer);

    std::uint32_t getIndexCount() const;
    void setIndexCount(std::uint32_t index_count);

    const BufferParameters& getStagingBufferParameters() const;
    BufferParameters& getStagingBufferParameters();
    void setStagingBufferParameters(const BufferParameters& staging_buffer);

    const VkCommandPool& getVkCommandPool() const;
    VkCommandPool& getVkCommandPool();
    void setVkCommandPool(const VkCommandPool& vk_command_pool);

    const std::vector<RenderingResourceParameters>& getRenderingResources()
            const;
    std::vector<RenderingResourceParameters>& getRenderingResources();
    void setRenderingResources(const std::vector<RenderingResourceParameters>&
                                       rendering_resources);

    const std::vector<VkSemaphore>& getFinishedRenderingSemaphores() const;
    std::vector<VkSemaphore>& getFinishedRenderingSemaphores();
    void setFinishedRenderingSemaphores(
            const std::vector<VkSemaphore>& finished_rendering_semaphores);

private:
    VkRenderPass m_vk_render_pass;
    BufferParameters m_uniform_buffer;
    DescriptorSetParameters m_descriptor_set_parameters;
    VkPipelineLayout m_vk_pipeline_layout;
    VkPipeline m_vk_graphics_pipeline;
    BufferParameters m_vertex_buffer;
    BufferParameters m_index_buffer;
    std::uint32_t m_index_count;
    BufferParameters m_staging_buffer;
    VkCommandPool m_vk_command_pool;
    std::vector<RenderingResourceParameters> m_rendering_resources;
    std::vector<VkSemaphore> m_finished_rendering_semaphores;
};

// ************************************************************ //
// Tutorial20                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial20 : public TutorialBase {
public:
    Tutorial20();
    ~Tutorial20() override;

    bool createRenderingResources();
    bool createStagingBuffer();
    bool createUniformBuffer();
    bool createDescriptorSetLayout();
    bool createDescriptorPool();
    bool allocateDescriptorSet();
    bool updateDescriptorSet();
    bool createRenderPass();
    bool createPipelineLayout();
    bool createPipeline();
    bool createVertexBuffer();
    bool createIndexBuffer();

    bool draw() override;

    void onMouseButton(int button,
                       bool pressed,
                       int pos_x,
                       int pos_y) override;
    void onMouseMove(int pos_x, int pos_y) override;

private:
    // Real per-type constants (vulkan_earth/src/ParticleGenerator's own
    // constructor args at each real call site: spawn=10, life=100 - see
    // Tank.cpp:350,830-855) - rescaled position/speed/size below to this
    // tutorial's own small world.
    static constexpr int kActiveFrames = 100;
    static constexpr std::size_t kParticlesPerEmitter = 8;
    static constexpr float kWorldScale = 0.03f;

    bool createCommandBuffers();
    bool createCommandPool(std::uint32_t queue_family_index,
                           VkCommandPool* pool);
    bool allocateCommandBuffers(VkCommandPool pool,
                                std::uint32_t count,
                                VkCommandBuffer* command_buffers);
    bool createSemaphores();
    bool createFences();
    bool createBuffer(VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags memory_property,
                      BufferParameters& buffer);
    bool copyBufferData(BufferParameters& destination,
                        const void* data,
                        std::uint32_t data_size,
                        VkAccessFlags dst_access_mask,
                        VkPipelineStageFlags dst_stage_mask);
    Tutorial20UniformBufferData getUniformBufferData() const;
    bool updateUniformBufferData();
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);
    const std::vector<Tutorial20VertexData>& getVertexData() const;
    const std::vector<std::uint32_t>& getIndexData() const;

    Math::Vec3<float> randomUnitVector() const;
    void respawnParticle(EffectParticle& particle,
                         ParticleKind kind,
                         Math::Vec3<float> const& emitter_position) const;
    // Ported from ParticleSmoke/ParticleAcid/ParticleFloat::update() -
    // advances one particle by one frame and stores its current color
    // in particle.color (alpha fixed at 0.4, matching every Particle::
    // draw()'s own glColor4f(r, g, b, 0.4)) so prepareFrame() doesn't
    // need to recompute it.
    void updateParticle(EffectParticle& particle) const;
    void updateParticles();
    void updateExplosion();

    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    bool childOnWindowSizeChanged() override;
    void childClear() override;

    VulkanTutorial20Parameters m_vulkan_tutorial20_parameters;
    OrbitCamera m_camera;

    std::vector<EffectParticle> m_particles;
    float m_explosion_radius;
    float m_explosion_timer;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL20_H
