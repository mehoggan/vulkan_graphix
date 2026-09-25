#ifndef VULKAN_GRAPHIX_TUTORIAL12_H
#define VULKAN_GRAPHIX_TUTORIAL12_H

// Ported from vulkan_earth's OpenGL->Vulkan migration pilot
// (VulkanTerrainPilot, see this repo's git history) into a proper numbered
// tutorial: a small, pilot-sized diamond-square-style generated terrain,
// Phong-lit and textured via Tutorial09's shader/UBO/depth-resource shape
// (Tutorial09 is already a lit, textured, depth-tested terrain tutorial -
// a closer match than Tutorial07 was for the skybox). The actual height-
// field generation (random-walk accumulation + box-blur smoothing) lives
// in the shared vulkan_graphix::TerrainGenerator (see TerrainGenerator.h),
// not here, so a future real vulkan_earth Vulkan port can call the same
// code instead of keeping a separate copy.

#include <cstddef>
#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/OrbitCamera.h"
#include "vulkan_graphix/TerrainGenerator.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/TutorialBase.h"
#include "vulkan_graphix/VertexTypes/AttributeTraits.hpp"

namespace vulkan_graphix {

// Matches Tutorial09VertexData's shape on purpose - this tutorial reuses
// Tutorial09's compiled shaders byte-for-byte (see createPipeline()).
struct Tutorial12VertexData {
    Math::Vec4<float> position;
    Math::Vec3<float> normal;
    Math::Vec2<float> texcoord;
};

using Tutorial12VertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>,
                                     Math::Vec3<float>,
                                     Math::Vec2<float>>;

// Matches Tutorial09UniformBufferData's shape byte-for-byte - same reason.
struct Tutorial12UniformBufferData {
    Math::Mat4<float> model;
    Math::Mat4<float> view;
    Math::Mat4<float> projection;
    Math::Vec4<float> light_position;
    Math::Vec4<float> light_color;
    Math::Vec4<float> view_position;
};

// ************************************************************ //
// VulkanTutorial12Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
struct VulkanTutorial12Parameters {
public:
    static const std::size_t resources_count = 3;

    VulkanTutorial12Parameters();

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

    std::uint32_t getVertexCount() const;
    void setVertexCount(std::uint32_t vertex_count);

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
    std::uint32_t m_vertex_count;
    BufferParameters m_staging_buffer;
    VkCommandPool m_vk_command_pool;
    std::vector<RenderingResourceParameters> m_rendering_resources;
    std::vector<VkSemaphore> m_finished_rendering_semaphores;
};

// ************************************************************ //
// Tutorial12                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial12 : public TutorialBase {
public:
    Tutorial12();
    ~Tutorial12() override;

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

    bool draw() override;

    void onMouseButton(int button,
                       bool pressed,
                       int pos_x,
                       int pos_y) override;
    void onMouseMove(int pos_x, int pos_y) override;

private:
    // Small pilot-sized stand-in for TerrainMaker's real size=256/scale=100
    // (390,150 vertices) - kept just large enough to show real diamond-
    // square-style relief while staying quick to build/render/verify.
    // Centered on the origin (OrbitCamera's fixed target) rather than
    // starting at (0,0) the way TerrainMaker's own world placement does,
    // since OrbitCamera can't be re-targeted away from the origin.
    static constexpr int kGridSize = 32;
    static constexpr int kGridScale = 1;
    static constexpr int kGenSteps = 150;
    static constexpr int kGenIncrease = 1;
    static constexpr float kGenRadius = 4.0f;
    static constexpr int kGenRandomJump = 5;
    static constexpr int kSmoothingPasses = 3;

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
    bool copyBufferData(BufferParameters& destination,
                        const void* data,
                        std::uint32_t data_size,
                        VkAccessFlags dst_access_mask,
                        VkPipelineStageFlags dst_stage_mask);
    Tutorial12UniformBufferData getUniformBufferData() const;
    bool updateUniformBufferData();
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);
    const std::vector<Tutorial12VertexData>& getVertexData();
    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    bool childOnWindowSizeChanged() override;
    void childClear() override;

    VulkanTutorial12Parameters m_vulkan_tutorial12_parameters;
    OrbitCamera m_camera;

    std::vector<Tutorial12VertexData> m_vertex_data;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL12_H
