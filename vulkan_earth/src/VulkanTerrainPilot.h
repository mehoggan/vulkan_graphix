#ifndef VULKAN_EARTH_VULKAN_TERRAIN_PILOT_H
#define VULKAN_EARTH_VULKAN_TERRAIN_PILOT_H

// Standalone Vulkan-migration pilot #2 for vulkan_earth (see
// VulkanSkyboxPilot.h for pilot #1 and the pattern this follows). Ports
// TerrainMaker's own CPU-side height-field generation (terrainGen/
// terrainSmoothe/calcNormal/smoothShadeNormal, none of which make any GL
// calls - see TerrainMaker.cpp) at a small, pilot-sized grid, then renders
// it Phong-lit and textured the way Tutorial09 does its own tessellated
// terrain - reusing Tutorial09's compiled shaders/UBO shape/depth-resource
// pattern rather than Tutorial07's (unlike the skybox pilot), since
// Tutorial09 is already a lit, textured, depth-tested terrain tutorial.
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

// Matches Tutorial09VertexData's shape on purpose: this pilot reuses
// Tutorial09's compiled shaders byte-for-byte (see createPipeline()), so
// the vertex layout has to match what those shaders were compiled against.
struct TerrainPilotVertexData {
    Math::Vec4<float> position;
    Math::Vec3<float> normal;
    Math::Vec2<float> texcoord;
};

using TerrainPilotVertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>,
                                     Math::Vec3<float>,
                                     Math::Vec2<float>>;

// Matches Tutorial09UniformBufferData's shape byte-for-byte - same reason.
struct TerrainPilotUniformBufferData {
    Math::Mat4<float> model;
    Math::Mat4<float> view;
    Math::Mat4<float> projection;
    Math::Vec4<float> light_position;
    Math::Vec4<float> light_color;
    Math::Vec4<float> view_position;
};

class VulkanTerrainPilot : public TutorialBase {
public:
    VulkanTerrainPilot();
    ~VulkanTerrainPilot() override;

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
    bool copyBufferData(BufferParameters& destination,
                        const void* data,
                        std::uint32_t data_size,
                        VkAccessFlags dst_access_mask,
                        VkPipelineStageFlags dst_stage_mask);
    TerrainPilotUniformBufferData getUniformBufferData() const;
    bool updateUniformBufferData();
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);
    const std::vector<TerrainPilotVertexData>& getVertexData();
    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    // Ported from TerrainMaker.cpp verbatim (same index conventions per
    // function, even where they're internally inconsistent with each other
    // in the original - see terrainSmoothe() vs. calcNormal()'s th_[][]
    // index order). None of these touch GL; this project's own AttributeTraits
    // -style interleaved vertex layout is built from their output afterward,
    // in getVertexData(), rather than TerrainMaker's separate VBO blocks.
    void prepTerrain();
    void terrainGen(int new_steps,
                    int new_increase,
                    float new_radius,
                    int new_random_jump);
    void terrainSmoothe(int box_width);
    void calcNormal(int x, int z, int flag, Math::Vec3<float>* normal) const;
    void smoothShadeNormal(int x, int z, Math::Vec3<float>* normal) const;

    bool childOnWindowSizeChanged() override;
    void childClear() override;

    static const std::size_t resources_count_ = 3;

    VkRenderPass vk_render_pass_;
    ImageParameters depth_image_parameters_;
    ImageParameters texture_image_parameters_;
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

    std::vector<std::vector<int>> terrain_heights_;
    std::vector<TerrainPilotVertexData> vertex_data_;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_EARTH_VULKAN_TERRAIN_PILOT_H
