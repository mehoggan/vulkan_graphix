#include "vulkan_graphix/Tutorial19.h"

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>

#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/UiGeometry.h"
#include "vulkan_graphix/VulkanCommon.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
struct ProjectileMeshInfo {
    const char* mesh_file;
    const char* texture_file;
    float scale;
    float world_x;
};

// Weapon::scale is real (WeaponDefault/WeaponAcid = 60, WeaponBFB =
// 100); world_x (side-by-side placement) is this tutorial's own layout
// choice, tuned via screenshot.
std::array<ProjectileMeshInfo, kProjectileMeshCount> const&
getProjectileMeshInfo() {
    static std::array<ProjectileMeshInfo, kProjectileMeshCount> const info = {
            {{"projectileDefault.ogl", "projectileDefault.raw", 60.0f, -220.0f},
             {"projectileAcid.ogl", "projectileAcid.raw", 60.0f, 0.0f},
             {"projectileBFB.ogl", "projectileBFB.raw", 100.0f, 220.0f}}};
    return info;
}

Math::Mat4<float> buildProjectileMatrix(float world_x, float scale) {
    return glm::translate(Math::Mat4<float>(1.0f),
                          Math::Vec3<float>(world_x, 0.0f, 0.0f)) *
           glm::scale(Math::Mat4<float>(1.0f), Math::Vec3<float>(scale));
}

struct WeaponDisplayData {
    const char* icon_file;
    const char* short_name;
    const char* description;
    int price;
    int damage;
};

// Real data from vulkan_earth/src/WeaponXxx.cpp's own constructors, not
// fabricated - ids 0-9, the real shop-purchasable set (ShopMenu.cpp:
// 205-214); WeaponDefault/id 10 is an internal Projectile fallback,
// never shop-purchasable, and is not included here.
std::array<WeaponDisplayData, kWeaponGridItemCount> const&
getWeaponDisplayData() {
    static std::array<WeaponDisplayData, kWeaponGridItemCount> const data = {{
            {"WeaponMFB.raw",
             "MFB",
             "MFB:     (Medium Force Bomb) Damage:300",
             60,
             300},
            {"WeaponBFB.raw",
             "BFB",
             "BFB:     (Big Force Bomb) Damage:400",
             100,
             400},
            {"WeaponAcid.raw",
             "Acid",
             "Acid:     Damage: 150, DOT: 10%% of total HP for 5 turns",
             100,
             150},
            {"WeaponThor.raw",
             "Thor",
             "Thor:     Damage: 200, Paralyze targets for 1 turn",
             80,
             200},
            {"WeaponEMP.raw",
             "EMP",
             "EMP:     Disrupt tanks in the target area for 5 turns.",
             60,
             0},
            {"WeaponPadlock.raw",
             "Padlock",
             "Padlock:     Damage: 50, Locks target's inventory for 4 turns",
             40,
             50},
            {"WeaponRevive.raw",
             "Revive",
             "Revive:     Revive/repair tanks in the target area",
             50,
             0},
            {"WeaponTeleport.raw",
             "Teleport",
             "Teleport:     Teleport to where the projectile lands on.",
             50,
             0},
            {"WeaponAtom.raw",
             "Atom",
             "Atom:     Damage: 999, Very small radius.",
             200,
             999},
            {"WeaponNuke.raw",
             "Nuke",
             "Nuke:     Do NOT use this weapon!!",
             500,
             800},
    }};
    return data;
}
}  // namespace

// ************************************************************ //
// VulkanTutorial19Parameters                                   //
// ************************************************************ //
VulkanTutorial19Parameters::VulkanTutorial19Parameters()
        : m_vk_render_pass(VK_NULL_HANDLE)
        , m_vk_3d_descriptor_set_layout(VK_NULL_HANDLE)
        , m_vk_grid_descriptor_set_layout(VK_NULL_HANDLE)
        , m_vk_descriptor_pool(VK_NULL_HANDLE)
        , m_vk_3d_descriptor_sets{VK_NULL_HANDLE, VK_NULL_HANDLE,
                                  VK_NULL_HANDLE}
        , m_vk_font_descriptor_set(VK_NULL_HANDLE)
        , m_vk_icon_descriptor_set(VK_NULL_HANDLE)
        , m_vk_3d_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_grid_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_3d_graphics_pipeline(VK_NULL_HANDLE)
        , m_vk_grid_graphics_pipeline(VK_NULL_HANDLE)
        , m_projectile_vertex_counts{0, 0, 0}
        , m_text_vertex_count(0)
        , m_icon_vertex_count(0)
        , m_vk_command_pool(VK_NULL_HANDLE)
        , m_rendering_resources(resources_count) {}

const VkRenderPass& VulkanTutorial19Parameters::getVkRenderPass() const {
    return m_vk_render_pass;
}
VkRenderPass& VulkanTutorial19Parameters::getVkRenderPass() {
    return m_vk_render_pass;
}
void VulkanTutorial19Parameters::setVkRenderPass(
        const VkRenderPass& vk_render_pass) {
    m_vk_render_pass = vk_render_pass;
}

const std::array<ImageParameters, kProjectileMeshCount>&
VulkanTutorial19Parameters::getProjectileImageParameters() const {
    return m_projectile_images;
}
std::array<ImageParameters, kProjectileMeshCount>&
VulkanTutorial19Parameters::getProjectileImageParameters() {
    return m_projectile_images;
}

const ImageParameters& VulkanTutorial19Parameters::getFontImageParameters()
        const {
    return m_font_image_parameters;
}
ImageParameters& VulkanTutorial19Parameters::getFontImageParameters() {
    return m_font_image_parameters;
}
void VulkanTutorial19Parameters::setFontImageParameters(
        const ImageParameters& image_parameters) {
    m_font_image_parameters = image_parameters;
}

const ImageParameters& VulkanTutorial19Parameters::getIconImageParameters()
        const {
    return m_icon_image_parameters;
}
ImageParameters& VulkanTutorial19Parameters::getIconImageParameters() {
    return m_icon_image_parameters;
}
void VulkanTutorial19Parameters::setIconImageParameters(
        const ImageParameters& image_parameters) {
    m_icon_image_parameters = image_parameters;
}

const BufferParameters&
VulkanTutorial19Parameters::getUniformBuffer3DParameters() const {
    return m_uniform_buffer_3d;
}
BufferParameters& VulkanTutorial19Parameters::getUniformBuffer3DParameters() {
    return m_uniform_buffer_3d;
}
void VulkanTutorial19Parameters::setUniformBuffer3DParameters(
        const BufferParameters& uniform_buffer) {
    m_uniform_buffer_3d = uniform_buffer;
}

const BufferParameters&
VulkanTutorial19Parameters::getUniformBufferGridParameters() const {
    return m_uniform_buffer_grid;
}
BufferParameters&
VulkanTutorial19Parameters::getUniformBufferGridParameters() {
    return m_uniform_buffer_grid;
}
void VulkanTutorial19Parameters::setUniformBufferGridParameters(
        const BufferParameters& uniform_buffer) {
    m_uniform_buffer_grid = uniform_buffer;
}

const VkDescriptorSetLayout&
VulkanTutorial19Parameters::getVk3DDescriptorSetLayout() const {
    return m_vk_3d_descriptor_set_layout;
}
VkDescriptorSetLayout&
VulkanTutorial19Parameters::getVk3DDescriptorSetLayout() {
    return m_vk_3d_descriptor_set_layout;
}
void VulkanTutorial19Parameters::setVk3DDescriptorSetLayout(
        const VkDescriptorSetLayout& other) {
    m_vk_3d_descriptor_set_layout = other;
}

const VkDescriptorSetLayout&
VulkanTutorial19Parameters::getVkGridDescriptorSetLayout() const {
    return m_vk_grid_descriptor_set_layout;
}
VkDescriptorSetLayout&
VulkanTutorial19Parameters::getVkGridDescriptorSetLayout() {
    return m_vk_grid_descriptor_set_layout;
}
void VulkanTutorial19Parameters::setVkGridDescriptorSetLayout(
        const VkDescriptorSetLayout& other) {
    m_vk_grid_descriptor_set_layout = other;
}

const VkDescriptorPool& VulkanTutorial19Parameters::getVkDescriptorPool()
        const {
    return m_vk_descriptor_pool;
}
VkDescriptorPool& VulkanTutorial19Parameters::getVkDescriptorPool() {
    return m_vk_descriptor_pool;
}
void VulkanTutorial19Parameters::setVkDescriptorPool(
        const VkDescriptorPool& other) {
    m_vk_descriptor_pool = other;
}

const std::array<VkDescriptorSet, kProjectileMeshCount>&
VulkanTutorial19Parameters::get3DVkDescriptorSets() const {
    return m_vk_3d_descriptor_sets;
}
std::array<VkDescriptorSet, kProjectileMeshCount>&
VulkanTutorial19Parameters::get3DVkDescriptorSets() {
    return m_vk_3d_descriptor_sets;
}

const VkDescriptorSet& VulkanTutorial19Parameters::getFontVkDescriptorSet()
        const {
    return m_vk_font_descriptor_set;
}
VkDescriptorSet& VulkanTutorial19Parameters::getFontVkDescriptorSet() {
    return m_vk_font_descriptor_set;
}
void VulkanTutorial19Parameters::setFontVkDescriptorSet(
        const VkDescriptorSet& other) {
    m_vk_font_descriptor_set = other;
}

const VkDescriptorSet& VulkanTutorial19Parameters::getIconVkDescriptorSet()
        const {
    return m_vk_icon_descriptor_set;
}
VkDescriptorSet& VulkanTutorial19Parameters::getIconVkDescriptorSet() {
    return m_vk_icon_descriptor_set;
}
void VulkanTutorial19Parameters::setIconVkDescriptorSet(
        const VkDescriptorSet& other) {
    m_vk_icon_descriptor_set = other;
}

const VkPipelineLayout& VulkanTutorial19Parameters::getVk3DPipelineLayout()
        const {
    return m_vk_3d_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial19Parameters::getVk3DPipelineLayout() {
    return m_vk_3d_pipeline_layout;
}
void VulkanTutorial19Parameters::setVk3DPipelineLayout(
        const VkPipelineLayout& other) {
    m_vk_3d_pipeline_layout = other;
}

const VkPipelineLayout& VulkanTutorial19Parameters::getVkGridPipelineLayout()
        const {
    return m_vk_grid_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial19Parameters::getVkGridPipelineLayout() {
    return m_vk_grid_pipeline_layout;
}
void VulkanTutorial19Parameters::setVkGridPipelineLayout(
        const VkPipelineLayout& other) {
    m_vk_grid_pipeline_layout = other;
}

const VkPipeline& VulkanTutorial19Parameters::getVk3DGraphicsPipeline()
        const {
    return m_vk_3d_graphics_pipeline;
}
VkPipeline& VulkanTutorial19Parameters::getVk3DGraphicsPipeline() {
    return m_vk_3d_graphics_pipeline;
}
void VulkanTutorial19Parameters::setVk3DGraphicsPipeline(
        const VkPipeline& other) {
    m_vk_3d_graphics_pipeline = other;
}

const VkPipeline& VulkanTutorial19Parameters::getVkGridGraphicsPipeline()
        const {
    return m_vk_grid_graphics_pipeline;
}
VkPipeline& VulkanTutorial19Parameters::getVkGridGraphicsPipeline() {
    return m_vk_grid_graphics_pipeline;
}
void VulkanTutorial19Parameters::setVkGridGraphicsPipeline(
        const VkPipeline& other) {
    m_vk_grid_graphics_pipeline = other;
}

const std::array<BufferParameters, kProjectileMeshCount>&
VulkanTutorial19Parameters::getProjectileVertexBufferParameters() const {
    return m_projectile_vertex_buffers;
}
std::array<BufferParameters, kProjectileMeshCount>&
VulkanTutorial19Parameters::getProjectileVertexBufferParameters() {
    return m_projectile_vertex_buffers;
}
const std::array<std::uint32_t, kProjectileMeshCount>&
VulkanTutorial19Parameters::getProjectileVertexCounts() const {
    return m_projectile_vertex_counts;
}
std::array<std::uint32_t, kProjectileMeshCount>&
VulkanTutorial19Parameters::getProjectileVertexCounts() {
    return m_projectile_vertex_counts;
}

const BufferParameters&
VulkanTutorial19Parameters::getGridVertexBufferParameters() const {
    return m_grid_vertex_buffer;
}
BufferParameters& VulkanTutorial19Parameters::getGridVertexBufferParameters() {
    return m_grid_vertex_buffer;
}
void VulkanTutorial19Parameters::setGridVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_grid_vertex_buffer = vertex_buffer;
}
std::uint32_t VulkanTutorial19Parameters::getTextVertexCount() const {
    return m_text_vertex_count;
}
void VulkanTutorial19Parameters::setTextVertexCount(
        std::uint32_t vertex_count) {
    m_text_vertex_count = vertex_count;
}
std::uint32_t VulkanTutorial19Parameters::getIconVertexCount() const {
    return m_icon_vertex_count;
}
void VulkanTutorial19Parameters::setIconVertexCount(
        std::uint32_t vertex_count) {
    m_icon_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial19Parameters::getStagingBufferParameters() const {
    return m_staging_buffer;
}
BufferParameters& VulkanTutorial19Parameters::getStagingBufferParameters() {
    return m_staging_buffer;
}
void VulkanTutorial19Parameters::setStagingBufferParameters(
        const BufferParameters& staging_buffer) {
    m_staging_buffer = staging_buffer;
}

const VkCommandPool& VulkanTutorial19Parameters::getVkCommandPool() const {
    return m_vk_command_pool;
}
VkCommandPool& VulkanTutorial19Parameters::getVkCommandPool() {
    return m_vk_command_pool;
}
void VulkanTutorial19Parameters::setVkCommandPool(
        const VkCommandPool& vk_command_pool) {
    m_vk_command_pool = vk_command_pool;
}

const std::vector<RenderingResourceParameters>&
VulkanTutorial19Parameters::getRenderingResources() const {
    return m_rendering_resources;
}
std::vector<RenderingResourceParameters>&
VulkanTutorial19Parameters::getRenderingResources() {
    return m_rendering_resources;
}
void VulkanTutorial19Parameters::setRenderingResources(
        const std::vector<RenderingResourceParameters>& rendering_resources) {
    m_rendering_resources = rendering_resources;
}

const std::vector<VkSemaphore>&
VulkanTutorial19Parameters::getFinishedRenderingSemaphores() const {
    return m_finished_rendering_semaphores;
}
std::vector<VkSemaphore>&
VulkanTutorial19Parameters::getFinishedRenderingSemaphores() {
    return m_finished_rendering_semaphores;
}
void VulkanTutorial19Parameters::setFinishedRenderingSemaphores(
        const std::vector<VkSemaphore>& finished_rendering_semaphores) {
    m_finished_rendering_semaphores = finished_rendering_semaphores;
}

// ************************************************************ //
// Tutorial19                                                   //
// ************************************************************ //
Tutorial19::Tutorial19()
        // Real weapon scales (60-100) applied to these meshes' own raw
        // modeling units puts each projectile at dozens of world units
        // across (BFB, the "Big Force Bomb", reads as noticeably larger
        // than the other two, as its name suggests) - spacing/distance
        // tuned via screenshot to keep all three clearly separated.
        : m_camera(0.05f, 0.2f, 800.0f), m_selected_index(0) {}

Tutorial19::~Tutorial19() { childClear(); }

void Tutorial19::onMouseButton(int button,
                               bool pressed,
                               int pos_x,
                               int pos_y) {
    constexpr int c_left_button = 1;
    if (button == c_left_button && pressed) {
        float const x = static_cast<float>(pos_x);
        float const y = static_cast<float>(pos_y);
        Math::Vec2<float> const cell_size = getCellSize();
        for (std::size_t index = 0; index < kWeaponGridItemCount; ++index) {
            Math::Vec2<float> const cell_top_left = getCellTopLeft(index);
            if (x >= cell_top_left.x && x <= cell_top_left.x + cell_size.x &&
                y >= cell_top_left.y && y <= cell_top_left.y + cell_size.y) {
                m_selected_index = index;
                return;
            }
        }
    }
    m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial19::onMouseMove(int pos_x, int pos_y) {
    m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial19::createCommandPool(std::uint32_t queue_family_index,
                                   VkCommandPool* pool) {
    if (!VulkanCommon::FrameResourceFactory(getVkDevice())
                 .createCommandPool(queue_family_index, pool)) {
        Logging::error(LOG_TAG, "Could not create command pool!");
        return false;
    }
    return true;
}

bool Tutorial19::allocateCommandBuffers(VkCommandPool pool,
                                        std::uint32_t count,
                                        VkCommandBuffer* command_buffers) {
    if (!VulkanCommon::FrameResourceFactory(getVkDevice())
                 .allocateCommandBuffers(pool, count, command_buffers)) {
        Logging::error(LOG_TAG, "Could not allocate command buffer!");
        return false;
    }
    return true;
}

bool Tutorial19::createCommandBuffers() {
    if (!createCommandPool(
                getGraphicsQueueParameters().getFamilyIndex(),
                &m_vulkan_tutorial19_parameters.getVkCommandPool())) {
        return false;
    }

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial19_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!allocateCommandBuffers(
                    m_vulkan_tutorial19_parameters.getVkCommandPool(),
                    1,
                    &rendering_resources[i].getVkCommandBuffer())) {
            return false;
        }
    }
    return true;
}

bool Tutorial19::createSemaphores() {
    VulkanCommon::FrameResourceFactory factory(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial19_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!factory.createSemaphore(
                    &rendering_resources[i].getImageAvailableVkSemaphore())) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    std::vector<VkSemaphore>& finished_rendering_semaphores =
            m_vulkan_tutorial19_parameters.getFinishedRenderingSemaphores();
    finished_rendering_semaphores.assign(
            getSwapchainParameters().getImageParameters().size(),
            VK_NULL_HANDLE);
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (!factory.createSemaphore(&finished_rendering_semaphores[i])) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    return true;
}

bool Tutorial19::createFences() {
    VulkanCommon::FrameResourceFactory factory(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial19_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!factory.createFence(true,
                                 &rendering_resources[i].getVkFence())) {
            Logging::error(LOG_TAG, "Could not create a fence!");
            return false;
        }
    }
    return true;
}

bool Tutorial19::createRenderingResources() {
    if (!createCommandBuffers()) {
        return false;
    }
    if (!createSemaphores()) {
        return false;
    }
    if (!createFences()) {
        return false;
    }
    return true;
}

bool Tutorial19::createBuffer(VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags memory_property,
                              BufferParameters& buffer) {
    if (!VulkanCommon::BufferFactory(getVkDevice(), getVkPhysicalDevice())
                 .create(usage, memory_property, buffer)) {
        Logging::error(LOG_TAG, "Could not create buffer!");
        return false;
    }
    return true;
}

bool Tutorial19::createStagingBuffer() {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial19_parameters.getStagingBufferParameters();
    // Reused sequentially for the icon atlas (kIconAtlasWidth x
    // kIconAtlasHeight RGBA = 5*256 x 2*256 x 4 = 2,621,440 bytes, the
    // largest single upload here), the font atlas (512x512 RGBA =
    // 1,048,576 bytes), and each projectile texture (512x512 RGBA =
    // 1,048,576 bytes) - sized with headroom over the largest one.
    staging_buffer.setSize(4 * 1024 * 1024);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

bool Tutorial19::createTextureFromPixels(std::uint32_t width,
                                         std::uint32_t height,
                                         const std::vector<char>& pixels,
                                         ImageParameters& out_image_parameters) {
    if (!VulkanCommon::createTextureFromPixels(
                VulkanCommon::ImageFactory(getVkDevice(),
                                           getVkPhysicalDevice()),
                VulkanCommon::StagedUploader(
                        getVkDevice(),
                        getGraphicsQueueParameters().getVkQueue(),
                        m_vulkan_tutorial19_parameters.getRenderingResources()
                                [0]
                                        .getVkCommandBuffer()),
                m_vulkan_tutorial19_parameters.getStagingBufferParameters(),
                width,
                height,
                pixels,
                VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                out_image_parameters)) {
        Logging::error(LOG_TAG, "Could not create texture from pixels!");
        return false;
    }
    return true;
}

bool Tutorial19::createProjectileTextures() {
    std::array<ProjectileMeshInfo, kProjectileMeshCount> const& mesh_info =
            getProjectileMeshInfo();
    std::array<ImageParameters, kProjectileMeshCount>& images =
            m_vulkan_tutorial19_parameters.getProjectileImageParameters();

    for (std::size_t i = 0; i < mesh_info.size(); ++i) {
        std::vector<char> texture_data =
                Tools::getRawImageData(mesh_info[i].texture_file, 512, 512);
        if (texture_data.empty()) {
            return false;
        }
        if (!createTextureFromPixels(512, 512, texture_data, images[i])) {
            return false;
        }
    }
    return true;
}

std::vector<char> Tutorial19::buildIconAtlasPixels() const {
    std::vector<char> atlas(
            static_cast<std::size_t>(kIconAtlasWidth) * kIconAtlasHeight * 4,
            0);

    std::array<WeaponDisplayData, kWeaponGridItemCount> const& weapons =
            getWeaponDisplayData();
    for (std::size_t index = 0; index < weapons.size(); ++index) {
        std::vector<char> const icon_pixels = Tools::getRawImageData(
                weapons[index].icon_file, kIconSize, kIconSize);
        if (icon_pixels.empty()) {
            Logging::error(LOG_TAG,
                           "Could not load icon \"",
                           weapons[index].icon_file,
                           "\"!");
            return {};
        }

        std::uint32_t const column =
                static_cast<std::uint32_t>(index) % kIconAtlasCols;
        std::uint32_t const grid_row =
                static_cast<std::uint32_t>(index) / kIconAtlasCols;
        std::uint32_t const dest_x = column * kIconSize;
        std::uint32_t const dest_y = grid_row * kIconSize;

        for (std::uint32_t y = 0; y < kIconSize; ++y) {
            const char* src_row = icon_pixels.data() +
                                  static_cast<std::size_t>(y) * kIconSize * 4;
            char* dest_row =
                    atlas.data() +
                    (static_cast<std::size_t>(dest_y + y) * kIconAtlasWidth +
                     dest_x) *
                            4;
            std::memcpy(dest_row, src_row, static_cast<std::size_t>(kIconSize) * 4);
        }
    }

    return atlas;
}

bool Tutorial19::createIconAtlas() {
    std::vector<char> const pixels = buildIconAtlasPixels();
    if (pixels.empty()) {
        return false;
    }

    return createTextureFromPixels(
            kIconAtlasWidth,
            kIconAtlasHeight,
            pixels,
            m_vulkan_tutorial19_parameters.getIconImageParameters());
}

bool Tutorial19::createFontAtlas() {
    if (!m_font.load(kFontPath, kFontPixelHeight)) {
        Logging::error(LOG_TAG,
                       "Could not load font \"",
                       kFontPath,
                       "\" - is the fonts-dejavu-core package installed?");
        return false;
    }

    return createTextureFromPixels(
            m_font.atlasWidth(),
            m_font.atlasHeight(),
            m_font.atlasPixels(),
            m_vulkan_tutorial19_parameters.getFontImageParameters());
}

Tutorial19UniformBufferData3D Tutorial19::get3DUniformBufferData() const {
    Tutorial19UniformBufferData3D data{};
    data.view = glm::lookAt(m_camera.eye(),
                            m_camera.target(),
                            Math::Vec3<float>(0.0f, 1.0f, 0.0f));

    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    data.projection = Tools::getPerspectiveProjectionMatrix(
            width / height, 45.0f, 1.0f, 2000.0f);

    return data;
}

bool Tutorial19::update3DUniformBufferData() {
    Tutorial19UniformBufferData3D const uniform_data = get3DUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial19_parameters.getUniformBuffer3DParameters();

    void* uniform_buffer_memory_pointer;
    if (vkMapMemory(getVkDevice(),
                    uniform_buffer.getVkDeviceMemory(),
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &uniform_buffer_memory_pointer) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not map uniform buffer memory!");
        return false;
    }
    std::memcpy(uniform_buffer_memory_pointer,
                &uniform_data,
                uniform_buffer.getSize());
    vkUnmapMemory(getVkDevice(), uniform_buffer.getVkDeviceMemory());

    return true;
}

Math::Mat4<float> Tutorial19::getGridUniformBufferData() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    return Tools::getOrthographicProjectionMatrix(
            0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

bool Tutorial19::updateGridUniformBufferData() {
    Math::Mat4<float> const uniform_data = getGridUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial19_parameters.getUniformBufferGridParameters();

    void* uniform_buffer_memory_pointer;
    if (vkMapMemory(getVkDevice(),
                    uniform_buffer.getVkDeviceMemory(),
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &uniform_buffer_memory_pointer) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not map uniform buffer memory!");
        return false;
    }
    std::memcpy(uniform_buffer_memory_pointer,
                &uniform_data,
                uniform_buffer.getSize());
    vkUnmapMemory(getVkDevice(), uniform_buffer.getVkDeviceMemory());

    return true;
}

bool Tutorial19::createUniformBuffers() {
    BufferParameters& uniform_buffer_3d =
            m_vulkan_tutorial19_parameters.getUniformBuffer3DParameters();
    uniform_buffer_3d.setSize(sizeof(Tutorial19UniformBufferData3D));
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      uniform_buffer_3d)) {
        Logging::error(LOG_TAG, "Could not create 3D uniform buffer!");
        return false;
    }
    if (!update3DUniformBufferData()) {
        return false;
    }

    BufferParameters& uniform_buffer_grid =
            m_vulkan_tutorial19_parameters.getUniformBufferGridParameters();
    uniform_buffer_grid.setSize(sizeof(Math::Mat4<float>));
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      uniform_buffer_grid)) {
        Logging::error(LOG_TAG, "Could not create grid uniform buffer!");
        return false;
    }
    return updateGridUniformBufferData();
}

bool Tutorial19::createDescriptorSetLayouts() {
    // 3D layout matches Tutorial16's shaders exactly.
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings_3d = {
            {.binding = 0,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = 1,
             .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
             .pImmutableSamplers = nullptr},
            {.binding = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
             .pImmutableSamplers = nullptr}};
    VkDescriptorSetLayoutCreateInfo layout_create_info_3d = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount =
                    static_cast<std::uint32_t>(layout_bindings_3d.size()),
            .pBindings = layout_bindings_3d.data()};
    VkDescriptorSetLayout vk_3d_layout;
    if (vkCreateDescriptorSetLayout(getVkDevice(),
                                    &layout_create_info_3d,
                                    nullptr,
                                    &vk_3d_layout) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create 3D descriptor set layout!");
        return false;
    }
    m_vulkan_tutorial19_parameters.setVk3DDescriptorSetLayout(vk_3d_layout);

    // Grid layout matches Tutorial15/17's shaders exactly.
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings_grid = {
            {.binding = 0,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
             .pImmutableSamplers = nullptr},
            {.binding = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = 1,
             .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
             .pImmutableSamplers = nullptr}};
    VkDescriptorSetLayoutCreateInfo layout_create_info_grid = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount =
                    static_cast<std::uint32_t>(layout_bindings_grid.size()),
            .pBindings = layout_bindings_grid.data()};
    VkDescriptorSetLayout vk_grid_layout;
    if (vkCreateDescriptorSetLayout(getVkDevice(),
                                    &layout_create_info_grid,
                                    nullptr,
                                    &vk_grid_layout) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create grid descriptor set layout!");
        return false;
    }
    m_vulkan_tutorial19_parameters.setVkGridDescriptorSetLayout(
            vk_grid_layout);

    return true;
}

bool Tutorial19::createDescriptorPool() {
    // 3 sets for the 3D pass (one per projectile) + 2 sets for the grid
    // pass (font, icon) = 5 sets total, each with one sampler and one
    // uniform buffer.
    std::vector<VkDescriptorPoolSize> pool_sizes = {
            {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 5},
            {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = 5}};

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .maxSets = 5,
            .poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size()),
            .pPoolSizes = pool_sizes.data()};

    VkDescriptorPool vk_descriptor_pool;
    if (vkCreateDescriptorPool(getVkDevice(),
                               &descriptor_pool_create_info,
                               nullptr,
                               &vk_descriptor_pool) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create descriptor pool!");
        return false;
    }
    m_vulkan_tutorial19_parameters.setVkDescriptorPool(vk_descriptor_pool);

    return true;
}

bool Tutorial19::allocateDescriptorSets() {
    VkDescriptorSetLayout vk_3d_layout =
            m_vulkan_tutorial19_parameters.getVk3DDescriptorSetLayout();
    std::array<VkDescriptorSet, kProjectileMeshCount>& sets_3d =
            m_vulkan_tutorial19_parameters.get3DVkDescriptorSets();
    for (std::size_t i = 0; i < sets_3d.size(); ++i) {
        VkDescriptorSetAllocateInfo allocate_info = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .pNext = nullptr,
                .descriptorPool =
                        m_vulkan_tutorial19_parameters.getVkDescriptorPool(),
                .descriptorSetCount = 1,
                .pSetLayouts = &vk_3d_layout};
        if (vkAllocateDescriptorSets(
                    getVkDevice(), &allocate_info, &sets_3d[i]) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not allocate 3D descriptor set!");
            return false;
        }
    }

    VkDescriptorSetLayout vk_grid_layout =
            m_vulkan_tutorial19_parameters.getVkGridDescriptorSetLayout();
    VkDescriptorSetAllocateInfo allocate_info_font = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool =
                    m_vulkan_tutorial19_parameters.getVkDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_grid_layout};
    VkDescriptorSet vk_font_set;
    if (vkAllocateDescriptorSets(
                getVkDevice(), &allocate_info_font, &vk_font_set) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate font descriptor set!");
        return false;
    }
    m_vulkan_tutorial19_parameters.setFontVkDescriptorSet(vk_font_set);

    VkDescriptorSetAllocateInfo allocate_info_icon = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool =
                    m_vulkan_tutorial19_parameters.getVkDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_grid_layout};
    VkDescriptorSet vk_icon_set;
    if (vkAllocateDescriptorSets(
                getVkDevice(), &allocate_info_icon, &vk_icon_set) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate icon descriptor set!");
        return false;
    }
    m_vulkan_tutorial19_parameters.setIconVkDescriptorSet(vk_icon_set);

    return true;
}

bool Tutorial19::updateDescriptorSets() {
    BufferParameters& uniform_buffer_3d =
            m_vulkan_tutorial19_parameters.getUniformBuffer3DParameters();
    VkDescriptorBufferInfo buffer_info_3d = {
            .buffer = uniform_buffer_3d.getVkBuffer(),
            .offset = 0,
            .range = uniform_buffer_3d.getSize()};

    std::array<ImageParameters, kProjectileMeshCount>& projectile_images =
            m_vulkan_tutorial19_parameters.getProjectileImageParameters();
    std::array<VkDescriptorSet, kProjectileMeshCount>& sets_3d =
            m_vulkan_tutorial19_parameters.get3DVkDescriptorSets();

    std::array<VkDescriptorImageInfo, kProjectileMeshCount> image_infos_3d{};
    std::vector<VkWriteDescriptorSet> descriptor_writes;
    for (std::size_t i = 0; i < kProjectileMeshCount; ++i) {
        image_infos_3d[i] = {
                .sampler = projectile_images[i].getVkSampler(),
                .imageView = projectile_images[i].getVkImageView(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
        descriptor_writes.push_back(
                {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                 .pNext = nullptr,
                 .dstSet = sets_3d[i],
                 .dstBinding = 0,
                 .dstArrayElement = 0,
                 .descriptorCount = 1,
                 .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                 .pImageInfo = &image_infos_3d[i],
                 .pBufferInfo = nullptr,
                 .pTexelBufferView = nullptr});
        descriptor_writes.push_back(
                {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                 .pNext = nullptr,
                 .dstSet = sets_3d[i],
                 .dstBinding = 1,
                 .dstArrayElement = 0,
                 .descriptorCount = 1,
                 .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                 .pImageInfo = nullptr,
                 .pBufferInfo = &buffer_info_3d,
                 .pTexelBufferView = nullptr});
    }

    BufferParameters& uniform_buffer_grid =
            m_vulkan_tutorial19_parameters.getUniformBufferGridParameters();
    VkDescriptorBufferInfo buffer_info_grid = {
            .buffer = uniform_buffer_grid.getVkBuffer(),
            .offset = 0,
            .range = uniform_buffer_grid.getSize()};

    ImageParameters& font_image =
            m_vulkan_tutorial19_parameters.getFontImageParameters();
    VkDescriptorImageInfo font_image_info = {
            .sampler = font_image.getVkSampler(),
            .imageView = font_image.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    ImageParameters& icon_image =
            m_vulkan_tutorial19_parameters.getIconImageParameters();
    VkDescriptorImageInfo icon_image_info = {
            .sampler = icon_image.getVkSampler(),
            .imageView = icon_image.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    descriptor_writes.push_back(
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial19_parameters.getFontVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &buffer_info_grid,
             .pTexelBufferView = nullptr});
    descriptor_writes.push_back(
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial19_parameters.getFontVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &font_image_info,
             .pBufferInfo = nullptr,
             .pTexelBufferView = nullptr});
    descriptor_writes.push_back(
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial19_parameters.getIconVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &buffer_info_grid,
             .pTexelBufferView = nullptr});
    descriptor_writes.push_back(
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial19_parameters.getIconVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &icon_image_info,
             .pBufferInfo = nullptr,
             .pTexelBufferView = nullptr});

    vkUpdateDescriptorSets(
            getVkDevice(),
            static_cast<std::uint32_t>(descriptor_writes.size()),
            descriptor_writes.data(),
            0,
            nullptr);
    return true;
}

bool Tutorial19::createRenderPass() {
    VkAttachmentDescription attachment_descriptions[] = {
            {.flags = 0,
             .format = getSwapchainParameters().getVkFormat(),
             .samples = VK_SAMPLE_COUNT_1_BIT,
             .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
             .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
             .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
             .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
             .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
             .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}};

    VkAttachmentReference color_attachment_references[] = {
            {.attachment = 0,
             .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}};

    VkSubpassDescription subpass_descriptions[] = {
            {.flags = 0,
             .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
             .inputAttachmentCount = 0,
             .pInputAttachments = nullptr,
             .colorAttachmentCount = 1,
             .pColorAttachments = color_attachment_references,
             .pResolveAttachments = nullptr,
             .pDepthStencilAttachment = nullptr,
             .preserveAttachmentCount = 0,
             .pPreserveAttachments = nullptr}};

    VkRenderPassCreateInfo render_pass_create_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .attachmentCount = 1,
            .pAttachments = attachment_descriptions,
            .subpassCount = 1,
            .pSubpasses = subpass_descriptions,
            .dependencyCount = 0,
            .pDependencies = nullptr};

    if (vkCreateRenderPass(
                getVkDevice(),
                &render_pass_create_info,
                nullptr,
                &m_vulkan_tutorial19_parameters.getVkRenderPass()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

bool Tutorial19::createPipelineLayouts() {
    VkDescriptorSetLayout vk_3d_layout =
            m_vulkan_tutorial19_parameters.getVk3DDescriptorSetLayout();
    VkPushConstantRange push_constant_range = {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(Tutorial19PushConstants)};
    VkPipelineLayoutCreateInfo layout_create_info_3d = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &vk_3d_layout,
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &push_constant_range};
    if (vkCreatePipelineLayout(
                getVkDevice(),
                &layout_create_info_3d,
                nullptr,
                &m_vulkan_tutorial19_parameters.getVk3DPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create 3D pipeline layout!");
        return false;
    }

    VkDescriptorSetLayout vk_grid_layout =
            m_vulkan_tutorial19_parameters.getVkGridDescriptorSetLayout();
    VkPipelineLayoutCreateInfo layout_create_info_grid = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &vk_grid_layout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr};
    if (vkCreatePipelineLayout(
                getVkDevice(),
                &layout_create_info_grid,
                nullptr,
                &m_vulkan_tutorial19_parameters.getVkGridPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create grid pipeline layout!");
        return false;
    }

    return true;
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
Tutorial19::createShaderModule(const char* filename) {
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule> module =
            VulkanCommon::createShaderModule(getVkDevice(), filename);
    if (!module) {
        Logging::error(LOG_TAG,
                       "Could not create shader module from a \"",
                       filename,
                       "\" file!");
    }
    return module;
}

bool Tutorial19::createPipelines() {
    // --- 3D pipeline (three projectile meshes) ---
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module_3d =
                    createShaderModule("shader.19_3d.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module_3d =
                    createShaderModule("shader.19_3d.frag.spv");
    if (!vertex_shader_module_3d || !fragment_shader_module_3d) {
        return false;
    }

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_3d = {
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_VERTEX_BIT,
             .module = vertex_shader_module_3d.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr},
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
             .module = fragment_shader_module_3d.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr}};

    std::vector<VkVertexInputBindingDescription> vertex_bindings_3d = {
            {.binding = 0,
             .stride = Tutorial19Vertex3DAttributeTraits::stride,
             .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};
    std::vector<VkVertexInputAttributeDescription> vertex_attributes_3d = {
            {.location = 0,
             .binding = 0,
             .format = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset = offsetof(struct Tutorial19Vertex3DData, position)},
            {.location = 1,
             .binding = 0,
             .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(struct Tutorial19Vertex3DData, texcoord)}};
    VkPipelineVertexInputStateCreateInfo vertex_input_state_3d = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount =
                    static_cast<std::uint32_t>(vertex_bindings_3d.size()),
            .pVertexBindingDescriptions = vertex_bindings_3d.data(),
            .vertexAttributeDescriptionCount =
                    static_cast<std::uint32_t>(vertex_attributes_3d.size()),
            .pVertexAttributeDescriptions = vertex_attributes_3d.data()};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_3d = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE};

    VkPipelineViewportStateCreateInfo viewport_state = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = nullptr,
            .scissorCount = 1,
            .pScissors = nullptr};

    VkPipelineRasterizationStateCreateInfo rasterization_3d = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            // NONE: these hand-authored assets' winding order hasn't
            // been verified against this project's usual CCW
            // convention, same reasoning Tutorial13/16 use.
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f};

    VkPipelineMultisampleStateCreateInfo multisample_state = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE};

    VkPipelineColorBlendAttachmentState color_blend_attachment_3d = {
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask =
                    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
    VkPipelineColorBlendStateCreateInfo color_blend_state_3d = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &color_blend_attachment_3d,
            .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};

    std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                                  VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .dynamicStateCount =
                    static_cast<std::uint32_t>(dynamic_states.size()),
            .pDynamicStates = dynamic_states.data()};

    VkGraphicsPipelineCreateInfo pipeline_create_info_3d = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount =
                    static_cast<std::uint32_t>(shader_stages_3d.size()),
            .pStages = shader_stages_3d.data(),
            .pVertexInputState = &vertex_input_state_3d,
            .pInputAssemblyState = &input_assembly_3d,
            .pTessellationState = nullptr,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterization_3d,
            .pMultisampleState = &multisample_state,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &color_blend_state_3d,
            .pDynamicState = &dynamic_state_create_info,
            .layout = m_vulkan_tutorial19_parameters.getVk3DPipelineLayout(),
            .renderPass = m_vulkan_tutorial19_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};
    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info_3d,
                nullptr,
                &m_vulkan_tutorial19_parameters.getVk3DGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create 3D graphics pipeline!");
        return false;
    }

    // --- Grid pipeline (2D weapon inventory grid) ---
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module_grid =
                    createShaderModule("shader.19_grid.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module_grid =
                    createShaderModule("shader.19_grid.frag.spv");
    if (!vertex_shader_module_grid || !fragment_shader_module_grid) {
        return false;
    }

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_grid = {
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_VERTEX_BIT,
             .module = vertex_shader_module_grid.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr},
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
             .module = fragment_shader_module_grid.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr}};

    std::vector<VkVertexInputBindingDescription> vertex_bindings_grid = {
            {.binding = 0,
             .stride = Tutorial19VertexGridAttributeTraits::stride,
             .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};
    std::vector<VkVertexInputAttributeDescription> vertex_attributes_grid = {
            {.location = 0,
             .binding = 0,
             .format = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset = offsetof(struct Tutorial19VertexGridData, position)},
            {.location = 1,
             .binding = 0,
             .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(struct Tutorial19VertexGridData, texcoord)},
            {.location = 2,
             .binding = 0,
             .format = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset = offsetof(struct Tutorial19VertexGridData, color)}};
    VkPipelineVertexInputStateCreateInfo vertex_input_state_grid = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount =
                    static_cast<std::uint32_t>(vertex_bindings_grid.size()),
            .pVertexBindingDescriptions = vertex_bindings_grid.data(),
            .vertexAttributeDescriptionCount =
                    static_cast<std::uint32_t>(vertex_attributes_grid.size()),
            .pVertexAttributeDescriptions = vertex_attributes_grid.data()};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_grid = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE};

    VkPipelineRasterizationStateCreateInfo rasterization_grid = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f};

    VkPipelineColorBlendAttachmentState color_blend_attachment_grid = {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask =
                    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
    VkPipelineColorBlendStateCreateInfo color_blend_state_grid = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &color_blend_attachment_grid,
            .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};

    VkGraphicsPipelineCreateInfo pipeline_create_info_grid = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount =
                    static_cast<std::uint32_t>(shader_stages_grid.size()),
            .pStages = shader_stages_grid.data(),
            .pVertexInputState = &vertex_input_state_grid,
            .pInputAssemblyState = &input_assembly_grid,
            .pTessellationState = nullptr,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterization_grid,
            .pMultisampleState = &multisample_state,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &color_blend_state_grid,
            .pDynamicState = &dynamic_state_create_info,
            .layout = m_vulkan_tutorial19_parameters.getVkGridPipelineLayout(),
            .renderPass = m_vulkan_tutorial19_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};
    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info_grid,
                nullptr,
                &m_vulkan_tutorial19_parameters.getVkGridGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create grid graphics pipeline!");
        return false;
    }

    return true;
}

std::vector<Tutorial19Vertex3DData> Tutorial19::loadMeshVertexData(
        const char* mesh_filename) const {
    std::vector<Tools::OglVertexData> const mesh_data =
            Tools::loadOglMeshData(mesh_filename);
    std::vector<Tutorial19Vertex3DData> vertex_data;
    vertex_data.reserve(mesh_data.size());
    for (Tools::OglVertexData const& vertex : mesh_data) {
        vertex_data.push_back(
                {Math::Vec4<float>(vertex.position, 1.0f), vertex.texcoord});
    }
    return vertex_data;
}

bool Tutorial19::createProjectileVertexBuffers() {
    std::array<ProjectileMeshInfo, kProjectileMeshCount> const& mesh_info =
            getProjectileMeshInfo();
    std::array<BufferParameters, kProjectileMeshCount>& vertex_buffers =
            m_vulkan_tutorial19_parameters
                    .getProjectileVertexBufferParameters();
    std::array<std::uint32_t, kProjectileMeshCount>& vertex_counts =
            m_vulkan_tutorial19_parameters.getProjectileVertexCounts();

    for (std::size_t i = 0; i < mesh_info.size(); ++i) {
        std::vector<Tutorial19Vertex3DData> const vertex_data =
                loadMeshVertexData(mesh_info[i].mesh_file);
        if (vertex_data.empty()) {
            Logging::error(LOG_TAG,
                           "Could not load mesh data from \"",
                           mesh_info[i].mesh_file,
                           "\"!");
            return false;
        }
        vertex_counts[i] = static_cast<std::uint32_t>(vertex_data.size());

        vertex_buffers[i].setSize(static_cast<std::uint32_t>(
                vertex_data.size() * sizeof(vertex_data[0])));
        if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                          vertex_buffers[i])) {
            Logging::error(LOG_TAG, "Could not create vertex buffer!");
            return false;
        }

        if (!copyBufferData(vertex_buffers[i],
                            vertex_data.data(),
                            vertex_buffers[i].getSize(),
                            VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT)) {
            Logging::error(LOG_TAG, "Could not upload vertex buffer data!");
            return false;
        }
    }

    return true;
}

bool Tutorial19::copyBufferData(BufferParameters& destination,
                                const void* data,
                                std::uint32_t data_size,
                                VkAccessFlags dst_access_mask,
                                VkPipelineStageFlags dst_stage_mask) {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial19_parameters.getStagingBufferParameters();

    if (!VulkanCommon::StagedUploader(
                getVkDevice(),
                getGraphicsQueueParameters().getVkQueue(),
                m_vulkan_tutorial19_parameters.getRenderingResources()[0]
                        .getVkCommandBuffer())
                 .uploadToBuffer(staging_buffer,
                                 destination,
                                 data,
                                 data_size,
                                 dst_access_mask,
                                 dst_stage_mask)) {
        Logging::error(LOG_TAG, "Could not upload buffer data!");
        return false;
    }

    return true;
}

Math::Vec2<float> Tutorial19::getIconUvMin(std::size_t index) const {
    std::uint32_t const column =
            static_cast<std::uint32_t>(index) % kIconAtlasCols;
    std::uint32_t const grid_row =
            static_cast<std::uint32_t>(index) / kIconAtlasCols;
    return Math::Vec2<float>(
            static_cast<float>(column) / static_cast<float>(kIconAtlasCols),
            static_cast<float>(grid_row) / static_cast<float>(kIconAtlasRows));
}

Math::Vec2<float> Tutorial19::getIconUvMax(std::size_t index) const {
    std::uint32_t const column =
            static_cast<std::uint32_t>(index) % kIconAtlasCols;
    std::uint32_t const grid_row =
            static_cast<std::uint32_t>(index) / kIconAtlasCols;
    return Math::Vec2<float>(
            static_cast<float>(column + 1) / static_cast<float>(kIconAtlasCols),
            static_cast<float>(grid_row + 1) /
                    static_cast<float>(kIconAtlasRows));
}

Math::Vec2<float> Tutorial19::getPanelTopLeft() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    return Math::Vec2<float>(width * 0.05f, height * 0.68f);
}

Math::Vec2<float> Tutorial19::getPanelSize() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    return Math::Vec2<float>(width * 0.9f, height * 0.3f);
}

Math::Vec2<float> Tutorial19::getGridTopLeft() const {
    Math::Vec2<float> const panel_top_left = getPanelTopLeft();
    return Math::Vec2<float>(panel_top_left.x + 16.0f,
                             panel_top_left.y + 20.0f);
}

Math::Vec2<float> Tutorial19::getGridSize() const {
    Math::Vec2<float> const panel_size = getPanelSize();
    return Math::Vec2<float>(panel_size.x - 32.0f, panel_size.y * 0.55f);
}

Math::Vec2<float> Tutorial19::getCellSize() const {
    Math::Vec2<float> const grid_size = getGridSize();
    constexpr float c_cell_gap = 8.0f;
    float const cell_width =
            (grid_size.x - static_cast<float>(kIconAtlasCols - 1) * c_cell_gap) /
            static_cast<float>(kIconAtlasCols);
    float const cell_height =
            (grid_size.y - static_cast<float>(kIconAtlasRows - 1) * c_cell_gap) /
            static_cast<float>(kIconAtlasRows);
    return Math::Vec2<float>(cell_width, cell_height);
}

Math::Vec2<float> Tutorial19::getCellTopLeft(std::size_t index) const {
    Math::Vec2<float> const grid_top_left = getGridTopLeft();
    Math::Vec2<float> const cell_size = getCellSize();
    constexpr float c_cell_gap = 8.0f;
    std::uint32_t const column =
            static_cast<std::uint32_t>(index) % kIconAtlasCols;
    std::uint32_t const grid_row =
            static_cast<std::uint32_t>(index) / kIconAtlasCols;
    return Math::Vec2<float>(
            grid_top_left.x +
                    static_cast<float>(column) * (cell_size.x + c_cell_gap),
            grid_top_left.y +
                    static_cast<float>(grid_row) * (cell_size.y + c_cell_gap));
}

float Tutorial19::getDescriptionTop() const {
    Math::Vec2<float> const grid_top_left = getGridTopLeft();
    Math::Vec2<float> const grid_size = getGridSize();
    return grid_top_left.y + grid_size.y + 24.0f;
}

std::vector<std::string> Tutorial19::wrapText(const std::string& text,
                                              float max_width) const {
    std::vector<std::string> lines;
    std::string current_line;
    std::size_t word_start = 0;
    for (std::size_t i = 0; i <= text.size(); ++i) {
        if (i < text.size() && text[i] != ' ') {
            continue;
        }
        std::string const word = text.substr(word_start, i - word_start);
        word_start = i + 1;
        if (word.empty()) {
            continue;
        }

        std::string const candidate =
                current_line.empty() ? word : current_line + " " + word;
        if (!current_line.empty() && m_font.textWidth(candidate) > max_width) {
            lines.push_back(current_line);
            current_line = word;
        } else {
            current_line = candidate;
        }
    }
    if (!current_line.empty()) {
        lines.push_back(current_line);
    }
    return lines;
}

void Tutorial19::appendGlyphQuad(
        std::vector<Tutorial19VertexGridData>& vertex_data,
        const BitmapFontGlyphQuad& glyph,
        Math::Vec4<float> color) const {
    Math::Vec2<float> const& top_left = glyph.top_left;
    Math::Vec2<float> const& bottom_right = glyph.bottom_right;
    Math::Vec2<float> const& uv_top_left = glyph.uv_top_left;
    Math::Vec2<float> const& uv_bottom_right = glyph.uv_bottom_right;

    Tutorial19VertexGridData const top_left_vertex{
            Math::Vec4<float>(top_left.x, top_left.y, 0.0f, 1.0f),
            uv_top_left,
            color};
    Tutorial19VertexGridData const bottom_left_vertex{
            Math::Vec4<float>(top_left.x, bottom_right.y, 0.0f, 1.0f),
            Math::Vec2<float>(uv_top_left.x, uv_bottom_right.y),
            color};
    Tutorial19VertexGridData const bottom_right_vertex{
            Math::Vec4<float>(bottom_right.x, bottom_right.y, 0.0f, 1.0f),
            uv_bottom_right,
            color};
    Tutorial19VertexGridData const top_right_vertex{
            Math::Vec4<float>(bottom_right.x, top_left.y, 0.0f, 1.0f),
            Math::Vec2<float>(uv_bottom_right.x, uv_top_left.y),
            color};

    vertex_data.push_back(top_left_vertex);
    vertex_data.push_back(bottom_left_vertex);
    vertex_data.push_back(bottom_right_vertex);
    vertex_data.push_back(top_left_vertex);
    vertex_data.push_back(bottom_right_vertex);
    vertex_data.push_back(top_right_vertex);
}

void Tutorial19::appendColoredQuad(
        std::vector<Tutorial19VertexGridData>& vertex_data,
        const std::array<Math::Vec2<float>, 4>& corners,
        Math::Vec4<float> color) const {
    Math::Vec2<float> const solid_uv = m_font.solidTexelUv();
    std::array<Tutorial19VertexGridData, 4> quad_vertices;
    for (std::size_t i = 0; i < corners.size(); ++i) {
        quad_vertices[i] = Tutorial19VertexGridData{
                Math::Vec4<float>(corners[i].x, corners[i].y, 0.0f, 1.0f),
                solid_uv,
                color};
    }
    vertex_data.push_back(quad_vertices[0]);
    vertex_data.push_back(quad_vertices[1]);
    vertex_data.push_back(quad_vertices[2]);
    vertex_data.push_back(quad_vertices[0]);
    vertex_data.push_back(quad_vertices[2]);
    vertex_data.push_back(quad_vertices[3]);
}

void Tutorial19::appendText(std::vector<Tutorial19VertexGridData>& vertex_data,
                            const std::string& text,
                            Math::Vec2<float> origin,
                            Math::Vec4<float> color) const {
    std::vector<BitmapFontGlyphQuad> const glyphs =
            m_font.layoutText(text, origin);
    for (BitmapFontGlyphQuad const& glyph : glyphs) {
        appendGlyphQuad(vertex_data, glyph, color);
    }
}

void Tutorial19::appendImageQuad(std::vector<Tutorial19VertexGridData>& vertex_data,
                                 Math::Vec2<float> top_left,
                                 Math::Vec2<float> size,
                                 Math::Vec2<float> uv_min,
                                 Math::Vec2<float> uv_max) const {
    Math::Vec4<float> const white(1.0f, 1.0f, 1.0f, 1.0f);
    Math::Vec2<float> const bottom_right(top_left.x + size.x,
                                         top_left.y + size.y);

    Tutorial19VertexGridData const top_left_vertex{
            Math::Vec4<float>(top_left.x, top_left.y, 0.0f, 1.0f),
            uv_min,
            white};
    Tutorial19VertexGridData const bottom_left_vertex{
            Math::Vec4<float>(top_left.x, bottom_right.y, 0.0f, 1.0f),
            Math::Vec2<float>(uv_min.x, uv_max.y),
            white};
    Tutorial19VertexGridData const bottom_right_vertex{
            Math::Vec4<float>(bottom_right.x, bottom_right.y, 0.0f, 1.0f),
            uv_max,
            white};
    Tutorial19VertexGridData const top_right_vertex{
            Math::Vec4<float>(bottom_right.x, top_left.y, 0.0f, 1.0f),
            Math::Vec2<float>(uv_max.x, uv_min.y),
            white};

    vertex_data.push_back(top_left_vertex);
    vertex_data.push_back(bottom_left_vertex);
    vertex_data.push_back(bottom_right_vertex);
    vertex_data.push_back(top_left_vertex);
    vertex_data.push_back(bottom_right_vertex);
    vertex_data.push_back(top_right_vertex);
}

std::vector<Tutorial19VertexGridData> Tutorial19::buildTextPassVertexData()
        const {
    std::vector<Tutorial19VertexGridData> vertex_data;
    vertex_data.reserve(kMaxGridVertexCount);

    Math::Vec2<float> const panel_top_left = getPanelTopLeft();
    Math::Vec2<float> const panel_size = getPanelSize();
    Math::Vec4<float> const panel_color(0.75f, 0.75f, 0.75f, 1.0f);
    std::vector<UiGeometry::ColoredQuad> const panel_bevel =
            UiGeometry::buildButtonBevel(
                    panel_top_left, panel_size, panel_color, false);
    for (UiGeometry::ColoredQuad const& quad : panel_bevel) {
        appendColoredQuad(vertex_data, quad.corners, quad.color);
    }

    Math::Vec4<float> const text_color(0.05f, 0.05f, 0.05f, 1.0f);
    std::string const title = "Tutorial 19 - Weapons";
    appendText(vertex_data,
              title,
              Math::Vec2<float>(panel_top_left.x + 16.0f,
                                panel_top_left.y + 16.0f),
              text_color);

    std::array<WeaponDisplayData, kWeaponGridItemCount> const& weapons =
            getWeaponDisplayData();
    Math::Vec2<float> const cell_size = getCellSize();
    for (std::size_t index = 0; index < weapons.size(); ++index) {
        Math::Vec2<float> const cell_top_left = getCellTopLeft(index);

        if (index == m_selected_index) {
            Math::Vec4<float> const highlight_color(0.95f, 0.82f, 0.25f, 1.0f);
            std::array<Math::Vec2<float>, 4> const highlight_corners = {
                    cell_top_left,
                    Math::Vec2<float>(cell_top_left.x,
                                      cell_top_left.y + cell_size.y),
                    Math::Vec2<float>(cell_top_left.x + cell_size.x,
                                      cell_top_left.y + cell_size.y),
                    Math::Vec2<float>(cell_top_left.x + cell_size.x,
                                      cell_top_left.y)};
            appendColoredQuad(vertex_data, highlight_corners, highlight_color);
        }

        std::string const label = "$" + std::to_string(weapons[index].price);
        float const label_width = m_font.textWidth(label);
        appendText(vertex_data,
                  label,
                  Math::Vec2<float>(
                          cell_top_left.x + cell_size.x * 0.5f -
                                  label_width * 0.5f,
                          cell_top_left.y + cell_size.y - 4.0f),
                  text_color);
    }

    float const description_top = getDescriptionTop();
    float const max_description_width = panel_size.x - 32.0f;
    std::vector<std::string> const description_lines = wrapText(
            weapons[m_selected_index].description, max_description_width);
    for (std::size_t line = 0; line < description_lines.size(); ++line) {
        appendText(vertex_data,
                  description_lines[line],
                  Math::Vec2<float>(panel_top_left.x + 16.0f,
                                    description_top +
                                            static_cast<float>(line) *
                                                    m_font.lineHeight()),
                  text_color);
    }

    return vertex_data;
}

std::vector<Tutorial19VertexGridData> Tutorial19::buildIconPassVertexData()
        const {
    std::vector<Tutorial19VertexGridData> vertex_data;
    vertex_data.reserve(kWeaponGridItemCount * 6);

    Math::Vec2<float> const cell_size = getCellSize();
    float const icon_size = std::min(cell_size.x, cell_size.y - 18.0f) * 0.8f;
    for (std::size_t index = 0; index < kWeaponGridItemCount; ++index) {
        Math::Vec2<float> const cell_top_left = getCellTopLeft(index);
        Math::Vec2<float> const icon_top_left(
                cell_top_left.x + cell_size.x * 0.5f - icon_size * 0.5f,
                cell_top_left.y + 4.0f);
        appendImageQuad(vertex_data,
                        icon_top_left,
                        Math::Vec2<float>(icon_size, icon_size),
                        getIconUvMin(index),
                        getIconUvMax(index));
    }

    return vertex_data;
}

bool Tutorial19::updateGridVertexBufferData() {
    std::vector<Tutorial19VertexGridData> const text_vertex_data =
            buildTextPassVertexData();
    std::vector<Tutorial19VertexGridData> const icon_vertex_data =
            buildIconPassVertexData();

    if (text_vertex_data.size() + icon_vertex_data.size() >
        kMaxGridVertexCount) {
        Logging::error(LOG_TAG,
                       "Weapon grid vertex data (",
                       text_vertex_data.size() + icon_vertex_data.size(),
                       " vertices) exceeds kMaxGridVertexCount (",
                       kMaxGridVertexCount,
                       ")!");
        return false;
    }
    m_vulkan_tutorial19_parameters.setTextVertexCount(
            static_cast<std::uint32_t>(text_vertex_data.size()));
    m_vulkan_tutorial19_parameters.setIconVertexCount(
            static_cast<std::uint32_t>(icon_vertex_data.size()));

    BufferParameters& vertex_buffer =
            m_vulkan_tutorial19_parameters.getGridVertexBufferParameters();
    void* vertex_buffer_memory_pointer;
    if (vkMapMemory(getVkDevice(),
                    vertex_buffer.getVkDeviceMemory(),
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &vertex_buffer_memory_pointer) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not map grid vertex buffer memory!");
        return false;
    }
    char* dest = static_cast<char*>(vertex_buffer_memory_pointer);
    if (!text_vertex_data.empty()) {
        std::memcpy(dest,
                    text_vertex_data.data(),
                    text_vertex_data.size() * sizeof(Tutorial19VertexGridData));
        dest += text_vertex_data.size() * sizeof(Tutorial19VertexGridData);
    }
    if (!icon_vertex_data.empty()) {
        std::memcpy(dest,
                    icon_vertex_data.data(),
                    icon_vertex_data.size() * sizeof(Tutorial19VertexGridData));
    }
    vkUnmapMemory(getVkDevice(), vertex_buffer.getVkDeviceMemory());

    return true;
}

bool Tutorial19::createGridVertexBuffer() {
    BufferParameters& vertex_buffer =
            m_vulkan_tutorial19_parameters.getGridVertexBufferParameters();
    vertex_buffer.setSize(static_cast<std::uint32_t>(
            kMaxGridVertexCount * sizeof(Tutorial19VertexGridData)));
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      vertex_buffer)) {
        Logging::error(LOG_TAG, "Could not create grid vertex buffer!");
        return false;
    }

    return updateGridVertexBufferData();
}

bool Tutorial19::createFramebuffer(VkFramebuffer& framebuffer,
                                   VkImageView image_view) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(getVkDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_vulkan_tutorial19_parameters.getVkRenderPass(),
            .attachmentCount = 1,
            .pAttachments = &image_view,
            .width = getSwapchainParameters().getVkExtent2d().width,
            .height = getSwapchainParameters().getVkExtent2d().height,
            .layers = 1};

    if (vkCreateFramebuffer(getVkDevice(),
                            &framebuffer_create_info,
                            nullptr,
                            &framebuffer) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create a framebuffer!");
        return false;
    }

    return true;
}

bool Tutorial19::prepareFrame(VkCommandBuffer command_buffer,
                              const ImageParameters& image_parameters,
                              VkFramebuffer& framebuffer) {
    if (!createFramebuffer(framebuffer, image_parameters.getVkImageView())) {
        return false;
    }

    VkCommandBufferBeginInfo command_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr};

    vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

    VkImageSubresourceRange image_subresource_range = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1};

    bool queues_differ = getPresentQueueParameters().getVkQueue() !=
                         getGraphicsQueueParameters().getVkQueue();
    std::uint32_t present_queue_family_index =
            queues_differ ? getPresentQueueParameters().getFamilyIndex()
                          : VK_QUEUE_FAMILY_IGNORED;
    std::uint32_t graphics_queue_family_index =
            queues_differ ? getGraphicsQueueParameters().getFamilyIndex()
                          : VK_QUEUE_FAMILY_IGNORED;
    VkImageMemoryBarrier barrier_from_present_to_draw = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = present_queue_family_index,
            .dstQueueFamilyIndex = graphics_queue_family_index,
            .image = image_parameters.getVkImage(),
            .subresourceRange = image_subresource_range};
    vkCmdPipelineBarrier(command_buffer,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier_from_present_to_draw);

    VkClearValue clear_value = {
            .color = {.float32 = {0.15f, 0.15f, 0.18f, 1.0f}}};

    VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = m_vulkan_tutorial19_parameters.getVkRenderPass(),
            .framebuffer = framebuffer,
            .renderArea = {.offset = {.x = 0, .y = 0},
                           .extent = getSwapchainParameters().getVkExtent2d()},
            .clearValueCount = 1,
            .pClearValues = &clear_value};

    vkCmdBeginRenderPass(command_buffer,
                         &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(
                    getSwapchainParameters().getVkExtent2d().width),
            .height = static_cast<float>(
                    getSwapchainParameters().getVkExtent2d().height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f};
    VkRect2D scissor = {.offset = {.x = 0, .y = 0},
                        .extent = getSwapchainParameters().getVkExtent2d()};
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    // --- 3D pass: three projectile meshes, each with its own texture
    // (descriptor set) and its own push-constant model matrix.
    vkCmdBindPipeline(command_buffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_vulkan_tutorial19_parameters.getVk3DGraphicsPipeline());

    VkDeviceSize const zero_offset = 0;
    std::array<ProjectileMeshInfo, kProjectileMeshCount> const& mesh_info =
            getProjectileMeshInfo();
    std::array<VkDescriptorSet, kProjectileMeshCount>& sets_3d =
            m_vulkan_tutorial19_parameters.get3DVkDescriptorSets();
    std::array<BufferParameters, kProjectileMeshCount>& vertex_buffers =
            m_vulkan_tutorial19_parameters
                    .getProjectileVertexBufferParameters();
    std::array<std::uint32_t, kProjectileMeshCount>& vertex_counts =
            m_vulkan_tutorial19_parameters.getProjectileVertexCounts();

    for (std::size_t i = 0; i < kProjectileMeshCount; ++i) {
        vkCmdBindDescriptorSets(
                command_buffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_vulkan_tutorial19_parameters.getVk3DPipelineLayout(),
                0,
                1,
                &sets_3d[i],
                0,
                nullptr);
        vkCmdBindVertexBuffers(command_buffer,
                               0,
                               1,
                               &vertex_buffers[i].getVkBuffer(),
                               &zero_offset);
        Tutorial19PushConstants push_constants{buildProjectileMatrix(
                mesh_info[i].world_x, mesh_info[i].scale)};
        vkCmdPushConstants(
                command_buffer,
                m_vulkan_tutorial19_parameters.getVk3DPipelineLayout(),
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof(Tutorial19PushConstants),
                &push_constants);
        vkCmdDraw(command_buffer, vertex_counts[i], 1, 0, 0);
    }

    // --- Grid pass: weapon icons/labels, drawn on top ---
    vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial19_parameters.getVkGridGraphicsPipeline());

    VkDeviceSize text_offset = 0;
    vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_vulkan_tutorial19_parameters.getGridVertexBufferParameters()
                     .getVkBuffer(),
            &text_offset);
    VkDescriptorSet vk_font_descriptor_set =
            m_vulkan_tutorial19_parameters.getFontVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial19_parameters.getVkGridPipelineLayout(),
            0,
            1,
            &vk_font_descriptor_set,
            0,
            nullptr);
    vkCmdDraw(command_buffer,
             m_vulkan_tutorial19_parameters.getTextVertexCount(),
             1,
             0,
             0);

    VkDeviceSize const icon_offset =
            static_cast<VkDeviceSize>(
                    m_vulkan_tutorial19_parameters.getTextVertexCount()) *
            sizeof(Tutorial19VertexGridData);
    vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_vulkan_tutorial19_parameters.getGridVertexBufferParameters()
                     .getVkBuffer(),
            &icon_offset);
    VkDescriptorSet vk_icon_descriptor_set =
            m_vulkan_tutorial19_parameters.getIconVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial19_parameters.getVkGridPipelineLayout(),
            0,
            1,
            &vk_icon_descriptor_set,
            0,
            nullptr);
    vkCmdDraw(command_buffer,
             m_vulkan_tutorial19_parameters.getIconVertexCount(),
             1,
             0,
             0);

    vkCmdEndRenderPass(command_buffer);

    VkImageMemoryBarrier barrier_from_draw_to_present = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = graphics_queue_family_index,
            .dstQueueFamilyIndex = present_queue_family_index,
            .image = image_parameters.getVkImage(),
            .subresourceRange = image_subresource_range};
    vkCmdPipelineBarrier(command_buffer,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier_from_draw_to_present);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not record command buffer!");
        return false;
    }
    return true;
}

bool Tutorial19::draw() {
    static std::size_t resource_index = 0;
    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial19_parameters.getRenderingResources();
    RenderingResourceParameters& current_rendering_resource =
            rendering_resources[resource_index];
    VkSwapchainKHR swap_chain = getSwapchainParameters().getVkSwapchainKhr();
    std::uint32_t image_index;

    resource_index = (resource_index + 1) %
                     VulkanTutorial19Parameters::resources_count;

    if (vkWaitForFences(getVkDevice(),
                        1,
                        &current_rendering_resource.getVkFence(),
                        VK_FALSE,
                        1000000000) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Waiting for fence takes too long!");
        return false;
    }
    vkResetFences(getVkDevice(), 1, &current_rendering_resource.getVkFence());

    // Vertex buffer/UBO writes race a previous frame's in-flight command
    // buffer without this, same reasoning Tutorial15/17 use for their
    // own per-frame vertex buffer writes.
    vkDeviceWaitIdle(getVkDevice());
    if (!update3DUniformBufferData()) {
        return false;
    }
    if (!updateGridVertexBufferData()) {
        return false;
    }

    VkResult result = vkAcquireNextImageKHR(
            getVkDevice(),
            swap_chain,
            UINT64_MAX,
            current_rendering_resource.getImageAvailableVkSemaphore(),
            VK_NULL_HANDLE,
            &image_index);
    switch (result) {
        case VK_SUCCESS:
        case VK_SUBOPTIMAL_KHR:
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            return onWindowSizeChanged();
        default:
            Logging::error(
                    LOG_TAG,
                    "Problem occurred during swap chain image acquisition!");
            return false;
    }

    if (!prepareFrame(
                current_rendering_resource.getVkCommandBuffer(),
                getSwapchainParameters().getImageParameters()[image_index],
                current_rendering_resource.getVkFramebuffer())) {
        return false;
    }

    VkSemaphore& finished_rendering_semaphore =
            m_vulkan_tutorial19_parameters.getFinishedRenderingSemaphores()
                    [image_index];

    VkPipelineStageFlags wait_dst_stage_mask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores =
                    &current_rendering_resource.getImageAvailableVkSemaphore(),
            .pWaitDstStageMask = &wait_dst_stage_mask,
            .commandBufferCount = 1,
            .pCommandBuffers =
                    &current_rendering_resource.getVkCommandBuffer(),
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &finished_rendering_semaphore};

    if (vkQueueSubmit(getGraphicsQueueParameters().getVkQueue(),
                      1,
                      &submit_info,
                      current_rendering_resource.getVkFence()) != VK_SUCCESS) {
        return false;
    }

    VkPresentInfoKHR present_info = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &finished_rendering_semaphore,
            .swapchainCount = 1,
            .pSwapchains = &swap_chain,
            .pImageIndices = &image_index,
            .pResults = nullptr};
    result = vkQueuePresentKHR(getPresentQueueParameters().getVkQueue(),
                               &present_info);

    switch (result) {
        case VK_SUCCESS:
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
        case VK_SUBOPTIMAL_KHR:
            return onWindowSizeChanged();
        default:
            Logging::error(LOG_TAG,
                           "Problem occurred during image presentation!");
            return false;
    }

    return true;
}

void Tutorial19::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

bool Tutorial19::childOnWindowSizeChanged() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return true;
    }
    vkDeviceWaitIdle(getVkDevice());

    if (!createRenderingResources()) {
        return false;
    }
    if (!createStagingBuffer()) {
        return false;
    }
    if (!createProjectileTextures()) {
        return false;
    }
    if (!createFontAtlas()) {
        return false;
    }
    if (!createIconAtlas()) {
        return false;
    }
    if (!createUniformBuffers()) {
        return false;
    }
    if (!createDescriptorSetLayouts()) {
        return false;
    }
    if (!createDescriptorPool()) {
        return false;
    }
    if (!allocateDescriptorSets()) {
        return false;
    }
    if (!updateDescriptorSets()) {
        return false;
    }
    if (!createRenderPass()) {
        return false;
    }
    if (!createPipelineLayouts()) {
        return false;
    }
    if (!createPipelines()) {
        return false;
    }
    if (!createProjectileVertexBuffers()) {
        return false;
    }
    return createGridVertexBuffer();
}

void Tutorial19::childClear() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return;
    }
    vkDeviceWaitIdle(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial19_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (rendering_resources[i].getVkFramebuffer() != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(getVkDevice(),
                                 rendering_resources[i].getVkFramebuffer(),
                                 nullptr);
            rendering_resources[i].setVkFramebuffer(VK_NULL_HANDLE);
        }
        if (rendering_resources[i].getVkCommandBuffer() != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(
                    getVkDevice(),
                    m_vulkan_tutorial19_parameters.getVkCommandPool(),
                    1,
                    &rendering_resources[i].getVkCommandBuffer());
        }
        if (rendering_resources[i].getImageAvailableVkSemaphore() !=
            VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(),
                    rendering_resources[i].getImageAvailableVkSemaphore(),
                    nullptr);
        }
        if (rendering_resources[i].getVkFence() != VK_NULL_HANDLE) {
            vkDestroyFence(
                    getVkDevice(), rendering_resources[i].getVkFence(), nullptr);
        }
    }

    std::vector<VkSemaphore>& finished_rendering_semaphores =
            m_vulkan_tutorial19_parameters.getFinishedRenderingSemaphores();
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (finished_rendering_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(), finished_rendering_semaphores[i], nullptr);
        }
    }
    finished_rendering_semaphores.clear();

    if (m_vulkan_tutorial19_parameters.getVkCommandPool() != VK_NULL_HANDLE) {
        vkDestroyCommandPool(
                getVkDevice(),
                m_vulkan_tutorial19_parameters.getVkCommandPool(),
                nullptr);
        m_vulkan_tutorial19_parameters.setVkCommandPool(VK_NULL_HANDLE);
    }

    std::array<BufferParameters, kProjectileMeshCount>& vertex_buffers =
            m_vulkan_tutorial19_parameters
                    .getProjectileVertexBufferParameters();
    for (BufferParameters& buffer : vertex_buffers) {
        destroyBuffer(buffer);
    }
    destroyBuffer(m_vulkan_tutorial19_parameters.getGridVertexBufferParameters());
    destroyBuffer(m_vulkan_tutorial19_parameters.getStagingBufferParameters());

    if (m_vulkan_tutorial19_parameters.getVk3DGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial19_parameters.getVk3DGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial19_parameters.setVk3DGraphicsPipeline(VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial19_parameters.getVkGridGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial19_parameters.getVkGridGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial19_parameters.setVkGridGraphicsPipeline(
                VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial19_parameters.getVk3DPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial19_parameters.getVk3DPipelineLayout(),
                nullptr);
        m_vulkan_tutorial19_parameters.setVk3DPipelineLayout(VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial19_parameters.getVkGridPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial19_parameters.getVkGridPipelineLayout(),
                nullptr);
        m_vulkan_tutorial19_parameters.setVkGridPipelineLayout(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial19_parameters.getVkRenderPass() != VK_NULL_HANDLE) {
        vkDestroyRenderPass(
                getVkDevice(),
                m_vulkan_tutorial19_parameters.getVkRenderPass(),
                nullptr);
        m_vulkan_tutorial19_parameters.setVkRenderPass(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial19_parameters.getVkDescriptorPool() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(
                getVkDevice(),
                m_vulkan_tutorial19_parameters.getVkDescriptorPool(),
                nullptr);
        m_vulkan_tutorial19_parameters.setVkDescriptorPool(VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial19_parameters.getVk3DDescriptorSetLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(
                getVkDevice(),
                m_vulkan_tutorial19_parameters.getVk3DDescriptorSetLayout(),
                nullptr);
        m_vulkan_tutorial19_parameters.setVk3DDescriptorSetLayout(
                VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial19_parameters.getVkGridDescriptorSetLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(
                getVkDevice(),
                m_vulkan_tutorial19_parameters.getVkGridDescriptorSetLayout(),
                nullptr);
        m_vulkan_tutorial19_parameters.setVkGridDescriptorSetLayout(
                VK_NULL_HANDLE);
    }

    destroyBuffer(m_vulkan_tutorial19_parameters.getUniformBuffer3DParameters());
    destroyBuffer(
            m_vulkan_tutorial19_parameters.getUniformBufferGridParameters());

    VulkanCommon::ImageFactory image_factory(getVkDevice(),
                                             getVkPhysicalDevice());
    std::array<ImageParameters, kProjectileMeshCount>& projectile_images =
            m_vulkan_tutorial19_parameters.getProjectileImageParameters();
    for (ImageParameters& image : projectile_images) {
        image_factory.destroy(image);
    }

    image_factory.destroy(
            m_vulkan_tutorial19_parameters.getFontImageParameters());
    image_factory.destroy(
            m_vulkan_tutorial19_parameters.getIconImageParameters());
}

}  // namespace vulkan_graphix
