#ifndef VULKAN_GRAPHIX_TUTORIAL16_H
#define VULKAN_GRAPHIX_TUTORIAL16_H

// Ported from vulkan_earth's real multi-part tank renderer (see
// vulkan_earth/src/Tank.h/.cpp and, for the concrete "Hellfire" tank
// this tutorial renders, vulkan_earth/src/TankB.cpp): a Tank::draw()
// independently positions and draws three mesh parts (body/head/turret
// - Tank tracks a fourth "wheel" part, but TankB never loads or draws
// one, so this tutorial doesn't either) through the same
// .ogl-mesh-plus-raw-texture pipeline Tutorial13 already ports (see
// Tools::loadOglMeshData()/Tools::getRawImageData()), all three parts
// sharing one texture (TestImage.raw) - just like TankB's own
// constructor loads it once per VBOShaderLibrary part.
//
// Tank::draw() applies each part's model matrix independently (no
// parent-child composition visible in Tank.cpp itself), so this
// tutorial renders a static "resting pose" built directly from TankB's
// own constructor data (body_offset/head_offset/turret_offset, a
// shared 50x scale, and the right/up/at basis Tank::initBody()/
// initHead()/initTurret() all set identically) rather than the live
// gameplay positioning (Player/GameState) that would normally combine
// those offsets with a moving tank's world position.
//
// Each part's model matrix is delivered as a vertex-stage push
// constant, set immediately before that part's own draw call -
// Tutorial10's LinePushConstants (a per-draw flat color) is this
// project's only other push-constant precedent; this is the first one
// carrying a model matrix.

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

// Same shape as Tutorial13VertexData on purpose: an unlit textured
// surface is exactly what these meshes need too.
struct Tutorial16VertexData {
    Math::Vec4<float> position;
    Math::Vec2<float> texcoord;
};

using Tutorial16VertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>, Math::Vec2<float>>;

// Shared by all three parts, updated once per frame as the orbit
// camera moves - see Tutorial10UniformBufferData for the same split.
struct Tutorial16UniformBufferData {
    Math::Mat4<float> view;
    Math::Mat4<float> projection;
};

// The active part's model matrix, set per draw call.
struct Tutorial16PushConstants {
    Math::Mat4<float> model;
};

// ************************************************************ //
// VulkanTutorial16Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
struct VulkanTutorial16Parameters {
public:
    static const std::size_t resources_count = 3;

    VulkanTutorial16Parameters();

    const VkRenderPass& getVkRenderPass() const;
    VkRenderPass& getVkRenderPass();
    void setVkRenderPass(const VkRenderPass& vk_render_pass);

    const ImageParameters& getImageParameters() const;
    ImageParameters& getImageParameters();
    void setImageParameters(const ImageParameters& image_parameters);

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

    const BufferParameters& getBodyVertexBufferParameters() const;
    BufferParameters& getBodyVertexBufferParameters();
    void setBodyVertexBufferParameters(const BufferParameters& vertex_buffer);
    std::uint32_t getBodyVertexCount() const;
    void setBodyVertexCount(std::uint32_t vertex_count);

    const BufferParameters& getHeadVertexBufferParameters() const;
    BufferParameters& getHeadVertexBufferParameters();
    void setHeadVertexBufferParameters(const BufferParameters& vertex_buffer);
    std::uint32_t getHeadVertexCount() const;
    void setHeadVertexCount(std::uint32_t vertex_count);

    const BufferParameters& getTurretVertexBufferParameters() const;
    BufferParameters& getTurretVertexBufferParameters();
    void setTurretVertexBufferParameters(
            const BufferParameters& vertex_buffer);
    std::uint32_t getTurretVertexCount() const;
    void setTurretVertexCount(std::uint32_t vertex_count);

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
    ImageParameters m_image_parameters;
    BufferParameters m_uniform_buffer;
    DescriptorSetParameters m_descriptor_set_parameters;
    VkPipelineLayout m_vk_pipeline_layout;
    VkPipeline m_vk_graphics_pipeline;
    BufferParameters m_body_vertex_buffer;
    std::uint32_t m_body_vertex_count;
    BufferParameters m_head_vertex_buffer;
    std::uint32_t m_head_vertex_count;
    BufferParameters m_turret_vertex_buffer;
    std::uint32_t m_turret_vertex_count;
    BufferParameters m_staging_buffer;
    VkCommandPool m_vk_command_pool;
    std::vector<RenderingResourceParameters> m_rendering_resources;
    std::vector<VkSemaphore> m_finished_rendering_semaphores;
};

// ************************************************************ //
// Tutorial16                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial16 : public TutorialBase {
public:
    Tutorial16();
    ~Tutorial16() override;

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
    bool createVertexBuffers();

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
    Tutorial16UniformBufferData getUniformBufferData() const;
    bool updateUniformBufferData();
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);
    std::vector<Tutorial16VertexData> loadPartVertexData(
            const char* mesh_filename) const;
    bool createPartVertexBuffer(const char* mesh_filename,
                                BufferParameters& vertex_buffer,
                                std::uint32_t& vertex_count);

    // TankB's own constructor values (body_offset/head_offset/
    // turret_offset, a shared 50x scale) combined with the right/up/at
    // basis Tank::initBody()/initHead()/initTurret() all set identically
    // - see this header's own top comment.
    Math::Mat4<float> getBodyModelMatrix() const;
    Math::Mat4<float> getHeadModelMatrix() const;
    Math::Mat4<float> getTurretModelMatrix() const;

    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    bool childOnWindowSizeChanged() override;
    void childClear() override;

    VulkanTutorial16Parameters m_vulkan_tutorial16_parameters;
    OrbitCamera m_camera;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL16_H
