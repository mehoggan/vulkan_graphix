#ifndef VULKAN_EARTH_VULKAN_PARTICLE_PILOT_H
#define VULKAN_EARTH_VULKAN_PARTICLE_PILOT_H

// Standalone Vulkan-migration pilot #4 for vulkan_earth (see
// VulkanSkyboxPilot.h for pilot #1 and the pattern this follows). Renders
// the shape Particle::render() actually draws for every tank-death/status
// effect (glPushMatrix(); glTranslatef(...); glColor4f(r,g,b,0.4);
// glutSolidSphere(size,10,10); glPopMatrix() - see Particle.cpp) as a flat,
// translucent, alpha-blended icosphere lit with the same Phong math
// Tutorial08 uses for its own Math::Sphere, via a small new fragment
// shader (resources/particle_pilot/Data/shader.frag) since Tutorial08's
// own compiled one hardcodes an opaque, non-alpha-blended object color -
// its vertex shader is otherwise reused unchanged. This project's own
// Math::Sphere/Icosahedron (used exactly this way by Tutorial08 already -
// see lib/Tutorial08.cpp's getVertexData()/getIndexData()) replaces
// glutSolidSphere. No texture, matching the original's flat-colored look.
// Additive only - doesn't touch or get linked into vulkan_earth_runner.

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

// Matches Tutorial08VertexData's shape on purpose - this pilot's Math::Sphere
// usage is otherwise identical to Tutorial08's own (see getVertexData()/
// getIndexData()).
struct ParticlePilotVertexData {
    Math::Vec4<float> position;
    Math::Vec3<float> normal;
};

using ParticlePilotVertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>, Math::Vec3<float>>;

// Matches Tutorial08UniformBufferData's shape byte-for-byte - the new
// fragment shader (resources/particle_pilot/Data/shader.frag) reads the
// same uniform block layout Tutorial08's own shader does.
struct ParticlePilotUniformBufferData {
    Math::Mat4<float> model;
    Math::Mat4<float> view;
    Math::Mat4<float> projection;
    Math::Vec4<float> light_position;
    Math::Vec4<float> light_color;
    Math::Vec4<float> view_position;
};

class VulkanParticlePilot : public TutorialBase {
public:
    VulkanParticlePilot();
    ~VulkanParticlePilot() override;

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
    bool allocateBufferMemory(VkBuffer buffer,
                              VkMemoryPropertyFlags property,
                              VkDeviceMemory* memory);
    bool copyBufferData(BufferParameters& destination,
                        const void* data,
                        std::uint32_t data_size,
                        VkAccessFlags dst_access_mask,
                        VkPipelineStageFlags dst_stage_mask);
    ParticlePilotUniformBufferData getUniformBufferData() const;
    bool updateUniformBufferData();
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);
    const std::vector<ParticlePilotVertexData>& getVertexData() const;
    const std::vector<std::uint32_t>& getIndexData() const;
    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    bool childOnWindowSizeChanged() override;
    void childClear() override;

    static const std::size_t resources_count_ = 3;

    VkRenderPass vk_render_pass_;
    BufferParameters uniform_buffer_;
    DescriptorSetParameters descriptor_set_parameters_;
    VkPipelineLayout vk_pipeline_layout_;
    VkPipeline vk_graphics_pipeline_;
    BufferParameters vertex_buffer_;
    BufferParameters index_buffer_;
    BufferParameters staging_buffer_;
    std::uint32_t index_count_;
    VkCommandPool vk_command_pool_;
    std::vector<RenderingResourceParameters> rendering_resources_;
    std::vector<VkSemaphore> finished_rendering_semaphores_;

    OrbitCamera camera_;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_EARTH_VULKAN_PARTICLE_PILOT_H
