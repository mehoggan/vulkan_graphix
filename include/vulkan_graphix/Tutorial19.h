#ifndef VULKAN_GRAPHIX_TUTORIAL19_H
#define VULKAN_GRAPHIX_TUTORIAL19_H

// Ported from vulkan_earth's Weapon: `Weapon` + its 10 concrete
// subclasses (see vulkan_earth/src/Weapon.h/.cpp and WeaponXxx.h/.cpp)
// are pure data - id, a real `description` string, `price`, `damage`,
// `radius`, `scale`, explosion colors, plus gameplay-effect virtuals -
// with no `draw()`, no `VBOShaderLibrary`, no `.ogl` reference anywhere
// (confirmed via grep across every subclass; `WeaponTest.h`'s
// `draw()` is a dead, unused abstract leftover with no concrete
// subclass). Structurally identical to `Item` - see Tutorial17.h's own
// top comment. The two real, renderable things adjacent to Weapon are:
//
//   - Projectile::draw() (vulkan_earth/src/Projectile.cpp:76-89) - looks
//     up a real 11-slot VBOShaderLibrary* mesh table by the equipped
//     weapon's uniqueidentifier and draws it scaled by that weapon's
//     real `scale` field. Most weapon ids reuse projectileDefault.ogl's
//     geometry with a swapped texture; Acid, BFB (and MFB, texture-
//     swapped from BFB), Thor, EMP, and Nuke have their own distinct
//     `.ogl` geometry. This tutorial shows three genuinely distinct
//     meshes - projectileDefault, projectileAcid, projectileBFB - each
//     scaled by its own real weapon's `scale` (60/60/100), reusing
//     Tutorial16's exact push-constant-model-matrix + shared-view/
//     projection-UBO pipeline shape. Each projectile has its own
//     dedicated texture (unlike Tutorial16's three tank parts, which
//     share one), so three descriptor sets are allocated from one
//     layout and bound in turn before each projectile's draw call -
//     the same "N sets from one layout, used in sequence" technique
//     Tutorial17 already established for its font/icon atlases.
//   - A weapon inventory grid - the same ControlItemGrid/Inventory
//     rendering Tutorial17 already ported for Item, fed the 10 real
//     shop-purchasable WeaponXxx subclasses' data instead (ids 0-9;
//     WeaponDefault/id 10 is an internal Projectile fallback, never
//     shop-purchasable - confirmed via ShopMenu.cpp only constructing
//     ids 0-9). Ten items fit a clean 5-column x 2-row grid, one more
//     column than Tutorial17's 4x2 Item grid.
//
// Both passes share one render pass (no depth buffer needed - unlike
// Tutorial18's two tanks, these three projectiles are placed side by
// side with no overlap risk under the tutorial's fixed-ish camera
// angle, the same simplification Tutorial16 itself originally used).

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "vulkan_graphix/BitmapFont.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/OrbitCamera.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/TutorialBase.h"
#include "vulkan_graphix/VertexTypes/AttributeTraits.hpp"

namespace vulkan_graphix {

// Same shape as Tutorial16VertexData.
struct Tutorial19Vertex3DData {
    Math::Vec4<float> position;
    Math::Vec2<float> texcoord;
};

using Tutorial19Vertex3DAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>, Math::Vec2<float>>;

// Same shape as Tutorial15/17/18-HUD VertexData.
struct Tutorial19VertexGridData {
    Math::Vec4<float> position;
    Math::Vec2<float> texcoord;
    Math::Vec4<float> color;
};

using Tutorial19VertexGridAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>,
                                     Math::Vec2<float>,
                                     Math::Vec4<float>>;

struct Tutorial19UniformBufferData3D {
    Math::Mat4<float> view;
    Math::Mat4<float> projection;
};

// Same shape as Tutorial16's push constant - just a model matrix, no
// color tint needed here.
struct Tutorial19PushConstants {
    Math::Mat4<float> model;
};

static constexpr std::size_t kWeaponGridItemCount = 10;
static constexpr std::size_t kProjectileMeshCount = 3;

// ************************************************************ //
// VulkanTutorial19Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
struct VulkanTutorial19Parameters {
public:
    static const std::size_t resources_count = 3;

    VulkanTutorial19Parameters();

    const VkRenderPass& getVkRenderPass() const;
    VkRenderPass& getVkRenderPass();
    void setVkRenderPass(const VkRenderPass& vk_render_pass);

    // One ImageParameters per projectile mesh (Default/Acid/BFB, in
    // that fixed order) - each has its own dedicated texture.
    const std::array<ImageParameters, kProjectileMeshCount>&
    getProjectileImageParameters() const;
    std::array<ImageParameters, kProjectileMeshCount>&
    getProjectileImageParameters();

    const ImageParameters& getFontImageParameters() const;
    ImageParameters& getFontImageParameters();
    void setFontImageParameters(const ImageParameters& image_parameters);

    const ImageParameters& getIconImageParameters() const;
    ImageParameters& getIconImageParameters();
    void setIconImageParameters(const ImageParameters& image_parameters);

    const BufferParameters& getUniformBuffer3DParameters() const;
    BufferParameters& getUniformBuffer3DParameters();
    void setUniformBuffer3DParameters(const BufferParameters& uniform_buffer);

    const BufferParameters& getUniformBufferGridParameters() const;
    BufferParameters& getUniformBufferGridParameters();
    void setUniformBufferGridParameters(
            const BufferParameters& uniform_buffer);

    const VkDescriptorSetLayout& getVk3DDescriptorSetLayout() const;
    VkDescriptorSetLayout& getVk3DDescriptorSetLayout();
    void setVk3DDescriptorSetLayout(const VkDescriptorSetLayout& other);

    const VkDescriptorSetLayout& getVkGridDescriptorSetLayout() const;
    VkDescriptorSetLayout& getVkGridDescriptorSetLayout();
    void setVkGridDescriptorSetLayout(const VkDescriptorSetLayout& other);

    const VkDescriptorPool& getVkDescriptorPool() const;
    VkDescriptorPool& getVkDescriptorPool();
    void setVkDescriptorPool(const VkDescriptorPool& other);

    // One 3D descriptor set per projectile mesh (Default/Acid/BFB).
    const std::array<VkDescriptorSet, kProjectileMeshCount>&
    get3DVkDescriptorSets() const;
    std::array<VkDescriptorSet, kProjectileMeshCount>& get3DVkDescriptorSets();

    const VkDescriptorSet& getFontVkDescriptorSet() const;
    VkDescriptorSet& getFontVkDescriptorSet();
    void setFontVkDescriptorSet(const VkDescriptorSet& other);

    const VkDescriptorSet& getIconVkDescriptorSet() const;
    VkDescriptorSet& getIconVkDescriptorSet();
    void setIconVkDescriptorSet(const VkDescriptorSet& other);

    const VkPipelineLayout& getVk3DPipelineLayout() const;
    VkPipelineLayout& getVk3DPipelineLayout();
    void setVk3DPipelineLayout(const VkPipelineLayout& other);

    const VkPipelineLayout& getVkGridPipelineLayout() const;
    VkPipelineLayout& getVkGridPipelineLayout();
    void setVkGridPipelineLayout(const VkPipelineLayout& other);

    const VkPipeline& getVk3DGraphicsPipeline() const;
    VkPipeline& getVk3DGraphicsPipeline();
    void setVk3DGraphicsPipeline(const VkPipeline& other);

    const VkPipeline& getVkGridGraphicsPipeline() const;
    VkPipeline& getVkGridGraphicsPipeline();
    void setVkGridGraphicsPipeline(const VkPipeline& other);

    // One vertex buffer + count per projectile mesh (Default/Acid/BFB).
    const std::array<BufferParameters, kProjectileMeshCount>&
    getProjectileVertexBufferParameters() const;
    std::array<BufferParameters, kProjectileMeshCount>&
    getProjectileVertexBufferParameters();
    const std::array<std::uint32_t, kProjectileMeshCount>&
    getProjectileVertexCounts() const;
    std::array<std::uint32_t, kProjectileMeshCount>& getProjectileVertexCounts();

    const BufferParameters& getGridVertexBufferParameters() const;
    BufferParameters& getGridVertexBufferParameters();
    void setGridVertexBufferParameters(const BufferParameters& vertex_buffer);
    std::uint32_t getTextVertexCount() const;
    void setTextVertexCount(std::uint32_t vertex_count);
    std::uint32_t getIconVertexCount() const;
    void setIconVertexCount(std::uint32_t vertex_count);

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
    std::array<ImageParameters, kProjectileMeshCount> m_projectile_images;
    ImageParameters m_font_image_parameters;
    ImageParameters m_icon_image_parameters;
    BufferParameters m_uniform_buffer_3d;
    BufferParameters m_uniform_buffer_grid;
    VkDescriptorSetLayout m_vk_3d_descriptor_set_layout;
    VkDescriptorSetLayout m_vk_grid_descriptor_set_layout;
    VkDescriptorPool m_vk_descriptor_pool;
    std::array<VkDescriptorSet, kProjectileMeshCount> m_vk_3d_descriptor_sets;
    VkDescriptorSet m_vk_font_descriptor_set;
    VkDescriptorSet m_vk_icon_descriptor_set;
    VkPipelineLayout m_vk_3d_pipeline_layout;
    VkPipelineLayout m_vk_grid_pipeline_layout;
    VkPipeline m_vk_3d_graphics_pipeline;
    VkPipeline m_vk_grid_graphics_pipeline;
    std::array<BufferParameters, kProjectileMeshCount>
            m_projectile_vertex_buffers;
    std::array<std::uint32_t, kProjectileMeshCount> m_projectile_vertex_counts;
    BufferParameters m_grid_vertex_buffer;
    std::uint32_t m_text_vertex_count;
    std::uint32_t m_icon_vertex_count;
    BufferParameters m_staging_buffer;
    VkCommandPool m_vk_command_pool;
    std::vector<RenderingResourceParameters> m_rendering_resources;
    std::vector<VkSemaphore> m_finished_rendering_semaphores;
};

// ************************************************************ //
// Tutorial19                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial19 : public TutorialBase {
public:
    Tutorial19();
    ~Tutorial19() override;

    bool createRenderingResources();
    bool createStagingBuffer();
    bool createProjectileTextures();
    bool createFontAtlas();
    bool createIconAtlas();
    bool createUniformBuffers();
    bool createDescriptorSetLayouts();
    bool createDescriptorPool();
    bool allocateDescriptorSets();
    bool updateDescriptorSets();
    bool createRenderPass();
    bool createPipelineLayouts();
    bool createPipelines();
    bool createProjectileVertexBuffers();
    bool createGridVertexBuffer();

    bool draw() override;

    void onMouseButton(int button,
                       bool pressed,
                       int pos_x,
                       int pos_y) override;
    void onMouseMove(int pos_x, int pos_y) override;

private:
    static constexpr std::size_t kMaxGridQuads = 512;
    static constexpr std::size_t kMaxGridVertexCount = kMaxGridQuads * 6;
    static constexpr float kFontPixelHeight = 18.0f;
    static constexpr const char* kFontPath =
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";

    static constexpr std::uint32_t kIconSize = 256;
    static constexpr std::uint32_t kIconAtlasCols = 5;
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
    bool allocateBufferMemory(VkBuffer buffer,
                              VkMemoryPropertyFlags property,
                              VkDeviceMemory* memory);
    bool createImage(std::uint32_t width,
                     std::uint32_t height,
                     VkImage* image);
    bool allocateImageMemory(VkImage image,
                             VkMemoryPropertyFlags property,
                             VkDeviceMemory* memory);
    bool createImageView(VkImage image, VkImageView* image_view);
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
    std::vector<char> buildIconAtlasPixels() const;

    Tutorial19UniformBufferData3D get3DUniformBufferData() const;
    bool update3DUniformBufferData();
    Math::Mat4<float> getGridUniformBufferData() const;
    bool updateGridUniformBufferData();

    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);

    std::vector<Tutorial19Vertex3DData> loadMeshVertexData(
            const char* mesh_filename) const;

    Math::Vec2<float> getIconUvMin(std::size_t index) const;
    Math::Vec2<float> getIconUvMax(std::size_t index) const;

    Math::Vec2<float> getPanelTopLeft() const;
    Math::Vec2<float> getPanelSize() const;
    Math::Vec2<float> getGridTopLeft() const;
    Math::Vec2<float> getGridSize() const;
    Math::Vec2<float> getCellSize() const;
    Math::Vec2<float> getCellTopLeft(std::size_t index) const;
    float getDescriptionTop() const;

    std::vector<std::string> wrapText(const std::string& text,
                                      float max_width) const;

    void appendGlyphQuad(std::vector<Tutorial19VertexGridData>& vertex_data,
                         const BitmapFontGlyphQuad& glyph,
                         Math::Vec4<float> color) const;
    void appendColoredQuad(std::vector<Tutorial19VertexGridData>& vertex_data,
                           const std::array<Math::Vec2<float>, 4>& corners,
                           Math::Vec4<float> color) const;
    void appendText(std::vector<Tutorial19VertexGridData>& vertex_data,
                    const std::string& text,
                    Math::Vec2<float> origin,
                    Math::Vec4<float> color) const;
    void appendImageQuad(std::vector<Tutorial19VertexGridData>& vertex_data,
                         Math::Vec2<float> top_left,
                         Math::Vec2<float> size,
                         Math::Vec2<float> uv_min,
                         Math::Vec2<float> uv_max) const;

    std::vector<Tutorial19VertexGridData> buildTextPassVertexData() const;
    std::vector<Tutorial19VertexGridData> buildIconPassVertexData() const;
    bool updateGridVertexBufferData();

    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    bool childOnWindowSizeChanged() override;
    void childClear() override;

    VulkanTutorial19Parameters m_vulkan_tutorial19_parameters;
    OrbitCamera m_camera;
    BitmapFont m_font;
    std::size_t m_selected_index;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL19_H
