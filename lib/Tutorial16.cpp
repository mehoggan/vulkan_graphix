#include "vulkan_graphix/Tutorial16.h"

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <cstring>

#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
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
    static Math::Mat4<float> const basis(Math::Vec4<float>(0.0f, 0.0f, 1.0f, 0.0f),
                                         Math::Vec4<float>(0.0f, 1.0f, 0.0f, 0.0f),
                                         Math::Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f),
                                         Math::Vec4<float>(0.0f, 0.0f, 0.0f, 1.0f));
    return basis;
}

Math::Mat4<float> getPartModelMatrix(Math::Vec3<float> const& offset) {
    return glm::translate(Math::Mat4<float>(1.0f), offset) * getPartBasis() *
           glm::scale(Math::Mat4<float>(1.0f),
                      Math::Vec3<float>(c_part_scale));
}
}  // namespace

// ************************************************************ //
// VulkanTutorial16Parameters                                   //
// ************************************************************ //
VulkanTutorial16Parameters::VulkanTutorial16Parameters()
        : m_vk_render_pass(VK_NULL_HANDLE)
        , m_vk_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_graphics_pipeline(VK_NULL_HANDLE)
        , m_body_vertex_count(0)
        , m_head_vertex_count(0)
        , m_turret_vertex_count(0)
        , m_vk_command_pool(VK_NULL_HANDLE)
        , m_rendering_resources(resources_count) {}

const VkRenderPass& VulkanTutorial16Parameters::getVkRenderPass() const {
    return m_vk_render_pass;
}
VkRenderPass& VulkanTutorial16Parameters::getVkRenderPass() {
    return m_vk_render_pass;
}
void VulkanTutorial16Parameters::setVkRenderPass(
        const VkRenderPass& vk_render_pass) {
    m_vk_render_pass = vk_render_pass;
}

const ImageParameters& VulkanTutorial16Parameters::getImageParameters()
        const {
    return m_image_parameters;
}
ImageParameters& VulkanTutorial16Parameters::getImageParameters() {
    return m_image_parameters;
}
void VulkanTutorial16Parameters::setImageParameters(
        const ImageParameters& image_parameters) {
    m_image_parameters = image_parameters;
}

const BufferParameters&
VulkanTutorial16Parameters::getUniformBufferParameters() const {
    return m_uniform_buffer;
}
BufferParameters& VulkanTutorial16Parameters::getUniformBufferParameters() {
    return m_uniform_buffer;
}
void VulkanTutorial16Parameters::setUniformBufferParameters(
        const BufferParameters& uniform_buffer) {
    m_uniform_buffer = uniform_buffer;
}

const DescriptorSetParameters&
VulkanTutorial16Parameters::getDescriptorSetParameters() const {
    return m_descriptor_set_parameters;
}
DescriptorSetParameters&
VulkanTutorial16Parameters::getDescriptorSetParameters() {
    return m_descriptor_set_parameters;
}
void VulkanTutorial16Parameters::setDescriptorSetParameters(
        const DescriptorSetParameters& descriptor_set_parameters) {
    m_descriptor_set_parameters = descriptor_set_parameters;
}

const VkPipelineLayout& VulkanTutorial16Parameters::getVkPipelineLayout()
        const {
    return m_vk_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial16Parameters::getVkPipelineLayout() {
    return m_vk_pipeline_layout;
}
void VulkanTutorial16Parameters::setVkPipelineLayout(
        const VkPipelineLayout& vk_pipeline_layout) {
    m_vk_pipeline_layout = vk_pipeline_layout;
}

const VkPipeline& VulkanTutorial16Parameters::getVkGraphicsPipeline() const {
    return m_vk_graphics_pipeline;
}
VkPipeline& VulkanTutorial16Parameters::getVkGraphicsPipeline() {
    return m_vk_graphics_pipeline;
}
void VulkanTutorial16Parameters::setVkGraphicsPipeline(
        const VkPipeline& vk_graphics_pipeline) {
    m_vk_graphics_pipeline = vk_graphics_pipeline;
}

const BufferParameters&
VulkanTutorial16Parameters::getBodyVertexBufferParameters() const {
    return m_body_vertex_buffer;
}
BufferParameters& VulkanTutorial16Parameters::getBodyVertexBufferParameters() {
    return m_body_vertex_buffer;
}
void VulkanTutorial16Parameters::setBodyVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_body_vertex_buffer = vertex_buffer;
}
std::uint32_t VulkanTutorial16Parameters::getBodyVertexCount() const {
    return m_body_vertex_count;
}
void VulkanTutorial16Parameters::setBodyVertexCount(
        std::uint32_t vertex_count) {
    m_body_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial16Parameters::getHeadVertexBufferParameters() const {
    return m_head_vertex_buffer;
}
BufferParameters& VulkanTutorial16Parameters::getHeadVertexBufferParameters() {
    return m_head_vertex_buffer;
}
void VulkanTutorial16Parameters::setHeadVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_head_vertex_buffer = vertex_buffer;
}
std::uint32_t VulkanTutorial16Parameters::getHeadVertexCount() const {
    return m_head_vertex_count;
}
void VulkanTutorial16Parameters::setHeadVertexCount(
        std::uint32_t vertex_count) {
    m_head_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial16Parameters::getTurretVertexBufferParameters() const {
    return m_turret_vertex_buffer;
}
BufferParameters&
VulkanTutorial16Parameters::getTurretVertexBufferParameters() {
    return m_turret_vertex_buffer;
}
void VulkanTutorial16Parameters::setTurretVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_turret_vertex_buffer = vertex_buffer;
}
std::uint32_t VulkanTutorial16Parameters::getTurretVertexCount() const {
    return m_turret_vertex_count;
}
void VulkanTutorial16Parameters::setTurretVertexCount(
        std::uint32_t vertex_count) {
    m_turret_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial16Parameters::getStagingBufferParameters() const {
    return m_staging_buffer;
}
BufferParameters& VulkanTutorial16Parameters::getStagingBufferParameters() {
    return m_staging_buffer;
}
void VulkanTutorial16Parameters::setStagingBufferParameters(
        const BufferParameters& staging_buffer) {
    m_staging_buffer = staging_buffer;
}

const VkCommandPool& VulkanTutorial16Parameters::getVkCommandPool() const {
    return m_vk_command_pool;
}
VkCommandPool& VulkanTutorial16Parameters::getVkCommandPool() {
    return m_vk_command_pool;
}
void VulkanTutorial16Parameters::setVkCommandPool(
        const VkCommandPool& vk_command_pool) {
    m_vk_command_pool = vk_command_pool;
}

const std::vector<RenderingResourceParameters>&
VulkanTutorial16Parameters::getRenderingResources() const {
    return m_rendering_resources;
}
std::vector<RenderingResourceParameters>&
VulkanTutorial16Parameters::getRenderingResources() {
    return m_rendering_resources;
}
void VulkanTutorial16Parameters::setRenderingResources(
        const std::vector<RenderingResourceParameters>& rendering_resources) {
    m_rendering_resources = rendering_resources;
}

const std::vector<VkSemaphore>&
VulkanTutorial16Parameters::getFinishedRenderingSemaphores() const {
    return m_finished_rendering_semaphores;
}
std::vector<VkSemaphore>&
VulkanTutorial16Parameters::getFinishedRenderingSemaphores() {
    return m_finished_rendering_semaphores;
}
void VulkanTutorial16Parameters::setFinishedRenderingSemaphores(
        const std::vector<VkSemaphore>& finished_rendering_semaphores) {
    m_finished_rendering_semaphores = finished_rendering_semaphores;
}

// ************************************************************ //
// Tutorial16                                                   //
// ************************************************************ //
Tutorial16::Tutorial16()
        // The assembled tank spans roughly [-210,210]x[-19,120]x[-150,150]
        // in world units (TankB's own offsets/scale are hundreds of units,
        // unlike every other OrbitCamera tutorial's single/double-digit
        // world) - 700 frames the whole thing; OrbitCamera's own zoom-out
        // clamp (40, see OrbitCamera.cpp) is far smaller than that, same
        // mismatch Tutorial12's terrain already runs into (it sets its own
        // initial distance to that same 40 clamp) - scrolling out from here
        // will clamp closer than this initial framing, but scrolling in to
        // inspect the mesh works over the its full range.
        : m_camera(0.6f, 0.15f, 400.0f) {}

Tutorial16::~Tutorial16() { childClear(); }

void Tutorial16::onMouseButton(int button,
                               bool pressed,
                               int pos_x,
                               int pos_y) {
    m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial16::onMouseMove(int pos_x, int pos_y) {
    m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial16::createCommandPool(std::uint32_t queue_family_index,
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

bool Tutorial16::allocateCommandBuffers(VkCommandPool pool,
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

bool Tutorial16::createCommandBuffers() {
    if (!createCommandPool(
                getGraphicsQueueParameters().getFamilyIndex(),
                &m_vulkan_tutorial16_parameters.getVkCommandPool())) {
        return false;
    }

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial16_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!allocateCommandBuffers(
                    m_vulkan_tutorial16_parameters.getVkCommandPool(),
                    1,
                    &rendering_resources[i].getVkCommandBuffer())) {
            return false;
        }
    }
    return true;
}

bool Tutorial16::createSemaphores() {
    VkSemaphoreCreateInfo semaphore_create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0};

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial16_parameters.getRenderingResources();
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
            m_vulkan_tutorial16_parameters.getFinishedRenderingSemaphores();
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

bool Tutorial16::createFences() {
    VkFenceCreateInfo fence_create_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial16_parameters.getRenderingResources();
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

bool Tutorial16::createRenderingResources() {
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

bool Tutorial16::allocateBufferMemory(VkBuffer buffer,
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

bool Tutorial16::createBuffer(VkBufferUsageFlags usage,
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

bool Tutorial16::createStagingBuffer() {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial16_parameters.getStagingBufferParameters();
    // TestImage.raw decodes to 1024x1024 RGBA = 4,194,304 bytes - the
    // larger of this tutorial's uses (the biggest single mesh, the body
    // at 1080 vertices * 24 bytes, is trivial next to it). Sized with
    // headroom rather than copied from another tutorial's constant - see
    // copyTextureData()/copyBufferData()'s size guards below for what
    // silently overflowing this buffer did to the original migration
    // pilots this project's earlier tutorials are ported from.
    staging_buffer.setSize(6 * 1024 * 1024);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

bool Tutorial16::createImage(std::uint32_t width,
                             std::uint32_t height,
                             VkImage* image) {
    VkImageCreateInfo image_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_R8G8B8A8_UNORM,
            .extent = {.width = width, .height = height, .depth = 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                     VK_IMAGE_USAGE_SAMPLED_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

    return vkCreateImage(getVkDevice(), &image_create_info, nullptr, image) ==
           VK_SUCCESS;
}

bool Tutorial16::allocateImageMemory(VkImage image,
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

bool Tutorial16::createImageView() {
    ImageParameters& image_parameters =
            m_vulkan_tutorial16_parameters.getImageParameters();
    VkImageViewCreateInfo image_view_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image_parameters.getVkImage(),
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_R8G8B8A8_UNORM,
            .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                           .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                           .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                           .a = VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                 .baseMipLevel = 0,
                                 .levelCount = 1,
                                 .baseArrayLayer = 0,
                                 .layerCount = 1}};

    return vkCreateImageView(getVkDevice(),
                             &image_view_create_info,
                             nullptr,
                             &image_parameters.getVkImageView()) ==
           VK_SUCCESS;
}

bool Tutorial16::createSampler(VkSampler* sampler) {
    VkSamplerCreateInfo sampler_create_info = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
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

bool Tutorial16::copyTextureData(char* texture_data,
                                 std::uint32_t data_size,
                                 std::uint32_t width,
                                 std::uint32_t height) {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial16_parameters.getStagingBufferParameters();
    ImageParameters& image_parameters =
            m_vulkan_tutorial16_parameters.getImageParameters();

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
            m_vulkan_tutorial16_parameters.getRenderingResources()[0]
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
             .image = image_parameters.getVkImage(),
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
                           image_parameters.getVkImage(),
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
            .image = image_parameters.getVkImage(),
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

bool Tutorial16::createTexture() {
    // Shared by all three parts - TankB's own constructor loads
    // "TestImage.raw" for its body/head/turret VBOShaderLibrary
    // instances alike.
    std::vector<char> texture_data =
            Tools::getRawImageData("TestImage.raw", 1024, 1024);
    if (texture_data.empty()) {
        return false;
    }

    ImageParameters& image_parameters =
            m_vulkan_tutorial16_parameters.getImageParameters();

    VkImage vk_image;
    if (!createImage(1024, 1024, &vk_image)) {
        Logging::error(LOG_TAG, "Could not create image!");
        return false;
    }
    image_parameters.setVkImage(vk_image);

    VkDeviceMemory vk_device_memory;
    if (!allocateImageMemory(image_parameters.getVkImage(),
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &vk_device_memory)) {
        Logging::error(LOG_TAG, "Could not allocate memory for image!");
        return false;
    }
    image_parameters.setVkDeviceMemory(vk_device_memory);

    if (vkBindImageMemory(getVkDevice(),
                          image_parameters.getVkImage(),
                          image_parameters.getVkDeviceMemory(),
                          0) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not bind memory to an image!");
        return false;
    }

    if (!createImageView()) {
        Logging::error(LOG_TAG, "Could not create image view!");
        return false;
    }

    VkSampler vk_sampler;
    if (!createSampler(&vk_sampler)) {
        Logging::error(LOG_TAG, "Could not create sampler!");
        return false;
    }
    image_parameters.setVkSampler(vk_sampler);

    if (!copyTextureData(texture_data.data(),
                         static_cast<std::uint32_t>(texture_data.size()),
                         1024,
                         1024)) {
        Logging::error(LOG_TAG,
                       "Could not upload texture data to device memory!");
        return false;
    }

    return true;
}

bool Tutorial16::createUniformBuffer() {
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial16_parameters.getUniformBufferParameters();
    uniform_buffer.setSize(sizeof(Tutorial16UniformBufferData));
    // Host-visible/coherent: rewritten every frame as the orbit camera
    // moves, same tradeoff Tutorial10/13 make for the same reason.
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      uniform_buffer)) {
        Logging::error(LOG_TAG, "Could not create uniform buffer!");
        return false;
    }

    return updateUniformBufferData();
}

Tutorial16UniformBufferData Tutorial16::getUniformBufferData() const {
    Tutorial16UniformBufferData data{};
    data.view = glm::lookAt(m_camera.eye(),
                            m_camera.target(),
                            Math::Vec3<float>(0.0f, 1.0f, 0.0f));

    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    // Near/far clip planes sized for this tutorial's much larger world
    // (hundreds of units, see the constructor's own comment) rather than
    // copied from another tutorial's 0.01-10 or 0.1-100 range.
    data.projection = Tools::getPerspectiveProjectionMatrix(
            width / height, 45.0f, 1.0f, 2000.0f);

    return data;
}

bool Tutorial16::updateUniformBufferData() {
    Tutorial16UniformBufferData const uniform_data = getUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial16_parameters.getUniformBufferParameters();

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

bool Tutorial16::createDescriptorSetLayout() {
    // Binding numbers match this tutorial's own shaders (shader.16.vert/
    // frag): binding 0 = sampler in the fragment stage, binding 1 =
    // uniform buffer (view+projection) in the vertex stage - the model
    // matrix travels as a push constant instead (see
    // createPipelineLayout()), not through this descriptor set.
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings = {
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

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount = static_cast<std::uint32_t>(layout_bindings.size()),
            .pBindings = layout_bindings.data()};

    VkDescriptorSetLayout vk_descriptor_set_layout;
    if (vkCreateDescriptorSetLayout(getVkDevice(),
                                    &descriptor_set_layout_create_info,
                                    nullptr,
                                    &vk_descriptor_set_layout) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create descriptor set layout!");
        return false;
    }
    m_vulkan_tutorial16_parameters.getDescriptorSetParameters()
            .setVkDescriptorSetLayout(vk_descriptor_set_layout);

    return true;
}

bool Tutorial16::createDescriptorPool() {
    std::vector<VkDescriptorPoolSize> pool_sizes = {
            {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = 1},
            {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1}};

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .maxSets = 1,
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
    m_vulkan_tutorial16_parameters.getDescriptorSetParameters()
            .setVkDescriptorPool(vk_descriptor_pool);

    return true;
}

bool Tutorial16::allocateDescriptorSet() {
    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial16_parameters.getDescriptorSetParameters();
    VkDescriptorSetLayout vk_descriptor_set_layout =
            descriptor_set.getVkDescriptorSetLayout();
    VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = descriptor_set.getVkDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_descriptor_set_layout};

    VkDescriptorSet vk_descriptor_set;
    if (vkAllocateDescriptorSets(getVkDevice(),
                                 &descriptor_set_allocate_info,
                                 &vk_descriptor_set) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate descriptor set!");
        return false;
    }
    descriptor_set.setVkDescriptorSet(vk_descriptor_set);

    return true;
}

bool Tutorial16::updateDescriptorSet() {
    ImageParameters& image_parameters =
            m_vulkan_tutorial16_parameters.getImageParameters();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial16_parameters.getUniformBufferParameters();
    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial16_parameters.getDescriptorSetParameters();

    VkDescriptorImageInfo image_info = {
            .sampler = image_parameters.getVkSampler(),
            .imageView = image_parameters.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    VkDescriptorBufferInfo buffer_info = {
            .buffer = uniform_buffer.getVkBuffer(),
            .offset = 0,
            .range = uniform_buffer.getSize()};

    std::vector<VkWriteDescriptorSet> descriptor_writes = {
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = descriptor_set.getVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &image_info,
             .pBufferInfo = nullptr,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = descriptor_set.getVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &buffer_info,
             .pTexelBufferView = nullptr}};

    vkUpdateDescriptorSets(
            getVkDevice(),
            static_cast<std::uint32_t>(descriptor_writes.size()),
            descriptor_writes.data(),
            0,
            nullptr);
    return true;
}

bool Tutorial16::createRenderPass() {
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
                &m_vulkan_tutorial16_parameters.getVkRenderPass()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

bool Tutorial16::createPipelineLayout() {
    VkDescriptorSetLayout vk_descriptor_set_layout =
            m_vulkan_tutorial16_parameters.getDescriptorSetParameters()
                    .getVkDescriptorSetLayout();

    VkPushConstantRange push_constant_range = {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(Tutorial16PushConstants)};

    VkPipelineLayoutCreateInfo layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &vk_descriptor_set_layout,
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &push_constant_range};

    if (vkCreatePipelineLayout(
                getVkDevice(),
                &layout_create_info,
                nullptr,
                &m_vulkan_tutorial16_parameters.getVkPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create pipeline layout!");
        return false;
    }

    return true;
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
Tutorial16::createShaderModule(const char* filename) {
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

bool Tutorial16::createPipeline() {
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module = createShaderModule("shader.16.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module = createShaderModule("shader.16.frag.spv");

    if (!vertex_shader_module || !fragment_shader_module) {
        return false;
    }

    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos = {
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_VERTEX_BIT,
             .module = vertex_shader_module.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr},
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
             .module = fragment_shader_module.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr}};

    std::vector<VkVertexInputBindingDescription> vertex_binding_descriptions =
            {{.binding = 0,
              .stride = Tutorial16VertexAttributeTraits::stride,
              .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};

    std::vector<VkVertexInputAttributeDescription>
            vertex_attribute_descriptions = {
                    {.location = 0,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset =
                             offsetof(struct Tutorial16VertexData, position)},
                    {.location = 1,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32_SFLOAT,
                     .offset =
                             offsetof(struct Tutorial16VertexData, texcoord)}};

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = static_cast<std::uint32_t>(
                    vertex_binding_descriptions.size()),
            .pVertexBindingDescriptions = vertex_binding_descriptions.data(),
            .vertexAttributeDescriptionCount = static_cast<std::uint32_t>(
                    vertex_attribute_descriptions.size()),
            .pVertexAttributeDescriptions =
                    vertex_attribute_descriptions.data()};

    // Non-indexed triangle list - matches VBOShaderLibrary::
    // drawClientData()'s own glDrawArrays(GL_TRIANGLES, ...); none of
    // these .ogl files carry index data (see loadPartVertexData()).
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE};

    VkPipelineViewportStateCreateInfo viewport_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = nullptr,
            .scissorCount = 1,
            .pScissors = nullptr};

    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            // NONE: these hand-authored assets' winding order hasn't been
            // verified against this project's usual CCW convention, same
            // reasoning Tutorial11/13 use for their own cullMode.
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f};

    VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE};

    VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
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

    VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &color_blend_attachment_state,
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

    VkGraphicsPipelineCreateInfo pipeline_create_info = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = static_cast<std::uint32_t>(
                    shader_stage_create_infos.size()),
            .pStages = shader_stage_create_infos.data(),
            .pVertexInputState = &vertex_input_state_create_info,
            .pInputAssemblyState = &input_assembly_state_create_info,
            .pTessellationState = nullptr,
            .pViewportState = &viewport_state_create_info,
            .pRasterizationState = &rasterization_state_create_info,
            .pMultisampleState = &multisample_state_create_info,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &color_blend_state_create_info,
            .pDynamicState = &dynamic_state_create_info,
            .layout = m_vulkan_tutorial16_parameters.getVkPipelineLayout(),
            .renderPass = m_vulkan_tutorial16_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};

    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info,
                nullptr,
                &m_vulkan_tutorial16_parameters.getVkGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create graphics pipeline!");
        return false;
    }
    return true;
}

std::vector<Tutorial16VertexData> Tutorial16::loadPartVertexData(
        const char* mesh_filename) const {
    // Normal is parsed by loadOglMeshData() (to stay honest about the
    // file's real layout) but dropped here, same as
    // VBOShaderLibrary::drawClientData()'s real fragment shader
    // effectively does (see Tutorial13.h for the same reasoning).
    std::vector<Tools::OglVertexData> const mesh_data =
            Tools::loadOglMeshData(mesh_filename);
    std::vector<Tutorial16VertexData> vertex_data;
    vertex_data.reserve(mesh_data.size());
    for (Tools::OglVertexData const& vertex : mesh_data) {
        vertex_data.push_back(
                {Math::Vec4<float>(vertex.position, 1.0f), vertex.texcoord});
    }
    return vertex_data;
}

bool Tutorial16::copyBufferData(BufferParameters& destination,
                                const void* data,
                                std::uint32_t data_size,
                                VkAccessFlags dst_access_mask,
                                VkPipelineStageFlags dst_stage_mask) {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial16_parameters.getStagingBufferParameters();

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
            m_vulkan_tutorial16_parameters.getRenderingResources()[0]
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

bool Tutorial16::createPartVertexBuffer(const char* mesh_filename,
                                        BufferParameters& vertex_buffer,
                                        std::uint32_t& vertex_count) {
    std::vector<Tutorial16VertexData> const vertex_data =
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

    return copyBufferData(vertex_buffer,
                          vertex_data.data(),
                          vertex_buffer.getSize(),
                          VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                          VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
}

bool Tutorial16::createVertexBuffers() {
    // getXVertexCount() is by-value only (matches Tutorial13's own
    // getVertexCount()) - createPartVertexBuffer's std::uint32_t&
    // out-param needs a real lvalue, so each count is written into a
    // local first and then stored via the setter.
    std::uint32_t body_vertex_count = 0;
    if (!createPartVertexBuffer(
                "Hellfire_Body.ogl",
                m_vulkan_tutorial16_parameters.getBodyVertexBufferParameters(),
                body_vertex_count)) {
        return false;
    }
    m_vulkan_tutorial16_parameters.setBodyVertexCount(body_vertex_count);

    std::uint32_t head_vertex_count = 0;
    if (!createPartVertexBuffer(
                "Hellfire_Head.ogl",
                m_vulkan_tutorial16_parameters.getHeadVertexBufferParameters(),
                head_vertex_count)) {
        return false;
    }
    m_vulkan_tutorial16_parameters.setHeadVertexCount(head_vertex_count);

    std::uint32_t turret_vertex_count = 0;
    if (!createPartVertexBuffer(
                "Hellfire_Turret.ogl",
                m_vulkan_tutorial16_parameters
                        .getTurretVertexBufferParameters(),
                turret_vertex_count)) {
        return false;
    }
    m_vulkan_tutorial16_parameters.setTurretVertexCount(turret_vertex_count);

    return true;
}

Math::Mat4<float> Tutorial16::getBodyModelMatrix() const {
    return getPartModelMatrix(c_body_offset);
}

Math::Mat4<float> Tutorial16::getHeadModelMatrix() const {
    return getPartModelMatrix(c_head_offset);
}

Math::Mat4<float> Tutorial16::getTurretModelMatrix() const {
    return getPartModelMatrix(c_turret_offset);
}

bool Tutorial16::createFramebuffer(VkFramebuffer& framebuffer,
                                   VkImageView image_view) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(getVkDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_vulkan_tutorial16_parameters.getVkRenderPass(),
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

bool Tutorial16::prepareFrame(VkCommandBuffer command_buffer,
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
            .renderPass = m_vulkan_tutorial16_parameters.getVkRenderPass(),
            .framebuffer = framebuffer,
            .renderArea = {.offset = {.x = 0, .y = 0},
                           .extent = getSwapchainParameters().getVkExtent2d()},
            .clearValueCount = 1,
            .pClearValues = &clear_value};

    vkCmdBeginRenderPass(command_buffer,
                         &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial16_parameters.getVkGraphicsPipeline());

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

    VkDescriptorSet vk_descriptor_set =
            m_vulkan_tutorial16_parameters.getDescriptorSetParameters()
                    .getVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial16_parameters.getVkPipelineLayout(),
            0,
            1,
            &vk_descriptor_set,
            0,
            nullptr);

    // Each part is bound, given its own model-matrix push constant, and
    // drawn independently - matches Tank::draw()'s own three separate
    // glPushMatrix()/glMultMatrixf()/drawClientData()/glPopMatrix()
    // blocks (see Tutorial16.h's top comment).
    VkDeviceSize offset = 0;

    Tutorial16PushConstants body_push_constants{getBodyModelMatrix()};
    vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_vulkan_tutorial16_parameters.getBodyVertexBufferParameters()
                     .getVkBuffer(),
            &offset);
    vkCmdPushConstants(command_buffer,
                       m_vulkan_tutorial16_parameters.getVkPipelineLayout(),
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(Tutorial16PushConstants),
                       &body_push_constants);
    vkCmdDraw(command_buffer,
             m_vulkan_tutorial16_parameters.getBodyVertexCount(),
             1,
             0,
             0);

    Tutorial16PushConstants head_push_constants{getHeadModelMatrix()};
    vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_vulkan_tutorial16_parameters.getHeadVertexBufferParameters()
                     .getVkBuffer(),
            &offset);
    vkCmdPushConstants(command_buffer,
                       m_vulkan_tutorial16_parameters.getVkPipelineLayout(),
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(Tutorial16PushConstants),
                       &head_push_constants);
    vkCmdDraw(command_buffer,
             m_vulkan_tutorial16_parameters.getHeadVertexCount(),
             1,
             0,
             0);

    Tutorial16PushConstants turret_push_constants{getTurretModelMatrix()};
    vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_vulkan_tutorial16_parameters.getTurretVertexBufferParameters()
                     .getVkBuffer(),
            &offset);
    vkCmdPushConstants(command_buffer,
                       m_vulkan_tutorial16_parameters.getVkPipelineLayout(),
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(Tutorial16PushConstants),
                       &turret_push_constants);
    vkCmdDraw(command_buffer,
             m_vulkan_tutorial16_parameters.getTurretVertexCount(),
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

bool Tutorial16::draw() {
    static std::size_t resource_index = 0;
    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial16_parameters.getRenderingResources();
    RenderingResourceParameters& current_rendering_resource =
            rendering_resources[resource_index];
    VkSwapchainKHR swap_chain = getSwapchainParameters().getVkSwapchainKhr();
    std::uint32_t image_index;

    resource_index = (resource_index + 1) %
                     VulkanTutorial16Parameters::resources_count;

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
    if (!updateUniformBufferData()) {
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
            m_vulkan_tutorial16_parameters.getFinishedRenderingSemaphores()
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

void Tutorial16::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

bool Tutorial16::childOnWindowSizeChanged() {
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
    if (!createTexture()) {
        return false;
    }
    if (!createUniformBuffer()) {
        return false;
    }
    if (!createDescriptorSetLayout()) {
        return false;
    }
    if (!createDescriptorPool()) {
        return false;
    }
    if (!allocateDescriptorSet()) {
        return false;
    }
    if (!updateDescriptorSet()) {
        return false;
    }
    if (!createRenderPass()) {
        return false;
    }
    if (!createPipelineLayout()) {
        return false;
    }
    if (!createPipeline()) {
        return false;
    }
    return createVertexBuffers();
}

void Tutorial16::childClear() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return;
    }
    vkDeviceWaitIdle(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial16_parameters.getRenderingResources();
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
                    m_vulkan_tutorial16_parameters.getVkCommandPool(),
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
            m_vulkan_tutorial16_parameters.getFinishedRenderingSemaphores();
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (finished_rendering_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(), finished_rendering_semaphores[i], nullptr);
        }
    }
    finished_rendering_semaphores.clear();

    if (m_vulkan_tutorial16_parameters.getVkCommandPool() != VK_NULL_HANDLE) {
        vkDestroyCommandPool(
                getVkDevice(),
                m_vulkan_tutorial16_parameters.getVkCommandPool(),
                nullptr);
        m_vulkan_tutorial16_parameters.setVkCommandPool(VK_NULL_HANDLE);
    }

    destroyBuffer(
            m_vulkan_tutorial16_parameters.getBodyVertexBufferParameters());
    destroyBuffer(
            m_vulkan_tutorial16_parameters.getHeadVertexBufferParameters());
    destroyBuffer(
            m_vulkan_tutorial16_parameters.getTurretVertexBufferParameters());
    destroyBuffer(m_vulkan_tutorial16_parameters.getStagingBufferParameters());

    if (m_vulkan_tutorial16_parameters.getVkGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial16_parameters.getVkGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial16_parameters.setVkGraphicsPipeline(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial16_parameters.getVkPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial16_parameters.getVkPipelineLayout(),
                nullptr);
        m_vulkan_tutorial16_parameters.setVkPipelineLayout(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial16_parameters.getVkRenderPass() != VK_NULL_HANDLE) {
        vkDestroyRenderPass(
                getVkDevice(),
                m_vulkan_tutorial16_parameters.getVkRenderPass(),
                nullptr);
        m_vulkan_tutorial16_parameters.setVkRenderPass(VK_NULL_HANDLE);
    }

    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial16_parameters.getDescriptorSetParameters();
    if (descriptor_set.getVkDescriptorPool() != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(
                getVkDevice(), descriptor_set.getVkDescriptorPool(), nullptr);
        descriptor_set.setVkDescriptorPool(VK_NULL_HANDLE);
    }
    if (descriptor_set.getVkDescriptorSetLayout() != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(getVkDevice(),
                                     descriptor_set.getVkDescriptorSetLayout(),
                                     nullptr);
        descriptor_set.setVkDescriptorSetLayout(VK_NULL_HANDLE);
    }

    destroyBuffer(m_vulkan_tutorial16_parameters.getUniformBufferParameters());

    ImageParameters& image_parameters =
            m_vulkan_tutorial16_parameters.getImageParameters();
    if (image_parameters.getVkSampler() != VK_NULL_HANDLE) {
        vkDestroySampler(
                getVkDevice(), image_parameters.getVkSampler(), nullptr);
        image_parameters.setVkSampler(VK_NULL_HANDLE);
    }
    if (image_parameters.getVkImageView() != VK_NULL_HANDLE) {
        vkDestroyImageView(
                getVkDevice(), image_parameters.getVkImageView(), nullptr);
        image_parameters.setVkImageView(VK_NULL_HANDLE);
    }
    if (image_parameters.getVkImage() != VK_NULL_HANDLE) {
        vkDestroyImage(getVkDevice(), image_parameters.getVkImage(), nullptr);
        image_parameters.setVkImage(VK_NULL_HANDLE);
    }
    if (image_parameters.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(
                getVkDevice(), image_parameters.getVkDeviceMemory(), nullptr);
        image_parameters.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

}  // namespace vulkan_graphix
