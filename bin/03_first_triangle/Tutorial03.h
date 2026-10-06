#ifndef VULKAN_GRAPHIX_TUTORIAL03_H
#define VULKAN_GRAPHIX_TUTORIAL03_H

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/Tutorial/TutorialBase.h"

namespace vulkan_graphix {

// ************************************************************ //
// VulkanTutorial03Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
struct VulkanTutorial03Parameters {
public:
    VulkanTutorial03Parameters();

    const VkRenderPass& getVkRenderPass() const;
    VkRenderPass& getVkRenderPass();
    void setVkRenderPass(const VkRenderPass& vk_render_pass);

    const std::vector<VkFramebuffer>& getVkFramebuffers() const;
    std::vector<VkFramebuffer>& getVkFramebuffers();
    void setVkFramebuffers(const std::vector<VkFramebuffer>& vk_framebuffers);

    const VkPipeline& getVkPipeline() const;
    VkPipeline& getVkPipeline();
    void setVkPipeline(const VkPipeline& vk_pipeline);

    const VkSemaphore& getImageAvailableVkSemaphore() const;
    VkSemaphore& getImageAvailableVkSemaphore();
    void setImageAvailableVkSemaphore(const VkSemaphore& vk_semaphore);

    // One per swapchain image, indexed by acquired image index rather than
    // a single semaphore reused every frame. See the comment in
    // createSemaphores() for why a single semaphore isn't safe here.
    const std::vector<VkSemaphore>& getRenderingFinishedSemaphores() const;
    std::vector<VkSemaphore>& getRenderingFinishedSemaphores();
    void setRenderingFinishedSemaphores(
      const std::vector<VkSemaphore>& rendering_finished_semaphores);

    const VkFence& getVkFence() const;
    VkFence& getVkFence();
    void setVkFence(const VkFence& vk_fence);

    const VkCommandPool& getVkCommandPool() const;
    VkCommandPool& getVkCommandPool();
    void setVkCommandPool(const VkCommandPool& vk_command_pool);

    const std::vector<VkCommandBuffer>& getVkCommandBuffers() const;
    std::vector<VkCommandBuffer>& getVkCommandBuffers();
    void setVkCommandBuffers(
      const std::vector<VkCommandBuffer>& vk_command_buffers);

private:
    VkRenderPass m_vk_render_pass;
    std::vector<VkFramebuffer> m_vk_framebuffers;
    VkPipeline m_vk_pipeline;
    VkSemaphore m_image_available_vk_semaphore;
    std::vector<VkSemaphore> m_rendering_finished_semaphores;
    VkFence m_vk_fence;
    VkCommandPool m_vk_command_pool;
    std::vector<VkCommandBuffer> m_vk_command_buffers;
};

// ************************************************************ //
// Tutorial03                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial03 : public TutorialBase {
public:
    Tutorial03();
    ~Tutorial03() override;

    bool createRenderPass();
    bool createFramebuffers();
    bool createPipeline();
    bool createSemaphores();
    bool createCommandBuffers();
    bool recordCommandBuffers();

    bool draw() override;

private:
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);
    Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>
    createPipelineLayout();
    bool createCommandPool(
      std::uint32_t queue_family_index, VkCommandPool* pool);
    bool allocateCommandBuffers(VkCommandPool pool,
      std::uint32_t count,
      VkCommandBuffer* command_buffers);

    void childClear() override;
    bool childOnWindowSizeChanged() override;

    VulkanTutorial03Parameters m_vulkan_tutorial03_parameters;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL03_H
