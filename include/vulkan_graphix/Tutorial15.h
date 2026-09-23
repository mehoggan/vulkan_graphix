#ifndef VULKAN_GRAPHIX_TUTORIAL15_H
#define VULKAN_GRAPHIX_TUTORIAL15_H

// A minimal 2D UI: a text title and one clickable button with a text
// label, demonstrating the two primitives vulkan_earth's whole menu
// system (MainMenu/SubMenu*/ReadyMenu/ShopMenu/ControlItem*) is built
// from - text (BitmapFont.h) and a beveled button face
// (UiGeometry::buildButtonBevel(), ported from
// vulkan_earth/src/ControlItemButton.cpp) - without porting that entire
// class hierarchy. Both of those pieces are pure CPU geometry/layout
// logic with no Vulkan coupling, so this tutorial's own job is just:
// upload the font atlas as a texture, and every frame, rebuild a small
// CPU vertex list from current UI state (title + button bevel + button
// label) and re-upload it into a host-visible vertex buffer - the
// correct Vulkan pattern for small, per-frame-dynamic geometry, the same
// way every other tutorial already maps/memcpy's its uniform buffer each
// frame rather than staging it.

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vulkan_graphix/BitmapFont.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/TutorialBase.h"
#include "vulkan_graphix/VertexTypes/AttributeTraits.hpp"

namespace vulkan_graphix {

struct Tutorial15VertexData {
    Math::Vec4<float> position;
    Math::Vec2<float> texcoord;
    Math::Vec4<float> color;
};

using Tutorial15VertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>,
                                     Math::Vec2<float>,
                                     Math::Vec4<float>>;

// ************************************************************ //
// VulkanTutorial15Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
struct VulkanTutorial15Parameters {
public:
    static const std::size_t resources_count = 3;

    VulkanTutorial15Parameters();

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

    // Host-visible/coherent, sized once for kMaxVertexCount - see
    // Tutorial15.h's own comment and updateVertexBufferData().
    const BufferParameters& getVertexBufferParameters() const;
    BufferParameters& getVertexBufferParameters();
    void setVertexBufferParameters(const BufferParameters& vertex_buffer);

    std::uint32_t getVertexCount() const;
    void setVertexCount(std::uint32_t vertex_count);

    // Only used once, to upload the font atlas texture.
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
    BufferParameters m_vertex_buffer;
    std::uint32_t m_vertex_count;
    BufferParameters m_staging_buffer;
    VkCommandPool m_vk_command_pool;
    std::vector<RenderingResourceParameters> m_rendering_resources;
    std::vector<VkSemaphore> m_finished_rendering_semaphores;
};

// ************************************************************ //
// Tutorial15                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial15 : public TutorialBase {
public:
    Tutorial15();
    ~Tutorial15() override;

    bool createRenderingResources();
    bool createStagingBuffer();
    bool createFontAtlas();
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

private:
    // Generous fixed capacity - a title plus one short button label is a
    // few dozen quads at most.
    static constexpr std::size_t kMaxQuads = 256;
    static constexpr std::size_t kMaxVertexCount = kMaxQuads * 6;
    static constexpr float kFontPixelHeight = 28.0f;
    // fonts-dejavu-core (see CLAUDE.md's Initial Setup) provides this.
    static constexpr const char* kFontPath =
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";

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
    Math::Mat4<float> getUniformBufferData() const;
    bool updateUniformBufferData();
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);

    // Layout: button top-left/size in the same screen-pixel, top-left-
    // origin, y-down convention the vertex/projection setup uses -
    // shared between rendering and onMouseButton()'s hit test so they
    // can never disagree.
    Math::Vec2<float> getButtonTopLeft() const;
    Math::Vec2<float> getButtonSize() const;
    std::string getButtonLabel() const;

    void appendGlyphQuad(std::vector<Tutorial15VertexData>& vertex_data,
                         const BitmapFontGlyphQuad& glyph,
                         Math::Vec4<float> color) const;
    void appendColoredQuad(std::vector<Tutorial15VertexData>& vertex_data,
                           const std::array<Math::Vec2<float>, 4>& corners,
                           Math::Vec4<float> color) const;
    void appendText(std::vector<Tutorial15VertexData>& vertex_data,
                    const std::string& text,
                    Math::Vec2<float> origin,
                    Math::Vec4<float> color) const;
    std::vector<Tutorial15VertexData> buildUiVertexData() const;
    bool updateVertexBufferData();

    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    bool childOnWindowSizeChanged() override;
    void childClear() override;

    VulkanTutorial15Parameters m_vulkan_tutorial15_parameters;
    BitmapFont m_font;
    bool m_button_pressed;
    int m_click_count;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL15_H
