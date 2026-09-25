#ifndef VULKAN_GRAPHIX_TUTORIAL17_H
#define VULKAN_GRAPHIX_TUTORIAL17_H

// Ported from vulkan_earth's Inventory (see vulkan_earth/src/Inventory.h/
// .cpp): a titled bevel-panel grid of item icons with per-slot remaining-
// count labels and a click-to-select description, using the real data
// from all 8 concrete Item subclasses (vulkan_earth/src/ItemXxx.cpp) -
// name/description/price/remaining come straight from those constructors,
// not fabricated. Item itself (see Item.h) has no draw() method at all -
// it's a pure data record - so there is nothing to port for Item beyond
// supplying that real data here; Inventory's ControlItemGrid panel (a
// bevel-bordered box of cells - the same five-quad raised/pressed bevel
// UiGeometry::buildButtonBevel() already ports, confirmed against
// ControlItemGrid::draw()'s own 0.75-based colors) plus its title/
// explain/descript text (BitmapFont, exactly like Tutorial15) is the
// real rendering target.
//
// Two textures are needed - a BitmapFont glyph atlas (all the panel's
// text) and a combined icon atlas (all 8 item icons, stitched from the
// real ItemXxx.raw 256x256 assets by buildIconAtlas(), tutorial-local
// plumbing around this fixed 8-icon set rather than a new shared
// component) - kept behind ONE pipeline/descriptor-set-layout shape
// (matching every other tutorial's one-texture-per-descriptor-set
// precedent) by using TWO descriptor sets from that one layout and
// drawing in two passes: bind the font set, draw every flat-color/text
// quad (panel bevel, title/explain/descript, per-cell labels, the
// selected-cell highlight); bind the icon set, draw the 8 icon quads on
// top of that (see prepareFrame()).

#include <array>
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

// Same shape as Tutorial15VertexData on purpose: reuses that tutorial's
// exact 2D alpha-blended pipeline shape (new resources/17/Data/shader.17.
// {vert,frag}, textually identical to Tutorial15's).
struct Tutorial17VertexData {
    Math::Vec4<float> position;
    Math::Vec2<float> texcoord;
    Math::Vec4<float> color;
};

using Tutorial17VertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>,
                                     Math::Vec2<float>,
                                     Math::Vec4<float>>;

static constexpr std::size_t kInventoryItemCount = 8;

// ************************************************************ //
// VulkanTutorial17Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
struct VulkanTutorial17Parameters {
public:
    static const std::size_t resources_count = 3;

    VulkanTutorial17Parameters();

    const VkRenderPass& getVkRenderPass() const;
    VkRenderPass& getVkRenderPass();
    void setVkRenderPass(const VkRenderPass& vk_render_pass);

    const ImageParameters& getFontImageParameters() const;
    ImageParameters& getFontImageParameters();
    void setFontImageParameters(const ImageParameters& image_parameters);

    const ImageParameters& getIconImageParameters() const;
    ImageParameters& getIconImageParameters();
    void setIconImageParameters(const ImageParameters& image_parameters);

    const BufferParameters& getUniformBufferParameters() const;
    BufferParameters& getUniformBufferParameters();
    void setUniformBufferParameters(const BufferParameters& uniform_buffer);

    // One shared layout/pool, two sets allocated from it (see this
    // header's top comment) - not two full DescriptorSetParameters,
    // which would duplicate the layout/pool for no reason.
    const VkDescriptorSetLayout& getVkDescriptorSetLayout() const;
    VkDescriptorSetLayout& getVkDescriptorSetLayout();
    void setVkDescriptorSetLayout(const VkDescriptorSetLayout& other);

    const VkDescriptorPool& getVkDescriptorPool() const;
    VkDescriptorPool& getVkDescriptorPool();
    void setVkDescriptorPool(const VkDescriptorPool& other);

    const VkDescriptorSet& getFontVkDescriptorSet() const;
    VkDescriptorSet& getFontVkDescriptorSet();
    void setFontVkDescriptorSet(const VkDescriptorSet& other);

    const VkDescriptorSet& getIconVkDescriptorSet() const;
    VkDescriptorSet& getIconVkDescriptorSet();
    void setIconVkDescriptorSet(const VkDescriptorSet& other);

    const VkPipelineLayout& getVkPipelineLayout() const;
    VkPipelineLayout& getVkPipelineLayout();
    void setVkPipelineLayout(const VkPipelineLayout& vk_pipeline_layout);

    const VkPipeline& getVkGraphicsPipeline() const;
    VkPipeline& getVkGraphicsPipeline();
    void setVkGraphicsPipeline(const VkPipeline& vk_graphics_pipeline);

    // Host-visible/coherent, sized once for kMaxVertexCount - holds both
    // passes' quads back-to-back (text-pass range first, icon-pass range
    // second) - see updateVertexBufferData().
    const BufferParameters& getVertexBufferParameters() const;
    BufferParameters& getVertexBufferParameters();
    void setVertexBufferParameters(const BufferParameters& vertex_buffer);

    std::uint32_t getTextVertexCount() const;
    void setTextVertexCount(std::uint32_t vertex_count);
    std::uint32_t getIconVertexCount() const;
    void setIconVertexCount(std::uint32_t vertex_count);

    // Only used twice, to upload the font atlas and then the icon atlas.
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
    ImageParameters m_font_image_parameters;
    ImageParameters m_icon_image_parameters;
    BufferParameters m_uniform_buffer;
    VkDescriptorSetLayout m_vk_descriptor_set_layout;
    VkDescriptorPool m_vk_descriptor_pool;
    VkDescriptorSet m_vk_font_descriptor_set;
    VkDescriptorSet m_vk_icon_descriptor_set;
    VkPipelineLayout m_vk_pipeline_layout;
    VkPipeline m_vk_graphics_pipeline;
    BufferParameters m_vertex_buffer;
    std::uint32_t m_text_vertex_count;
    std::uint32_t m_icon_vertex_count;
    BufferParameters m_staging_buffer;
    VkCommandPool m_vk_command_pool;
    std::vector<RenderingResourceParameters> m_rendering_resources;
    std::vector<VkSemaphore> m_finished_rendering_semaphores;
};

// ************************************************************ //
// Tutorial17                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial17 : public TutorialBase {
public:
    Tutorial17();
    ~Tutorial17() override;

    bool createRenderingResources();
    bool createStagingBuffer();
    bool createFontAtlas();
    bool createIconAtlas();
    bool createUniformBuffer();
    bool createDescriptorSetLayout();
    bool createDescriptorPool();
    bool allocateDescriptorSets();
    bool updateDescriptorSets();
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
    // A title/explain line, a (possibly multi-line, wrapped) description,
    // an 8-cell grid's labels, a panel bevel, and a selection highlight
    // is comfortably under this - generous headroom kept anyway, same
    // reasoning as Tutorial15's own kMaxQuads.
    static constexpr std::size_t kMaxQuads = 512;
    static constexpr std::size_t kMaxVertexCount = kMaxQuads * 6;
    static constexpr float kFontPixelHeight = 20.0f;
    static constexpr const char* kFontPath =
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";

    static constexpr std::uint32_t kIconSize = 256;
    static constexpr std::uint32_t kIconAtlasCols = 4;
    static constexpr std::uint32_t kIconAtlasRows = 2;
    static constexpr std::uint32_t kIconAtlasWidth = kIconSize * kIconAtlasCols;
    static constexpr std::uint32_t kIconAtlasHeight =
            kIconSize * kIconAtlasRows;

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
    // Shared by createFontAtlas()/createIconAtlas(): creates the image,
    // memory, view, sampler, and uploads pixels, all in one call.
    bool createTextureFromPixels(std::uint32_t width,
                                 std::uint32_t height,
                                 const std::vector<char>& pixels,
                                 ImageParameters& out_image_parameters);
    std::vector<char> buildIconAtlasPixels() const;

    Math::Mat4<float> getUniformBufferData() const;
    bool updateUniformBufferData();
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);

    // Layout - shared between rendering and onMouseButton()'s hit test,
    // same top-left-origin/y-down screen-pixel convention Tutorial15
    // uses, so the two can never disagree.
    Math::Vec2<float> getPanelTopLeft() const;
    Math::Vec2<float> getPanelSize() const;
    Math::Vec2<float> getGridTopLeft() const;
    Math::Vec2<float> getGridSize() const;
    Math::Vec2<float> getCellSize() const;
    Math::Vec2<float> getCellTopLeft(std::size_t index) const;
    float getDescriptionTop() const;

    Math::Vec2<float> getIconUvMin(std::size_t index) const;
    Math::Vec2<float> getIconUvMax(std::size_t index) const;

    std::vector<std::string> wrapText(const std::string& text,
                                      float max_width) const;

    void appendGlyphQuad(std::vector<Tutorial17VertexData>& vertex_data,
                         const BitmapFontGlyphQuad& glyph,
                         Math::Vec4<float> color) const;
    void appendColoredQuad(std::vector<Tutorial17VertexData>& vertex_data,
                           const std::array<Math::Vec2<float>, 4>& corners,
                           Math::Vec4<float> color) const;
    void appendText(std::vector<Tutorial17VertexData>& vertex_data,
                    const std::string& text,
                    Math::Vec2<float> origin,
                    Math::Vec4<float> color) const;
    void appendImageQuad(std::vector<Tutorial17VertexData>& vertex_data,
                         Math::Vec2<float> top_left,
                         Math::Vec2<float> size,
                         Math::Vec2<float> uv_min,
                         Math::Vec2<float> uv_max) const;

    std::vector<Tutorial17VertexData> buildTextPassVertexData() const;
    std::vector<Tutorial17VertexData> buildIconPassVertexData() const;
    bool updateVertexBufferData();

    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    bool childOnWindowSizeChanged() override;
    void childClear() override;

    VulkanTutorial17Parameters m_vulkan_tutorial17_parameters;
    BitmapFont m_font;
    std::size_t m_selected_index;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL17_H
