#include "vulkan_graphix/Tutorial22.h"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cmath>
#include <cstddef>
#include <cstring>

#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/UiGeometry.h"
#include "vulkan_graphix/VulkanCommon.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
constexpr VkFormat c_depth_format = VK_FORMAT_D32_SFLOAT;

// TankB's own constructor values - see Tutorial16.h's top comment for
// why these are taken verbatim.
constexpr float c_part_scale = 50.0f;
Math::Vec3<float> const c_body_offset(0.0f, 65.0f, 0.0f);
Math::Vec3<float> const c_head_offset(0.0f, 70.0f, 0.0f);
Math::Vec3<float> const c_turret_offset(0.0f, 0.0001f, -50.0001f);

// Tank::initBody()/initHead()/initTurret() all set this identical
// right/up/at basis.
Math::Mat4<float> const& getPartBasis() {
    static Math::Mat4<float> const basis(
            Math::Vec4<float>(0.0f, 0.0f, 1.0f, 0.0f),
            Math::Vec4<float>(0.0f, 1.0f, 0.0f, 0.0f),
            Math::Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f),
            Math::Vec4<float>(0.0f, 0.0f, 0.0f, 1.0f));
    return basis;
}

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

Math::Vec3<float> const& getBodyTranslation() { return c_body_offset; }

Math::Vec3<float> const& getHeadTranslation() {
    static Math::Vec3<float> const translation =
            getBodyTranslation() +
            rotateOffsetThroughBasis(getPartBasis(), c_head_offset);
    return translation;
}

Math::Vec3<float> const& getTurretTranslation() {
    static Math::Vec3<float> const translation =
            getHeadTranslation() +
            rotateOffsetThroughBasis(getPartBasis(), c_turret_offset);
    return translation;
}

// Fixed preview camera - matches Tutorial16's own working OrbitCamera
// values (yaw=0.6, pitch=-0.05, distance=650), just evaluated once since
// ReadyMenu's own preview camera is a static gluLookAt, not mouse-
// orbitable - only the tank itself spins.
Math::Vec3<float> const c_preview_eye =
        650.0f * Math::Vec3<float>(std::cos(-0.05f) * std::sin(0.6f),
                                   std::sin(-0.05f),
                                   std::cos(-0.05f) * std::cos(0.6f));
}  // namespace

// ************************************************************ //
// VulkanTutorial22Parameters                                   //
// ************************************************************ //
VulkanTutorial22Parameters::VulkanTutorial22Parameters()
        : m_vk_render_pass(VK_NULL_HANDLE)
        , m_vk_tank_descriptor_set_layout(VK_NULL_HANDLE)
        , m_vk_panel_descriptor_set_layout(VK_NULL_HANDLE)
        , m_vk_descriptor_pool(VK_NULL_HANDLE)
        , m_vk_tank_descriptor_set(VK_NULL_HANDLE)
        , m_vk_panel_descriptor_set(VK_NULL_HANDLE)
        , m_vk_tank_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_panel_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_tank_graphics_pipeline(VK_NULL_HANDLE)
        , m_vk_panel_graphics_pipeline(VK_NULL_HANDLE)
        , m_tank_vertex_counts{0, 0, 0}
        , m_panel_vertex_count(0)
        , m_vk_command_pool(VK_NULL_HANDLE)
        , m_rendering_resources(resources_count) {}

const VkRenderPass& VulkanTutorial22Parameters::getVkRenderPass() const {
    return m_vk_render_pass;
}
VkRenderPass& VulkanTutorial22Parameters::getVkRenderPass() {
    return m_vk_render_pass;
}
void VulkanTutorial22Parameters::setVkRenderPass(
        const VkRenderPass& vk_render_pass) {
    m_vk_render_pass = vk_render_pass;
}

const ImageParameters& VulkanTutorial22Parameters::getDepthImageParameters()
        const {
    return m_depth_image_parameters;
}
ImageParameters& VulkanTutorial22Parameters::getDepthImageParameters() {
    return m_depth_image_parameters;
}
void VulkanTutorial22Parameters::setDepthImageParameters(
        const ImageParameters& depth_image) {
    m_depth_image_parameters = depth_image;
}

const ImageParameters& VulkanTutorial22Parameters::getTankImageParameters()
        const {
    return m_tank_image_parameters;
}
ImageParameters& VulkanTutorial22Parameters::getTankImageParameters() {
    return m_tank_image_parameters;
}
void VulkanTutorial22Parameters::setTankImageParameters(
        const ImageParameters& image_parameters) {
    m_tank_image_parameters = image_parameters;
}

const ImageParameters& VulkanTutorial22Parameters::getFontImageParameters()
        const {
    return m_font_image_parameters;
}
ImageParameters& VulkanTutorial22Parameters::getFontImageParameters() {
    return m_font_image_parameters;
}
void VulkanTutorial22Parameters::setFontImageParameters(
        const ImageParameters& image_parameters) {
    m_font_image_parameters = image_parameters;
}

const BufferParameters&
VulkanTutorial22Parameters::getTankUniformBufferParameters() const {
    return m_tank_uniform_buffer;
}
BufferParameters&
VulkanTutorial22Parameters::getTankUniformBufferParameters() {
    return m_tank_uniform_buffer;
}
void VulkanTutorial22Parameters::setTankUniformBufferParameters(
        const BufferParameters& uniform_buffer) {
    m_tank_uniform_buffer = uniform_buffer;
}

const BufferParameters&
VulkanTutorial22Parameters::getPanelUniformBufferParameters() const {
    return m_panel_uniform_buffer;
}
BufferParameters&
VulkanTutorial22Parameters::getPanelUniformBufferParameters() {
    return m_panel_uniform_buffer;
}
void VulkanTutorial22Parameters::setPanelUniformBufferParameters(
        const BufferParameters& uniform_buffer) {
    m_panel_uniform_buffer = uniform_buffer;
}

const VkDescriptorSetLayout&
VulkanTutorial22Parameters::getVkTankDescriptorSetLayout() const {
    return m_vk_tank_descriptor_set_layout;
}
VkDescriptorSetLayout&
VulkanTutorial22Parameters::getVkTankDescriptorSetLayout() {
    return m_vk_tank_descriptor_set_layout;
}
void VulkanTutorial22Parameters::setVkTankDescriptorSetLayout(
        const VkDescriptorSetLayout& other) {
    m_vk_tank_descriptor_set_layout = other;
}

const VkDescriptorSetLayout&
VulkanTutorial22Parameters::getVkPanelDescriptorSetLayout() const {
    return m_vk_panel_descriptor_set_layout;
}
VkDescriptorSetLayout&
VulkanTutorial22Parameters::getVkPanelDescriptorSetLayout() {
    return m_vk_panel_descriptor_set_layout;
}
void VulkanTutorial22Parameters::setVkPanelDescriptorSetLayout(
        const VkDescriptorSetLayout& other) {
    m_vk_panel_descriptor_set_layout = other;
}

const VkDescriptorPool& VulkanTutorial22Parameters::getVkDescriptorPool()
        const {
    return m_vk_descriptor_pool;
}
VkDescriptorPool& VulkanTutorial22Parameters::getVkDescriptorPool() {
    return m_vk_descriptor_pool;
}
void VulkanTutorial22Parameters::setVkDescriptorPool(
        const VkDescriptorPool& other) {
    m_vk_descriptor_pool = other;
}

const VkDescriptorSet& VulkanTutorial22Parameters::getTankVkDescriptorSet()
        const {
    return m_vk_tank_descriptor_set;
}
VkDescriptorSet& VulkanTutorial22Parameters::getTankVkDescriptorSet() {
    return m_vk_tank_descriptor_set;
}
void VulkanTutorial22Parameters::setTankVkDescriptorSet(
        const VkDescriptorSet& other) {
    m_vk_tank_descriptor_set = other;
}

const VkDescriptorSet& VulkanTutorial22Parameters::getPanelVkDescriptorSet()
        const {
    return m_vk_panel_descriptor_set;
}
VkDescriptorSet& VulkanTutorial22Parameters::getPanelVkDescriptorSet() {
    return m_vk_panel_descriptor_set;
}
void VulkanTutorial22Parameters::setPanelVkDescriptorSet(
        const VkDescriptorSet& other) {
    m_vk_panel_descriptor_set = other;
}

const VkPipelineLayout& VulkanTutorial22Parameters::getVkTankPipelineLayout()
        const {
    return m_vk_tank_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial22Parameters::getVkTankPipelineLayout() {
    return m_vk_tank_pipeline_layout;
}
void VulkanTutorial22Parameters::setVkTankPipelineLayout(
        const VkPipelineLayout& other) {
    m_vk_tank_pipeline_layout = other;
}

const VkPipelineLayout& VulkanTutorial22Parameters::getVkPanelPipelineLayout()
        const {
    return m_vk_panel_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial22Parameters::getVkPanelPipelineLayout() {
    return m_vk_panel_pipeline_layout;
}
void VulkanTutorial22Parameters::setVkPanelPipelineLayout(
        const VkPipelineLayout& other) {
    m_vk_panel_pipeline_layout = other;
}

const VkPipeline& VulkanTutorial22Parameters::getVkTankGraphicsPipeline()
        const {
    return m_vk_tank_graphics_pipeline;
}
VkPipeline& VulkanTutorial22Parameters::getVkTankGraphicsPipeline() {
    return m_vk_tank_graphics_pipeline;
}
void VulkanTutorial22Parameters::setVkTankGraphicsPipeline(
        const VkPipeline& other) {
    m_vk_tank_graphics_pipeline = other;
}

const VkPipeline& VulkanTutorial22Parameters::getVkPanelGraphicsPipeline()
        const {
    return m_vk_panel_graphics_pipeline;
}
VkPipeline& VulkanTutorial22Parameters::getVkPanelGraphicsPipeline() {
    return m_vk_panel_graphics_pipeline;
}
void VulkanTutorial22Parameters::setVkPanelGraphicsPipeline(
        const VkPipeline& other) {
    m_vk_panel_graphics_pipeline = other;
}

const std::array<BufferParameters, kTutorial22TankPartCount>&
VulkanTutorial22Parameters::getTankVertexBufferParameters() const {
    return m_tank_vertex_buffers;
}
std::array<BufferParameters, kTutorial22TankPartCount>&
VulkanTutorial22Parameters::getTankVertexBufferParameters() {
    return m_tank_vertex_buffers;
}
const std::array<std::uint32_t, kTutorial22TankPartCount>&
VulkanTutorial22Parameters::getTankVertexCounts() const {
    return m_tank_vertex_counts;
}
std::array<std::uint32_t, kTutorial22TankPartCount>&
VulkanTutorial22Parameters::getTankVertexCounts() {
    return m_tank_vertex_counts;
}

const BufferParameters&
VulkanTutorial22Parameters::getPanelVertexBufferParameters() const {
    return m_panel_vertex_buffer;
}
BufferParameters&
VulkanTutorial22Parameters::getPanelVertexBufferParameters() {
    return m_panel_vertex_buffer;
}
void VulkanTutorial22Parameters::setPanelVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_panel_vertex_buffer = vertex_buffer;
}
std::uint32_t VulkanTutorial22Parameters::getPanelVertexCount() const {
    return m_panel_vertex_count;
}
void VulkanTutorial22Parameters::setPanelVertexCount(
        std::uint32_t vertex_count) {
    m_panel_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial22Parameters::getStagingBufferParameters() const {
    return m_staging_buffer;
}
BufferParameters& VulkanTutorial22Parameters::getStagingBufferParameters() {
    return m_staging_buffer;
}
void VulkanTutorial22Parameters::setStagingBufferParameters(
        const BufferParameters& staging_buffer) {
    m_staging_buffer = staging_buffer;
}

const VkCommandPool& VulkanTutorial22Parameters::getVkCommandPool() const {
    return m_vk_command_pool;
}
VkCommandPool& VulkanTutorial22Parameters::getVkCommandPool() {
    return m_vk_command_pool;
}
void VulkanTutorial22Parameters::setVkCommandPool(
        const VkCommandPool& vk_command_pool) {
    m_vk_command_pool = vk_command_pool;
}

const std::vector<RenderingResourceParameters>&
VulkanTutorial22Parameters::getRenderingResources() const {
    return m_rendering_resources;
}
std::vector<RenderingResourceParameters>&
VulkanTutorial22Parameters::getRenderingResources() {
    return m_rendering_resources;
}
void VulkanTutorial22Parameters::setRenderingResources(
        const std::vector<RenderingResourceParameters>& rendering_resources) {
    m_rendering_resources = rendering_resources;
}

const std::vector<VkSemaphore>&
VulkanTutorial22Parameters::getFinishedRenderingSemaphores() const {
    return m_finished_rendering_semaphores;
}
std::vector<VkSemaphore>&
VulkanTutorial22Parameters::getFinishedRenderingSemaphores() {
    return m_finished_rendering_semaphores;
}
void VulkanTutorial22Parameters::setFinishedRenderingSemaphores(
        const std::vector<VkSemaphore>& finished_rendering_semaphores) {
    m_finished_rendering_semaphores = finished_rendering_semaphores;
}

// ************************************************************ //
// Tutorial22                                                   //
// ************************************************************ //
Tutorial22::Tutorial22()
        : m_button_pressed(false), m_click_count(0), m_tank_angle(0.0f) {}

Tutorial22::~Tutorial22() { childClear(); }

void Tutorial22::onMouseButton(int button,
                               bool pressed,
                               int pos_x,
                               int pos_y) {
    constexpr int c_left_button = 1;
    if (button != c_left_button) {
        return;
    }

    if (!pressed) {
        m_button_pressed = false;
        return;
    }

    Math::Vec2<float> const top_left = getButtonTopLeft();
    Math::Vec2<float> const size = getButtonSize();
    float const x = static_cast<float>(pos_x);
    float const y = static_cast<float>(pos_y);
    if (x >= top_left.x && x <= top_left.x + size.x && y >= top_left.y &&
        y <= top_left.y + size.y) {
        m_button_pressed = true;
        ++m_click_count;
    } else {
        m_button_pressed = false;
    }
}

bool Tutorial22::createCommandPool(std::uint32_t queue_family_index,
                                   VkCommandPool* pool) {
    if (!VulkanCommon::FrameResourceFactory(getVkDevice())
                 .createCommandPool(queue_family_index, pool)) {
        Logging::error(LOG_TAG, "Could not create command pool!");
        return false;
    }
    return true;
}

bool Tutorial22::allocateCommandBuffers(VkCommandPool pool,
                                        std::uint32_t count,
                                        VkCommandBuffer* command_buffers) {
    if (!VulkanCommon::FrameResourceFactory(getVkDevice())
                 .allocateCommandBuffers(pool, count, command_buffers)) {
        Logging::error(LOG_TAG, "Could not allocate command buffer!");
        return false;
    }
    return true;
}

bool Tutorial22::createCommandBuffers() {
    if (!createCommandPool(
                getGraphicsQueueParameters().getFamilyIndex(),
                &m_vulkan_tutorial22_parameters.getVkCommandPool())) {
        return false;
    }

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial22_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!allocateCommandBuffers(
                    m_vulkan_tutorial22_parameters.getVkCommandPool(),
                    1,
                    &rendering_resources[i].getVkCommandBuffer())) {
            return false;
        }
    }
    return true;
}

bool Tutorial22::createSemaphores() {
    VulkanCommon::FrameResourceFactory factory(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial22_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!factory.createSemaphore(
                    &rendering_resources[i].getImageAvailableVkSemaphore())) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    std::vector<VkSemaphore>& finished_rendering_semaphores =
            m_vulkan_tutorial22_parameters.getFinishedRenderingSemaphores();
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

bool Tutorial22::createFences() {
    VulkanCommon::FrameResourceFactory factory(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial22_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!factory.createFence(true,
                                 &rendering_resources[i].getVkFence())) {
            Logging::error(LOG_TAG, "Could not create a fence!");
            return false;
        }
    }
    return true;
}

bool Tutorial22::createRenderingResources() {
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

bool Tutorial22::createBuffer(VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags memory_property,
                              BufferParameters& buffer) {
    if (!VulkanCommon::BufferFactory(getVkDevice(), getVkPhysicalDevice())
                 .create(usage, memory_property, buffer)) {
        Logging::error(LOG_TAG, "Could not create buffer!");
        return false;
    }
    return true;
}

bool Tutorial22::createStagingBuffer() {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial22_parameters.getStagingBufferParameters();
    // Reused sequentially for the tank texture and the font atlas -
    // TestImage.raw (1024x1024 RGBA = 4,194,304 bytes) is the larger of
    // the two, sized with headroom above that.
    staging_buffer.setSize(6u * 1024u * 1024u);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

bool Tutorial22::createImage(std::uint32_t width,
                             std::uint32_t height,
                             VkFormat format,
                             VkImageUsageFlags usage,
                             VkImage* image) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .createImage(width, height, format, usage, image);
}

bool Tutorial22::allocateImageMemory(VkImage image,
                                     VkMemoryPropertyFlags property,
                                     VkDeviceMemory* memory) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .allocateMemory(image, property, memory);
}

bool Tutorial22::createImageView(VkImage image,
                                 VkFormat format,
                                 VkImageAspectFlags aspect_mask,
                                 VkImageView* image_view) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .createImageView(image, format, aspect_mask, image_view);
}

bool Tutorial22::createDepthResources() {
    ImageParameters& depth_image =
            m_vulkan_tutorial22_parameters.getDepthImageParameters();

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

bool Tutorial22::destroyDepthResources() {
    VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .destroy(m_vulkan_tutorial22_parameters.getDepthImageParameters());
    return true;
}

bool Tutorial22::createTextureFromPixels(std::uint32_t width,
                                         std::uint32_t height,
                                         const std::vector<char>& pixels,
                                         ImageParameters& out_image_parameters) {
    if (!VulkanCommon::createTextureFromPixels(
                VulkanCommon::ImageFactory(getVkDevice(),
                                           getVkPhysicalDevice()),
                VulkanCommon::StagedUploader(
                        getVkDevice(),
                        getGraphicsQueueParameters().getVkQueue(),
                        m_vulkan_tutorial22_parameters.getRenderingResources()
                                [0]
                                        .getVkCommandBuffer()),
                m_vulkan_tutorial22_parameters.getStagingBufferParameters(),
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

bool Tutorial22::createTankTexture() {
    std::vector<char> texture_data =
            Tools::getRawImageData("TestImage.raw", 1024, 1024);
    if (texture_data.empty()) {
        return false;
    }

    return createTextureFromPixels(
            1024,
            1024,
            texture_data,
            m_vulkan_tutorial22_parameters.getTankImageParameters());
}

bool Tutorial22::createFontAtlas() {
    if (!m_font.load(kFontPath, kFontPixelHeight)) {
        Logging::error(LOG_TAG,
                       "Could not load font \"",
                       kFontPath,
                       "\" - is the fonts-dejavu-core package installed?");
        return false;
    }

    std::vector<char> const atlas_pixels = m_font.atlasPixels();
    return createTextureFromPixels(
            m_font.atlasWidth(),
            m_font.atlasHeight(),
            atlas_pixels,
            m_vulkan_tutorial22_parameters.getFontImageParameters());
}

Math::Vec2<float> Tutorial22::getPreviewSize() const {
    return Math::Vec2<float>(280.0f, 200.0f);
}

Math::Vec2<float> Tutorial22::getPreviewTopLeft() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    Math::Vec2<float> const size = getPreviewSize();
    return Math::Vec2<float>(width * 0.5f - size.x * 0.5f,
                             height * 0.5f - size.y * 0.5f - 40.0f);
}

Tutorial22TankUniformBufferData Tutorial22::getTankUniformBufferData() const {
    Tutorial22TankUniformBufferData data{};
    data.view = glm::lookAt(c_preview_eye,
                            Math::Vec3<float>(0.0f, 0.0f, 0.0f),
                            Math::Vec3<float>(0.0f, 1.0f, 0.0f));

    Math::Vec2<float> const preview_size = getPreviewSize();
    data.projection = Tools::getPerspectiveProjectionMatrix(
            preview_size.x / preview_size.y, 45.0f, 1.0f, 5000.0f);

    return data;
}

bool Tutorial22::updateTankUniformBufferData() {
    Tutorial22TankUniformBufferData const uniform_data =
            getTankUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial22_parameters.getTankUniformBufferParameters();

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

Math::Mat4<float> Tutorial22::getPanelUniformBufferData() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    return Tools::getOrthographicProjectionMatrix(
            0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

bool Tutorial22::updatePanelUniformBufferData() {
    Math::Mat4<float> const uniform_data = getPanelUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial22_parameters.getPanelUniformBufferParameters();

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

bool Tutorial22::createUniformBuffers() {
    BufferParameters& tank_uniform_buffer =
            m_vulkan_tutorial22_parameters.getTankUniformBufferParameters();
    tank_uniform_buffer.setSize(sizeof(Tutorial22TankUniformBufferData));
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      tank_uniform_buffer)) {
        Logging::error(LOG_TAG, "Could not create tank uniform buffer!");
        return false;
    }
    if (!updateTankUniformBufferData()) {
        return false;
    }

    BufferParameters& panel_uniform_buffer =
            m_vulkan_tutorial22_parameters.getPanelUniformBufferParameters();
    panel_uniform_buffer.setSize(sizeof(Math::Mat4<float>));
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      panel_uniform_buffer)) {
        Logging::error(LOG_TAG, "Could not create panel uniform buffer!");
        return false;
    }
    return updatePanelUniformBufferData();
}

bool Tutorial22::createDescriptorSetLayouts() {
    // Matches Tutorial16's own shader layout exactly (0 = sampler in the
    // fragment stage, 1 = UBO in the vertex stage).
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings_tank = {
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
    VkDescriptorSetLayoutCreateInfo layout_create_info_tank = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount =
                    static_cast<std::uint32_t>(layout_bindings_tank.size()),
            .pBindings = layout_bindings_tank.data()};
    VkDescriptorSetLayout vk_tank_layout;
    if (vkCreateDescriptorSetLayout(getVkDevice(),
                                    &layout_create_info_tank,
                                    nullptr,
                                    &vk_tank_layout) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create tank descriptor set layout!");
        return false;
    }
    m_vulkan_tutorial22_parameters.setVkTankDescriptorSetLayout(
            vk_tank_layout);

    // Matches Tutorial15's own shader layout exactly (0 = UBO in the
    // vertex stage, 1 = sampler in the fragment stage).
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings_panel = {
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
    VkDescriptorSetLayoutCreateInfo layout_create_info_panel = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount =
                    static_cast<std::uint32_t>(layout_bindings_panel.size()),
            .pBindings = layout_bindings_panel.data()};
    VkDescriptorSetLayout vk_panel_layout;
    if (vkCreateDescriptorSetLayout(getVkDevice(),
                                    &layout_create_info_panel,
                                    nullptr,
                                    &vk_panel_layout) != VK_SUCCESS) {
        Logging::error(LOG_TAG,
                       "Could not create panel descriptor set layout!");
        return false;
    }
    m_vulkan_tutorial22_parameters.setVkPanelDescriptorSetLayout(
            vk_panel_layout);

    return true;
}

bool Tutorial22::createDescriptorPool() {
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
    m_vulkan_tutorial22_parameters.setVkDescriptorPool(vk_descriptor_pool);

    return true;
}

bool Tutorial22::allocateDescriptorSets() {
    VkDescriptorSetLayout vk_tank_layout =
            m_vulkan_tutorial22_parameters.getVkTankDescriptorSetLayout();
    VkDescriptorSetAllocateInfo allocate_info_tank = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool =
                    m_vulkan_tutorial22_parameters.getVkDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_tank_layout};
    VkDescriptorSet vk_tank_set;
    if (vkAllocateDescriptorSets(
                getVkDevice(), &allocate_info_tank, &vk_tank_set) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate tank descriptor set!");
        return false;
    }
    m_vulkan_tutorial22_parameters.setTankVkDescriptorSet(vk_tank_set);

    VkDescriptorSetLayout vk_panel_layout =
            m_vulkan_tutorial22_parameters.getVkPanelDescriptorSetLayout();
    VkDescriptorSetAllocateInfo allocate_info_panel = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool =
                    m_vulkan_tutorial22_parameters.getVkDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_panel_layout};
    VkDescriptorSet vk_panel_set;
    if (vkAllocateDescriptorSets(
                getVkDevice(), &allocate_info_panel, &vk_panel_set) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate panel descriptor set!");
        return false;
    }
    m_vulkan_tutorial22_parameters.setPanelVkDescriptorSet(vk_panel_set);

    return true;
}

bool Tutorial22::updateDescriptorSets() {
    ImageParameters& tank_image =
            m_vulkan_tutorial22_parameters.getTankImageParameters();
    VkDescriptorImageInfo tank_image_info = {
            .sampler = tank_image.getVkSampler(),
            .imageView = tank_image.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
    BufferParameters& tank_uniform_buffer =
            m_vulkan_tutorial22_parameters.getTankUniformBufferParameters();
    VkDescriptorBufferInfo tank_buffer_info = {
            .buffer = tank_uniform_buffer.getVkBuffer(),
            .offset = 0,
            .range = tank_uniform_buffer.getSize()};

    BufferParameters& panel_uniform_buffer =
            m_vulkan_tutorial22_parameters.getPanelUniformBufferParameters();
    VkDescriptorBufferInfo panel_buffer_info = {
            .buffer = panel_uniform_buffer.getVkBuffer(),
            .offset = 0,
            .range = panel_uniform_buffer.getSize()};
    ImageParameters& font_image =
            m_vulkan_tutorial22_parameters.getFontImageParameters();
    VkDescriptorImageInfo panel_image_info = {
            .sampler = font_image.getVkSampler(),
            .imageView = font_image.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    std::vector<VkWriteDescriptorSet> descriptor_writes = {
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial22_parameters.getTankVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &tank_image_info,
             .pBufferInfo = nullptr,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial22_parameters.getTankVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &tank_buffer_info,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet =
                     m_vulkan_tutorial22_parameters.getPanelVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &panel_buffer_info,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet =
                     m_vulkan_tutorial22_parameters.getPanelVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &panel_image_info,
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

bool Tutorial22::createRenderPass() {
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
                &m_vulkan_tutorial22_parameters.getVkRenderPass()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

bool Tutorial22::createPipelineLayouts() {
    VkDescriptorSetLayout vk_tank_layout =
            m_vulkan_tutorial22_parameters.getVkTankDescriptorSetLayout();
    VkPushConstantRange push_constant_range = {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(Tutorial22TankPushConstants)};
    VkPipelineLayoutCreateInfo layout_create_info_tank = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &vk_tank_layout,
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &push_constant_range};
    if (vkCreatePipelineLayout(
                getVkDevice(),
                &layout_create_info_tank,
                nullptr,
                &m_vulkan_tutorial22_parameters.getVkTankPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create tank pipeline layout!");
        return false;
    }

    VkDescriptorSetLayout vk_panel_layout =
            m_vulkan_tutorial22_parameters.getVkPanelDescriptorSetLayout();
    VkPipelineLayoutCreateInfo layout_create_info_panel = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &vk_panel_layout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr};
    if (vkCreatePipelineLayout(
                getVkDevice(),
                &layout_create_info_panel,
                nullptr,
                &m_vulkan_tutorial22_parameters.getVkPanelPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create panel pipeline layout!");
        return false;
    }

    return true;
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
Tutorial22::createShaderModule(const char* filename) {
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

bool Tutorial22::createPipelines() {
    // --- Tank preview pipeline (unlit, textured, real depth test/write,
    // dynamic viewport/scissor confined to the preview sub-region at
    // draw time) ---
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module_tank =
                    createShaderModule("shader.22_3d.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module_tank =
                    createShaderModule("shader.22_3d.frag.spv");
    if (!vertex_shader_module_tank || !fragment_shader_module_tank) {
        return false;
    }

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_tank = {
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_VERTEX_BIT,
             .module = vertex_shader_module_tank.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr},
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
             .module = fragment_shader_module_tank.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr}};

    std::vector<VkVertexInputBindingDescription> vertex_bindings_tank = {
            {.binding = 0,
             .stride = Tutorial22TankVertexAttributeTraits::stride,
             .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};
    std::vector<VkVertexInputAttributeDescription> vertex_attributes_tank = {
            {.location = 0,
             .binding = 0,
             .format = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset = offsetof(struct Tutorial22TankVertexData, position)},
            {.location = 1,
             .binding = 0,
             .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(struct Tutorial22TankVertexData, texcoord)}};
    VkPipelineVertexInputStateCreateInfo vertex_input_state_tank = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount =
                    static_cast<std::uint32_t>(vertex_bindings_tank.size()),
            .pVertexBindingDescriptions = vertex_bindings_tank.data(),
            .vertexAttributeDescriptionCount =
                    static_cast<std::uint32_t>(vertex_attributes_tank.size()),
            .pVertexAttributeDescriptions = vertex_attributes_tank.data()};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_tank = {
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

    VkPipelineRasterizationStateCreateInfo rasterization_tank = {
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

    VkPipelineDepthStencilStateCreateInfo depth_stencil_tank = {
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

    VkPipelineColorBlendAttachmentState color_blend_attachment_opaque = {
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
    VkPipelineColorBlendStateCreateInfo color_blend_state_tank = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &color_blend_attachment_opaque,
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

    VkGraphicsPipelineCreateInfo pipeline_create_info_tank = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount =
                    static_cast<std::uint32_t>(shader_stages_tank.size()),
            .pStages = shader_stages_tank.data(),
            .pVertexInputState = &vertex_input_state_tank,
            .pInputAssemblyState = &input_assembly_tank,
            .pTessellationState = nullptr,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterization_tank,
            .pMultisampleState = &multisample_state,
            .pDepthStencilState = &depth_stencil_tank,
            .pColorBlendState = &color_blend_state_tank,
            .pDynamicState = &dynamic_state_create_info,
            .layout = m_vulkan_tutorial22_parameters.getVkTankPipelineLayout(),
            .renderPass = m_vulkan_tutorial22_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};
    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info_tank,
                nullptr,
                &m_vulkan_tutorial22_parameters.getVkTankGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create tank graphics pipeline!");
        return false;
    }

    // --- Panel pipeline (2D, depth disabled, alpha blend, full extent) ---
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module_panel =
                    createShaderModule("shader.22_panel.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module_panel =
                    createShaderModule("shader.22_panel.frag.spv");
    if (!vertex_shader_module_panel || !fragment_shader_module_panel) {
        return false;
    }

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_panel = {
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_VERTEX_BIT,
             .module = vertex_shader_module_panel.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr},
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
             .module = fragment_shader_module_panel.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr}};

    std::vector<VkVertexInputBindingDescription> vertex_bindings_panel = {
            {.binding = 0,
             .stride = Tutorial22PanelVertexAttributeTraits::stride,
             .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};
    std::vector<VkVertexInputAttributeDescription> vertex_attributes_panel = {
            {.location = 0,
             .binding = 0,
             .format = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset = offsetof(struct Tutorial22PanelVertexData, position)},
            {.location = 1,
             .binding = 0,
             .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(struct Tutorial22PanelVertexData, texcoord)},
            {.location = 2,
             .binding = 0,
             .format = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset = offsetof(struct Tutorial22PanelVertexData, color)}};
    VkPipelineVertexInputStateCreateInfo vertex_input_state_panel = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount =
                    static_cast<std::uint32_t>(vertex_bindings_panel.size()),
            .pVertexBindingDescriptions = vertex_bindings_panel.data(),
            .vertexAttributeDescriptionCount = static_cast<std::uint32_t>(
                    vertex_attributes_panel.size()),
            .pVertexAttributeDescriptions = vertex_attributes_panel.data()};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_panel = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE};

    VkPipelineRasterizationStateCreateInfo rasterization_panel = {
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

    VkPipelineColorBlendAttachmentState color_blend_attachment_alpha = {
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
    VkPipelineColorBlendStateCreateInfo color_blend_state_panel = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &color_blend_attachment_alpha,
            .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};

    // Depth disabled entirely: the 2D panel is drawn first, full-screen,
    // and should never be occluded by (or occlude) the 3D preview drawn
    // on top of it within its own sub-region.
    VkGraphicsPipelineCreateInfo pipeline_create_info_panel = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount =
                    static_cast<std::uint32_t>(shader_stages_panel.size()),
            .pStages = shader_stages_panel.data(),
            .pVertexInputState = &vertex_input_state_panel,
            .pInputAssemblyState = &input_assembly_panel,
            .pTessellationState = nullptr,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterization_panel,
            .pMultisampleState = &multisample_state,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &color_blend_state_panel,
            .pDynamicState = &dynamic_state_create_info,
            .layout =
                    m_vulkan_tutorial22_parameters.getVkPanelPipelineLayout(),
            .renderPass = m_vulkan_tutorial22_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};
    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info_panel,
                nullptr,
                &m_vulkan_tutorial22_parameters.getVkPanelGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create panel graphics pipeline!");
        return false;
    }

    return true;
}

std::vector<Tutorial22TankVertexData> Tutorial22::loadTankPartVertexData(
        const char* mesh_filename) const {
    std::vector<Tools::OglVertexData> const mesh_data =
            Tools::loadOglMeshData(mesh_filename);
    std::vector<Tutorial22TankVertexData> vertex_data;
    vertex_data.reserve(mesh_data.size());
    for (Tools::OglVertexData const& vertex : mesh_data) {
        vertex_data.push_back(
                {Math::Vec4<float>(vertex.position, 1.0f), vertex.texcoord});
    }
    return vertex_data;
}

bool Tutorial22::copyBufferData(BufferParameters& destination,
                                const void* data,
                                std::uint32_t data_size,
                                VkAccessFlags dst_access_mask,
                                VkPipelineStageFlags dst_stage_mask) {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial22_parameters.getStagingBufferParameters();

    if (!VulkanCommon::StagedUploader(
                getVkDevice(),
                getGraphicsQueueParameters().getVkQueue(),
                m_vulkan_tutorial22_parameters.getRenderingResources()[0]
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

bool Tutorial22::createTankPartVertexBuffer(const char* mesh_filename,
                                            BufferParameters& vertex_buffer,
                                            std::uint32_t& vertex_count) {
    std::vector<Tutorial22TankVertexData> const vertex_data =
            loadTankPartVertexData(mesh_filename);
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

    return copyBufferData(vertex_buffer,
                          vertex_data.data(),
                          vertex_buffer.getSize(),
                          VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                          VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
}

bool Tutorial22::createTankVertexBuffers() {
    std::array<BufferParameters, kTutorial22TankPartCount>& vertex_buffers =
            m_vulkan_tutorial22_parameters.getTankVertexBufferParameters();
    std::array<std::uint32_t, kTutorial22TankPartCount>& vertex_counts =
            m_vulkan_tutorial22_parameters.getTankVertexCounts();

    static const std::array<const char*, kTutorial22TankPartCount>
            mesh_files = {{"Hellfire_Body.ogl",
                           "Hellfire_Head.ogl",
                           "Hellfire_Turret.ogl"}};

    for (std::size_t i = 0; i < kTutorial22TankPartCount; ++i) {
        if (!createTankPartVertexBuffer(
                    mesh_files[i], vertex_buffers[i], vertex_counts[i])) {
            return false;
        }
    }
    return true;
}

Math::Mat4<float> Tutorial22::getTankPartModelMatrix(
        Math::Vec3<float> const& part_translation) const {
    return glm::rotate(Math::Mat4<float>(1.0f),
                       m_tank_angle,
                       Math::Vec3<float>(0.0f, 1.0f, 0.0f)) *
           buildPartMatrix(part_translation);
}

Math::Vec2<float> Tutorial22::getButtonSize() const {
    return Math::Vec2<float>(180.0f, 50.0f);
}

Math::Vec2<float> Tutorial22::getButtonTopLeft() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    Math::Vec2<float> const size = getButtonSize();
    return Math::Vec2<float>(width * 0.5f - size.x * 0.5f, height - 100.0f);
}

std::string Tutorial22::getButtonLabel() const {
    if (m_click_count == 0) {
        return "Ready";
    }
    return "Ready x" + std::to_string(m_click_count);
}

void Tutorial22::appendGlyphQuad(
        std::vector<Tutorial22PanelVertexData>& vertex_data,
        const BitmapFontGlyphQuad& glyph,
        Math::Vec4<float> color) const {
    Math::Vec2<float> const& top_left = glyph.top_left;
    Math::Vec2<float> const& bottom_right = glyph.bottom_right;
    Math::Vec2<float> const& uv_top_left = glyph.uv_top_left;
    Math::Vec2<float> const& uv_bottom_right = glyph.uv_bottom_right;

    Tutorial22PanelVertexData const top_left_vertex{
            Math::Vec4<float>(top_left.x, top_left.y, 0.0f, 1.0f),
            uv_top_left,
            color};
    Tutorial22PanelVertexData const bottom_left_vertex{
            Math::Vec4<float>(top_left.x, bottom_right.y, 0.0f, 1.0f),
            Math::Vec2<float>(uv_top_left.x, uv_bottom_right.y),
            color};
    Tutorial22PanelVertexData const bottom_right_vertex{
            Math::Vec4<float>(bottom_right.x, bottom_right.y, 0.0f, 1.0f),
            uv_bottom_right,
            color};
    Tutorial22PanelVertexData const top_right_vertex{
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

void Tutorial22::appendColoredQuad(
        std::vector<Tutorial22PanelVertexData>& vertex_data,
        const std::array<Math::Vec2<float>, 4>& corners,
        Math::Vec4<float> color) const {
    Math::Vec2<float> const solid_uv = m_font.solidTexelUv();
    std::array<Tutorial22PanelVertexData, 4> quad_vertices;
    for (std::size_t i = 0; i < corners.size(); ++i) {
        quad_vertices[i] = Tutorial22PanelVertexData{
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

void Tutorial22::appendText(std::vector<Tutorial22PanelVertexData>& vertex_data,
                            const std::string& text,
                            Math::Vec2<float> origin,
                            Math::Vec4<float> color) const {
    std::vector<BitmapFontGlyphQuad> const glyphs =
            m_font.layoutText(text, origin);
    for (BitmapFontGlyphQuad const& glyph : glyphs) {
        appendGlyphQuad(vertex_data, glyph, color);
    }
}

std::vector<Tutorial22PanelVertexData> Tutorial22::buildPanelVertexData()
        const {
    std::vector<Tutorial22PanelVertexData> vertex_data;
    vertex_data.reserve(kMaxPanelVertexCount);

    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    Math::Vec4<float> const text_color(0.05f, 0.05f, 0.05f, 1.0f);

    // Main menu panel background - the identical 5-quad bevel every real
    // vulkan_earth menu screen (MainMenu/ReadyMenu/ShopMenu/SubMenu*)
    // opens with, confirmed via research (see this header's own top
    // comment).
    Math::Vec2<float> const panel_top_left(width * 0.5f - 200.0f,
                                           height * 0.5f - 240.0f);
    Math::Vec2<float> const panel_size(400.0f, 480.0f);
    Math::Vec4<float> const panel_color(0.35f, 0.38f, 0.45f, 1.0f);
    for (UiGeometry::ColoredQuad const& quad : UiGeometry::buildButtonBevel(
                 panel_top_left, panel_size, panel_color, false)) {
        appendColoredQuad(vertex_data, quad.corners, quad.color);
    }

    std::string const title = "Ready?";
    float const title_width = m_font.textWidth(title);
    Math::Vec2<float> const title_origin(
            width * 0.5f - title_width * 0.5f, panel_top_left.y + 40.0f);
    appendText(vertex_data, title, title_origin, text_color);

    // A sunken "well" frame around the live 3D preview - matches
    // ReadyMenu's own tank preview being composited inside its menu
    // panel, not floating over it unframed.
    Math::Vec2<float> const preview_top_left = getPreviewTopLeft();
    Math::Vec2<float> const preview_size = getPreviewSize();
    constexpr float c_frame_margin = 10.0f;
    Math::Vec2<float> const frame_top_left(
            preview_top_left.x - c_frame_margin,
            preview_top_left.y - c_frame_margin);
    Math::Vec2<float> const frame_size(preview_size.x + 2.0f * c_frame_margin,
                                       preview_size.y + 2.0f * c_frame_margin);
    Math::Vec4<float> const frame_color(0.15f, 0.16f, 0.2f, 1.0f);
    for (UiGeometry::ColoredQuad const& quad : UiGeometry::buildButtonBevel(
                 frame_top_left, frame_size, frame_color, true)) {
        appendColoredQuad(vertex_data, quad.corners, quad.color);
    }

    Math::Vec2<float> const button_top_left = getButtonTopLeft();
    Math::Vec2<float> const button_size = getButtonSize();
    Math::Vec4<float> const button_color(0.3f, 0.5f, 0.75f, 1.0f);
    for (UiGeometry::ColoredQuad const& quad : UiGeometry::buildButtonBevel(
                 button_top_left, button_size, button_color,
                 m_button_pressed)) {
        appendColoredQuad(vertex_data, quad.corners, quad.color);
    }

    std::string const label = getButtonLabel();
    float const label_width = m_font.textWidth(label);
    Math::Vec2<float> const label_origin(
            button_top_left.x + button_size.x * 0.5f - label_width * 0.5f,
            button_top_left.y + button_size.y * 0.5f +
                    kFontPixelHeight * 0.3f);
    appendText(vertex_data, label, label_origin, text_color);

    return vertex_data;
}

bool Tutorial22::updatePanelVertexBufferData() {
    std::vector<Tutorial22PanelVertexData> const vertex_data =
            buildPanelVertexData();
    if (vertex_data.size() > kMaxPanelVertexCount) {
        Logging::error(LOG_TAG,
                       "Panel vertex data (",
                       vertex_data.size(),
                       " vertices) exceeds kMaxPanelVertexCount (",
                       kMaxPanelVertexCount,
                       ")!");
        return false;
    }
    m_vulkan_tutorial22_parameters.setPanelVertexCount(
            static_cast<std::uint32_t>(vertex_data.size()));

    BufferParameters& vertex_buffer =
            m_vulkan_tutorial22_parameters.getPanelVertexBufferParameters();
    void* vertex_buffer_memory_pointer;
    if (vkMapMemory(getVkDevice(),
                    vertex_buffer.getVkDeviceMemory(),
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &vertex_buffer_memory_pointer) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not map vertex buffer memory!");
        return false;
    }
    if (!vertex_data.empty()) {
        std::memcpy(vertex_buffer_memory_pointer,
                    vertex_data.data(),
                    vertex_data.size() * sizeof(Tutorial22PanelVertexData));
    }
    vkUnmapMemory(getVkDevice(), vertex_buffer.getVkDeviceMemory());

    return true;
}

bool Tutorial22::createPanelVertexBuffer() {
    BufferParameters& vertex_buffer =
            m_vulkan_tutorial22_parameters.getPanelVertexBufferParameters();
    vertex_buffer.setSize(static_cast<std::uint32_t>(
            kMaxPanelVertexCount * sizeof(Tutorial22PanelVertexData)));
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      vertex_buffer)) {
        Logging::error(LOG_TAG, "Could not create panel vertex buffer!");
        return false;
    }

    return updatePanelVertexBufferData();
}

bool Tutorial22::createFramebuffer(VkFramebuffer& framebuffer,
                                   VkImageView image_view) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(getVkDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    std::array<VkImageView, 2> const attachments = {
            image_view,
            m_vulkan_tutorial22_parameters.getDepthImageParameters()
                    .getVkImageView()};

    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_vulkan_tutorial22_parameters.getVkRenderPass(),
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

bool Tutorial22::prepareFrame(VkCommandBuffer command_buffer,
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
    clear_values[0].color = {.float32 = {0.78f, 0.8f, 0.83f, 1.0f}};
    clear_values[1].depthStencil = {.depth = 1.0f, .stencil = 0};

    VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = m_vulkan_tutorial22_parameters.getVkRenderPass(),
            .framebuffer = framebuffer,
            .renderArea = {.offset = {.x = 0, .y = 0},
                           .extent = getSwapchainParameters().getVkExtent2d()},
            .clearValueCount = static_cast<std::uint32_t>(clear_values.size()),
            .pClearValues = clear_values.data()};

    vkCmdBeginRenderPass(command_buffer,
                         &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    VkDeviceSize const zero_offset = 0;

    // --- Pass 1: 2D panel, full swapchain extent, depth disabled ---
    vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial22_parameters.getVkPanelGraphicsPipeline());

    VkViewport full_viewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(
                    getSwapchainParameters().getVkExtent2d().width),
            .height = static_cast<float>(
                    getSwapchainParameters().getVkExtent2d().height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f};
    VkRect2D full_scissor = {
            .offset = {.x = 0, .y = 0},
            .extent = getSwapchainParameters().getVkExtent2d()};
    vkCmdSetViewport(command_buffer, 0, 1, &full_viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &full_scissor);

    VkDescriptorSet vk_panel_descriptor_set =
            m_vulkan_tutorial22_parameters.getPanelVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial22_parameters.getVkPanelPipelineLayout(),
            0,
            1,
            &vk_panel_descriptor_set,
            0,
            nullptr);
    vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_vulkan_tutorial22_parameters.getPanelVertexBufferParameters()
                     .getVkBuffer(),
            &zero_offset);
    vkCmdDraw(command_buffer,
             m_vulkan_tutorial22_parameters.getPanelVertexCount(),
             1,
             0,
             0);

    // --- Pass 2: 3D tank preview, scissored to its own sub-region,
    // drawn on top of the panel so it appears composited inside it ---
    vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial22_parameters.getVkTankGraphicsPipeline());

    Math::Vec2<float> const preview_top_left = getPreviewTopLeft();
    Math::Vec2<float> const preview_size = getPreviewSize();
    VkViewport preview_viewport = {.x = preview_top_left.x,
                                   .y = preview_top_left.y,
                                   .width = preview_size.x,
                                   .height = preview_size.y,
                                   .minDepth = 0.0f,
                                   .maxDepth = 1.0f};
    VkRect2D preview_scissor = {
            .offset = {.x = static_cast<std::int32_t>(preview_top_left.x),
                      .y = static_cast<std::int32_t>(preview_top_left.y)},
            .extent = {.width = static_cast<std::uint32_t>(preview_size.x),
                      .height = static_cast<std::uint32_t>(preview_size.y)}};
    vkCmdSetViewport(command_buffer, 0, 1, &preview_viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &preview_scissor);

    VkDescriptorSet vk_tank_descriptor_set =
            m_vulkan_tutorial22_parameters.getTankVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial22_parameters.getVkTankPipelineLayout(),
            0,
            1,
            &vk_tank_descriptor_set,
            0,
            nullptr);

    std::array<Math::Vec3<float>, kTutorial22TankPartCount> const
            part_translations = {{getBodyTranslation(), getHeadTranslation(),
                                  getTurretTranslation()}};
    std::array<BufferParameters, kTutorial22TankPartCount>& tank_vertex_buffers =
            m_vulkan_tutorial22_parameters.getTankVertexBufferParameters();
    std::array<std::uint32_t, kTutorial22TankPartCount>& tank_vertex_counts =
            m_vulkan_tutorial22_parameters.getTankVertexCounts();
    for (std::size_t i = 0; i < kTutorial22TankPartCount; ++i) {
        vkCmdBindVertexBuffers(command_buffer,
                               0,
                               1,
                               &tank_vertex_buffers[i].getVkBuffer(),
                               &zero_offset);
        Tutorial22TankPushConstants tank_push_constants{
                getTankPartModelMatrix(part_translations[i])};
        vkCmdPushConstants(
                command_buffer,
                m_vulkan_tutorial22_parameters.getVkTankPipelineLayout(),
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof(Tutorial22TankPushConstants),
                &tank_push_constants);
        vkCmdDraw(command_buffer, tank_vertex_counts[i], 1, 0, 0);
    }

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

bool Tutorial22::draw() {
    static std::size_t resource_index = 0;
    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial22_parameters.getRenderingResources();
    RenderingResourceParameters& current_rendering_resource =
            rendering_resources[resource_index];
    VkSwapchainKHR swap_chain = getSwapchainParameters().getVkSwapchainKhr();
    std::uint32_t image_index;

    resource_index = (resource_index + 1) %
                     VulkanTutorial22Parameters::resources_count;

    if (vkWaitForFences(getVkDevice(),
                        1,
                        &current_rendering_resource.getVkFence(),
                        VK_FALSE,
                        1000000000) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Waiting for fence takes too long!");
        return false;
    }
    vkResetFences(getVkDevice(), 1, &current_rendering_resource.getVkFence());

    // ReadyMenu's own tank_angle += 0.25f per frame - this preview's
    // continuous spin, independent of mouse/camera input.
    m_tank_angle += kTankSpinStepRadians;

    vkDeviceWaitIdle(getVkDevice());
    if (!updatePanelVertexBufferData()) {
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
            m_vulkan_tutorial22_parameters.getFinishedRenderingSemaphores()
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

void Tutorial22::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

bool Tutorial22::childOnWindowSizeChanged() {
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
    return createPanelVertexBuffer();
}

void Tutorial22::childClear() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return;
    }
    vkDeviceWaitIdle(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial22_parameters.getRenderingResources();
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
                    m_vulkan_tutorial22_parameters.getVkCommandPool(),
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
            m_vulkan_tutorial22_parameters.getFinishedRenderingSemaphores();
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (finished_rendering_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(), finished_rendering_semaphores[i], nullptr);
        }
    }
    finished_rendering_semaphores.clear();

    if (m_vulkan_tutorial22_parameters.getVkCommandPool() != VK_NULL_HANDLE) {
        vkDestroyCommandPool(
                getVkDevice(),
                m_vulkan_tutorial22_parameters.getVkCommandPool(),
                nullptr);
        m_vulkan_tutorial22_parameters.setVkCommandPool(VK_NULL_HANDLE);
    }

    std::array<BufferParameters, kTutorial22TankPartCount>& tank_vertex_buffers =
            m_vulkan_tutorial22_parameters.getTankVertexBufferParameters();
    for (BufferParameters& buffer : tank_vertex_buffers) {
        destroyBuffer(buffer);
    }
    destroyBuffer(m_vulkan_tutorial22_parameters.getPanelVertexBufferParameters());
    destroyBuffer(m_vulkan_tutorial22_parameters.getStagingBufferParameters());

    if (m_vulkan_tutorial22_parameters.getVkTankGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial22_parameters.getVkTankGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial22_parameters.setVkTankGraphicsPipeline(
                VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial22_parameters.getVkPanelGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial22_parameters.getVkPanelGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial22_parameters.setVkPanelGraphicsPipeline(
                VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial22_parameters.getVkTankPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial22_parameters.getVkTankPipelineLayout(),
                nullptr);
        m_vulkan_tutorial22_parameters.setVkTankPipelineLayout(VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial22_parameters.getVkPanelPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial22_parameters.getVkPanelPipelineLayout(),
                nullptr);
        m_vulkan_tutorial22_parameters.setVkPanelPipelineLayout(
                VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial22_parameters.getVkRenderPass() != VK_NULL_HANDLE) {
        vkDestroyRenderPass(
                getVkDevice(),
                m_vulkan_tutorial22_parameters.getVkRenderPass(),
                nullptr);
        m_vulkan_tutorial22_parameters.setVkRenderPass(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial22_parameters.getVkDescriptorPool() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(
                getVkDevice(),
                m_vulkan_tutorial22_parameters.getVkDescriptorPool(),
                nullptr);
        m_vulkan_tutorial22_parameters.setVkDescriptorPool(VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial22_parameters.getVkTankDescriptorSetLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(
                getVkDevice(),
                m_vulkan_tutorial22_parameters.getVkTankDescriptorSetLayout(),
                nullptr);
        m_vulkan_tutorial22_parameters.setVkTankDescriptorSetLayout(
                VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial22_parameters.getVkPanelDescriptorSetLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(
                getVkDevice(),
                m_vulkan_tutorial22_parameters.getVkPanelDescriptorSetLayout(),
                nullptr);
        m_vulkan_tutorial22_parameters.setVkPanelDescriptorSetLayout(
                VK_NULL_HANDLE);
    }

    destroyBuffer(m_vulkan_tutorial22_parameters.getTankUniformBufferParameters());
    destroyBuffer(
            m_vulkan_tutorial22_parameters.getPanelUniformBufferParameters());

    VulkanCommon::ImageFactory image_factory(getVkDevice(),
                                             getVkPhysicalDevice());
    image_factory.destroy(
            m_vulkan_tutorial22_parameters.getTankImageParameters());
    image_factory.destroy(
            m_vulkan_tutorial22_parameters.getFontImageParameters());

    destroyDepthResources();
}

}  // namespace vulkan_graphix
