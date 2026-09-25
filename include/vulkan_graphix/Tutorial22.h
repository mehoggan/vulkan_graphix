#ifndef VULKAN_GRAPHIX_TUTORIAL22_H
#define VULKAN_GRAPHIX_TUTORIAL22_H

// Ported from vulkan_earth's GameState menu layer - GameState itself only
// covers in-match GAME_PLAY and owns no menus (confirmed via a full read
// of GameState.h); the menu screens (MainMenu/ReadyMenu/ShopMenu/
// SubMenu*) are separate top-level classes in VulkanEarth.cpp. Every one
// of them opens with the identical 5-quad bevel-panel background
// UiGeometry::buildButtonBevel() already implements generically -
// Tutorial17/19 already call it this exact way for their own panel
// backgrounds, so that piece needs no new code here either; this
// tutorial reuses Tutorial15's exact 2D pipeline/shader (shader.
// 22_panel.{vert,frag} are textually identical to Tutorial15's own) for
// the panel/title/button.
//
// What's genuinely new is ReadyMenu's own real technique
// (ReadyMenu.cpp:909-982): a live rotating 3D tank preview rendered into
// a scissored sub-region of the screen, via its own glViewport/
// gluPerspective/gluLookAt and a `tank_angle += 0.25f` per-frame spin -
// composited inside the 2D menu panel. This tutorial reuses Tutorial16's
// exact tank pipeline/push-constant shape (shader.22_3d.{vert,frag} are
// textually identical to Tutorial16's own, and the body/head/turret
// hierarchical positioning is TankB's same real offsets/basis/scale
// Tutorial16/21 already use) but binds it with a VkViewport/VkRect2D
// confined to a sub-region of the screen instead of the full swapchain
// extent - the first time this project's viewport doesn't cover the
// whole frame - and adds one extra Y-axis rotation to each part's model
// matrix that increments every draw() call, mirroring ReadyMenu's own
// tank_angle. Unlike Tutorial16 (OrbitCamera, mouse-orbitable), this
// preview's own view is fixed, matching ReadyMenu's own static
// gluLookAt - the only motion is the tank's own spin, not the camera.
//
// One render pass, two pipelines: the 2D panel pipeline (depth disabled,
// covers the full swapchain extent, drawn first) and the 3D preview
// pipeline (real depth test/write, scissored to the sub-region, drawn
// second so it appears composited inside the panel) - same multi-
// pipeline-one-render-pass technique Tutorial18 introduced, now paired
// with a viewport that doesn't cover the whole frame.

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

// Matches Tutorial16VertexData's shape byte-for-byte.
struct Tutorial22TankVertexData {
    Math::Vec4<float> position;
    Math::Vec2<float> texcoord;
};

using Tutorial22TankVertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>, Math::Vec2<float>>;

struct Tutorial22TankUniformBufferData {
    Math::Mat4<float> view;
    Math::Mat4<float> projection;
};

// The active part's model matrix (translation/basis/scale plus the
// preview's own continuous spin), set per draw call.
struct Tutorial22TankPushConstants {
    Math::Mat4<float> model;
};

// Matches Tutorial15VertexData's shape byte-for-byte.
struct Tutorial22PanelVertexData {
    Math::Vec4<float> position;
    Math::Vec2<float> texcoord;
    Math::Vec4<float> color;
};

using Tutorial22PanelVertexAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>,
                                     Math::Vec2<float>,
                                     Math::Vec4<float>>;

static constexpr std::size_t kTutorial22TankPartCount = 3;

// ************************************************************ //
// VulkanTutorial22Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
struct VulkanTutorial22Parameters {
public:
    static const std::size_t resources_count = 3;

    VulkanTutorial22Parameters();

    const VkRenderPass& getVkRenderPass() const;
    VkRenderPass& getVkRenderPass();
    void setVkRenderPass(const VkRenderPass& vk_render_pass);

    const ImageParameters& getDepthImageParameters() const;
    ImageParameters& getDepthImageParameters();
    void setDepthImageParameters(const ImageParameters& depth_image);

    const ImageParameters& getTankImageParameters() const;
    ImageParameters& getTankImageParameters();
    void setTankImageParameters(const ImageParameters& image_parameters);

    const ImageParameters& getFontImageParameters() const;
    ImageParameters& getFontImageParameters();
    void setFontImageParameters(const ImageParameters& image_parameters);

    const BufferParameters& getTankUniformBufferParameters() const;
    BufferParameters& getTankUniformBufferParameters();
    void setTankUniformBufferParameters(const BufferParameters& uniform_buffer);

    const BufferParameters& getPanelUniformBufferParameters() const;
    BufferParameters& getPanelUniformBufferParameters();
    void setPanelUniformBufferParameters(
            const BufferParameters& uniform_buffer);

    const VkDescriptorSetLayout& getVkTankDescriptorSetLayout() const;
    VkDescriptorSetLayout& getVkTankDescriptorSetLayout();
    void setVkTankDescriptorSetLayout(const VkDescriptorSetLayout& other);

    const VkDescriptorSetLayout& getVkPanelDescriptorSetLayout() const;
    VkDescriptorSetLayout& getVkPanelDescriptorSetLayout();
    void setVkPanelDescriptorSetLayout(const VkDescriptorSetLayout& other);

    const VkDescriptorPool& getVkDescriptorPool() const;
    VkDescriptorPool& getVkDescriptorPool();
    void setVkDescriptorPool(const VkDescriptorPool& other);

    const VkDescriptorSet& getTankVkDescriptorSet() const;
    VkDescriptorSet& getTankVkDescriptorSet();
    void setTankVkDescriptorSet(const VkDescriptorSet& other);

    const VkDescriptorSet& getPanelVkDescriptorSet() const;
    VkDescriptorSet& getPanelVkDescriptorSet();
    void setPanelVkDescriptorSet(const VkDescriptorSet& other);

    const VkPipelineLayout& getVkTankPipelineLayout() const;
    VkPipelineLayout& getVkTankPipelineLayout();
    void setVkTankPipelineLayout(const VkPipelineLayout& other);

    const VkPipelineLayout& getVkPanelPipelineLayout() const;
    VkPipelineLayout& getVkPanelPipelineLayout();
    void setVkPanelPipelineLayout(const VkPipelineLayout& other);

    const VkPipeline& getVkTankGraphicsPipeline() const;
    VkPipeline& getVkTankGraphicsPipeline();
    void setVkTankGraphicsPipeline(const VkPipeline& other);

    const VkPipeline& getVkPanelGraphicsPipeline() const;
    VkPipeline& getVkPanelGraphicsPipeline();
    void setVkPanelGraphicsPipeline(const VkPipeline& other);

    const std::array<BufferParameters, kTutorial22TankPartCount>&
    getTankVertexBufferParameters() const;
    std::array<BufferParameters, kTutorial22TankPartCount>&
    getTankVertexBufferParameters();
    const std::array<std::uint32_t, kTutorial22TankPartCount>&
    getTankVertexCounts() const;
    std::array<std::uint32_t, kTutorial22TankPartCount>& getTankVertexCounts();

    // Host-visible/coherent, sized once for kMaxPanelVertexCount - rebuilt
    // and re-uploaded every frame from current UI state, same reasoning as
    // Tutorial15's own vertex buffer.
    const BufferParameters& getPanelVertexBufferParameters() const;
    BufferParameters& getPanelVertexBufferParameters();
    void setPanelVertexBufferParameters(const BufferParameters& vertex_buffer);
    std::uint32_t getPanelVertexCount() const;
    void setPanelVertexCount(std::uint32_t vertex_count);

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

    const std::vector<VkSemaphore>& getFinishedRenderingSemaphores() const;
    std::vector<VkSemaphore>& getFinishedRenderingSemaphores();
    void setFinishedRenderingSemaphores(
            const std::vector<VkSemaphore>& finished_rendering_semaphores);

private:
    VkRenderPass m_vk_render_pass;
    ImageParameters m_depth_image_parameters;
    ImageParameters m_tank_image_parameters;
    ImageParameters m_font_image_parameters;
    BufferParameters m_tank_uniform_buffer;
    BufferParameters m_panel_uniform_buffer;
    VkDescriptorSetLayout m_vk_tank_descriptor_set_layout;
    VkDescriptorSetLayout m_vk_panel_descriptor_set_layout;
    VkDescriptorPool m_vk_descriptor_pool;
    VkDescriptorSet m_vk_tank_descriptor_set;
    VkDescriptorSet m_vk_panel_descriptor_set;
    VkPipelineLayout m_vk_tank_pipeline_layout;
    VkPipelineLayout m_vk_panel_pipeline_layout;
    VkPipeline m_vk_tank_graphics_pipeline;
    VkPipeline m_vk_panel_graphics_pipeline;
    std::array<BufferParameters, kTutorial22TankPartCount>
            m_tank_vertex_buffers;
    std::array<std::uint32_t, kTutorial22TankPartCount> m_tank_vertex_counts;
    BufferParameters m_panel_vertex_buffer;
    std::uint32_t m_panel_vertex_count;
    BufferParameters m_staging_buffer;
    VkCommandPool m_vk_command_pool;
    std::vector<RenderingResourceParameters> m_rendering_resources;
    std::vector<VkSemaphore> m_finished_rendering_semaphores;
};

// ************************************************************ //
// Tutorial22                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial22 : public TutorialBase {
public:
    Tutorial22();
    ~Tutorial22() override;

    bool createRenderingResources();
    bool createStagingBuffer();
    bool createDepthResources();
    bool createTankTexture();
    bool createFontAtlas();
    bool createUniformBuffers();
    bool createDescriptorSetLayouts();
    bool createDescriptorPool();
    bool allocateDescriptorSets();
    bool updateDescriptorSets();
    bool createRenderPass();
    bool createPipelineLayouts();
    bool createPipelines();
    bool createTankVertexBuffers();
    bool createPanelVertexBuffer();

    bool draw() override;

    void onMouseButton(int button,
                       bool pressed,
                       int pos_x,
                       int pos_y) override;

private:
    // Generous fixed capacity, same reasoning as Tutorial15's own
    // kMaxQuads - a title, a framed preview border, and one button label.
    static constexpr std::size_t kMaxPanelQuads = 256;
    static constexpr std::size_t kMaxPanelVertexCount = kMaxPanelQuads * 6;
    static constexpr float kFontPixelHeight = 28.0f;
    static constexpr const char* kFontPath =
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
    // ReadyMenu.cpp's own tank_angle += 0.25f per frame.
    static constexpr float kTankSpinStepRadians = 0.25f;

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
    bool createSampler(VkSampler* sampler);
    bool copyTextureData(VkImage image,
                         char* texture_data,
                         std::uint32_t data_size,
                         std::uint32_t width,
                         std::uint32_t height);
    bool createTextureFromPixels(std::uint32_t width,
                                 std::uint32_t height,
                                 const std::vector<char>& pixels,
                                 ImageParameters& out_image_parameters);
    bool destroyDepthResources();
    bool copyBufferData(BufferParameters& destination,
                        const void* data,
                        std::uint32_t data_size,
                        VkAccessFlags dst_access_mask,
                        VkPipelineStageFlags dst_stage_mask);

    Tutorial22TankUniformBufferData getTankUniformBufferData() const;
    bool updateTankUniformBufferData();
    Math::Mat4<float> getPanelUniformBufferData() const;
    bool updatePanelUniformBufferData();

    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);

    // The sub-region of the screen the tank preview renders into, in the
    // same top-left-origin, y-down screen-pixel convention the 2D panel
    // uses - shared by prepareFrame()'s dynamic viewport/scissor and the
    // 2D panel's own frame-border bevel so they can never disagree.
    Math::Vec2<float> getPreviewTopLeft() const;
    Math::Vec2<float> getPreviewSize() const;

    // TankB's own real offsets/basis/scale (see Tutorial16.h's own top
    // comment) with one extra Y-axis rotation - this preview's own
    // continuous spin - applied around the whole assembly before its
    // real per-part translation, mirroring ReadyMenu.cpp's own
    // tank_angle-driven glRotatef call.
    std::vector<Tutorial22TankVertexData> loadTankPartVertexData(
            const char* mesh_filename) const;
    bool createTankPartVertexBuffer(const char* mesh_filename,
                                    BufferParameters& vertex_buffer,
                                    std::uint32_t& vertex_count);
    Math::Mat4<float> getTankPartModelMatrix(
            Math::Vec3<float> const& part_translation) const;

    Math::Vec2<float> getButtonTopLeft() const;
    Math::Vec2<float> getButtonSize() const;
    std::string getButtonLabel() const;

    void appendGlyphQuad(std::vector<Tutorial22PanelVertexData>& vertex_data,
                         const BitmapFontGlyphQuad& glyph,
                         Math::Vec4<float> color) const;
    void appendColoredQuad(
            std::vector<Tutorial22PanelVertexData>& vertex_data,
            const std::array<Math::Vec2<float>, 4>& corners,
            Math::Vec4<float> color) const;
    void appendText(std::vector<Tutorial22PanelVertexData>& vertex_data,
                    const std::string& text,
                    Math::Vec2<float> origin,
                    Math::Vec4<float> color) const;
    std::vector<Tutorial22PanelVertexData> buildPanelVertexData() const;
    bool updatePanelVertexBufferData();

    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    bool childOnWindowSizeChanged() override;
    void childClear() override;

    VulkanTutorial22Parameters m_vulkan_tutorial22_parameters;
    BitmapFont m_font;
    bool m_button_pressed;
    int m_click_count;
    float m_tank_angle;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL22_H
