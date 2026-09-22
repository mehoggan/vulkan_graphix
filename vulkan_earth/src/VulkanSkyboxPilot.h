#ifndef VULKAN_EARTH_VULKAN_SKYBOX_PILOT_H
#define VULKAN_EARTH_VULKAN_SKYBOX_PILOT_H

// Standalone proof-of-pattern pilot for the vulkan_earth OpenGL -> Vulkan
// migration (see the migration plan in this session's history). It
// deliberately touches nothing else under vulkan_earth/src/ - SkyboxFactory,
// GameState, and the GLUT main loop are all untouched. It reuses this
// repo's own tutorial framework (TutorialBase, Tools, OrbitCamera, Math)
// exactly the way Tutorial07/Tutorial09 do, replicating SkyboxFactory's
// existing six-quad cube geometry/UV mapping at a small, sane scale
// instead of the game's *100 world units.
//
// Unlike the numbered tutorials, this class holds its Vulkan handles as
// plain private members instead of a separate "VulkanXXXParameters"
// wrapper class - that indirection exists in the tutorials so several
// unrelated tutorials can share a consistent shape, which doesn't apply
// to a single one-off pilot.

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

// Same vertex shape as Tutorial07VertexData (position + texcoord) on
// purpose: this pilot reuses Tutorial07's compiled shaders byte-for-byte
// (see createPipeline()), so the vertex layout has to match what those
// shaders were compiled against exactly.
struct SkyboxPilotVertexData {
    Math::Vec4<float> position;
    Math::Vec2<float> texcoord;
};

using SkyboxPilotVertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>, Math::Vec2<float>>;

class VulkanSkyboxPilot : public TutorialBase {
public:
    VulkanSkyboxPilot();
    ~VulkanSkyboxPilot() override;

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
    const std::vector<SkyboxPilotVertexData>& getVertexData() const;
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
    ImageParameters image_parameters_;
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
    // One per swapchain image, indexed by acquired image index - see the
    // comment in Tutorial07's createSemaphores() for why (not duplicated
    // here, same reasoning applies verbatim).
    std::vector<VkSemaphore> finished_rendering_semaphores_;

    OrbitCamera camera_;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_EARTH_VULKAN_SKYBOX_PILOT_H
