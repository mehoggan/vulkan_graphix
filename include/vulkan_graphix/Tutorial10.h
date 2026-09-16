#ifndef VULKAN_GRAPHIX_TUTORIAL10_H
#define VULKAN_GRAPHIX_TUTORIAL10_H

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
// LineVertexData                                               //
//                                                              //
// One vertex layout for both things this tutorial draws with   //
// LINE_STRIP: the sampled curve itself (a real polyline, not a //
// swept mesh) and its control polygon.                         //
// ************************************************************ //
struct LineVertexData {
    Math::Vec4<float> position;
};

using LineVertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>>;

// ************************************************************ //
// UniformBufferData                                            //
//                                                              //
// No model matrix: both line strips are generated directly in  //
// world space. No lighting fields either - flat-colored lines  //
// have no surface to light.                                    //
// ************************************************************ //
struct UniformBufferData {
    Math::Mat4<float> view;
    Math::Mat4<float> projection;
};

// ************************************************************ //
// LinePushConstants                                            //
//                                                              //
// The active line strip's flat color, set per draw call.       //
// ************************************************************ //
struct LinePushConstants {
    Math::Vec4<float> color;
};

// ************************************************************ //
// VulkanTutorial10Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
class VulkanTutorial10Parameters {
public:
    static const std::size_t resources_count = 3;

    VulkanTutorial10Parameters();

    const VkRenderPass& getVkRenderPass() const;
    VkRenderPass& getVkRenderPass();
    void setVkRenderPass(const VkRenderPass& vk_render_pass);

    const ImageParameters& getDepthImageParameters() const;
    ImageParameters& getDepthImageParameters();
    void setDepthImageParameters(const ImageParameters& depth_image);

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

    const VkPipeline& getVkLinePipeline() const;
    VkPipeline& getVkLinePipeline();
    void setVkLinePipeline(const VkPipeline& vk_line_pipeline);

    const BufferParameters& getCurveVertexBufferParameters() const;
    BufferParameters& getCurveVertexBufferParameters();
    void setCurveVertexBufferParameters(const BufferParameters& vertex_buffer);

    std::uint32_t getCurveVertexCount() const;
    void setCurveVertexCount(std::uint32_t vertex_count);

    const BufferParameters& getControlPolygonVertexBufferParameters() const;
    BufferParameters& getControlPolygonVertexBufferParameters();
    void setControlPolygonVertexBufferParameters(
            const BufferParameters& vertex_buffer);

    std::uint32_t getControlPolygonVertexCount() const;
    void setControlPolygonVertexCount(std::uint32_t vertex_count);

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
    BufferParameters m_uniform_buffer;
    DescriptorSetParameters m_descriptor_set_parameters;
    VkPipelineLayout m_vk_pipeline_layout;
    VkPipeline m_vk_line_pipeline;
    BufferParameters m_curve_vertex_buffer;
    std::uint32_t m_curve_vertex_count;
    BufferParameters m_control_polygon_vertex_buffer;
    std::uint32_t m_control_polygon_vertex_count;
    BufferParameters m_staging_buffer;
    VkCommandPool m_vk_command_pool;
    std::vector<RenderingResourceParameters> m_rendering_resources;
    std::vector<VkSemaphore> m_finished_rendering_semaphores;
};

// ************************************************************ //
// Tutorial10                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial10 : public TutorialBase {
public:
    Tutorial10();
    ~Tutorial10() override;

    bool createRenderingResources();
    bool createStagingBuffer();
    bool createDepthResources();
    bool createUniformBuffer();
    bool createDescriptorSetLayout();
    bool createDescriptorPool();
    bool allocateDescriptorSet();
    bool updateDescriptorSet();
    bool createRenderPass();
    bool createPipelineLayout();
    bool createLinePipeline();
    bool createCurveVertexBuffer();
    bool createControlPolygonVertexBuffer();

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
    bool destroyDepthResources();
    UniformBufferData getUniformBufferData() const;
    bool updateUniformBufferData();
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);
    const std::vector<LineVertexData>& getCurveVertexData() const;
    const std::vector<LineVertexData>& getControlPolygonVertexData() const;
    float getLineWidth() const;
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

    VulkanTutorial10Parameters m_vulkan_tutorial10_parameters;
    OrbitCamera m_camera;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL10_H
