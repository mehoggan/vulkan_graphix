#include "vulkan_graphix/Tutorial18.h"

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
constexpr VkFormat c_depth_format = VK_FORMAT_D32_SFLOAT;

// TankB's own constructor values (vulkan_earth/src/TankB.cpp) - see
// Tutorial16.h's top comment for why these are taken verbatim rather
// than recomputed from live gameplay state.
constexpr float c_part_scale = 50.0f;
Math::Vec3<float> const c_body_offset(0.0f, 65.0f, 0.0f);
Math::Vec3<float> const c_head_offset(0.0f, 70.0f, 0.0f);
Math::Vec3<float> const c_turret_offset(0.0f, 0.0001f, -50.0001f);

// Tank::initBody()/initHead()/initTurret() (vulkan_earth/src/Tank.cpp)
// all set this identical right/up/at basis - not an identity rotation,
// a fixed axis permutation (model-local +X ends up along world +Z).
Math::Mat4<float> const& getPartBasis() {
    static Math::Mat4<float> const basis(
            Math::Vec4<float>(0.0f, 0.0f, 1.0f, 0.0f),
            Math::Vec4<float>(0.0f, 1.0f, 0.0f, 0.0f),
            Math::Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f),
            Math::Vec4<float>(0.0f, 0.0f, 0.0f, 1.0f));
    return basis;
}

// Mirrors Tank::setTankPos()'s own construction exactly (Tank.cpp:73-99)
// - see the fix applied to Tutorial16 alongside this tutorial for the
// full explanation. A child part's offset is rotated through its
// parent's own basis columns before being added to the parent's
// translation.
Math::Vec3<float> rotateOffsetThroughBasis(Math::Mat4<float> const& basis,
                                           Math::Vec3<float> const& offset) {
    return Math::Vec3<float>(basis * Math::Vec4<float>(offset, 0.0f));
}

Math::Mat4<float> buildPartMatrix(Math::Vec3<float> const& translation) {
    return glm::translate(Math::Mat4<float>(1.0f), translation) *
           getPartBasis() *
           glm::scale(Math::Mat4<float>(1.0f),
                      Math::Vec3<float>(c_part_scale));
}

// The real GameState::drawHUD() color ramps (GameState.cpp:~700-762):
// health goes green-to-red as it drops, power (inverted) goes red-to-
// green as it rises - ported as-is, not second-guessed.
Math::Vec4<float> getHealthBarColor(float ratio) {
    return Math::Vec4<float>(1.0f - ratio, ratio, 0.0f, 1.0f);
}
Math::Vec4<float> getPowerBarColor(float ratio) {
    return Math::Vec4<float>(ratio, 1.0f - ratio, 0.0f, 1.0f);
}
}  // namespace

// ************************************************************ //
// VulkanTutorial18Parameters                                   //
// ************************************************************ //
VulkanTutorial18Parameters::VulkanTutorial18Parameters()
        : m_vk_render_pass(VK_NULL_HANDLE)
        , m_vk_3d_descriptor_set_layout(VK_NULL_HANDLE)
        , m_vk_hud_descriptor_set_layout(VK_NULL_HANDLE)
        , m_vk_descriptor_pool(VK_NULL_HANDLE)
        , m_vk_3d_descriptor_set(VK_NULL_HANDLE)
        , m_vk_hud_descriptor_set(VK_NULL_HANDLE)
        , m_vk_3d_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_hud_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_3d_graphics_pipeline(VK_NULL_HANDLE)
        , m_vk_hud_graphics_pipeline(VK_NULL_HANDLE)
        , m_body_vertex_count(0)
        , m_head_vertex_count(0)
        , m_turret_vertex_count(0)
        , m_hud_vertex_count(0)
        , m_vk_command_pool(VK_NULL_HANDLE)
        , m_rendering_resources(resources_count) {}

const VkRenderPass& VulkanTutorial18Parameters::getVkRenderPass() const {
    return m_vk_render_pass;
}
VkRenderPass& VulkanTutorial18Parameters::getVkRenderPass() {
    return m_vk_render_pass;
}
void VulkanTutorial18Parameters::setVkRenderPass(
        const VkRenderPass& vk_render_pass) {
    m_vk_render_pass = vk_render_pass;
}

const ImageParameters& VulkanTutorial18Parameters::getDepthImageParameters()
        const {
    return m_depth_image_parameters;
}
ImageParameters& VulkanTutorial18Parameters::getDepthImageParameters() {
    return m_depth_image_parameters;
}
void VulkanTutorial18Parameters::setDepthImageParameters(
        const ImageParameters& depth_image) {
    m_depth_image_parameters = depth_image;
}

const ImageParameters& VulkanTutorial18Parameters::getTankImageParameters()
        const {
    return m_tank_image_parameters;
}
ImageParameters& VulkanTutorial18Parameters::getTankImageParameters() {
    return m_tank_image_parameters;
}
void VulkanTutorial18Parameters::setTankImageParameters(
        const ImageParameters& image_parameters) {
    m_tank_image_parameters = image_parameters;
}

const ImageParameters& VulkanTutorial18Parameters::getFontImageParameters()
        const {
    return m_font_image_parameters;
}
ImageParameters& VulkanTutorial18Parameters::getFontImageParameters() {
    return m_font_image_parameters;
}
void VulkanTutorial18Parameters::setFontImageParameters(
        const ImageParameters& image_parameters) {
    m_font_image_parameters = image_parameters;
}

const BufferParameters&
VulkanTutorial18Parameters::getUniformBuffer3DParameters() const {
    return m_uniform_buffer_3d;
}
BufferParameters& VulkanTutorial18Parameters::getUniformBuffer3DParameters() {
    return m_uniform_buffer_3d;
}
void VulkanTutorial18Parameters::setUniformBuffer3DParameters(
        const BufferParameters& uniform_buffer) {
    m_uniform_buffer_3d = uniform_buffer;
}

const BufferParameters&
VulkanTutorial18Parameters::getUniformBufferHudParameters() const {
    return m_uniform_buffer_hud;
}
BufferParameters&
VulkanTutorial18Parameters::getUniformBufferHudParameters() {
    return m_uniform_buffer_hud;
}
void VulkanTutorial18Parameters::setUniformBufferHudParameters(
        const BufferParameters& uniform_buffer) {
    m_uniform_buffer_hud = uniform_buffer;
}

const VkDescriptorSetLayout&
VulkanTutorial18Parameters::getVk3DDescriptorSetLayout() const {
    return m_vk_3d_descriptor_set_layout;
}
VkDescriptorSetLayout&
VulkanTutorial18Parameters::getVk3DDescriptorSetLayout() {
    return m_vk_3d_descriptor_set_layout;
}
void VulkanTutorial18Parameters::setVk3DDescriptorSetLayout(
        const VkDescriptorSetLayout& other) {
    m_vk_3d_descriptor_set_layout = other;
}

const VkDescriptorSetLayout&
VulkanTutorial18Parameters::getVkHudDescriptorSetLayout() const {
    return m_vk_hud_descriptor_set_layout;
}
VkDescriptorSetLayout&
VulkanTutorial18Parameters::getVkHudDescriptorSetLayout() {
    return m_vk_hud_descriptor_set_layout;
}
void VulkanTutorial18Parameters::setVkHudDescriptorSetLayout(
        const VkDescriptorSetLayout& other) {
    m_vk_hud_descriptor_set_layout = other;
}

const VkDescriptorPool& VulkanTutorial18Parameters::getVkDescriptorPool()
        const {
    return m_vk_descriptor_pool;
}
VkDescriptorPool& VulkanTutorial18Parameters::getVkDescriptorPool() {
    return m_vk_descriptor_pool;
}
void VulkanTutorial18Parameters::setVkDescriptorPool(
        const VkDescriptorPool& other) {
    m_vk_descriptor_pool = other;
}

const VkDescriptorSet& VulkanTutorial18Parameters::get3DVkDescriptorSet()
        const {
    return m_vk_3d_descriptor_set;
}
VkDescriptorSet& VulkanTutorial18Parameters::get3DVkDescriptorSet() {
    return m_vk_3d_descriptor_set;
}
void VulkanTutorial18Parameters::set3DVkDescriptorSet(
        const VkDescriptorSet& other) {
    m_vk_3d_descriptor_set = other;
}

const VkDescriptorSet& VulkanTutorial18Parameters::getHudVkDescriptorSet()
        const {
    return m_vk_hud_descriptor_set;
}
VkDescriptorSet& VulkanTutorial18Parameters::getHudVkDescriptorSet() {
    return m_vk_hud_descriptor_set;
}
void VulkanTutorial18Parameters::setHudVkDescriptorSet(
        const VkDescriptorSet& other) {
    m_vk_hud_descriptor_set = other;
}

const VkPipelineLayout& VulkanTutorial18Parameters::getVk3DPipelineLayout()
        const {
    return m_vk_3d_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial18Parameters::getVk3DPipelineLayout() {
    return m_vk_3d_pipeline_layout;
}
void VulkanTutorial18Parameters::setVk3DPipelineLayout(
        const VkPipelineLayout& other) {
    m_vk_3d_pipeline_layout = other;
}

const VkPipelineLayout& VulkanTutorial18Parameters::getVkHudPipelineLayout()
        const {
    return m_vk_hud_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial18Parameters::getVkHudPipelineLayout() {
    return m_vk_hud_pipeline_layout;
}
void VulkanTutorial18Parameters::setVkHudPipelineLayout(
        const VkPipelineLayout& other) {
    m_vk_hud_pipeline_layout = other;
}

const VkPipeline& VulkanTutorial18Parameters::getVk3DGraphicsPipeline()
        const {
    return m_vk_3d_graphics_pipeline;
}
VkPipeline& VulkanTutorial18Parameters::getVk3DGraphicsPipeline() {
    return m_vk_3d_graphics_pipeline;
}
void VulkanTutorial18Parameters::setVk3DGraphicsPipeline(
        const VkPipeline& other) {
    m_vk_3d_graphics_pipeline = other;
}

const VkPipeline& VulkanTutorial18Parameters::getVkHudGraphicsPipeline()
        const {
    return m_vk_hud_graphics_pipeline;
}
VkPipeline& VulkanTutorial18Parameters::getVkHudGraphicsPipeline() {
    return m_vk_hud_graphics_pipeline;
}
void VulkanTutorial18Parameters::setVkHudGraphicsPipeline(
        const VkPipeline& other) {
    m_vk_hud_graphics_pipeline = other;
}

const BufferParameters&
VulkanTutorial18Parameters::getBodyVertexBufferParameters() const {
    return m_body_vertex_buffer;
}
BufferParameters& VulkanTutorial18Parameters::getBodyVertexBufferParameters() {
    return m_body_vertex_buffer;
}
void VulkanTutorial18Parameters::setBodyVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_body_vertex_buffer = vertex_buffer;
}
std::uint32_t VulkanTutorial18Parameters::getBodyVertexCount() const {
    return m_body_vertex_count;
}
void VulkanTutorial18Parameters::setBodyVertexCount(
        std::uint32_t vertex_count) {
    m_body_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial18Parameters::getHeadVertexBufferParameters() const {
    return m_head_vertex_buffer;
}
BufferParameters& VulkanTutorial18Parameters::getHeadVertexBufferParameters() {
    return m_head_vertex_buffer;
}
void VulkanTutorial18Parameters::setHeadVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_head_vertex_buffer = vertex_buffer;
}
std::uint32_t VulkanTutorial18Parameters::getHeadVertexCount() const {
    return m_head_vertex_count;
}
void VulkanTutorial18Parameters::setHeadVertexCount(
        std::uint32_t vertex_count) {
    m_head_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial18Parameters::getTurretVertexBufferParameters() const {
    return m_turret_vertex_buffer;
}
BufferParameters&
VulkanTutorial18Parameters::getTurretVertexBufferParameters() {
    return m_turret_vertex_buffer;
}
void VulkanTutorial18Parameters::setTurretVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_turret_vertex_buffer = vertex_buffer;
}
std::uint32_t VulkanTutorial18Parameters::getTurretVertexCount() const {
    return m_turret_vertex_count;
}
void VulkanTutorial18Parameters::setTurretVertexCount(
        std::uint32_t vertex_count) {
    m_turret_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial18Parameters::getHudVertexBufferParameters() const {
    return m_hud_vertex_buffer;
}
BufferParameters& VulkanTutorial18Parameters::getHudVertexBufferParameters() {
    return m_hud_vertex_buffer;
}
void VulkanTutorial18Parameters::setHudVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_hud_vertex_buffer = vertex_buffer;
}
std::uint32_t VulkanTutorial18Parameters::getHudVertexCount() const {
    return m_hud_vertex_count;
}
void VulkanTutorial18Parameters::setHudVertexCount(
        std::uint32_t vertex_count) {
    m_hud_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial18Parameters::getStagingBufferParameters() const {
    return m_staging_buffer;
}
BufferParameters& VulkanTutorial18Parameters::getStagingBufferParameters() {
    return m_staging_buffer;
}
void VulkanTutorial18Parameters::setStagingBufferParameters(
        const BufferParameters& staging_buffer) {
    m_staging_buffer = staging_buffer;
}

const VkCommandPool& VulkanTutorial18Parameters::getVkCommandPool() const {
    return m_vk_command_pool;
}
VkCommandPool& VulkanTutorial18Parameters::getVkCommandPool() {
    return m_vk_command_pool;
}
void VulkanTutorial18Parameters::setVkCommandPool(
        const VkCommandPool& vk_command_pool) {
    m_vk_command_pool = vk_command_pool;
}

const std::vector<RenderingResourceParameters>&
VulkanTutorial18Parameters::getRenderingResources() const {
    return m_rendering_resources;
}
std::vector<RenderingResourceParameters>&
VulkanTutorial18Parameters::getRenderingResources() {
    return m_rendering_resources;
}
void VulkanTutorial18Parameters::setRenderingResources(
        const std::vector<RenderingResourceParameters>& rendering_resources) {
    m_rendering_resources = rendering_resources;
}

const std::vector<VkSemaphore>&
VulkanTutorial18Parameters::getFinishedRenderingSemaphores() const {
    return m_finished_rendering_semaphores;
}
std::vector<VkSemaphore>&
VulkanTutorial18Parameters::getFinishedRenderingSemaphores() {
    return m_finished_rendering_semaphores;
}
void VulkanTutorial18Parameters::setFinishedRenderingSemaphores(
        const std::vector<VkSemaphore>& finished_rendering_semaphores) {
    m_finished_rendering_semaphores = finished_rendering_semaphores;
}

// ************************************************************ //
// Tutorial18                                                   //
// ************************************************************ //
Tutorial18::Tutorial18()
        // Two tanks 800 units apart (world X = +-400) span roughly
        // [-610,610] in X once each tank's own ~210-unit body half-
        // extent is added - a wider world than Tutorial16's single
        // tank, so this needs a larger distance still; tuned via
        // screenshot the same way.
        : m_camera(0.5f, 0.08f, 1600.0f) {}

Tutorial18::~Tutorial18() { childClear(); }

void Tutorial18::onMouseButton(int button,
                               bool pressed,
                               int pos_x,
                               int pos_y) {
    m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial18::onMouseMove(int pos_x, int pos_y) {
    m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial18::createCommandPool(std::uint32_t queue_family_index,
                                   VkCommandPool* pool) {
    if (!VulkanCommon::FrameResourceFactory(getVkDevice())
                 .createCommandPool(queue_family_index, pool)) {
        Logging::error(LOG_TAG, "Could not create command pool!");
        return false;
    }
    return true;
}

bool Tutorial18::allocateCommandBuffers(VkCommandPool pool,
                                        std::uint32_t count,
                                        VkCommandBuffer* command_buffers) {
    if (!VulkanCommon::FrameResourceFactory(getVkDevice())
                 .allocateCommandBuffers(pool, count, command_buffers)) {
        Logging::error(LOG_TAG, "Could not allocate command buffer!");
        return false;
    }
    return true;
}

bool Tutorial18::createCommandBuffers() {
    if (!createCommandPool(
                getGraphicsQueueParameters().getFamilyIndex(),
                &m_vulkan_tutorial18_parameters.getVkCommandPool())) {
        return false;
    }

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial18_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!allocateCommandBuffers(
                    m_vulkan_tutorial18_parameters.getVkCommandPool(),
                    1,
                    &rendering_resources[i].getVkCommandBuffer())) {
            return false;
        }
    }
    return true;
}

bool Tutorial18::createSemaphores() {
    VulkanCommon::FrameResourceFactory factory(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial18_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!factory.createSemaphore(
                    &rendering_resources[i].getImageAvailableVkSemaphore())) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    std::vector<VkSemaphore>& finished_rendering_semaphores =
            m_vulkan_tutorial18_parameters.getFinishedRenderingSemaphores();
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

bool Tutorial18::createFences() {
    VulkanCommon::FrameResourceFactory factory(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial18_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!factory.createFence(true,
                                 &rendering_resources[i].getVkFence())) {
            Logging::error(LOG_TAG, "Could not create a fence!");
            return false;
        }
    }
    return true;
}

bool Tutorial18::createRenderingResources() {
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

bool Tutorial18::createBuffer(VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags memory_property,
                              BufferParameters& buffer) {
    if (!VulkanCommon::BufferFactory(getVkDevice(), getVkPhysicalDevice())
                 .create(usage, memory_property, buffer)) {
        Logging::error(LOG_TAG, "Could not create buffer!");
        return false;
    }
    return true;
}

bool Tutorial18::createStagingBuffer() {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial18_parameters.getStagingBufferParameters();
    // Reused sequentially for two uploads: TestImage.raw (1024x1024
    // RGBA = 4,194,304 bytes) and the font atlas (512x512 RGBA =
    // 1,048,576 bytes) - sized with headroom over the larger one.
    staging_buffer.setSize(6 * 1024 * 1024);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

bool Tutorial18::createImage(std::uint32_t width,
                             std::uint32_t height,
                             VkFormat format,
                             VkImageUsageFlags usage,
                             VkImage* image) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .createImage(width, height, format, usage, image);
}

bool Tutorial18::allocateImageMemory(VkImage image,
                                     VkMemoryPropertyFlags property,
                                     VkDeviceMemory* memory) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .allocateMemory(image, property, memory);
}

bool Tutorial18::createImageView(VkImage image,
                                 VkFormat format,
                                 VkImageAspectFlags aspect_mask,
                                 VkImageView* image_view) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .createImageView(image, format, aspect_mask, image_view);
}

bool Tutorial18::createSampler(VkSampler* sampler) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .createSampler(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, sampler);
}

bool Tutorial18::createDepthResources() {
    ImageParameters& depth_image =
            m_vulkan_tutorial18_parameters.getDepthImageParameters();

    VkImage vk_image;
    if (!createImage(getSwapchainParameters().getVkExtent2d().width,
                     getSwapchainParameters().getVkExtent2d().height,
                     c_depth_format,
                     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                     &vk_image)) {
        Logging::error(LOG_TAG, "Could not create depth image!");
        return false;
    }
    depth_image.setVkImage(vk_image);

    VkDeviceMemory vk_device_memory;
    if (!allocateImageMemory(depth_image.getVkImage(),
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &vk_device_memory)) {
        Logging::error(LOG_TAG, "Could not allocate memory for depth image!");
        return false;
    }
    depth_image.setVkDeviceMemory(vk_device_memory);

    if (vkBindImageMemory(getVkDevice(),
                          depth_image.getVkImage(),
                          depth_image.getVkDeviceMemory(),
                          0) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not bind memory to depth image!");
        return false;
    }

    VkImageView vk_image_view;
    if (!createImageView(depth_image.getVkImage(),
                         c_depth_format,
                         VK_IMAGE_ASPECT_DEPTH_BIT,
                         &vk_image_view)) {
        Logging::error(LOG_TAG, "Could not create depth image view!");
        return false;
    }
    depth_image.setVkImageView(vk_image_view);

    return true;
}

bool Tutorial18::destroyDepthResources() {
    VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .destroy(m_vulkan_tutorial18_parameters.getDepthImageParameters());
    return true;
}

bool Tutorial18::createTextureFromPixels(std::uint32_t width,
                                         std::uint32_t height,
                                         const std::vector<char>& pixels,
                                         ImageParameters& out_image_parameters) {
    if (!VulkanCommon::createTextureFromPixels(
                VulkanCommon::ImageFactory(getVkDevice(),
                                           getVkPhysicalDevice()),
                VulkanCommon::StagedUploader(
                        getVkDevice(),
                        getGraphicsQueueParameters().getVkQueue(),
                        m_vulkan_tutorial18_parameters.getRenderingResources()
                                [0]
                                        .getVkCommandBuffer()),
                m_vulkan_tutorial18_parameters.getStagingBufferParameters(),
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

bool Tutorial18::createTankTexture() {
    // Shared by both tank instances - TankB's own constructor loads
    // "TestImage.raw" for all three of its VBOShaderLibrary parts.
    std::vector<char> texture_data =
            Tools::getRawImageData("TestImage.raw", 1024, 1024);
    if (texture_data.empty()) {
        return false;
    }

    return createTextureFromPixels(
            1024,
            1024,
            texture_data,
            m_vulkan_tutorial18_parameters.getTankImageParameters());
}

bool Tutorial18::createFontAtlas() {
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
            m_vulkan_tutorial18_parameters.getFontImageParameters());
}

Tutorial18UniformBufferData3D Tutorial18::get3DUniformBufferData() const {
    Tutorial18UniformBufferData3D data{};
    data.view = glm::lookAt(m_camera.eye(),
                            m_camera.target(),
                            Math::Vec3<float>(0.0f, 1.0f, 0.0f));

    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    // Near/far sized for this tutorial's wider two-tank world (see the
    // constructor's own comment), not copied from Tutorial16's range.
    data.projection = Tools::getPerspectiveProjectionMatrix(
            width / height, 45.0f, 1.0f, 4000.0f);

    return data;
}

bool Tutorial18::update3DUniformBufferData() {
    Tutorial18UniformBufferData3D const uniform_data = get3DUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial18_parameters.getUniformBuffer3DParameters();

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

Math::Mat4<float> Tutorial18::getHudUniformBufferData() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    return Tools::getOrthographicProjectionMatrix(
            0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

bool Tutorial18::updateHudUniformBufferData() {
    Math::Mat4<float> const uniform_data = getHudUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial18_parameters.getUniformBufferHudParameters();

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

bool Tutorial18::createUniformBuffers() {
    BufferParameters& uniform_buffer_3d =
            m_vulkan_tutorial18_parameters.getUniformBuffer3DParameters();
    uniform_buffer_3d.setSize(sizeof(Tutorial18UniformBufferData3D));
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

    BufferParameters& uniform_buffer_hud =
            m_vulkan_tutorial18_parameters.getUniformBufferHudParameters();
    uniform_buffer_hud.setSize(sizeof(Math::Mat4<float>));
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      uniform_buffer_hud)) {
        Logging::error(LOG_TAG, "Could not create HUD uniform buffer!");
        return false;
    }
    return updateHudUniformBufferData();
}

bool Tutorial18::createDescriptorSetLayouts() {
    // 3D layout matches Tutorial16's shaders exactly (0 = sampler in the
    // fragment stage, 1 = uniform buffer in the vertex stage).
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
    m_vulkan_tutorial18_parameters.setVk3DDescriptorSetLayout(vk_3d_layout);

    // HUD layout matches Tutorial15/17's shaders exactly (0 = uniform
    // buffer in the vertex stage, 1 = sampler in the fragment stage).
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings_hud = {
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
    VkDescriptorSetLayoutCreateInfo layout_create_info_hud = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount =
                    static_cast<std::uint32_t>(layout_bindings_hud.size()),
            .pBindings = layout_bindings_hud.data()};
    VkDescriptorSetLayout vk_hud_layout;
    if (vkCreateDescriptorSetLayout(getVkDevice(),
                                    &layout_create_info_hud,
                                    nullptr,
                                    &vk_hud_layout) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create HUD descriptor set layout!");
        return false;
    }
    m_vulkan_tutorial18_parameters.setVkHudDescriptorSetLayout(vk_hud_layout);

    return true;
}

bool Tutorial18::createDescriptorPool() {
    std::vector<VkDescriptorPoolSize> pool_sizes = {
            {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 2},
            {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = 2}};

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .maxSets = 2,
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
    m_vulkan_tutorial18_parameters.setVkDescriptorPool(vk_descriptor_pool);

    return true;
}

bool Tutorial18::allocateDescriptorSets() {
    VkDescriptorSetLayout vk_3d_layout =
            m_vulkan_tutorial18_parameters.getVk3DDescriptorSetLayout();
    VkDescriptorSetAllocateInfo allocate_info_3d = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool =
                    m_vulkan_tutorial18_parameters.getVkDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_3d_layout};
    VkDescriptorSet vk_3d_set;
    if (vkAllocateDescriptorSets(
                getVkDevice(), &allocate_info_3d, &vk_3d_set) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate 3D descriptor set!");
        return false;
    }
    m_vulkan_tutorial18_parameters.set3DVkDescriptorSet(vk_3d_set);

    VkDescriptorSetLayout vk_hud_layout =
            m_vulkan_tutorial18_parameters.getVkHudDescriptorSetLayout();
    VkDescriptorSetAllocateInfo allocate_info_hud = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool =
                    m_vulkan_tutorial18_parameters.getVkDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_hud_layout};
    VkDescriptorSet vk_hud_set;
    if (vkAllocateDescriptorSets(getVkDevice(),
                                 &allocate_info_hud,
                                 &vk_hud_set) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate HUD descriptor set!");
        return false;
    }
    m_vulkan_tutorial18_parameters.setHudVkDescriptorSet(vk_hud_set);

    return true;
}

bool Tutorial18::updateDescriptorSets() {
    BufferParameters& uniform_buffer_3d =
            m_vulkan_tutorial18_parameters.getUniformBuffer3DParameters();
    ImageParameters& tank_image =
            m_vulkan_tutorial18_parameters.getTankImageParameters();
    VkDescriptorImageInfo tank_image_info = {
            .sampler = tank_image.getVkSampler(),
            .imageView = tank_image.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
    VkDescriptorBufferInfo buffer_info_3d = {
            .buffer = uniform_buffer_3d.getVkBuffer(),
            .offset = 0,
            .range = uniform_buffer_3d.getSize()};

    BufferParameters& uniform_buffer_hud =
            m_vulkan_tutorial18_parameters.getUniformBufferHudParameters();
    ImageParameters& font_image =
            m_vulkan_tutorial18_parameters.getFontImageParameters();
    VkDescriptorImageInfo font_image_info = {
            .sampler = font_image.getVkSampler(),
            .imageView = font_image.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
    VkDescriptorBufferInfo buffer_info_hud = {
            .buffer = uniform_buffer_hud.getVkBuffer(),
            .offset = 0,
            .range = uniform_buffer_hud.getSize()};

    std::vector<VkWriteDescriptorSet> descriptor_writes = {
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial18_parameters.get3DVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &tank_image_info,
             .pBufferInfo = nullptr,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial18_parameters.get3DVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &buffer_info_3d,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial18_parameters.getHudVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &buffer_info_hud,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial18_parameters.getHudVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &font_image_info,
             .pBufferInfo = nullptr,
             .pTexelBufferView = nullptr}};

    vkUpdateDescriptorSets(
            getVkDevice(),
            static_cast<std::uint32_t>(descriptor_writes.size()),
            descriptor_writes.data(),
            0,
            nullptr);
    return true;
}

bool Tutorial18::createRenderPass() {
    VkAttachmentDescription attachment_descriptions[] = {
            {.flags = 0,
             .format = getSwapchainParameters().getVkFormat(),
             .samples = VK_SAMPLE_COUNT_1_BIT,
             .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
             .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
             .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
             .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
             .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
             .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
            {.flags = 0,
             .format = c_depth_format,
             .samples = VK_SAMPLE_COUNT_1_BIT,
             .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
             .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
             .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
             .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
             .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
             .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}};

    VkAttachmentReference color_attachment_references[] = {
            {.attachment = 0,
             .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}};
    VkAttachmentReference depth_attachment_reference = {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpass_descriptions[] = {
            {.flags = 0,
             .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
             .inputAttachmentCount = 0,
             .pInputAttachments = nullptr,
             .colorAttachmentCount = 1,
             .pColorAttachments = color_attachment_references,
             .pResolveAttachments = nullptr,
             .pDepthStencilAttachment = &depth_attachment_reference,
             .preserveAttachmentCount = 0,
             .pPreserveAttachments = nullptr}};

    VkRenderPassCreateInfo render_pass_create_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .attachmentCount = 2,
            .pAttachments = attachment_descriptions,
            .subpassCount = 1,
            .pSubpasses = subpass_descriptions,
            .dependencyCount = 0,
            .pDependencies = nullptr};

    if (vkCreateRenderPass(
                getVkDevice(),
                &render_pass_create_info,
                nullptr,
                &m_vulkan_tutorial18_parameters.getVkRenderPass()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

bool Tutorial18::createPipelineLayouts() {
    VkDescriptorSetLayout vk_3d_layout =
            m_vulkan_tutorial18_parameters.getVk3DDescriptorSetLayout();
    VkPushConstantRange push_constant_range = {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
                          VK_SHADER_STAGE_FRAGMENT_BIT,
            .offset = 0,
            .size = sizeof(Tutorial18PushConstants)};
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
                &m_vulkan_tutorial18_parameters.getVk3DPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create 3D pipeline layout!");
        return false;
    }

    VkDescriptorSetLayout vk_hud_layout =
            m_vulkan_tutorial18_parameters.getVkHudDescriptorSetLayout();
    VkPipelineLayoutCreateInfo layout_create_info_hud = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &vk_hud_layout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr};
    if (vkCreatePipelineLayout(
                getVkDevice(),
                &layout_create_info_hud,
                nullptr,
                &m_vulkan_tutorial18_parameters.getVkHudPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create HUD pipeline layout!");
        return false;
    }

    return true;
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
Tutorial18::createShaderModule(const char* filename) {
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

bool Tutorial18::createPipelines() {
    // --- 3D pipeline (two tanks, depth-tested) ---
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module_3d =
                    createShaderModule("shader.18_3d.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module_3d =
                    createShaderModule("shader.18_3d.frag.spv");
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
             .stride = Tutorial18Vertex3DAttributeTraits::stride,
             .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};
    std::vector<VkVertexInputAttributeDescription> vertex_attributes_3d = {
            {.location = 0,
             .binding = 0,
             .format = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset = offsetof(struct Tutorial18Vertex3DData, position)},
            {.location = 1,
             .binding = 0,
             .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(struct Tutorial18Vertex3DData, texcoord)}};
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
            // NONE: this hand-authored asset's winding order hasn't been
            // verified against this project's usual CCW convention,
            // same reasoning Tutorial16 uses for its own cullMode.
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

    // Real depth testing - two independent 3D objects under a freely
    // orbiting camera, unlike Tutorial16's single clustered object.
    VkPipelineDepthStencilStateCreateInfo depth_stencil_3d = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
            .front = {},
            .back = {},
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f};

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
            .pDepthStencilState = &depth_stencil_3d,
            .pColorBlendState = &color_blend_state_3d,
            .pDynamicState = &dynamic_state_create_info,
            .layout = m_vulkan_tutorial18_parameters.getVk3DPipelineLayout(),
            .renderPass = m_vulkan_tutorial18_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};
    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info_3d,
                nullptr,
                &m_vulkan_tutorial18_parameters.getVk3DGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create 3D graphics pipeline!");
        return false;
    }

    // --- HUD pipeline (2D overlay, depth test/write disabled) ---
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module_hud =
                    createShaderModule("shader.18_hud.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module_hud =
                    createShaderModule("shader.18_hud.frag.spv");
    if (!vertex_shader_module_hud || !fragment_shader_module_hud) {
        return false;
    }

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_hud = {
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_VERTEX_BIT,
             .module = vertex_shader_module_hud.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr},
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
             .module = fragment_shader_module_hud.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr}};

    std::vector<VkVertexInputBindingDescription> vertex_bindings_hud = {
            {.binding = 0,
             .stride = Tutorial18VertexHudAttributeTraits::stride,
             .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};
    std::vector<VkVertexInputAttributeDescription> vertex_attributes_hud = {
            {.location = 0,
             .binding = 0,
             .format = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset = offsetof(struct Tutorial18VertexHudData, position)},
            {.location = 1,
             .binding = 0,
             .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(struct Tutorial18VertexHudData, texcoord)},
            {.location = 2,
             .binding = 0,
             .format = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset = offsetof(struct Tutorial18VertexHudData, color)}};
    VkPipelineVertexInputStateCreateInfo vertex_input_state_hud = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount =
                    static_cast<std::uint32_t>(vertex_bindings_hud.size()),
            .pVertexBindingDescriptions = vertex_bindings_hud.data(),
            .vertexAttributeDescriptionCount =
                    static_cast<std::uint32_t>(vertex_attributes_hud.size()),
            .pVertexAttributeDescriptions = vertex_attributes_hud.data()};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_hud = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE};

    VkPipelineRasterizationStateCreateInfo rasterization_hud = {
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

    // Depth test/write off: the render pass has a depth attachment (the
    // 3D pass above uses it), but the HUD is a 2D overlay drawn after -
    // mirrors GameState::drawHUD()'s own glDisable(GL_DEPTH_TEST)
    // bracketing, adapted to Vulkan's per-pipeline depth state.
    VkPipelineDepthStencilStateCreateInfo depth_stencil_hud = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthTestEnable = VK_FALSE,
            .depthWriteEnable = VK_FALSE,
            .depthCompareOp = VK_COMPARE_OP_ALWAYS,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
            .front = {},
            .back = {},
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f};

    VkPipelineColorBlendAttachmentState color_blend_attachment_hud = {
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
    VkPipelineColorBlendStateCreateInfo color_blend_state_hud = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &color_blend_attachment_hud,
            .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};

    VkGraphicsPipelineCreateInfo pipeline_create_info_hud = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount =
                    static_cast<std::uint32_t>(shader_stages_hud.size()),
            .pStages = shader_stages_hud.data(),
            .pVertexInputState = &vertex_input_state_hud,
            .pInputAssemblyState = &input_assembly_hud,
            .pTessellationState = nullptr,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterization_hud,
            .pMultisampleState = &multisample_state,
            .pDepthStencilState = &depth_stencil_hud,
            .pColorBlendState = &color_blend_state_hud,
            .pDynamicState = &dynamic_state_create_info,
            .layout = m_vulkan_tutorial18_parameters.getVkHudPipelineLayout(),
            .renderPass = m_vulkan_tutorial18_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};
    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info_hud,
                nullptr,
                &m_vulkan_tutorial18_parameters.getVkHudGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create HUD graphics pipeline!");
        return false;
    }

    return true;
}

std::vector<Tutorial18Vertex3DData> Tutorial18::loadPartVertexData(
        const char* mesh_filename) const {
    std::vector<Tools::OglVertexData> const mesh_data =
            Tools::loadOglMeshData(mesh_filename);
    std::vector<Tutorial18Vertex3DData> vertex_data;
    vertex_data.reserve(mesh_data.size());
    for (Tools::OglVertexData const& vertex : mesh_data) {
        vertex_data.push_back(
                {Math::Vec4<float>(vertex.position, 1.0f), vertex.texcoord});
    }
    return vertex_data;
}

bool Tutorial18::createPartVertexBuffer(const char* mesh_filename,
                                        BufferParameters& vertex_buffer,
                                        std::uint32_t& vertex_count) {
    std::vector<Tutorial18Vertex3DData> const vertex_data =
            loadPartVertexData(mesh_filename);
    if (vertex_data.empty()) {
        Logging::error(
                LOG_TAG, "Could not load mesh data from \"", mesh_filename, "\"!");
        return false;
    }
    vertex_count = static_cast<std::uint32_t>(vertex_data.size());

    vertex_buffer.setSize(static_cast<std::uint32_t>(
            vertex_data.size() * sizeof(vertex_data[0])));
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      vertex_buffer)) {
        Logging::error(LOG_TAG, "Could not create vertex buffer!");
        return false;
    }

    BufferParameters& staging_buffer =
            m_vulkan_tutorial18_parameters.getStagingBufferParameters();
    std::uint32_t const data_size = vertex_buffer.getSize();
    if (data_size > staging_buffer.getSize()) {
        Logging::error(LOG_TAG,
                       "Vertex data (",
                       data_size,
                       " bytes) does not fit in the staging buffer (",
                       staging_buffer.getSize(),
                       " bytes)!");
        return false;
    }

    void* staging_buffer_memory_pointer;
    if (vkMapMemory(getVkDevice(),
                    staging_buffer.getVkDeviceMemory(),
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &staging_buffer_memory_pointer) != VK_SUCCESS) {
        Logging::error(
                LOG_TAG,
                "Could not map memory and upload data to a staging buffer!");
        return false;
    }
    std::memcpy(staging_buffer_memory_pointer, vertex_data.data(), data_size);
    VkMappedMemoryRange flush_range = {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .pNext = nullptr,
            .memory = staging_buffer.getVkDeviceMemory(),
            .offset = 0,
            .size = VK_WHOLE_SIZE};
    vkFlushMappedMemoryRanges(getVkDevice(), 1, &flush_range);
    vkUnmapMemory(getVkDevice(), staging_buffer.getVkDeviceMemory());

    VkCommandBuffer command_buffer =
            m_vulkan_tutorial18_parameters.getRenderingResources()[0]
                    .getVkCommandBuffer();
    VkCommandBufferBeginInfo command_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr};
    vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

    VkBufferCopy buffer_copy_info = {
            .srcOffset = 0, .dstOffset = 0, .size = data_size};
    vkCmdCopyBuffer(command_buffer,
                    staging_buffer.getVkBuffer(),
                    vertex_buffer.getVkBuffer(),
                    1,
                    &buffer_copy_info);

    VkBufferMemoryBarrier buffer_memory_barrier = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = vertex_buffer.getVkBuffer(),
            .offset = 0,
            .size = VK_WHOLE_SIZE};
    vkCmdPipelineBarrier(command_buffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                         0,
                         0,
                         nullptr,
                         1,
                         &buffer_memory_barrier,
                         0,
                         nullptr);

    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = nullptr,
                                .waitSemaphoreCount = 0,
                                .pWaitSemaphores = nullptr,
                                .pWaitDstStageMask = nullptr,
                                .commandBufferCount = 1,
                                .pCommandBuffers = &command_buffer,
                                .signalSemaphoreCount = 0,
                                .pSignalSemaphores = nullptr};
    if (vkQueueSubmit(getGraphicsQueueParameters().getVkQueue(),
                      1,
                      &submit_info,
                      VK_NULL_HANDLE) != VK_SUCCESS) {
        return false;
    }
    vkDeviceWaitIdle(getVkDevice());

    return true;
}

bool Tutorial18::createTankVertexBuffers() {
    std::uint32_t body_vertex_count = 0;
    if (!createPartVertexBuffer(
                "Hellfire_Body.ogl",
                m_vulkan_tutorial18_parameters.getBodyVertexBufferParameters(),
                body_vertex_count)) {
        return false;
    }
    m_vulkan_tutorial18_parameters.setBodyVertexCount(body_vertex_count);

    std::uint32_t head_vertex_count = 0;
    if (!createPartVertexBuffer(
                "Hellfire_Head.ogl",
                m_vulkan_tutorial18_parameters.getHeadVertexBufferParameters(),
                head_vertex_count)) {
        return false;
    }
    m_vulkan_tutorial18_parameters.setHeadVertexCount(head_vertex_count);

    std::uint32_t turret_vertex_count = 0;
    if (!createPartVertexBuffer(
                "Hellfire_Turret.ogl",
                m_vulkan_tutorial18_parameters
                        .getTurretVertexBufferParameters(),
                turret_vertex_count)) {
        return false;
    }
    m_vulkan_tutorial18_parameters.setTurretVertexCount(turret_vertex_count);

    return true;
}

const std::array<Tutorial18PlayerInfo, 2>& Tutorial18::getPlayers() const {
    // Illustrative snapshot data (positions/colors/HP/power are this
    // tutorial's own reasonable choices, not extracted from a fixed
    // in-source default - see Tutorial18.h's top comment for exactly
    // which fields are real vs. illustrative). max_hp = armor*100 is
    // the real formula GameState::drawHUD() itself uses; TankB's real
    // armor is 5, so 500.
    static std::array<Tutorial18PlayerInfo, 2> const players = {
            {{Math::Vec3<float>(400.0f, 0.0f, 0.0f),
              Math::Vec4<float>(0.85f, 0.25f, 0.25f, 1.0f),
              "Player 1",
              460,
              500,
              0.65f},
             {Math::Vec3<float>(-400.0f, 0.0f, 0.0f),
              Math::Vec4<float>(0.3f, 0.45f, 0.9f, 1.0f),
              "Player 2",
              150,
              500,
              0.3f}}};
    return players;
}

Math::Mat4<float> Tutorial18::getBodyModelMatrix(
        Math::Vec3<float> const& world_position) const {
    return buildPartMatrix(world_position + c_body_offset);
}

Math::Mat4<float> Tutorial18::getHeadModelMatrix(
        Math::Vec3<float> const& world_position) const {
    Math::Vec3<float> const body_translation = world_position + c_body_offset;
    return buildPartMatrix(
            body_translation +
            rotateOffsetThroughBasis(getPartBasis(), c_head_offset));
}

Math::Mat4<float> Tutorial18::getTurretModelMatrix(
        Math::Vec3<float> const& world_position) const {
    Math::Vec3<float> const body_translation = world_position + c_body_offset;
    Math::Vec3<float> const head_translation =
            body_translation +
            rotateOffsetThroughBasis(getPartBasis(), c_head_offset);
    return buildPartMatrix(
            head_translation +
            rotateOffsetThroughBasis(getPartBasis(), c_turret_offset));
}

Math::Vec2<float> Tutorial18::getPanelSize() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    return Math::Vec2<float>(width * 0.42f, height * 0.27f);
}

Math::Vec2<float> Tutorial18::getPanelTopLeft(std::size_t player_index) const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    Math::Vec2<float> const size = getPanelSize();
    float const margin = width * 0.03f;
    float const panel_top = height * 0.03f;
    if (player_index == 0) {
        return Math::Vec2<float>(margin, panel_top);
    }
    return Math::Vec2<float>(width - margin - size.x, panel_top);
}

void Tutorial18::appendGlyphQuad(
        std::vector<Tutorial18VertexHudData>& vertex_data,
        const BitmapFontGlyphQuad& glyph,
        Math::Vec4<float> color) const {
    Math::Vec2<float> const& top_left = glyph.top_left;
    Math::Vec2<float> const& bottom_right = glyph.bottom_right;
    Math::Vec2<float> const& uv_top_left = glyph.uv_top_left;
    Math::Vec2<float> const& uv_bottom_right = glyph.uv_bottom_right;

    Tutorial18VertexHudData const top_left_vertex{
            Math::Vec4<float>(top_left.x, top_left.y, 0.0f, 1.0f),
            uv_top_left,
            color};
    Tutorial18VertexHudData const bottom_left_vertex{
            Math::Vec4<float>(top_left.x, bottom_right.y, 0.0f, 1.0f),
            Math::Vec2<float>(uv_top_left.x, uv_bottom_right.y),
            color};
    Tutorial18VertexHudData const bottom_right_vertex{
            Math::Vec4<float>(bottom_right.x, bottom_right.y, 0.0f, 1.0f),
            uv_bottom_right,
            color};
    Tutorial18VertexHudData const top_right_vertex{
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

void Tutorial18::appendColoredQuad(
        std::vector<Tutorial18VertexHudData>& vertex_data,
        const std::array<Math::Vec2<float>, 4>& corners,
        Math::Vec4<float> color) const {
    Math::Vec2<float> const solid_uv = m_font.solidTexelUv();
    std::array<Tutorial18VertexHudData, 4> quad_vertices;
    for (std::size_t i = 0; i < corners.size(); ++i) {
        quad_vertices[i] = Tutorial18VertexHudData{
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

void Tutorial18::appendText(std::vector<Tutorial18VertexHudData>& vertex_data,
                            const std::string& text,
                            Math::Vec2<float> origin,
                            Math::Vec4<float> color) const {
    std::vector<BitmapFontGlyphQuad> const glyphs =
            m_font.layoutText(text, origin);
    for (BitmapFontGlyphQuad const& glyph : glyphs) {
        appendGlyphQuad(vertex_data, glyph, color);
    }
}

void Tutorial18::appendBar(std::vector<Tutorial18VertexHudData>& vertex_data,
                           Math::Vec2<float> top_left,
                           Math::Vec2<float> size,
                           float ratio,
                           Math::Vec4<float> (*get_bar_color)(float)) const {
    Math::Vec4<float> const backing_color(0.25f, 0.25f, 0.25f, 1.0f);
    std::array<Math::Vec2<float>, 4> const backing_corners = {
            top_left,
            Math::Vec2<float>(top_left.x, top_left.y + size.y),
            Math::Vec2<float>(top_left.x + size.x, top_left.y + size.y),
            Math::Vec2<float>(top_left.x + size.x, top_left.y)};
    appendColoredQuad(vertex_data, backing_corners, backing_color);

    float const fill_width = size.x * std::clamp(ratio, 0.0f, 1.0f);
    if (fill_width > 0.0f) {
        Math::Vec4<float> const fill_color = get_bar_color(ratio);
        std::array<Math::Vec2<float>, 4> const fill_corners = {
                top_left,
                Math::Vec2<float>(top_left.x, top_left.y + size.y),
                Math::Vec2<float>(top_left.x + fill_width,
                                  top_left.y + size.y),
                Math::Vec2<float>(top_left.x + fill_width, top_left.y)};
        appendColoredQuad(vertex_data, fill_corners, fill_color);
    }
}

std::vector<Tutorial18VertexHudData> Tutorial18::buildHudVertexData() const {
    std::vector<Tutorial18VertexHudData> vertex_data;
    vertex_data.reserve(kMaxHudVertexCount);

    Math::Vec4<float> const text_color(0.05f, 0.05f, 0.05f, 1.0f);
    Math::Vec4<float> const panel_color(0.75f, 0.75f, 0.75f, 1.0f);
    Math::Vec2<float> const panel_size = getPanelSize();
    std::array<Tutorial18PlayerInfo, 2> const& players = getPlayers();

    for (std::size_t i = 0; i < players.size(); ++i) {
        Math::Vec2<float> const panel_top_left = getPanelTopLeft(i);

        std::vector<UiGeometry::ColoredQuad> const bevel =
                UiGeometry::buildButtonBevel(
                        panel_top_left, panel_size, panel_color, false);
        for (UiGeometry::ColoredQuad const& quad : bevel) {
            appendColoredQuad(vertex_data, quad.corners, quad.color);
        }

        appendText(vertex_data,
                  players[i].name,
                  Math::Vec2<float>(panel_top_left.x + 16.0f,
                                    panel_top_left.y + 26.0f),
                  players[i].team_color);

        std::string const hp_text = "HP: " + std::to_string(players[i].hp) +
                                    " / " + std::to_string(players[i].max_hp);
        appendText(vertex_data,
                  hp_text,
                  Math::Vec2<float>(panel_top_left.x + 16.0f,
                                    panel_top_left.y + 52.0f),
                  text_color);

        float const health_ratio = static_cast<float>(players[i].hp) /
                                   static_cast<float>(players[i].max_hp);
        appendBar(vertex_data,
                 Math::Vec2<float>(panel_top_left.x + 16.0f,
                                   panel_top_left.y + 60.0f),
                 Math::Vec2<float>(panel_size.x - 32.0f, 14.0f),
                 health_ratio,
                 &getHealthBarColor);

        std::string const power_text =
                "Power: " +
                std::to_string(
                        static_cast<int>(players[i].power_ratio * 1000.0f));
        appendText(vertex_data,
                  power_text,
                  Math::Vec2<float>(panel_top_left.x + 16.0f,
                                    panel_top_left.y + 100.0f),
                  text_color);

        appendBar(vertex_data,
                 Math::Vec2<float>(panel_top_left.x + 16.0f,
                                   panel_top_left.y + 108.0f),
                 Math::Vec2<float>(panel_size.x - 32.0f, 14.0f),
                 players[i].power_ratio,
                 &getPowerBarColor);
    }

    return vertex_data;
}

bool Tutorial18::updateHudVertexBufferData() {
    std::vector<Tutorial18VertexHudData> const vertex_data =
            buildHudVertexData();
    if (vertex_data.size() > kMaxHudVertexCount) {
        Logging::error(LOG_TAG,
                       "HUD vertex data (",
                       vertex_data.size(),
                       " vertices) exceeds kMaxHudVertexCount (",
                       kMaxHudVertexCount,
                       ")!");
        return false;
    }
    m_vulkan_tutorial18_parameters.setHudVertexCount(
            static_cast<std::uint32_t>(vertex_data.size()));

    BufferParameters& vertex_buffer =
            m_vulkan_tutorial18_parameters.getHudVertexBufferParameters();
    void* vertex_buffer_memory_pointer;
    if (vkMapMemory(getVkDevice(),
                    vertex_buffer.getVkDeviceMemory(),
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &vertex_buffer_memory_pointer) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not map HUD vertex buffer memory!");
        return false;
    }
    if (!vertex_data.empty()) {
        std::memcpy(vertex_buffer_memory_pointer,
                    vertex_data.data(),
                    vertex_data.size() * sizeof(Tutorial18VertexHudData));
    }
    vkUnmapMemory(getVkDevice(), vertex_buffer.getVkDeviceMemory());

    return true;
}

bool Tutorial18::createHudVertexBuffer() {
    BufferParameters& vertex_buffer =
            m_vulkan_tutorial18_parameters.getHudVertexBufferParameters();
    vertex_buffer.setSize(static_cast<std::uint32_t>(
            kMaxHudVertexCount * sizeof(Tutorial18VertexHudData)));
    // Host-visible/coherent - written once here, not every frame, since
    // this tutorial's HUD content is a static illustrative snapshot
    // (see Tutorial18.h's top comment) with no click-driven state to
    // rebuild for, unlike Tutorial15/17's own per-frame rebuild.
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      vertex_buffer)) {
        Logging::error(LOG_TAG, "Could not create HUD vertex buffer!");
        return false;
    }

    return updateHudVertexBufferData();
}

bool Tutorial18::createFramebuffer(VkFramebuffer& framebuffer,
                                   VkImageView image_view) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(getVkDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    std::array<VkImageView, 2> const attachments = {
            image_view,
            m_vulkan_tutorial18_parameters.getDepthImageParameters()
                    .getVkImageView()};

    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_vulkan_tutorial18_parameters.getVkRenderPass(),
            .attachmentCount = static_cast<std::uint32_t>(attachments.size()),
            .pAttachments = attachments.data(),
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

bool Tutorial18::prepareFrame(VkCommandBuffer command_buffer,
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

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = {.float32 = {0.15f, 0.15f, 0.18f, 1.0f}};
    clear_values[1].depthStencil = {.depth = 1.0f, .stencil = 0};

    VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = m_vulkan_tutorial18_parameters.getVkRenderPass(),
            .framebuffer = framebuffer,
            .renderArea = {.offset = {.x = 0, .y = 0},
                           .extent = getSwapchainParameters().getVkExtent2d()},
            .clearValueCount = static_cast<std::uint32_t>(clear_values.size()),
            .pClearValues = clear_values.data()};

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

    // --- 3D pass: two tanks, three parts each, one push constant per
    // part carrying that tank's model matrix and team-color tint (see
    // Tutorial18.h's top comment). Vertex buffers are the same three
    // (body/head/turret) static buffers reused for both tank instances.
    vkCmdBindPipeline(command_buffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_vulkan_tutorial18_parameters.getVk3DGraphicsPipeline());
    VkDescriptorSet vk_3d_descriptor_set =
            m_vulkan_tutorial18_parameters.get3DVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial18_parameters.getVk3DPipelineLayout(),
            0,
            1,
            &vk_3d_descriptor_set,
            0,
            nullptr);

    VkDeviceSize const zero_offset = 0;
    std::array<Tutorial18PlayerInfo, 2> const& players = getPlayers();
    for (Tutorial18PlayerInfo const& player : players) {
        Tutorial18PushConstants body_push_constants{
                getBodyModelMatrix(player.world_position), player.team_color};
        vkCmdBindVertexBuffers(
                command_buffer,
                0,
                1,
                &m_vulkan_tutorial18_parameters
                         .getBodyVertexBufferParameters()
                         .getVkBuffer(),
                &zero_offset);
        vkCmdPushConstants(
                command_buffer,
                m_vulkan_tutorial18_parameters.getVk3DPipelineLayout(),
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(Tutorial18PushConstants),
                &body_push_constants);
        vkCmdDraw(command_buffer,
                 m_vulkan_tutorial18_parameters.getBodyVertexCount(),
                 1,
                 0,
                 0);

        Tutorial18PushConstants head_push_constants{
                getHeadModelMatrix(player.world_position), player.team_color};
        vkCmdBindVertexBuffers(
                command_buffer,
                0,
                1,
                &m_vulkan_tutorial18_parameters
                         .getHeadVertexBufferParameters()
                         .getVkBuffer(),
                &zero_offset);
        vkCmdPushConstants(
                command_buffer,
                m_vulkan_tutorial18_parameters.getVk3DPipelineLayout(),
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(Tutorial18PushConstants),
                &head_push_constants);
        vkCmdDraw(command_buffer,
                 m_vulkan_tutorial18_parameters.getHeadVertexCount(),
                 1,
                 0,
                 0);

        Tutorial18PushConstants turret_push_constants{
                getTurretModelMatrix(player.world_position),
                player.team_color};
        vkCmdBindVertexBuffers(
                command_buffer,
                0,
                1,
                &m_vulkan_tutorial18_parameters
                         .getTurretVertexBufferParameters()
                         .getVkBuffer(),
                &zero_offset);
        vkCmdPushConstants(
                command_buffer,
                m_vulkan_tutorial18_parameters.getVk3DPipelineLayout(),
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(Tutorial18PushConstants),
                &turret_push_constants);
        vkCmdDraw(command_buffer,
                 m_vulkan_tutorial18_parameters.getTurretVertexCount(),
                 1,
                 0,
                 0);
    }

    // --- HUD pass: per-player name/HP/power panels, drawn on top ---
    vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial18_parameters.getVkHudGraphicsPipeline());
    vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_vulkan_tutorial18_parameters.getHudVertexBufferParameters()
                     .getVkBuffer(),
            &zero_offset);
    VkDescriptorSet vk_hud_descriptor_set =
            m_vulkan_tutorial18_parameters.getHudVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial18_parameters.getVkHudPipelineLayout(),
            0,
            1,
            &vk_hud_descriptor_set,
            0,
            nullptr);
    vkCmdDraw(command_buffer,
             m_vulkan_tutorial18_parameters.getHudVertexCount(),
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

bool Tutorial18::draw() {
    static std::size_t resource_index = 0;
    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial18_parameters.getRenderingResources();
    RenderingResourceParameters& current_rendering_resource =
            rendering_resources[resource_index];
    VkSwapchainKHR swap_chain = getSwapchainParameters().getVkSwapchainKhr();
    std::uint32_t image_index;

    resource_index = (resource_index + 1) %
                     VulkanTutorial18Parameters::resources_count;

    if (vkWaitForFences(getVkDevice(),
                        1,
                        &current_rendering_resource.getVkFence(),
                        VK_FALSE,
                        1000000000) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Waiting for fence takes too long!");
        return false;
    }
    vkResetFences(getVkDevice(), 1, &current_rendering_resource.getVkFence());

    vkDeviceWaitIdle(getVkDevice());
    if (!update3DUniformBufferData()) {
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
            m_vulkan_tutorial18_parameters.getFinishedRenderingSemaphores()
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

void Tutorial18::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

bool Tutorial18::childOnWindowSizeChanged() {
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
    if (!createDepthResources()) {
        return false;
    }
    if (!createTankTexture()) {
        return false;
    }
    if (!createFontAtlas()) {
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
    if (!createTankVertexBuffers()) {
        return false;
    }
    return createHudVertexBuffer();
}

void Tutorial18::childClear() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return;
    }
    vkDeviceWaitIdle(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial18_parameters.getRenderingResources();
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
                    m_vulkan_tutorial18_parameters.getVkCommandPool(),
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
            m_vulkan_tutorial18_parameters.getFinishedRenderingSemaphores();
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (finished_rendering_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(), finished_rendering_semaphores[i], nullptr);
        }
    }
    finished_rendering_semaphores.clear();

    if (m_vulkan_tutorial18_parameters.getVkCommandPool() != VK_NULL_HANDLE) {
        vkDestroyCommandPool(
                getVkDevice(),
                m_vulkan_tutorial18_parameters.getVkCommandPool(),
                nullptr);
        m_vulkan_tutorial18_parameters.setVkCommandPool(VK_NULL_HANDLE);
    }

    destroyBuffer(
            m_vulkan_tutorial18_parameters.getBodyVertexBufferParameters());
    destroyBuffer(
            m_vulkan_tutorial18_parameters.getHeadVertexBufferParameters());
    destroyBuffer(
            m_vulkan_tutorial18_parameters.getTurretVertexBufferParameters());
    destroyBuffer(
            m_vulkan_tutorial18_parameters.getHudVertexBufferParameters());
    destroyBuffer(m_vulkan_tutorial18_parameters.getStagingBufferParameters());

    if (m_vulkan_tutorial18_parameters.getVk3DGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial18_parameters.getVk3DGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial18_parameters.setVk3DGraphicsPipeline(VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial18_parameters.getVkHudGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial18_parameters.getVkHudGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial18_parameters.setVkHudGraphicsPipeline(
                VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial18_parameters.getVk3DPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial18_parameters.getVk3DPipelineLayout(),
                nullptr);
        m_vulkan_tutorial18_parameters.setVk3DPipelineLayout(VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial18_parameters.getVkHudPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial18_parameters.getVkHudPipelineLayout(),
                nullptr);
        m_vulkan_tutorial18_parameters.setVkHudPipelineLayout(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial18_parameters.getVkRenderPass() != VK_NULL_HANDLE) {
        vkDestroyRenderPass(
                getVkDevice(),
                m_vulkan_tutorial18_parameters.getVkRenderPass(),
                nullptr);
        m_vulkan_tutorial18_parameters.setVkRenderPass(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial18_parameters.getVkDescriptorPool() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(
                getVkDevice(),
                m_vulkan_tutorial18_parameters.getVkDescriptorPool(),
                nullptr);
        m_vulkan_tutorial18_parameters.setVkDescriptorPool(VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial18_parameters.getVk3DDescriptorSetLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(
                getVkDevice(),
                m_vulkan_tutorial18_parameters.getVk3DDescriptorSetLayout(),
                nullptr);
        m_vulkan_tutorial18_parameters.setVk3DDescriptorSetLayout(
                VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial18_parameters.getVkHudDescriptorSetLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(
                getVkDevice(),
                m_vulkan_tutorial18_parameters.getVkHudDescriptorSetLayout(),
                nullptr);
        m_vulkan_tutorial18_parameters.setVkHudDescriptorSetLayout(
                VK_NULL_HANDLE);
    }

    destroyBuffer(m_vulkan_tutorial18_parameters.getUniformBuffer3DParameters());
    destroyBuffer(
            m_vulkan_tutorial18_parameters.getUniformBufferHudParameters());

    VulkanCommon::ImageFactory image_factory(getVkDevice(),
                                             getVkPhysicalDevice());
    image_factory.destroy(
            m_vulkan_tutorial18_parameters.getTankImageParameters());
    image_factory.destroy(
            m_vulkan_tutorial18_parameters.getFontImageParameters());

    destroyDepthResources();
}

}  // namespace vulkan_graphix
