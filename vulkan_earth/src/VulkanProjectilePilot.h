#ifndef VULKAN_EARTH_VULKAN_PROJECTILE_PILOT_H
#define VULKAN_EARTH_VULKAN_PROJECTILE_PILOT_H

// Standalone Vulkan-migration pilot #3 for vulkan_earth (see
// VulkanSkyboxPilot.h for pilot #1 and the pattern this follows). Parses
// the real tank-shell mesh (Projectiles/projectileDefault.ogl -
// VBOShaderLibrary::loadClientData()'s own plain-ASCII, 8-floats-per-vertex,
// unindexed format: texcoord.st, normal.xyz, vertex.xyz, no header) and
// renders it unlit and textured, the same way the real game effectively
// does - FragmentTank.vs samples a normal_texture but never actually uses
// it in the final color, so the parsed normal is dropped here too. Reuses
// Tutorial07's compiled shaders byte-for-byte, same as the skybox pilot,
// since {position, texcoord} + unlit-textured is exactly what both need.
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

// Matches Tutorial07VertexData's shape on purpose - see SkyboxPilotVertexData
// in VulkanSkyboxPilot.h for the same reasoning; this pilot reuses the same
// compiled shaders.
struct ProjectilePilotVertexData {
    Math::Vec4<float> position;
    Math::Vec2<float> texcoord;
};

using ProjectilePilotVertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>, Math::Vec2<float>>;

class VulkanProjectilePilot : public TutorialBase {
public:
    VulkanProjectilePilot();
    ~VulkanProjectilePilot() override;

    bool createRenderingResources();
    bool createStagingBuffer();
    bool createTexture();
    bool createUniformBuffer();
    bool createDescriptorSetLayout();
    bool createDescriptorPool();
    bool allocateDescriptorSet();
    bool updateDescriptorSet();
    bool createRenderPass();
    bool createPipelineLayout();
    bool createPipeline();
    bool createVertexBuffer();

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
    bool createImage(std::uint32_t width,
                     std::uint32_t height,
                     VkImage* image);
    bool allocateImageMemory(VkImage image,
                             VkMemoryPropertyFlags property,
                             VkDeviceMemory* memory);
    bool createImageView();
    bool createSampler(VkSampler* sampler);
    bool copyTextureData(char* texture_data,
                         std::uint32_t data_size,
                         std::uint32_t width,
                         std::uint32_t height);
    bool copyBufferData(BufferParameters& destination,
                        const void* data,
                        std::uint32_t data_size,
                        VkAccessFlags dst_access_mask,
                        VkPipelineStageFlags dst_stage_mask);
    Math::Mat4<float> getUniformBufferData() const;
    bool updateUniformBufferData();
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);
    const std::vector<ProjectilePilotVertexData>& getVertexData();
    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    bool childOnWindowSizeChanged() override;
    void childClear() override;

    static const std::size_t resources_count_ = 3;

    VkRenderPass vk_render_pass_;
    ImageParameters image_parameters_;
    BufferParameters uniform_buffer_;
    DescriptorSetParameters descriptor_set_parameters_;
    VkPipelineLayout vk_pipeline_layout_;
    VkPipeline vk_graphics_pipeline_;
    BufferParameters vertex_buffer_;
    BufferParameters staging_buffer_;
    std::uint32_t vertex_count_;
    VkCommandPool vk_command_pool_;
    std::vector<RenderingResourceParameters> rendering_resources_;
    std::vector<VkSemaphore> finished_rendering_semaphores_;

    OrbitCamera camera_;

    std::vector<ProjectilePilotVertexData> vertex_data_;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_EARTH_VULKAN_PROJECTILE_PILOT_H
