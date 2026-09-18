#ifndef VULKAN_GRAPHIX_TUTORIAL09_H
#define VULKAN_GRAPHIX_TUTORIAL09_H

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

// ************************************************************ //
// VertexData                                                   //
//                                                              //
// Struct describing data type and format of vertex attributes  //
// ************************************************************ //
struct Tutorial09VertexData {
    Math::Vec4<float> position;
    Math::Vec3<float> normal;
    Math::Vec2<float> texcoord;
};

using VertexAttributeTraits = VertexTypes::AttributeTraits<Math::Vec4<float>,
                                                           Math::Vec3<float>,
                                                           Math::Vec2<float>>;

// ************************************************************ //
// UniformBufferData                                            //
//                                                              //
// Layout of the Phong lighting uniform buffer. Vec3 fields are //
// promoted to Vec4 for std140 alignment.                       //
// ************************************************************ //
struct Tutorial09UniformBufferData {
    Math::Mat4<float> model;
    Math::Mat4<float> view;
    Math::Mat4<float> projection;
    Math::Vec4<float> light_position;
    Math::Vec4<float> light_color;
    Math::Vec4<float> view_position;
};

// ************************************************************ //
// VulkanTutorial09Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
class VulkanTutorial09Parameters {
public:
    static const std::size_t resources_count = 3;

    VulkanTutorial09Parameters();

    const VkRenderPass& getVkRenderPass() const;
    VkRenderPass& getVkRenderPass();
    void setVkRenderPass(const VkRenderPass& vk_render_pass);

    const ImageParameters& getDepthImageParameters() const;
    ImageParameters& getDepthImageParameters();
    void setDepthImageParameters(const ImageParameters& depth_image);

    const ImageParameters& getTextureImageParameters() const;
    ImageParameters& getTextureImageParameters();
    void setTextureImageParameters(const ImageParameters& texture_image);

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

    // One per swapchain image, indexed by acquired image index rather than
    // by rendering-resource slot. See the comment in createSemaphores() for
    // why this can't just live in RenderingResourceParameters.
    const std::vector<VkSemaphore>& getFinishedRenderingSemaphores() const;
    std::vector<VkSemaphore>& getFinishedRenderingSemaphores();
    void setFinishedRenderingSemaphores(
            const std::vector<VkSemaphore>& finished_rendering_semaphores);

private:
    VkRenderPass m_vk_render_pass;
    ImageParameters m_depth_image_parameters;
    ImageParameters m_texture_image_parameters;
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
// Tutorial09                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial09 : public TutorialBase {
public:
    Tutorial09();
    ~Tutorial09() override;

    bool createRenderingResources();
    bool createStagingBuffer();
    bool createDepthResources();
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
                     VkFormat format,
                     VkImageUsageFlags usage,
                     VkImage* image);
    bool allocateImageMemory(VkImage image,
                             VkMemoryPropertyFlags property,
                             VkDeviceMemory* memory);
    bool createImageView(VkImage image,
                         VkFormat format,
                         VkImageAspectFlags aspect_mask,
                         VkImageView* image_view);
    bool createSampler(VkSamplerAddressMode address_mode, VkSampler* sampler);
    bool copyTextureData(char* texture_data,
                         std::uint32_t data_size,
                         std::uint32_t width,
                         std::uint32_t height);
    bool destroyDepthResources();
    Tutorial09UniformBufferData getUniformBufferData() const;
    bool updateUniformBufferData();
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);
    const std::vector<Tutorial09VertexData>& getVertexData() const;
    const std::vector<std::uint32_t>& getIndexData() const;
    bool copyBufferData(BufferParameters& destination,
                        const void* data,
                        std::uint32_t data_size,
                        VkAccessFlags dst_access_mask,
                        VkPipelineStageFlags dst_stage_mask);
    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    bool childOnWindowSizeChanged() override;
    void childClear() override;

    VulkanTutorial09Parameters m_vulkan_tutorial09_parameters;
    OrbitCamera m_camera;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL09_H
