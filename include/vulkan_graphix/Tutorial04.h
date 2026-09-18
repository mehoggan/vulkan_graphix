#ifndef VULKAN_GRAPHIX_TUTORIAL04_H
#define VULKAN_GRAPHIX_TUTORIAL04_H

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
struct Tutorial04VertexData {
    Math::Vec4<float> position;
    Math::Vec4<float> color;
};

using VertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>, Math::Vec4<float>>;

// ************************************************************ //
// VulkanTutorial04Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
struct VulkanTutorial04Parameters {
public:
    static const std::size_t resources_count = 3;

    VulkanTutorial04Parameters();

    const VkRenderPass& getVkRenderPass() const;
    VkRenderPass& getVkRenderPass();
    void setVkRenderPass(const VkRenderPass& vk_render_pass);

    const VkPipeline& getVkGraphicsPipeline() const;
    VkPipeline& getVkGraphicsPipeline();
    void setVkGraphicsPipeline(const VkPipeline& vk_graphics_pipeline);

    const BufferParameters& getVertexBufferParameters() const;
    BufferParameters& getVertexBufferParameters();
    void setVertexBufferParameters(const BufferParameters& vertex_buffer);

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
    VkCommandPool m_vk_command_pool;
    std::vector<RenderingResourceParameters> m_rendering_resources;
    std::vector<VkSemaphore> m_finished_rendering_semaphores;
};

// ************************************************************ //
// Tutorial04                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial04 : public TutorialBase {
public:
    Tutorial04();
    ~Tutorial04() override;

    bool createRenderPass();
    bool createPipeline();
    bool createVertexBuffer();
    bool createRenderingResources();

    bool draw() override;

private:
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);
    Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>
    createPipelineLayout();
    bool allocateBufferMemory(VkBuffer buffer, VkDeviceMemory* memory);
    bool createCommandPool(std::uint32_t queue_family_index,
                           VkCommandPool* pool);
    bool allocateCommandBuffers(VkCommandPool pool,
                                std::uint32_t count,
                                VkCommandBuffer* command_buffers);
    bool createCommandBuffers();
    bool createSemaphores();
    bool createFences();
    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);

    void childClear() override;
    bool childOnWindowSizeChanged() override;

    VulkanTutorial04Parameters m_vulkan_tutorial04_parameters;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL04_H
