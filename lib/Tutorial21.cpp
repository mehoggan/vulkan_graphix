#include "vulkan_graphix/Tutorial21.h"

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <cstring>

#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
constexpr VkFormat c_texture_format = VK_FORMAT_R8G8B8A8_UNORM;
constexpr VkFormat c_depth_format = VK_FORMAT_D32_SFLOAT;

// TankB's own constructor values (vulkan_earth/src/TankB.cpp) - see
// Tutorial16.h's top comment for why these are taken verbatim.
constexpr float c_part_scale = 50.0f;
Math::Vec3<float> const c_body_offset(0.0f, 65.0f, 0.0f);
Math::Vec3<float> const c_head_offset(0.0f, 70.0f, 0.0f);
Math::Vec3<float> const c_turret_offset(0.0f, 0.0001f, -50.0001f);

// Tank::initBody()/initHead()/initTurret() (vulkan_earth/src/Tank.cpp)
// all set this identical right/up/at basis.
Math::Mat4<float> const& getPartBasis() {
    static Math::Mat4<float> const basis(
            Math::Vec4<float>(0.0f, 0.0f, 1.0f, 0.0f),
            Math::Vec4<float>(0.0f, 1.0f, 0.0f, 0.0f),
            Math::Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f),
            Math::Vec4<float>(0.0f, 0.0f, 0.0f, 1.0f));
    return basis;
}

// Mirrors Tank::setTankPos()'s own construction exactly (see the
// Tutorial16 fix and Tutorial18's own use of this same helper).
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
}  // namespace

// ************************************************************ //
// VulkanTutorial21Parameters                                   //
// ************************************************************ //
VulkanTutorial21Parameters::VulkanTutorial21Parameters()
        : m_vk_render_pass(VK_NULL_HANDLE)
        , m_vk_terrain_descriptor_set_layout(VK_NULL_HANDLE)
        , m_vk_object_descriptor_set_layout(VK_NULL_HANDLE)
        , m_vk_descriptor_pool(VK_NULL_HANDLE)
        , m_vk_terrain_descriptor_set(VK_NULL_HANDLE)
        , m_vk_tank_descriptor_set(VK_NULL_HANDLE)
        , m_vk_skybox_descriptor_set(VK_NULL_HANDLE)
        , m_vk_terrain_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_object_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_terrain_graphics_pipeline(VK_NULL_HANDLE)
        , m_vk_object_graphics_pipeline(VK_NULL_HANDLE)
        , m_vk_skybox_graphics_pipeline(VK_NULL_HANDLE)
        , m_terrain_vertex_count(0)
        , m_tank_vertex_counts{0, 0, 0}
        , m_skybox_index_count(0)
        , m_vk_command_pool(VK_NULL_HANDLE)
        , m_rendering_resources(resources_count) {}

const VkRenderPass& VulkanTutorial21Parameters::getVkRenderPass() const {
    return m_vk_render_pass;
}
VkRenderPass& VulkanTutorial21Parameters::getVkRenderPass() {
    return m_vk_render_pass;
}
void VulkanTutorial21Parameters::setVkRenderPass(
        const VkRenderPass& vk_render_pass) {
    m_vk_render_pass = vk_render_pass;
}

const ImageParameters& VulkanTutorial21Parameters::getDepthImageParameters()
        const {
    return m_depth_image_parameters;
}
ImageParameters& VulkanTutorial21Parameters::getDepthImageParameters() {
    return m_depth_image_parameters;
}
void VulkanTutorial21Parameters::setDepthImageParameters(
        const ImageParameters& depth_image) {
    m_depth_image_parameters = depth_image;
}

const ImageParameters& VulkanTutorial21Parameters::getTerrainImageParameters()
        const {
    return m_terrain_image_parameters;
}
ImageParameters& VulkanTutorial21Parameters::getTerrainImageParameters() {
    return m_terrain_image_parameters;
}
void VulkanTutorial21Parameters::setTerrainImageParameters(
        const ImageParameters& image_parameters) {
    m_terrain_image_parameters = image_parameters;
}

const ImageParameters& VulkanTutorial21Parameters::getTankImageParameters()
        const {
    return m_tank_image_parameters;
}
ImageParameters& VulkanTutorial21Parameters::getTankImageParameters() {
    return m_tank_image_parameters;
}
void VulkanTutorial21Parameters::setTankImageParameters(
        const ImageParameters& image_parameters) {
    m_tank_image_parameters = image_parameters;
}

const ImageParameters& VulkanTutorial21Parameters::getSkyboxImageParameters()
        const {
    return m_skybox_image_parameters;
}
ImageParameters& VulkanTutorial21Parameters::getSkyboxImageParameters() {
    return m_skybox_image_parameters;
}
void VulkanTutorial21Parameters::setSkyboxImageParameters(
        const ImageParameters& image_parameters) {
    m_skybox_image_parameters = image_parameters;
}

const BufferParameters&
VulkanTutorial21Parameters::getTerrainUniformBufferParameters() const {
    return m_terrain_uniform_buffer;
}
BufferParameters&
VulkanTutorial21Parameters::getTerrainUniformBufferParameters() {
    return m_terrain_uniform_buffer;
}
void VulkanTutorial21Parameters::setTerrainUniformBufferParameters(
        const BufferParameters& uniform_buffer) {
    m_terrain_uniform_buffer = uniform_buffer;
}

const BufferParameters&
VulkanTutorial21Parameters::getObjectUniformBufferParameters() const {
    return m_object_uniform_buffer;
}
BufferParameters&
VulkanTutorial21Parameters::getObjectUniformBufferParameters() {
    return m_object_uniform_buffer;
}
void VulkanTutorial21Parameters::setObjectUniformBufferParameters(
        const BufferParameters& uniform_buffer) {
    m_object_uniform_buffer = uniform_buffer;
}

const VkDescriptorSetLayout&
VulkanTutorial21Parameters::getVkTerrainDescriptorSetLayout() const {
    return m_vk_terrain_descriptor_set_layout;
}
VkDescriptorSetLayout&
VulkanTutorial21Parameters::getVkTerrainDescriptorSetLayout() {
    return m_vk_terrain_descriptor_set_layout;
}
void VulkanTutorial21Parameters::setVkTerrainDescriptorSetLayout(
        const VkDescriptorSetLayout& other) {
    m_vk_terrain_descriptor_set_layout = other;
}

const VkDescriptorSetLayout&
VulkanTutorial21Parameters::getVkObjectDescriptorSetLayout() const {
    return m_vk_object_descriptor_set_layout;
}
VkDescriptorSetLayout&
VulkanTutorial21Parameters::getVkObjectDescriptorSetLayout() {
    return m_vk_object_descriptor_set_layout;
}
void VulkanTutorial21Parameters::setVkObjectDescriptorSetLayout(
        const VkDescriptorSetLayout& other) {
    m_vk_object_descriptor_set_layout = other;
}

const VkDescriptorPool& VulkanTutorial21Parameters::getVkDescriptorPool()
        const {
    return m_vk_descriptor_pool;
}
VkDescriptorPool& VulkanTutorial21Parameters::getVkDescriptorPool() {
    return m_vk_descriptor_pool;
}
void VulkanTutorial21Parameters::setVkDescriptorPool(
        const VkDescriptorPool& other) {
    m_vk_descriptor_pool = other;
}

const VkDescriptorSet& VulkanTutorial21Parameters::getTerrainVkDescriptorSet()
        const {
    return m_vk_terrain_descriptor_set;
}
VkDescriptorSet& VulkanTutorial21Parameters::getTerrainVkDescriptorSet() {
    return m_vk_terrain_descriptor_set;
}
void VulkanTutorial21Parameters::setTerrainVkDescriptorSet(
        const VkDescriptorSet& other) {
    m_vk_terrain_descriptor_set = other;
}

const VkDescriptorSet& VulkanTutorial21Parameters::getTankVkDescriptorSet()
        const {
    return m_vk_tank_descriptor_set;
}
VkDescriptorSet& VulkanTutorial21Parameters::getTankVkDescriptorSet() {
    return m_vk_tank_descriptor_set;
}
void VulkanTutorial21Parameters::setTankVkDescriptorSet(
        const VkDescriptorSet& other) {
    m_vk_tank_descriptor_set = other;
}

const VkDescriptorSet& VulkanTutorial21Parameters::getSkyboxVkDescriptorSet()
        const {
    return m_vk_skybox_descriptor_set;
}
VkDescriptorSet& VulkanTutorial21Parameters::getSkyboxVkDescriptorSet() {
    return m_vk_skybox_descriptor_set;
}
void VulkanTutorial21Parameters::setSkyboxVkDescriptorSet(
        const VkDescriptorSet& other) {
    m_vk_skybox_descriptor_set = other;
}

const VkPipelineLayout&
VulkanTutorial21Parameters::getVkTerrainPipelineLayout() const {
    return m_vk_terrain_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial21Parameters::getVkTerrainPipelineLayout() {
    return m_vk_terrain_pipeline_layout;
}
void VulkanTutorial21Parameters::setVkTerrainPipelineLayout(
        const VkPipelineLayout& other) {
    m_vk_terrain_pipeline_layout = other;
}

const VkPipelineLayout& VulkanTutorial21Parameters::getVkObjectPipelineLayout()
        const {
    return m_vk_object_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial21Parameters::getVkObjectPipelineLayout() {
    return m_vk_object_pipeline_layout;
}
void VulkanTutorial21Parameters::setVkObjectPipelineLayout(
        const VkPipelineLayout& other) {
    m_vk_object_pipeline_layout = other;
}

const VkPipeline& VulkanTutorial21Parameters::getVkTerrainGraphicsPipeline()
        const {
    return m_vk_terrain_graphics_pipeline;
}
VkPipeline& VulkanTutorial21Parameters::getVkTerrainGraphicsPipeline() {
    return m_vk_terrain_graphics_pipeline;
}
void VulkanTutorial21Parameters::setVkTerrainGraphicsPipeline(
        const VkPipeline& other) {
    m_vk_terrain_graphics_pipeline = other;
}

const VkPipeline& VulkanTutorial21Parameters::getVkObjectGraphicsPipeline()
        const {
    return m_vk_object_graphics_pipeline;
}
VkPipeline& VulkanTutorial21Parameters::getVkObjectGraphicsPipeline() {
    return m_vk_object_graphics_pipeline;
}
void VulkanTutorial21Parameters::setVkObjectGraphicsPipeline(
        const VkPipeline& other) {
    m_vk_object_graphics_pipeline = other;
}

const VkPipeline& VulkanTutorial21Parameters::getVkSkyboxGraphicsPipeline()
        const {
    return m_vk_skybox_graphics_pipeline;
}
VkPipeline& VulkanTutorial21Parameters::getVkSkyboxGraphicsPipeline() {
    return m_vk_skybox_graphics_pipeline;
}
void VulkanTutorial21Parameters::setVkSkyboxGraphicsPipeline(
        const VkPipeline& other) {
    m_vk_skybox_graphics_pipeline = other;
}

const BufferParameters&
VulkanTutorial21Parameters::getTerrainVertexBufferParameters() const {
    return m_terrain_vertex_buffer;
}
BufferParameters&
VulkanTutorial21Parameters::getTerrainVertexBufferParameters() {
    return m_terrain_vertex_buffer;
}
void VulkanTutorial21Parameters::setTerrainVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_terrain_vertex_buffer = vertex_buffer;
}
std::uint32_t VulkanTutorial21Parameters::getTerrainVertexCount() const {
    return m_terrain_vertex_count;
}
void VulkanTutorial21Parameters::setTerrainVertexCount(
        std::uint32_t vertex_count) {
    m_terrain_vertex_count = vertex_count;
}

const std::array<BufferParameters, kTankPartCount>&
VulkanTutorial21Parameters::getTankVertexBufferParameters() const {
    return m_tank_vertex_buffers;
}
std::array<BufferParameters, kTankPartCount>&
VulkanTutorial21Parameters::getTankVertexBufferParameters() {
    return m_tank_vertex_buffers;
}
const std::array<std::uint32_t, kTankPartCount>&
VulkanTutorial21Parameters::getTankVertexCounts() const {
    return m_tank_vertex_counts;
}
std::array<std::uint32_t, kTankPartCount>&
VulkanTutorial21Parameters::getTankVertexCounts() {
    return m_tank_vertex_counts;
}

const BufferParameters&
VulkanTutorial21Parameters::getSkyboxVertexBufferParameters() const {
    return m_skybox_vertex_buffer;
}
BufferParameters&
VulkanTutorial21Parameters::getSkyboxVertexBufferParameters() {
    return m_skybox_vertex_buffer;
}
void VulkanTutorial21Parameters::setSkyboxVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_skybox_vertex_buffer = vertex_buffer;
}
const BufferParameters&
VulkanTutorial21Parameters::getSkyboxIndexBufferParameters() const {
    return m_skybox_index_buffer;
}
BufferParameters&
VulkanTutorial21Parameters::getSkyboxIndexBufferParameters() {
    return m_skybox_index_buffer;
}
void VulkanTutorial21Parameters::setSkyboxIndexBufferParameters(
        const BufferParameters& index_buffer) {
    m_skybox_index_buffer = index_buffer;
}
std::uint32_t VulkanTutorial21Parameters::getSkyboxIndexCount() const {
    return m_skybox_index_count;
}
void VulkanTutorial21Parameters::setSkyboxIndexCount(
        std::uint32_t index_count) {
    m_skybox_index_count = index_count;
}

const BufferParameters&
VulkanTutorial21Parameters::getStagingBufferParameters() const {
    return m_staging_buffer;
}
BufferParameters& VulkanTutorial21Parameters::getStagingBufferParameters() {
    return m_staging_buffer;
}
void VulkanTutorial21Parameters::setStagingBufferParameters(
        const BufferParameters& staging_buffer) {
    m_staging_buffer = staging_buffer;
}

const VkCommandPool& VulkanTutorial21Parameters::getVkCommandPool() const {
    return m_vk_command_pool;
}
VkCommandPool& VulkanTutorial21Parameters::getVkCommandPool() {
    return m_vk_command_pool;
}
void VulkanTutorial21Parameters::setVkCommandPool(
        const VkCommandPool& vk_command_pool) {
    m_vk_command_pool = vk_command_pool;
}

const std::vector<RenderingResourceParameters>&
VulkanTutorial21Parameters::getRenderingResources() const {
    return m_rendering_resources;
}
std::vector<RenderingResourceParameters>&
VulkanTutorial21Parameters::getRenderingResources() {
    return m_rendering_resources;
}
void VulkanTutorial21Parameters::setRenderingResources(
        const std::vector<RenderingResourceParameters>& rendering_resources) {
    m_rendering_resources = rendering_resources;
}

const std::vector<VkSemaphore>&
VulkanTutorial21Parameters::getFinishedRenderingSemaphores() const {
    return m_finished_rendering_semaphores;
}
std::vector<VkSemaphore>&
VulkanTutorial21Parameters::getFinishedRenderingSemaphores() {
    return m_finished_rendering_semaphores;
}
void VulkanTutorial21Parameters::setFinishedRenderingSemaphores(
        const std::vector<VkSemaphore>& finished_rendering_semaphores) {
    m_finished_rendering_semaphores = finished_rendering_semaphores;
}

// ************************************************************ //
// Tutorial21                                                   //
// ************************************************************ //
Tutorial21::Tutorial21()
        // Terrain half-extent = (64-1)*16/2 = ~504 world units; the tank
        // (hundreds of units tall/wide) plus the terrain together need a
        // much larger view than any single previous tutorial's world -
        // tuned via screenshot. A steep pitch (looking down onto the
        // terrain rather than across it) is deliberate: TerrainGenerator::
        // terrainSmoothe() zeroes the outermost ring of heights every pass
        // (a real, faithfully-ported part of the original algorithm), which
        // creates a real one-cell-wide cliff at the grid's edge - a shallow
        // pitch grazes under that cliff and exposes its unlit backside.
        : m_camera(0.5f, 0.85f, 1800.0f)
        , m_terrain_generator(kGridSize, kGridScale)
        , m_terrain_generated(false) {}

Tutorial21::~Tutorial21() { childClear(); }

void Tutorial21::onMouseButton(int button,
                               bool pressed,
                               int pos_x,
                               int pos_y) {
    m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial21::onMouseMove(int pos_x, int pos_y) {
    m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial21::createCommandPool(std::uint32_t queue_family_index,
                                   VkCommandPool* pool) {
    VkCommandPoolCreateInfo cmd_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
                     VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = queue_family_index};

    if (vkCreateCommandPool(
                getVkDevice(), &cmd_pool_create_info, nullptr, pool) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create command pool!");
        return false;
    }
    return true;
}

bool Tutorial21::allocateCommandBuffers(VkCommandPool pool,
                                        std::uint32_t count,
                                        VkCommandBuffer* command_buffers) {
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = count};

    if (vkAllocateCommandBuffers(getVkDevice(),
                                 &command_buffer_allocate_info,
                                 command_buffers) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate command buffer!");
        return false;
    }
    return true;
}

bool Tutorial21::createCommandBuffers() {
    if (!createCommandPool(
                getGraphicsQueueParameters().getFamilyIndex(),
                &m_vulkan_tutorial21_parameters.getVkCommandPool())) {
        return false;
    }

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial21_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!allocateCommandBuffers(
                    m_vulkan_tutorial21_parameters.getVkCommandPool(),
                    1,
                    &rendering_resources[i].getVkCommandBuffer())) {
            return false;
        }
    }
    return true;
}

bool Tutorial21::createSemaphores() {
    VkSemaphoreCreateInfo semaphore_create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0};

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial21_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (vkCreateSemaphore(
                    getVkDevice(),
                    &semaphore_create_info,
                    nullptr,
                    &rendering_resources[i].getImageAvailableVkSemaphore()) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    std::vector<VkSemaphore>& finished_rendering_semaphores =
            m_vulkan_tutorial21_parameters.getFinishedRenderingSemaphores();
    finished_rendering_semaphores.assign(
            getSwapchainParameters().getImageParameters().size(),
            VK_NULL_HANDLE);
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (vkCreateSemaphore(getVkDevice(),
                              &semaphore_create_info,
                              nullptr,
                              &finished_rendering_semaphores[i]) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    return true;
}

bool Tutorial21::createFences() {
    VkFenceCreateInfo fence_create_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial21_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (vkCreateFence(getVkDevice(),
                          &fence_create_info,
                          nullptr,
                          &rendering_resources[i].getVkFence()) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not create a fence!");
            return false;
        }
    }
    return true;
}

bool Tutorial21::createRenderingResources() {
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

bool Tutorial21::allocateBufferMemory(VkBuffer buffer,
                                      VkMemoryPropertyFlags property,
                                      VkDeviceMemory* memory) {
    VkMemoryRequirements buffer_memory_requirements;
    vkGetBufferMemoryRequirements(
            getVkDevice(), buffer, &buffer_memory_requirements);

    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(getVkPhysicalDevice(),
                                        &memory_properties);

    for (std::uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((buffer_memory_requirements.memoryTypeBits & (1 << i)) &&
            ((memory_properties.memoryTypes[i].propertyFlags & property) ==
             property)) {
            VkMemoryAllocateInfo memory_allocate_info = {
                    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                    .pNext = nullptr,
                    .allocationSize = buffer_memory_requirements.size,
                    .memoryTypeIndex = i};

            if (vkAllocateMemory(getVkDevice(),
                                 &memory_allocate_info,
                                 nullptr,
                                 memory) == VK_SUCCESS) {
                return true;
            }
        }
    }
    return false;
}

bool Tutorial21::createBuffer(VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags memory_property,
                              BufferParameters& buffer) {
    VkBufferCreateInfo buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = buffer.getSize(),
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr};

    VkBuffer vk_buffer;
    if (vkCreateBuffer(
                getVkDevice(), &buffer_create_info, nullptr, &vk_buffer) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create buffer!");
        return false;
    }
    buffer.setVkBuffer(vk_buffer);

    VkDeviceMemory vk_device_memory;
    if (!allocateBufferMemory(
                buffer.getVkBuffer(), memory_property, &vk_device_memory)) {
        Logging::error(LOG_TAG, "Could not allocate memory for a buffer!");
        return false;
    }
    buffer.setVkDeviceMemory(vk_device_memory);

    if (vkBindBufferMemory(getVkDevice(),
                           buffer.getVkBuffer(),
                           buffer.getVkDeviceMemory(),
                           0) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not bind memory to a buffer!");
        return false;
    }

    return true;
}

bool Tutorial21::createStagingBuffer() {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial21_parameters.getStagingBufferParameters();
    // Reused sequentially for every texture/vertex/index upload - the
    // largest is Rocky.raw (2048x2048 RGBA = 16,777,216 bytes once
    // expanded by getRawImageData()), sized with headroom above that.
    staging_buffer.setSize(20u * 1024u * 1024u);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

bool Tutorial21::createImage(std::uint32_t width,
                             std::uint32_t height,
                             VkFormat format,
                             VkImageUsageFlags usage,
                             VkImage* image) {
    VkImageCreateInfo image_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent = {.width = width, .height = height, .depth = 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

    return vkCreateImage(getVkDevice(), &image_create_info, nullptr, image) ==
           VK_SUCCESS;
}

bool Tutorial21::allocateImageMemory(VkImage image,
                                     VkMemoryPropertyFlags property,
                                     VkDeviceMemory* memory) {
    VkMemoryRequirements image_memory_requirements;
    vkGetImageMemoryRequirements(
            getVkDevice(), image, &image_memory_requirements);

    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(getVkPhysicalDevice(),
                                        &memory_properties);

    for (std::uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((image_memory_requirements.memoryTypeBits & (1 << i)) &&
            ((memory_properties.memoryTypes[i].propertyFlags & property) ==
             property)) {
            VkMemoryAllocateInfo memory_allocate_info = {
                    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                    .pNext = nullptr,
                    .allocationSize = image_memory_requirements.size,
                    .memoryTypeIndex = i};

            if (vkAllocateMemory(getVkDevice(),
                                 &memory_allocate_info,
                                 nullptr,
                                 memory) == VK_SUCCESS) {
                return true;
            }
        }
    }
    return false;
}

bool Tutorial21::createImageView(VkImage image,
                                 VkFormat format,
                                 VkImageAspectFlags aspect_mask,
                                 VkImageView* image_view) {
    VkImageViewCreateInfo image_view_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                           .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                           .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                           .a = VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange = {.aspectMask = aspect_mask,
                                 .baseMipLevel = 0,
                                 .levelCount = 1,
                                 .baseArrayLayer = 0,
                                 .layerCount = 1}};

    return vkCreateImageView(getVkDevice(),
                             &image_view_create_info,
                             nullptr,
                             image_view) == VK_SUCCESS;
}

bool Tutorial21::createSampler(VkSamplerAddressMode address_mode,
                               VkSampler* sampler) {
    VkSamplerCreateInfo sampler_create_info = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
            .addressModeU = address_mode,
            .addressModeV = address_mode,
            .addressModeW = address_mode,
            .mipLodBias = 0.0f,
            .anisotropyEnable = VK_FALSE,
            .maxAnisotropy = 1.0f,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
            .unnormalizedCoordinates = VK_FALSE};

    return vkCreateSampler(
                   getVkDevice(), &sampler_create_info, nullptr, sampler) ==
           VK_SUCCESS;
}

bool Tutorial21::createDepthResources() {
    ImageParameters& depth_image =
            m_vulkan_tutorial21_parameters.getDepthImageParameters();

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

bool Tutorial21::destroyDepthResources() {
    ImageParameters& depth_image =
            m_vulkan_tutorial21_parameters.getDepthImageParameters();

    if (depth_image.getVkImageView() != VK_NULL_HANDLE) {
        vkDestroyImageView(
                getVkDevice(), depth_image.getVkImageView(), nullptr);
        depth_image.setVkImageView(VK_NULL_HANDLE);
    }
    if (depth_image.getVkImage() != VK_NULL_HANDLE) {
        vkDestroyImage(getVkDevice(), depth_image.getVkImage(), nullptr);
        depth_image.setVkImage(VK_NULL_HANDLE);
    }
    if (depth_image.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), depth_image.getVkDeviceMemory(), nullptr);
        depth_image.setVkDeviceMemory(VK_NULL_HANDLE);
    }
    return true;
}

bool Tutorial21::copyTextureData(VkImage image,
                                 char* texture_data,
                                 std::uint32_t data_size,
                                 std::uint32_t width,
                                 std::uint32_t height) {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial21_parameters.getStagingBufferParameters();

    if (data_size > staging_buffer.getSize()) {
        Logging::error(LOG_TAG,
                       "Texture data (",
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
        Logging::error(LOG_TAG,
                       "Could not map memory and upload texture data to a "
                       "staging buffer!");
        return false;
    }

    std::memcpy(staging_buffer_memory_pointer, texture_data, data_size);

    VkMappedMemoryRange flush_range = {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .pNext = nullptr,
            .memory = staging_buffer.getVkDeviceMemory(),
            .offset = 0,
            .size = VK_WHOLE_SIZE};
    vkFlushMappedMemoryRanges(getVkDevice(), 1, &flush_range);

    vkUnmapMemory(getVkDevice(), staging_buffer.getVkDeviceMemory());

    VkCommandBufferBeginInfo command_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr};

    VkCommandBuffer command_buffer =
            m_vulkan_tutorial21_parameters.getRenderingResources()[0]
                    .getVkCommandBuffer();

    vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

    VkImageSubresourceRange image_subresource_range = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1};

    VkImageMemoryBarrier image_memory_barrier_from_undefined_to_transfer_dst =
            {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
             .pNext = nullptr,
             .srcAccessMask = 0,
             .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
             .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
             .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
             .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
             .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
             .image = image,
             .subresourceRange = image_subresource_range};
    vkCmdPipelineBarrier(command_buffer,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &image_memory_barrier_from_undefined_to_transfer_dst);

    VkBufferImageCopy buffer_image_copy_info = {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                 .mipLevel = 0,
                                 .baseArrayLayer = 0,
                                 .layerCount = 1},
            .imageOffset = {.x = 0, .y = 0, .z = 0},
            .imageExtent = {.width = width, .height = height, .depth = 1}};
    vkCmdCopyBufferToImage(command_buffer,
                           staging_buffer.getVkBuffer(),
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &buffer_image_copy_info);

    VkImageMemoryBarrier image_memory_barrier_from_transfer_to_shader_read = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = image_subresource_range};
    vkCmdPipelineBarrier(command_buffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &image_memory_barrier_from_transfer_to_shader_read);

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

bool Tutorial21::createTextureFromPixels(std::uint32_t width,
                                         std::uint32_t height,
                                         const std::vector<char>& pixels,
                                         VkSamplerAddressMode address_mode,
                                         ImageParameters& out_image_parameters) {
    VkImage vk_image;
    if (!createImage(width,
                     height,
                     c_texture_format,
                     VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                             VK_IMAGE_USAGE_SAMPLED_BIT,
                     &vk_image)) {
        Logging::error(LOG_TAG, "Could not create image!");
        return false;
    }
    out_image_parameters.setVkImage(vk_image);

    VkDeviceMemory vk_device_memory;
    if (!allocateImageMemory(out_image_parameters.getVkImage(),
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &vk_device_memory)) {
        Logging::error(LOG_TAG, "Could not allocate memory for image!");
        return false;
    }
    out_image_parameters.setVkDeviceMemory(vk_device_memory);

    if (vkBindImageMemory(getVkDevice(),
                          out_image_parameters.getVkImage(),
                          out_image_parameters.getVkDeviceMemory(),
                          0) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not bind memory to an image!");
        return false;
    }

    VkImageView vk_image_view;
    if (!createImageView(out_image_parameters.getVkImage(),
                         c_texture_format,
                         VK_IMAGE_ASPECT_COLOR_BIT,
                         &vk_image_view)) {
        Logging::error(LOG_TAG, "Could not create image view!");
        return false;
    }
    out_image_parameters.setVkImageView(vk_image_view);

    VkSampler vk_sampler;
    if (!createSampler(address_mode, &vk_sampler)) {
        Logging::error(LOG_TAG, "Could not create sampler!");
        return false;
    }
    out_image_parameters.setVkSampler(vk_sampler);

    std::vector<char> local_pixels = pixels;
    if (!copyTextureData(out_image_parameters.getVkImage(),
                         local_pixels.data(),
                         static_cast<std::uint32_t>(local_pixels.size()),
                         width,
                         height)) {
        Logging::error(LOG_TAG,
                       "Could not upload texture data to device memory!");
        return false;
    }

    return true;
}

bool Tutorial21::createTerrainTexture() {
    std::vector<char> texture_data =
            Tools::getRawImageData("Rocky.raw", 2048, 2048);
    if (texture_data.empty()) {
        return false;
    }

    // REPEAT, like Tutorial12's own ground texture: texcoords tile
    // across the terrain rather than being clamped into one stretched
    // copy.
    return createTextureFromPixels(
            2048,
            2048,
            texture_data,
            VK_SAMPLER_ADDRESS_MODE_REPEAT,
            m_vulkan_tutorial21_parameters.getTerrainImageParameters());
}

bool Tutorial21::createTankTexture() {
    std::vector<char> texture_data =
            Tools::getRawImageData("TestImage.raw", 1024, 1024);
    if (texture_data.empty()) {
        return false;
    }

    return createTextureFromPixels(
            1024,
            1024,
            texture_data,
            VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            m_vulkan_tutorial21_parameters.getTankImageParameters());
}

bool Tutorial21::createSkyboxTexture() {
    int width = 0;
    int height = 0;
    int data_size = 0;
    std::vector<char> texture_data = Tools::getImageData(
            "SkyBox.jpg", 4, &width, &height, nullptr, &data_size);
    if (texture_data.empty()) {
        return false;
    }

    return createTextureFromPixels(
            static_cast<std::uint32_t>(width),
            static_cast<std::uint32_t>(height),
            texture_data,
            VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            m_vulkan_tutorial21_parameters.getSkyboxImageParameters());
}

Tutorial21TerrainUniformBufferData Tutorial21::getTerrainUniformBufferData()
        const {
    Tutorial21TerrainUniformBufferData data{};
    data.model = Math::Mat4<float>(1.0f);  // static terrain, no rotation
    data.view = glm::lookAt(m_camera.eye(),
                            m_camera.target(),
                            Math::Vec3<float>(0.0f, 1.0f, 0.0f));

    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    // Far plane needs headroom past the skybox's own farthest corner as
    // seen from the camera (kSkyboxHalfExtent*sqrt(3) + the camera's own
    // distance from the origin, worst case ~6996 world units here) - too
    // tight a far plane clips a skybox corner, which produced a real,
    // screenshot-confirmed solid-black triangle on one face.
    data.projection = Tools::getPerspectiveProjectionMatrix(
            width / height, 45.0f, 1.0f, 8000.0f);

    // Same real light Tutorial12 uses, just repositioned/rescaled for
    // this tutorial's much larger terrain.
    data.light_position = Math::Vec4<float>(400.0f, 600.0f, 400.0f, 1.0f);
    data.light_color = Math::Vec4<float>(1.0f, 1.0f, 1.0f, 1.0f);
    data.view_position = Math::Vec4<float>(m_camera.eye(), 1.0f);

    return data;
}

bool Tutorial21::updateTerrainUniformBufferData() {
    Tutorial21TerrainUniformBufferData const uniform_data =
            getTerrainUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial21_parameters.getTerrainUniformBufferParameters();

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

Tutorial21ObjectUniformBufferData Tutorial21::getObjectUniformBufferData()
        const {
    Tutorial21ObjectUniformBufferData data{};
    data.view = glm::lookAt(m_camera.eye(),
                            m_camera.target(),
                            Math::Vec3<float>(0.0f, 1.0f, 0.0f));

    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    data.projection = Tools::getPerspectiveProjectionMatrix(
            width / height, 45.0f, 1.0f, 8000.0f);

    return data;
}

bool Tutorial21::updateObjectUniformBufferData() {
    Tutorial21ObjectUniformBufferData const uniform_data =
            getObjectUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial21_parameters.getObjectUniformBufferParameters();

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

bool Tutorial21::createUniformBuffers() {
    BufferParameters& terrain_uniform_buffer =
            m_vulkan_tutorial21_parameters.getTerrainUniformBufferParameters();
    terrain_uniform_buffer.setSize(sizeof(Tutorial21TerrainUniformBufferData));
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      terrain_uniform_buffer)) {
        Logging::error(LOG_TAG, "Could not create terrain uniform buffer!");
        return false;
    }
    if (!updateTerrainUniformBufferData()) {
        return false;
    }

    BufferParameters& object_uniform_buffer =
            m_vulkan_tutorial21_parameters.getObjectUniformBufferParameters();
    object_uniform_buffer.setSize(sizeof(Tutorial21ObjectUniformBufferData));
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      object_uniform_buffer)) {
        Logging::error(LOG_TAG, "Could not create object uniform buffer!");
        return false;
    }
    return updateObjectUniformBufferData();
}

bool Tutorial21::createDescriptorSetLayouts() {
    // Terrain layout matches shader.21_terrain's own bindings exactly:
    // 0 = UBO (both stages reference it), 1 = sampler (fragment only).
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings_terrain = {
            {.binding = 0,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
                          VK_SHADER_STAGE_FRAGMENT_BIT,
             .pImmutableSamplers = nullptr},
            {.binding = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = 1,
             .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
             .pImmutableSamplers = nullptr}};
    VkDescriptorSetLayoutCreateInfo layout_create_info_terrain = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount =
                    static_cast<std::uint32_t>(layout_bindings_terrain.size()),
            .pBindings = layout_bindings_terrain.data()};
    VkDescriptorSetLayout vk_terrain_layout;
    if (vkCreateDescriptorSetLayout(getVkDevice(),
                                    &layout_create_info_terrain,
                                    nullptr,
                                    &vk_terrain_layout) != VK_SUCCESS) {
        Logging::error(LOG_TAG,
                       "Could not create terrain descriptor set layout!");
        return false;
    }
    m_vulkan_tutorial21_parameters.setVkTerrainDescriptorSetLayout(
            vk_terrain_layout);

    // Object layout matches Tutorial16's shaders exactly (0 = sampler
    // in the fragment stage, 1 = UBO in the vertex stage) - shared by
    // both the tank and the skybox.
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings_object = {
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
    VkDescriptorSetLayoutCreateInfo layout_create_info_object = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount =
                    static_cast<std::uint32_t>(layout_bindings_object.size()),
            .pBindings = layout_bindings_object.data()};
    VkDescriptorSetLayout vk_object_layout;
    if (vkCreateDescriptorSetLayout(getVkDevice(),
                                    &layout_create_info_object,
                                    nullptr,
                                    &vk_object_layout) != VK_SUCCESS) {
        Logging::error(LOG_TAG,
                       "Could not create object descriptor set layout!");
        return false;
    }
    m_vulkan_tutorial21_parameters.setVkObjectDescriptorSetLayout(
            vk_object_layout);

    return true;
}

bool Tutorial21::createDescriptorPool() {
    // 1 terrain set + 2 object sets (tank, skybox) = 3 sets, each with
    // one sampler and one uniform buffer.
    std::vector<VkDescriptorPoolSize> pool_sizes = {
            {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 3},
            {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = 3}};

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .maxSets = 3,
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
    m_vulkan_tutorial21_parameters.setVkDescriptorPool(vk_descriptor_pool);

    return true;
}

bool Tutorial21::allocateDescriptorSets() {
    VkDescriptorSetLayout vk_terrain_layout =
            m_vulkan_tutorial21_parameters.getVkTerrainDescriptorSetLayout();
    VkDescriptorSetAllocateInfo allocate_info_terrain = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool =
                    m_vulkan_tutorial21_parameters.getVkDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_terrain_layout};
    VkDescriptorSet vk_terrain_set;
    if (vkAllocateDescriptorSets(
                getVkDevice(), &allocate_info_terrain, &vk_terrain_set) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate terrain descriptor set!");
        return false;
    }
    m_vulkan_tutorial21_parameters.setTerrainVkDescriptorSet(vk_terrain_set);

    VkDescriptorSetLayout vk_object_layout =
            m_vulkan_tutorial21_parameters.getVkObjectDescriptorSetLayout();
    VkDescriptorSetAllocateInfo allocate_info_object = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool =
                    m_vulkan_tutorial21_parameters.getVkDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_object_layout};

    VkDescriptorSet vk_tank_set;
    if (vkAllocateDescriptorSets(
                getVkDevice(), &allocate_info_object, &vk_tank_set) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate tank descriptor set!");
        return false;
    }
    m_vulkan_tutorial21_parameters.setTankVkDescriptorSet(vk_tank_set);

    VkDescriptorSet vk_skybox_set;
    if (vkAllocateDescriptorSets(
                getVkDevice(), &allocate_info_object, &vk_skybox_set) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate skybox descriptor set!");
        return false;
    }
    m_vulkan_tutorial21_parameters.setSkyboxVkDescriptorSet(vk_skybox_set);

    return true;
}

bool Tutorial21::updateDescriptorSets() {
    BufferParameters& terrain_uniform_buffer =
            m_vulkan_tutorial21_parameters.getTerrainUniformBufferParameters();
    ImageParameters& terrain_image =
            m_vulkan_tutorial21_parameters.getTerrainImageParameters();
    VkDescriptorBufferInfo terrain_buffer_info = {
            .buffer = terrain_uniform_buffer.getVkBuffer(),
            .offset = 0,
            .range = terrain_uniform_buffer.getSize()};
    VkDescriptorImageInfo terrain_image_info = {
            .sampler = terrain_image.getVkSampler(),
            .imageView = terrain_image.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    BufferParameters& object_uniform_buffer =
            m_vulkan_tutorial21_parameters.getObjectUniformBufferParameters();
    VkDescriptorBufferInfo object_buffer_info = {
            .buffer = object_uniform_buffer.getVkBuffer(),
            .offset = 0,
            .range = object_uniform_buffer.getSize()};

    ImageParameters& tank_image =
            m_vulkan_tutorial21_parameters.getTankImageParameters();
    VkDescriptorImageInfo tank_image_info = {
            .sampler = tank_image.getVkSampler(),
            .imageView = tank_image.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    ImageParameters& skybox_image =
            m_vulkan_tutorial21_parameters.getSkyboxImageParameters();
    VkDescriptorImageInfo skybox_image_info = {
            .sampler = skybox_image.getVkSampler(),
            .imageView = skybox_image.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    std::vector<VkWriteDescriptorSet> descriptor_writes = {
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet =
                     m_vulkan_tutorial21_parameters.getTerrainVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &terrain_buffer_info,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet =
                     m_vulkan_tutorial21_parameters.getTerrainVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &terrain_image_info,
             .pBufferInfo = nullptr,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial21_parameters.getTankVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &tank_image_info,
             .pBufferInfo = nullptr,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial21_parameters.getTankVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &object_buffer_info,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet =
                     m_vulkan_tutorial21_parameters.getSkyboxVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &skybox_image_info,
             .pBufferInfo = nullptr,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet =
                     m_vulkan_tutorial21_parameters.getSkyboxVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &object_buffer_info,
             .pTexelBufferView = nullptr}};

    vkUpdateDescriptorSets(
            getVkDevice(),
            static_cast<std::uint32_t>(descriptor_writes.size()),
            descriptor_writes.data(),
            0,
            nullptr);
    return true;
}

bool Tutorial21::createRenderPass() {
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
                &m_vulkan_tutorial21_parameters.getVkRenderPass()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

bool Tutorial21::createPipelineLayouts() {
    VkDescriptorSetLayout vk_terrain_layout =
            m_vulkan_tutorial21_parameters.getVkTerrainDescriptorSetLayout();
    VkPipelineLayoutCreateInfo layout_create_info_terrain = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &vk_terrain_layout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr};
    if (vkCreatePipelineLayout(
                getVkDevice(),
                &layout_create_info_terrain,
                nullptr,
                &m_vulkan_tutorial21_parameters.getVkTerrainPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create terrain pipeline layout!");
        return false;
    }

    VkDescriptorSetLayout vk_object_layout =
            m_vulkan_tutorial21_parameters.getVkObjectDescriptorSetLayout();
    VkPushConstantRange push_constant_range = {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(Tutorial21PushConstants)};
    VkPipelineLayoutCreateInfo layout_create_info_object = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &vk_object_layout,
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &push_constant_range};
    if (vkCreatePipelineLayout(
                getVkDevice(),
                &layout_create_info_object,
                nullptr,
                &m_vulkan_tutorial21_parameters.getVkObjectPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create object pipeline layout!");
        return false;
    }

    return true;
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
Tutorial21::createShaderModule(const char* filename) {
    const std::vector<char> code = Tools::getBinaryFileContents(filename);
    if (code.empty()) {
        return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>();
    }

    VkShaderModuleCreateInfo shader_module_create_info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = code.size(),
            .pCode = reinterpret_cast<const std::uint32_t*>(code.data())};

    VkShaderModule shader_module;
    if (vkCreateShaderModule(getVkDevice(),
                             &shader_module_create_info,
                             nullptr,
                             &shader_module) != VK_SUCCESS) {
        Logging::error(LOG_TAG,
                       "Could not create shader module from a \"",
                       filename,
                       "\" file!");
        return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>();
    }

    return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>(
            shader_module, vkDestroyShaderModule, getVkDevice());
}

bool Tutorial21::createPipelines() {
    // --- Terrain pipeline (Phong-lit, depth-tested) ---
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module_terrain =
                    createShaderModule("shader.21_terrain.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module_terrain =
                    createShaderModule("shader.21_terrain.frag.spv");
    if (!vertex_shader_module_terrain || !fragment_shader_module_terrain) {
        return false;
    }

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_terrain = {
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_VERTEX_BIT,
             .module = vertex_shader_module_terrain.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr},
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
             .module = fragment_shader_module_terrain.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr}};

    std::vector<VkVertexInputBindingDescription> vertex_bindings_terrain = {
            {.binding = 0,
             .stride = Tutorial21TerrainVertexAttributeTraits::stride,
             .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};
    std::vector<VkVertexInputAttributeDescription>
            vertex_attributes_terrain = {
                    {.location = 0,
                     .binding = 0,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset = offsetof(struct Tutorial21TerrainVertexData,
                                        position)},
                    {.location = 1,
                     .binding = 0,
                     .format = VK_FORMAT_R32G32B32_SFLOAT,
                     .offset = offsetof(struct Tutorial21TerrainVertexData,
                                        normal)},
                    {.location = 2,
                     .binding = 0,
                     .format = VK_FORMAT_R32G32_SFLOAT,
                     .offset = offsetof(struct Tutorial21TerrainVertexData,
                                        texcoord)}};
    VkPipelineVertexInputStateCreateInfo vertex_input_state_terrain = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount =
                    static_cast<std::uint32_t>(vertex_bindings_terrain.size()),
            .pVertexBindingDescriptions = vertex_bindings_terrain.data(),
            .vertexAttributeDescriptionCount = static_cast<std::uint32_t>(
                    vertex_attributes_terrain.size()),
            .pVertexAttributeDescriptions = vertex_attributes_terrain.data()};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_terrain = {
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

    VkPipelineRasterizationStateCreateInfo rasterization_terrain = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            // NONE - same reasoning Tutorial12 uses for its own cullMode.
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

    VkPipelineDepthStencilStateCreateInfo depth_stencil_on = {
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
    VkPipelineColorBlendStateCreateInfo color_blend_state_terrain = {
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

    VkGraphicsPipelineCreateInfo pipeline_create_info_terrain = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount =
                    static_cast<std::uint32_t>(shader_stages_terrain.size()),
            .pStages = shader_stages_terrain.data(),
            .pVertexInputState = &vertex_input_state_terrain,
            .pInputAssemblyState = &input_assembly_terrain,
            .pTessellationState = nullptr,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterization_terrain,
            .pMultisampleState = &multisample_state,
            .pDepthStencilState = &depth_stencil_on,
            .pColorBlendState = &color_blend_state_terrain,
            .pDynamicState = &dynamic_state_create_info,
            .layout =
                    m_vulkan_tutorial21_parameters.getVkTerrainPipelineLayout(),
            .renderPass = m_vulkan_tutorial21_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};
    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info_terrain,
                nullptr,
                &m_vulkan_tutorial21_parameters
                         .getVkTerrainGraphicsPipeline()) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create terrain graphics pipeline!");
        return false;
    }

    // --- Object pipeline (unlit, textured - tank; real depth test/write) ---
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module_object =
                    createShaderModule("shader.21_object.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module_object =
                    createShaderModule("shader.21_object.frag.spv");
    if (!vertex_shader_module_object || !fragment_shader_module_object) {
        return false;
    }

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_object = {
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_VERTEX_BIT,
             .module = vertex_shader_module_object.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr},
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
             .module = fragment_shader_module_object.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr}};

    std::vector<VkVertexInputBindingDescription> vertex_bindings_object = {
            {.binding = 0,
             .stride = Tutorial21ObjectVertexAttributeTraits::stride,
             .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};
    std::vector<VkVertexInputAttributeDescription> vertex_attributes_object = {
            {.location = 0,
             .binding = 0,
             .format = VK_FORMAT_R32G32B32A32_SFLOAT,
             .offset = offsetof(struct Tutorial21ObjectVertexData, position)},
            {.location = 1,
             .binding = 0,
             .format = VK_FORMAT_R32G32_SFLOAT,
             .offset = offsetof(struct Tutorial21ObjectVertexData, texcoord)}};
    VkPipelineVertexInputStateCreateInfo vertex_input_state_object = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount =
                    static_cast<std::uint32_t>(vertex_bindings_object.size()),
            .pVertexBindingDescriptions = vertex_bindings_object.data(),
            .vertexAttributeDescriptionCount = static_cast<std::uint32_t>(
                    vertex_attributes_object.size()),
            .pVertexAttributeDescriptions = vertex_attributes_object.data()};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_object = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE};

    VkPipelineRasterizationStateCreateInfo rasterization_object = {
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

    // Real depth test/write for the tank - it should both be occluded by
    // terrain in front of it and occlude terrain behind it, same as
    // every other solid object in this scene.
    VkPipelineDepthStencilStateCreateInfo depth_stencil_object = {
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

    VkPipelineColorBlendStateCreateInfo color_blend_state_object = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &color_blend_attachment_opaque,
            .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};

    VkGraphicsPipelineCreateInfo pipeline_create_info_object = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount =
                    static_cast<std::uint32_t>(shader_stages_object.size()),
            .pStages = shader_stages_object.data(),
            .pVertexInputState = &vertex_input_state_object,
            .pInputAssemblyState = &input_assembly_object,
            .pTessellationState = nullptr,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterization_object,
            .pMultisampleState = &multisample_state,
            .pDepthStencilState = &depth_stencil_object,
            .pColorBlendState = &color_blend_state_object,
            .pDynamicState = &dynamic_state_create_info,
            .layout =
                    m_vulkan_tutorial21_parameters.getVkObjectPipelineLayout(),
            .renderPass = m_vulkan_tutorial21_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};
    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info_object,
                nullptr,
                &m_vulkan_tutorial21_parameters.getVkObjectGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create object graphics pipeline!");
        return false;
    }

    // --- Skybox pipeline (same shaders/layout/vertex format as the tank's
    // object pipeline, but with depth testing off entirely) ---
    //
    // The skybox is drawn first, against a freshly-cleared depth buffer
    // (cleared to the far value, 1.0). Real depth testing here is both
    // unnecessary (nothing has been drawn yet to occlude against) and
    // actively harmful: at kSkyboxHalfExtent (3000 world units) against
    // this scene's far plane (6000), the skybox cube's projected depth
    // for some fragments rounds to exactly 1.0 at floating-point
    // precision limits, which fails a strict "less than" test against
    // the 1.0-cleared buffer and leaves the (near-black) clear color
    // showing through as a visible hole - confirmed via screenshot
    // (a dark wedge, unaffected by changing the terrain's own height-
    // generation constants, ruling out a terrain cause). Disabling the
    // skybox's own depth test/write entirely avoids the precision edge
    // case altogether, while the tank's own pipeline (just above) keeps
    // real depth test/write for correct terrain/tank compositing.
    VkPipelineDepthStencilStateCreateInfo depth_stencil_skybox = {
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

    VkGraphicsPipelineCreateInfo pipeline_create_info_skybox = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount =
                    static_cast<std::uint32_t>(shader_stages_object.size()),
            .pStages = shader_stages_object.data(),
            .pVertexInputState = &vertex_input_state_object,
            .pInputAssemblyState = &input_assembly_object,
            .pTessellationState = nullptr,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterization_object,
            .pMultisampleState = &multisample_state,
            .pDepthStencilState = &depth_stencil_skybox,
            .pColorBlendState = &color_blend_state_object,
            .pDynamicState = &dynamic_state_create_info,
            .layout =
                    m_vulkan_tutorial21_parameters.getVkObjectPipelineLayout(),
            .renderPass = m_vulkan_tutorial21_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};
    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info_skybox,
                nullptr,
                &m_vulkan_tutorial21_parameters.getVkSkyboxGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create skybox graphics pipeline!");
        return false;
    }

    return true;
}

void Tutorial21::ensureTerrainGenerated() {
    if (m_terrain_generated) {
        return;
    }
    m_terrain_generator.generate(
            kGenSteps, kGenIncrease, kGenRadius, kGenRandomJump,
            kSmoothingPasses);
    m_terrain_generated = true;
}

const std::vector<Tutorial21TerrainVertexData>&
Tutorial21::getTerrainVertexData() {
    if (!m_terrain_vertex_data.empty()) {
        return m_terrain_vertex_data;
    }
    ensureTerrainGenerated();

    // Same six-vertices-per-cell construction as Tutorial12::
    // getVertexData() (see that tutorial's own comments for the
    // texcoord-tiling chunk_size trick) - just parameterized by this
    // tutorial's own (larger) grid constants. Unlike Tutorial12, this
    // calls TerrainGenerator::normalAt() unconditionally at every grid
    // cell instead of leaving a hard (0,0,0) normal at the i==0/j==0
    // boundary: normalAt()'s own calcNormal() helper already falls back
    // to a safe (0,1,0) for any out-of-range neighbor it needs, so the
    // extra zero-vector guard only ever replaces that safe fallback with
    // a genuinely degenerate normal - normalize((0,0,0)) is NaN, which
    // propagates through the Phong lighting math into a solid-black
    // fragment. That produced a real, screenshot-confirmed black wedge
    // along the grid's near edge (reproducible regardless of the actual
    // generated height values, confirming it was this normal bug and not
    // a height-generation or skybox depth issue).
    int const chunk_size = kGridSize / 2;
    float const chunk_span = static_cast<float>(chunk_size - 1);
    float const half_extent =
            static_cast<float>(kGridSize - 1) * kGridScale / 2.0f;

    m_terrain_vertex_data.reserve(static_cast<std::size_t>(kGridSize - 1) *
                                  static_cast<std::size_t>(kGridSize - 1) * 6);

    for (int i = 0; i < kGridSize - 1; ++i) {
        for (int j = 0; j < kGridSize - 1; ++j) {
            auto make_position = [&](int grid_x, int grid_z, int height_row,
                                     int height_col) {
                return Math::Vec4<float>(
                        static_cast<float>(grid_x * kGridScale) - half_extent,
                        static_cast<float>(m_terrain_generator.heightAt(
                                height_row, height_col)),
                        static_cast<float>(grid_z * kGridScale) - half_extent,
                        1.0f);
            };

            Math::Vec2<float> t_i(
                    static_cast<float>(i % (chunk_size - 1)) / chunk_span,
                    static_cast<float>(j % (chunk_size - 1)) / chunk_span);
            Math::Vec3<float> const n_i = m_terrain_generator.normalAt(j, i);
            m_terrain_vertex_data.push_back(
                    {make_position(j, i, i, j), n_i, t_i});

            Math::Vec2<float> t_j(
                    (static_cast<float>(i % (chunk_size - 1)) + 1) /
                            chunk_span,
                    static_cast<float>(j % (chunk_size - 1)) / chunk_span);
            Math::Vec3<float> const n_j =
                    m_terrain_generator.normalAt(j, i + 1);
            m_terrain_vertex_data.push_back(
                    {make_position(j, i + 1, i + 1, j), n_j, t_j});

            Math::Vec2<float> t_k(
                    static_cast<float>(i % (chunk_size - 1)) / chunk_span,
                    (static_cast<float>(j % (chunk_size - 1)) + 1) /
                            chunk_span);
            Math::Vec3<float> const n_k =
                    m_terrain_generator.normalAt(j + 1, i);
            m_terrain_vertex_data.push_back(
                    {make_position(j + 1, i, i, j + 1), n_k, t_k});

            m_terrain_vertex_data.push_back(
                    {make_position(j, i + 1, i + 1, j), n_j, t_j});

            Math::Vec2<float> t_y(
                    (static_cast<float>(i % (chunk_size - 1)) + 1) /
                            chunk_span,
                    (static_cast<float>(j % (chunk_size - 1)) + 1) /
                            chunk_span);
            Math::Vec3<float> const n_y =
                    m_terrain_generator.normalAt(j + 1, i + 1);
            m_terrain_vertex_data.push_back(
                    {make_position(j + 1, i + 1, i + 1, j + 1), n_y, t_y});

            m_terrain_vertex_data.push_back(
                    {make_position(j + 1, i, i, j + 1), n_k, t_k});
        }
    }

    return m_terrain_vertex_data;
}

float Tutorial21::getTankGroundHeight() {
    ensureTerrainGenerated();
    return static_cast<float>(
            m_terrain_generator.heightAt(kGridSize / 2, kGridSize / 2));
}

Math::Mat4<float> Tutorial21::getTankPartModelMatrix(
        Math::Vec3<float> const& part_translation) const {
    return buildPartMatrix(part_translation);
}

const std::vector<Tutorial21ObjectVertexData>& Tutorial21::getSkyboxVertexData()
        const {
    // Same six-face layout/UV mapping as Tutorial11's own
    // getVertexData() (see that tutorial's own comments), just at this
    // tutorial's own kSkyboxHalfExtent scale instead of Tutorial11's
    // small pilot scale.
    static const std::vector<Tutorial21ObjectVertexData> vertex_data = [] {
        float const x_min = -kSkyboxHalfExtent;
        float const x_max = kSkyboxHalfExtent;
        float const y_min = -kSkyboxHalfExtent;
        float const y_max = kSkyboxHalfExtent * 0.5f;
        float const z_min = -kSkyboxHalfExtent;
        float const z_max = kSkyboxHalfExtent;

        return std::vector<Tutorial21ObjectVertexData>{
                // front (z = z_min)
                {Math::Vec4<float>(x_min, y_min, z_min, 1.0f),
                 Math::Vec2<float>(0.0f, 1.0f)},
                {Math::Vec4<float>(x_max, y_min, z_min, 1.0f),
                 Math::Vec2<float>(0.0f, 0.0f)},
                {Math::Vec4<float>(x_max, y_max, z_min, 1.0f),
                 Math::Vec2<float>(1.0f, 0.0f)},
                {Math::Vec4<float>(x_min, y_max, z_min, 1.0f),
                 Math::Vec2<float>(1.0f, 1.0f)},
                // right (x = x_max)
                {Math::Vec4<float>(x_max, y_min, z_min, 1.0f),
                 Math::Vec2<float>(0.0f, 1.0f)},
                {Math::Vec4<float>(x_max, y_min, z_max, 1.0f),
                 Math::Vec2<float>(0.0f, 0.0f)},
                {Math::Vec4<float>(x_max, y_max, z_max, 1.0f),
                 Math::Vec2<float>(1.0f, 0.0f)},
                {Math::Vec4<float>(x_max, y_max, z_min, 1.0f),
                 Math::Vec2<float>(1.0f, 1.0f)},
                // back (z = z_max)
                {Math::Vec4<float>(x_max, y_min, z_max, 1.0f),
                 Math::Vec2<float>(0.0f, 1.0f)},
                {Math::Vec4<float>(x_min, y_min, z_max, 1.0f),
                 Math::Vec2<float>(0.0f, 0.0f)},
                {Math::Vec4<float>(x_min, y_max, z_max, 1.0f),
                 Math::Vec2<float>(1.0f, 0.0f)},
                {Math::Vec4<float>(x_max, y_max, z_max, 1.0f),
                 Math::Vec2<float>(1.0f, 1.0f)},
                // left (x = x_min)
                {Math::Vec4<float>(x_min, y_min, z_max, 1.0f),
                 Math::Vec2<float>(0.0f, 1.0f)},
                {Math::Vec4<float>(x_min, y_min, z_min, 1.0f),
                 Math::Vec2<float>(0.0f, 0.0f)},
                {Math::Vec4<float>(x_min, y_max, z_min, 1.0f),
                 Math::Vec2<float>(1.0f, 0.0f)},
                {Math::Vec4<float>(x_min, y_max, z_max, 1.0f),
                 Math::Vec2<float>(1.0f, 1.0f)},
                // top (y = y_max)
                {Math::Vec4<float>(x_min, y_max, z_min, 1.0f),
                 Math::Vec2<float>(0.0f, 1.0f)},
                {Math::Vec4<float>(x_max, y_max, z_min, 1.0f),
                 Math::Vec2<float>(0.0f, 0.0f)},
                {Math::Vec4<float>(x_max, y_max, z_max, 1.0f),
                 Math::Vec2<float>(1.0f, 0.0f)},
                {Math::Vec4<float>(x_min, y_max, z_max, 1.0f),
                 Math::Vec2<float>(1.0f, 1.0f)},
                // bottom (y = y_min)
                {Math::Vec4<float>(x_min, y_min, z_min, 1.0f),
                 Math::Vec2<float>(0.0f, 1.0f)},
                {Math::Vec4<float>(x_min, y_min, z_max, 1.0f),
                 Math::Vec2<float>(0.0f, 0.0f)},
                {Math::Vec4<float>(x_max, y_min, z_max, 1.0f),
                 Math::Vec2<float>(1.0f, 0.0f)},
                {Math::Vec4<float>(x_max, y_min, z_min, 1.0f),
                 Math::Vec2<float>(1.0f, 1.0f)},
        };
    }();

    return vertex_data;
}

const std::vector<std::uint32_t>& Tutorial21::getSkyboxIndexData() const {
    static const std::vector<std::uint32_t> index_data = {
            0,  1,  2,  0,  2,  3,   // front
            4,  5,  6,  4,  6,  7,   // right
            8,  9,  10, 8,  10, 11,  // back
            12, 13, 14, 12, 14, 15,  // left
            16, 17, 18, 16, 18, 19,  // top
            20, 21, 22, 20, 22, 23,  // bottom
    };
    return index_data;
}

bool Tutorial21::copyBufferData(BufferParameters& destination,
                                const void* data,
                                std::uint32_t data_size,
                                VkAccessFlags dst_access_mask,
                                VkPipelineStageFlags dst_stage_mask) {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial21_parameters.getStagingBufferParameters();

    if (data_size > staging_buffer.getSize()) {
        Logging::error(LOG_TAG,
                       "Buffer data (",
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

    std::memcpy(staging_buffer_memory_pointer, data, data_size);

    VkMappedMemoryRange flush_range = {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .pNext = nullptr,
            .memory = staging_buffer.getVkDeviceMemory(),
            .offset = 0,
            .size = VK_WHOLE_SIZE};
    vkFlushMappedMemoryRanges(getVkDevice(), 1, &flush_range);

    vkUnmapMemory(getVkDevice(), staging_buffer.getVkDeviceMemory());

    VkCommandBuffer command_buffer =
            m_vulkan_tutorial21_parameters.getRenderingResources()[0]
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
                    destination.getVkBuffer(),
                    1,
                    &buffer_copy_info);

    VkBufferMemoryBarrier buffer_memory_barrier = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .dstAccessMask = dst_access_mask,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = destination.getVkBuffer(),
            .offset = 0,
            .size = VK_WHOLE_SIZE};
    vkCmdPipelineBarrier(command_buffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         dst_stage_mask,
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

bool Tutorial21::createTerrainVertexBuffer() {
    const std::vector<Tutorial21TerrainVertexData>& vertex_data =
            getTerrainVertexData();
    m_vulkan_tutorial21_parameters.setTerrainVertexCount(
            static_cast<std::uint32_t>(vertex_data.size()));

    BufferParameters& vertex_buffer =
            m_vulkan_tutorial21_parameters.getTerrainVertexBufferParameters();
    vertex_buffer.setSize(static_cast<std::uint32_t>(
            vertex_data.size() * sizeof(vertex_data[0])));
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      vertex_buffer)) {
        Logging::error(LOG_TAG, "Could not create terrain vertex buffer!");
        return false;
    }

    return copyBufferData(vertex_buffer,
                          vertex_data.data(),
                          vertex_buffer.getSize(),
                          VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                          VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
}

std::vector<Tutorial21ObjectVertexData> Tutorial21::loadTankPartVertexData(
        const char* mesh_filename) const {
    std::vector<Tools::OglVertexData> const mesh_data =
            Tools::loadOglMeshData(mesh_filename);
    std::vector<Tutorial21ObjectVertexData> vertex_data;
    vertex_data.reserve(mesh_data.size());
    for (Tools::OglVertexData const& vertex : mesh_data) {
        vertex_data.push_back(
                {Math::Vec4<float>(vertex.position, 1.0f), vertex.texcoord});
    }
    return vertex_data;
}

bool Tutorial21::createTankPartVertexBuffer(const char* mesh_filename,
                                            BufferParameters& vertex_buffer,
                                            std::uint32_t& vertex_count) {
    std::vector<Tutorial21ObjectVertexData> const vertex_data =
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

bool Tutorial21::createTankVertexBuffers() {
    std::array<BufferParameters, kTankPartCount>& vertex_buffers =
            m_vulkan_tutorial21_parameters.getTankVertexBufferParameters();
    std::array<std::uint32_t, kTankPartCount>& vertex_counts =
            m_vulkan_tutorial21_parameters.getTankVertexCounts();

    static const std::array<const char*, kTankPartCount> mesh_files = {
            {"Hellfire_Body.ogl", "Hellfire_Head.ogl", "Hellfire_Turret.ogl"}};

    for (std::size_t i = 0; i < kTankPartCount; ++i) {
        if (!createTankPartVertexBuffer(
                    mesh_files[i], vertex_buffers[i], vertex_counts[i])) {
            return false;
        }
    }
    return true;
}

bool Tutorial21::createSkyboxBuffers() {
    const std::vector<Tutorial21ObjectVertexData>& vertex_data =
            getSkyboxVertexData();
    BufferParameters& vertex_buffer =
            m_vulkan_tutorial21_parameters.getSkyboxVertexBufferParameters();
    vertex_buffer.setSize(static_cast<std::uint32_t>(
            vertex_data.size() * sizeof(vertex_data[0])));
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      vertex_buffer)) {
        Logging::error(LOG_TAG, "Could not create skybox vertex buffer!");
        return false;
    }
    if (!copyBufferData(vertex_buffer,
                        vertex_data.data(),
                        vertex_buffer.getSize(),
                        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                        VK_PIPELINE_STAGE_VERTEX_INPUT_BIT)) {
        return false;
    }

    const std::vector<std::uint32_t>& index_data = getSkyboxIndexData();
    m_vulkan_tutorial21_parameters.setSkyboxIndexCount(
            static_cast<std::uint32_t>(index_data.size()));
    BufferParameters& index_buffer =
            m_vulkan_tutorial21_parameters.getSkyboxIndexBufferParameters();
    index_buffer.setSize(static_cast<std::uint32_t>(
            index_data.size() * sizeof(index_data[0])));
    if (!createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      index_buffer)) {
        Logging::error(LOG_TAG, "Could not create skybox index buffer!");
        return false;
    }
    return copyBufferData(index_buffer,
                          index_data.data(),
                          index_buffer.getSize(),
                          VK_ACCESS_INDEX_READ_BIT,
                          VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
}

bool Tutorial21::createFramebuffer(VkFramebuffer& framebuffer,
                                   VkImageView image_view) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(getVkDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    std::array<VkImageView, 2> const attachments = {
            image_view,
            m_vulkan_tutorial21_parameters.getDepthImageParameters()
                    .getVkImageView()};

    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_vulkan_tutorial21_parameters.getVkRenderPass(),
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

bool Tutorial21::prepareFrame(VkCommandBuffer command_buffer,
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
    clear_values[0].color = {.float32 = {0.05f, 0.05f, 0.08f, 1.0f}};
    clear_values[1].depthStencil = {.depth = 1.0f, .stencil = 0};

    VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = m_vulkan_tutorial21_parameters.getVkRenderPass(),
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

    VkDeviceSize const zero_offset = 0;

    // --- Pass 1: skybox first, its own pipeline with depth testing off
    // entirely (see createPipelines() for why).
    vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial21_parameters.getVkSkyboxGraphicsPipeline());

    VkDescriptorSet vk_skybox_descriptor_set =
            m_vulkan_tutorial21_parameters.getSkyboxVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial21_parameters.getVkObjectPipelineLayout(),
            0,
            1,
            &vk_skybox_descriptor_set,
            0,
            nullptr);
    vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_vulkan_tutorial21_parameters.getSkyboxVertexBufferParameters()
                     .getVkBuffer(),
            &zero_offset);
    vkCmdBindIndexBuffer(
            command_buffer,
            m_vulkan_tutorial21_parameters.getSkyboxIndexBufferParameters()
                    .getVkBuffer(),
            0,
            VK_INDEX_TYPE_UINT32);
    Tutorial21PushConstants skybox_push_constants{Math::Mat4<float>(1.0f)};
    vkCmdPushConstants(command_buffer,
                       m_vulkan_tutorial21_parameters.getVkObjectPipelineLayout(),
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(Tutorial21PushConstants),
                       &skybox_push_constants);
    vkCmdDrawIndexed(
            command_buffer,
            m_vulkan_tutorial21_parameters.getSkyboxIndexCount(),
            1,
            0,
            0,
            0);

    // --- Pass 2: terrain, Phong-lit, real depth test/write ---
    vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial21_parameters.getVkTerrainGraphicsPipeline());
    VkDescriptorSet vk_terrain_descriptor_set =
            m_vulkan_tutorial21_parameters.getTerrainVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial21_parameters.getVkTerrainPipelineLayout(),
            0,
            1,
            &vk_terrain_descriptor_set,
            0,
            nullptr);
    vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_vulkan_tutorial21_parameters.getTerrainVertexBufferParameters()
                     .getVkBuffer(),
            &zero_offset);
    vkCmdDraw(command_buffer,
             m_vulkan_tutorial21_parameters.getTerrainVertexCount(),
             1,
             0,
             0);

    // --- Pass 3: tank, three parts, real hierarchical positioning
    // (Tank::setTankPos()) rooted at a real terrain height query ---
    vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial21_parameters.getVkObjectGraphicsPipeline());
    VkDescriptorSet vk_tank_descriptor_set =
            m_vulkan_tutorial21_parameters.getTankVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial21_parameters.getVkObjectPipelineLayout(),
            0,
            1,
            &vk_tank_descriptor_set,
            0,
            nullptr);

    Math::Vec3<float> const tank_world_position(
            0.0f, getTankGroundHeight(), 0.0f);
    Math::Vec3<float> const body_translation =
            tank_world_position + c_body_offset;
    Math::Vec3<float> const head_translation =
            body_translation +
            rotateOffsetThroughBasis(getPartBasis(), c_head_offset);
    Math::Vec3<float> const turret_translation =
            head_translation +
            rotateOffsetThroughBasis(getPartBasis(), c_turret_offset);
    std::array<Math::Vec3<float>, kTankPartCount> const part_translations = {
            {body_translation, head_translation, turret_translation}};

    std::array<BufferParameters, kTankPartCount>& tank_vertex_buffers =
            m_vulkan_tutorial21_parameters.getTankVertexBufferParameters();
    std::array<std::uint32_t, kTankPartCount>& tank_vertex_counts =
            m_vulkan_tutorial21_parameters.getTankVertexCounts();
    for (std::size_t i = 0; i < kTankPartCount; ++i) {
        vkCmdBindVertexBuffers(command_buffer,
                               0,
                               1,
                               &tank_vertex_buffers[i].getVkBuffer(),
                               &zero_offset);
        Tutorial21PushConstants tank_push_constants{
                getTankPartModelMatrix(part_translations[i])};
        vkCmdPushConstants(
                command_buffer,
                m_vulkan_tutorial21_parameters.getVkObjectPipelineLayout(),
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof(Tutorial21PushConstants),
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

bool Tutorial21::draw() {
    static std::size_t resource_index = 0;
    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial21_parameters.getRenderingResources();
    RenderingResourceParameters& current_rendering_resource =
            rendering_resources[resource_index];
    VkSwapchainKHR swap_chain = getSwapchainParameters().getVkSwapchainKhr();
    std::uint32_t image_index;

    resource_index = (resource_index + 1) %
                     VulkanTutorial21Parameters::resources_count;

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
    if (!updateTerrainUniformBufferData()) {
        return false;
    }
    if (!updateObjectUniformBufferData()) {
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
            m_vulkan_tutorial21_parameters.getFinishedRenderingSemaphores()
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

void Tutorial21::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

bool Tutorial21::childOnWindowSizeChanged() {
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
    if (!createTerrainTexture()) {
        return false;
    }
    if (!createTankTexture()) {
        return false;
    }
    if (!createSkyboxTexture()) {
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
    if (!createTerrainVertexBuffer()) {
        return false;
    }
    if (!createTankVertexBuffers()) {
        return false;
    }
    return createSkyboxBuffers();
}

void Tutorial21::childClear() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return;
    }
    vkDeviceWaitIdle(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial21_parameters.getRenderingResources();
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
                    m_vulkan_tutorial21_parameters.getVkCommandPool(),
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
            m_vulkan_tutorial21_parameters.getFinishedRenderingSemaphores();
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (finished_rendering_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(), finished_rendering_semaphores[i], nullptr);
        }
    }
    finished_rendering_semaphores.clear();

    if (m_vulkan_tutorial21_parameters.getVkCommandPool() != VK_NULL_HANDLE) {
        vkDestroyCommandPool(
                getVkDevice(),
                m_vulkan_tutorial21_parameters.getVkCommandPool(),
                nullptr);
        m_vulkan_tutorial21_parameters.setVkCommandPool(VK_NULL_HANDLE);
    }

    destroyBuffer(
            m_vulkan_tutorial21_parameters.getTerrainVertexBufferParameters());
    std::array<BufferParameters, kTankPartCount>& tank_vertex_buffers =
            m_vulkan_tutorial21_parameters.getTankVertexBufferParameters();
    for (BufferParameters& buffer : tank_vertex_buffers) {
        destroyBuffer(buffer);
    }
    destroyBuffer(
            m_vulkan_tutorial21_parameters.getSkyboxVertexBufferParameters());
    destroyBuffer(
            m_vulkan_tutorial21_parameters.getSkyboxIndexBufferParameters());
    destroyBuffer(m_vulkan_tutorial21_parameters.getStagingBufferParameters());

    if (m_vulkan_tutorial21_parameters.getVkTerrainGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial21_parameters.getVkTerrainGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial21_parameters.setVkTerrainGraphicsPipeline(
                VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial21_parameters.getVkObjectGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial21_parameters.getVkObjectGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial21_parameters.setVkObjectGraphicsPipeline(
                VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial21_parameters.getVkSkyboxGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial21_parameters.getVkSkyboxGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial21_parameters.setVkSkyboxGraphicsPipeline(
                VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial21_parameters.getVkTerrainPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial21_parameters.getVkTerrainPipelineLayout(),
                nullptr);
        m_vulkan_tutorial21_parameters.setVkTerrainPipelineLayout(
                VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial21_parameters.getVkObjectPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial21_parameters.getVkObjectPipelineLayout(),
                nullptr);
        m_vulkan_tutorial21_parameters.setVkObjectPipelineLayout(
                VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial21_parameters.getVkRenderPass() != VK_NULL_HANDLE) {
        vkDestroyRenderPass(
                getVkDevice(),
                m_vulkan_tutorial21_parameters.getVkRenderPass(),
                nullptr);
        m_vulkan_tutorial21_parameters.setVkRenderPass(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial21_parameters.getVkDescriptorPool() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(
                getVkDevice(),
                m_vulkan_tutorial21_parameters.getVkDescriptorPool(),
                nullptr);
        m_vulkan_tutorial21_parameters.setVkDescriptorPool(VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial21_parameters.getVkTerrainDescriptorSetLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(
                getVkDevice(),
                m_vulkan_tutorial21_parameters
                        .getVkTerrainDescriptorSetLayout(),
                nullptr);
        m_vulkan_tutorial21_parameters.setVkTerrainDescriptorSetLayout(
                VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial21_parameters.getVkObjectDescriptorSetLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(
                getVkDevice(),
                m_vulkan_tutorial21_parameters.getVkObjectDescriptorSetLayout(),
                nullptr);
        m_vulkan_tutorial21_parameters.setVkObjectDescriptorSetLayout(
                VK_NULL_HANDLE);
    }

    destroyBuffer(
            m_vulkan_tutorial21_parameters.getTerrainUniformBufferParameters());
    destroyBuffer(
            m_vulkan_tutorial21_parameters.getObjectUniformBufferParameters());

    ImageParameters& terrain_image =
            m_vulkan_tutorial21_parameters.getTerrainImageParameters();
    if (terrain_image.getVkSampler() != VK_NULL_HANDLE) {
        vkDestroySampler(getVkDevice(), terrain_image.getVkSampler(), nullptr);
        terrain_image.setVkSampler(VK_NULL_HANDLE);
    }
    if (terrain_image.getVkImageView() != VK_NULL_HANDLE) {
        vkDestroyImageView(
                getVkDevice(), terrain_image.getVkImageView(), nullptr);
        terrain_image.setVkImageView(VK_NULL_HANDLE);
    }
    if (terrain_image.getVkImage() != VK_NULL_HANDLE) {
        vkDestroyImage(getVkDevice(), terrain_image.getVkImage(), nullptr);
        terrain_image.setVkImage(VK_NULL_HANDLE);
    }
    if (terrain_image.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(
                getVkDevice(), terrain_image.getVkDeviceMemory(), nullptr);
        terrain_image.setVkDeviceMemory(VK_NULL_HANDLE);
    }

    ImageParameters& tank_image =
            m_vulkan_tutorial21_parameters.getTankImageParameters();
    if (tank_image.getVkSampler() != VK_NULL_HANDLE) {
        vkDestroySampler(getVkDevice(), tank_image.getVkSampler(), nullptr);
        tank_image.setVkSampler(VK_NULL_HANDLE);
    }
    if (tank_image.getVkImageView() != VK_NULL_HANDLE) {
        vkDestroyImageView(getVkDevice(), tank_image.getVkImageView(), nullptr);
        tank_image.setVkImageView(VK_NULL_HANDLE);
    }
    if (tank_image.getVkImage() != VK_NULL_HANDLE) {
        vkDestroyImage(getVkDevice(), tank_image.getVkImage(), nullptr);
        tank_image.setVkImage(VK_NULL_HANDLE);
    }
    if (tank_image.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), tank_image.getVkDeviceMemory(), nullptr);
        tank_image.setVkDeviceMemory(VK_NULL_HANDLE);
    }

    ImageParameters& skybox_image =
            m_vulkan_tutorial21_parameters.getSkyboxImageParameters();
    if (skybox_image.getVkSampler() != VK_NULL_HANDLE) {
        vkDestroySampler(getVkDevice(), skybox_image.getVkSampler(), nullptr);
        skybox_image.setVkSampler(VK_NULL_HANDLE);
    }
    if (skybox_image.getVkImageView() != VK_NULL_HANDLE) {
        vkDestroyImageView(
                getVkDevice(), skybox_image.getVkImageView(), nullptr);
        skybox_image.setVkImageView(VK_NULL_HANDLE);
    }
    if (skybox_image.getVkImage() != VK_NULL_HANDLE) {
        vkDestroyImage(getVkDevice(), skybox_image.getVkImage(), nullptr);
        skybox_image.setVkImage(VK_NULL_HANDLE);
    }
    if (skybox_image.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), skybox_image.getVkDeviceMemory(), nullptr);
        skybox_image.setVkDeviceMemory(VK_NULL_HANDLE);
    }

    destroyDepthResources();
}

}  // namespace vulkan_graphix
