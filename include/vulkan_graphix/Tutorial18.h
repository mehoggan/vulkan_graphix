#ifndef VULKAN_GRAPHIX_TUTORIAL18_H
#define VULKAN_GRAPHIX_TUTORIAL18_H

// Ported from vulkan_earth's Player: `Player`/`PlayerHuman`/`PlayerCPU`
// (see vulkan_earth/src/Player.h/.cpp, PlayerHuman.h/.cpp,
// PlayerCPU.h/.cpp) have no draw()/rendering code of their own at all -
// confirmed by grep across all three, the only hits are a dead, empty
// `drawHUD(){}` free-function stub and a debug-only line/plane
// visualizer that never touches a Tank. The two real, renderable things
// Player is adjacent to are:
//
//   - Tank::setTankPos(x,y,z) (vulkan_earth/src/Tank.cpp:73-105) - the
//     real per-frame world-positioning algorithm, which this tutorial
//     ports faithfully (see the fix applied to Tutorial16 alongside
//     this tutorial: setTankPos() composes hierarchically - a child
//     part's offset is rotated through its *parent's* own basis columns
//     before being added to the parent's translation, not added
//     directly to a common origin).
//   - GameState::drawHUD() (vulkan_earth/src/GameState.cpp:516-772) -
//     the real per-player HUD: name text tinted by that player's real
//     `color[4]`, an HP number plus a health bar using the real
//     `glColor3f(1 - ratio, ratio, 0)` color ramp, and a power number
//     plus a power bar using the real (inverted) `glColor3f(ratio,
//     1 - ratio, 0)` ramp. Not ported: the wait-counter, weapon-slot
//     icon, "previous power/angle" ghost text, and status-effect
//     (shield/acid) name-color overrides - real but deep turn-state,
//     the same kind of cut Tutorial16 already made for live gameplay
//     reorientation. HP/power values here are static illustrative
//     snapshots (one near-full-health tank, one damaged, to show the
//     ramp) rather than a live-simulated turn - "resting pose, not
//     live simulation", same as Tank.
//
// Two Hellfire tanks (reusing Tutorial16's already-staged
// Hellfire_Body/Head/Turret.ogl + TestImage.raw - see bin/Makefile.am)
// are placed at two world positions via that real setTankPos()
// composition and tinted by an illustrative per-player team color
// (Player::color[4] is a real field with no fixed default in the
// source - the specific red/blue chosen here is this tutorial's own
// call). This needs a real depth buffer (unlike Tutorial16's single
// clustered object) since two independent 3D objects under a freely
// orbiting camera need real depth testing to composite correctly.
//
// The HUD is a second pipeline in the *same* render pass, with depth
// test/write disabled, drawn after the 3D pass - this mirrors
// GameState::drawHUD()'s own glDisable(GL_DEPTH_TEST)/draw HUD/
// glEnable(GL_DEPTH_TEST) bracketing, adapted to Vulkan's per-pipeline
// depth state instead of GL's global mode switch. It reuses
// BitmapFont/UiGeometry exactly as Tutorial15/17 do.

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

// Same shape as Tutorial16VertexData - the two tanks reuse Tutorial16's
// exact unlit-textured mesh pipeline shape.
struct Tutorial18Vertex3DData {
    Math::Vec4<float> position;
    Math::Vec2<float> texcoord;
};

using Tutorial18Vertex3DAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>, Math::Vec2<float>>;

// Same shape as Tutorial15/17VertexData - the HUD reuses that exact 2D
// alpha-blended pipeline shape.
struct Tutorial18VertexHudData {
    Math::Vec4<float> position;
    Math::Vec2<float> texcoord;
    Math::Vec4<float> color;
};

using Tutorial18VertexHudAttributeTraits =
        VertexTypes::AttributeTraits<Math::Vec4<float>,
                                     Math::Vec2<float>,
                                     Math::Vec4<float>>;

struct Tutorial18UniformBufferData3D {
    Math::Mat4<float> view;
    Math::Mat4<float> projection;
};

// Per-tank-instance model matrix and team-color tint, set immediately
// before each part's draw call - extends Tutorial16's own Mat4-only
// push constant (Tutorial16PushConstants) with a Vec4 tint.
struct Tutorial18PushConstants {
    Math::Mat4<float> model;
    Math::Vec4<float> color;
};

// One player's real+illustrative HUD data (see this header's own top
// comment for which fields are real vs. this tutorial's own static
// snapshot choice).
struct Tutorial18PlayerInfo {
    Math::Vec3<float> world_position;
    Math::Vec4<float> team_color;
    std::string name;
    int hp;
    int max_hp;
    float power_ratio;
};

// ************************************************************ //
// VulkanTutorial18Parameters                                   //
//                                                              //
// Vulkan specific parameters                                   //
// ************************************************************ //
struct VulkanTutorial18Parameters {
public:
    static const std::size_t resources_count = 3;

    VulkanTutorial18Parameters();

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

    const BufferParameters& getUniformBuffer3DParameters() const;
    BufferParameters& getUniformBuffer3DParameters();
    void setUniformBuffer3DParameters(const BufferParameters& uniform_buffer);

    const BufferParameters& getUniformBufferHudParameters() const;
    BufferParameters& getUniformBufferHudParameters();
    void setUniformBufferHudParameters(const BufferParameters& uniform_buffer);

    const VkDescriptorSetLayout& getVk3DDescriptorSetLayout() const;
    VkDescriptorSetLayout& getVk3DDescriptorSetLayout();
    void setVk3DDescriptorSetLayout(const VkDescriptorSetLayout& other);

    const VkDescriptorSetLayout& getVkHudDescriptorSetLayout() const;
    VkDescriptorSetLayout& getVkHudDescriptorSetLayout();
    void setVkHudDescriptorSetLayout(const VkDescriptorSetLayout& other);

    const VkDescriptorPool& getVkDescriptorPool() const;
    VkDescriptorPool& getVkDescriptorPool();
    void setVkDescriptorPool(const VkDescriptorPool& other);

    const VkDescriptorSet& get3DVkDescriptorSet() const;
    VkDescriptorSet& get3DVkDescriptorSet();
    void set3DVkDescriptorSet(const VkDescriptorSet& other);

    const VkDescriptorSet& getHudVkDescriptorSet() const;
    VkDescriptorSet& getHudVkDescriptorSet();
    void setHudVkDescriptorSet(const VkDescriptorSet& other);

    const VkPipelineLayout& getVk3DPipelineLayout() const;
    VkPipelineLayout& getVk3DPipelineLayout();
    void setVk3DPipelineLayout(const VkPipelineLayout& other);

    const VkPipelineLayout& getVkHudPipelineLayout() const;
    VkPipelineLayout& getVkHudPipelineLayout();
    void setVkHudPipelineLayout(const VkPipelineLayout& other);

    const VkPipeline& getVk3DGraphicsPipeline() const;
    VkPipeline& getVk3DGraphicsPipeline();
    void setVk3DGraphicsPipeline(const VkPipeline& other);

    const VkPipeline& getVkHudGraphicsPipeline() const;
    VkPipeline& getVkHudGraphicsPipeline();
    void setVkHudGraphicsPipeline(const VkPipeline& other);

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

    // Host-visible/coherent, rebuilt every frame from CPU-side HUD state
    // (matches Tutorial15/17's own vertex buffer).
    const BufferParameters& getHudVertexBufferParameters() const;
    BufferParameters& getHudVertexBufferParameters();
    void setHudVertexBufferParameters(const BufferParameters& vertex_buffer);
    std::uint32_t getHudVertexCount() const;
    void setHudVertexCount(std::uint32_t vertex_count);

    // Reused sequentially for two one-time uploads: TestImage.raw and
    // the font atlas.
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
    ImageParameters m_tank_image_parameters;
    ImageParameters m_font_image_parameters;
    BufferParameters m_uniform_buffer_3d;
    BufferParameters m_uniform_buffer_hud;
    VkDescriptorSetLayout m_vk_3d_descriptor_set_layout;
    VkDescriptorSetLayout m_vk_hud_descriptor_set_layout;
    VkDescriptorPool m_vk_descriptor_pool;
    VkDescriptorSet m_vk_3d_descriptor_set;
    VkDescriptorSet m_vk_hud_descriptor_set;
    VkPipelineLayout m_vk_3d_pipeline_layout;
    VkPipelineLayout m_vk_hud_pipeline_layout;
    VkPipeline m_vk_3d_graphics_pipeline;
    VkPipeline m_vk_hud_graphics_pipeline;
    BufferParameters m_body_vertex_buffer;
    std::uint32_t m_body_vertex_count;
    BufferParameters m_head_vertex_buffer;
    std::uint32_t m_head_vertex_count;
    BufferParameters m_turret_vertex_buffer;
    std::uint32_t m_turret_vertex_count;
    BufferParameters m_hud_vertex_buffer;
    std::uint32_t m_hud_vertex_count;
    BufferParameters m_staging_buffer;
    VkCommandPool m_vk_command_pool;
    std::vector<RenderingResourceParameters> m_rendering_resources;
    std::vector<VkSemaphore> m_finished_rendering_semaphores;
};

// ************************************************************ //
// Tutorial18                                                   //
//                                                              //
// Class for presenting Vulkan usage topics                     //
// ************************************************************ //
class Tutorial18 : public TutorialBase {
public:
    Tutorial18();
    ~Tutorial18() override;

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
    bool createHudVertexBuffer();

    bool draw() override;

    void onMouseButton(int button,
                       bool pressed,
                       int pos_x,
                       int pos_y) override;
    void onMouseMove(int pos_x, int pos_y) override;

private:
    static constexpr std::size_t kMaxHudQuads = 256;
    static constexpr std::size_t kMaxHudVertexCount = kMaxHudQuads * 6;
    static constexpr float kFontPixelHeight = 18.0f;
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

    Tutorial18UniformBufferData3D get3DUniformBufferData() const;
    bool update3DUniformBufferData();
    Math::Mat4<float> getHudUniformBufferData() const;
    bool updateHudUniformBufferData();

    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
    createShaderModule(const char* filename);

    std::vector<Tutorial18Vertex3DData> loadPartVertexData(
            const char* mesh_filename) const;
    bool createPartVertexBuffer(const char* mesh_filename,
                                BufferParameters& vertex_buffer,
                                std::uint32_t& vertex_count);

    // The two players' real+illustrative HUD/position data - see
    // Tutorial18.h's top comment.
    const std::array<Tutorial18PlayerInfo, 2>& getPlayers() const;

    // TankB's real body/head/turret offsets, composed hierarchically for
    // a tank placed at world_position exactly as Tank::setTankPos() does
    // (see the fix applied to Tutorial16 alongside this tutorial).
    Math::Mat4<float> getBodyModelMatrix(
            Math::Vec3<float> const& world_position) const;
    Math::Mat4<float> getHeadModelMatrix(
            Math::Vec3<float> const& world_position) const;
    Math::Mat4<float> getTurretModelMatrix(
            Math::Vec3<float> const& world_position) const;

    Math::Vec2<float> getPanelTopLeft(std::size_t player_index) const;
    Math::Vec2<float> getPanelSize() const;

    void appendGlyphQuad(std::vector<Tutorial18VertexHudData>& vertex_data,
                         const BitmapFontGlyphQuad& glyph,
                         Math::Vec4<float> color) const;
    void appendColoredQuad(std::vector<Tutorial18VertexHudData>& vertex_data,
                           const std::array<Math::Vec2<float>, 4>& corners,
                           Math::Vec4<float> color) const;
    void appendText(std::vector<Tutorial18VertexHudData>& vertex_data,
                    const std::string& text,
                    Math::Vec2<float> origin,
                    Math::Vec4<float> color) const;
    // Outline + ratio-filled bar, colored via get_bar_color(ratio) - the
    // real GameState::drawHUD() color-ramp formula, passed in so health
    // and power bars (which invert the ramp) can share this one helper.
    void appendBar(std::vector<Tutorial18VertexHudData>& vertex_data,
                   Math::Vec2<float> top_left,
                   Math::Vec2<float> size,
                   float ratio,
                   Math::Vec4<float> (*get_bar_color)(float ratio)) const;

    std::vector<Tutorial18VertexHudData> buildHudVertexData() const;
    bool updateHudVertexBufferData();

    bool prepareFrame(VkCommandBuffer command_buffer,
                      const ImageParameters& image_parameters,
                      VkFramebuffer& framebuffer);
    bool createFramebuffer(VkFramebuffer& framebuffer, VkImageView image_view);
    void destroyBuffer(BufferParameters& buffer);

    bool childOnWindowSizeChanged() override;
    void childClear() override;

    VulkanTutorial18Parameters m_vulkan_tutorial18_parameters;
    OrbitCamera m_camera;
    BitmapFont m_font;
};

}  // namespace vulkan_graphix

#endif  // VULKAN_GRAPHIX_TUTORIAL18_H
