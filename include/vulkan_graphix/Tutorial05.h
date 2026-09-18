#ifndef VULKAN_GRAPHIX_TUTORIAL05_H
#define VULKAN_GRAPHIX_TUTORIAL05_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/TutorialBase.h"
#include "vulkan_graphix/VertexTypes/AttributeTraits.hpp"

namespace vulkan_graphix {

// ************************************************************ //
// VertexData                                                   //
//                                                              //
// Struct describing data type and format of vertex attributes  //
// ************************************************************ //
struct Tutorial05VertexData {
    Math::Vec4<float> position;
    Math::Vec4<float> color;
};

using VertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>, Math::Vec4<float>>;

// ************************************************************ //
// VulkanTutorial05Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
struct VulkanTutorial05Parameters {
public:
    static const std::size_t resources_count = 3;

    VulkanTutorial05Parameters();

    const VkRenderPass& getVkRenderPass() const;
    VkRenderPass& getVkRenderPass();
    void setVkRenderPass(const VkRenderPass& vk_render_pass);

    const VkPipeline& getVkGraphicsPipeline() const;
    VkPipeline& getVkGraphicsPipeline();
    void setVkGraphicsPipeline(const VkPipeline& vk_graphics_pipeline);

    const BufferParameters& getVertexBufferParameters() const;
    BufferParameters& getVertexBufferParameters();
    void setVertexBufferParameters(const BufferParameters& vertex_buffer);

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

    // One per swapchain image, indexed by acquired image index rather than
    // by rendering-resource slot. See the comment in createSemaphores() for
    // why this can't just live in RenderingResourceParameters.
    const std::vector<VkSemaphore>& getFinishedRenderingSemaphores() const;
    std::vector<VkSemaphore>& getFinishedRenderingSemaphores();
    void setFinishedRenderingSemaphores(
            const std::vector<VkSemaphore>& finished_rendering_semaphores);

private:
    VkRenderPass m_vk_render_pass;
    VkPipeline m_vk_graphics_pipeline;
    BufferParameters m_vertex_buffer;
    BufferParameters m_staging_buffer;
    VkCommandPool m_vk_command_pool;
    std::vector<RenderingResourceParameters> m_rendering_resources;
    std::vector<VkSemaphore> m_finished_rendering_semaphores;
};

// ************************************************************ //
// Tutorial05                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial05 : public TutorialBase {
public:
    Tutorial05();
    ~Tutorial05() override;

    bool createRenderingResources();
    bool createRenderPass();
    bool createPipeline();
    bool createVertexBuffer();
    bool createStagingBuffer();
    bool copyVertexData();

    bool draw() override;

private:
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);
    Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>
    createPipelineLayout();
    bool createCommandPool(std::uint32_t queue_family_index,
                           VkCommandPool* pool);
    bool allocateCommandBuffers(VkCommandPool pool,
                                std::uint32_t count,
                                VkCommandBuffer* command_buffers);
    bool createCommandBuffers();
    bool createSemaphores();
    bool createFences();
    bool createBuffer(VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags memory_property,
                      BufferParameters& buffer);
    bool allocateBufferMemory(VkBuffer buffer,
                              VkMemoryPropertyFlags property,
                              VkDeviceMemory* memory);
    const std::vector<Tutorial05VertexData>& getVertexData() const;
    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    void childClear() override;
    bool childOnWindowSizeChanged() override;

    VulkanTutorial05Parameters m_vulkan_tutorial05_parameters;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL05_H
