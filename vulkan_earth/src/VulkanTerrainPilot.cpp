#include "VulkanTerrainPilot.h"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
constexpr VkFormat c_depth_format = VK_FORMAT_D32_SFLOAT;
constexpr VkFormat c_texture_format = VK_FORMAT_R8G8B8A8_UNORM;
}  // namespace

VulkanTerrainPilot::VulkanTerrainPilot()
        : vk_render_pass_(VK_NULL_HANDLE)
        , vk_pipeline_layout_(VK_NULL_HANDLE)
        , vk_graphics_pipeline_(VK_NULL_HANDLE)
        , vertex_count_(0)
        , vk_command_pool_(VK_NULL_HANDLE)
        , rendering_resources_(resources_count_)
        // Pulled back to OrbitCamera's max distance (40) and pitched down
        // less steeply than a first attempt at 35/0.5 - that framing filled
        // the whole view with one close-up rock face; this one shows the
        // grid's full silhouette with sky around it.
        , camera_(0.5f, 0.35f, 40.0f) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

VulkanTerrainPilot::~VulkanTerrainPilot() { childClear(); }

bool VulkanTerrainPilot::createCommandPool(std::uint32_t queue_family_index,
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

bool VulkanTerrainPilot::allocateCommandBuffers(
        VkCommandPool pool,
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

bool VulkanTerrainPilot::createCommandBuffers() {
    if (!createCommandPool(getGraphicsQueueParameters().getFamilyIndex(),
                           &vk_command_pool_)) {
        return false;
    }

    for (std::size_t i = 0; i < rendering_resources_.size(); ++i) {
        if (!allocateCommandBuffers(
                    vk_command_pool_,
                    1,
                    &rendering_resources_[i].getVkCommandBuffer())) {
            return false;
        }
    }
    return true;
}

bool VulkanTerrainPilot::createSemaphores() {
    VkSemaphoreCreateInfo semaphore_create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0};

    for (std::size_t i = 0; i < rendering_resources_.size(); ++i) {
        if (vkCreateSemaphore(
                    getVkDevice(),
                    &semaphore_create_info,
                    nullptr,
                    &rendering_resources_[i].getImageAvailableVkSemaphore()) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    finished_rendering_semaphores_.assign(
            getSwapchainParameters().getImageParameters().size(),
            VK_NULL_HANDLE);
    for (std::size_t i = 0; i < finished_rendering_semaphores_.size(); ++i) {
        if (vkCreateSemaphore(getVkDevice(),
                              &semaphore_create_info,
                              nullptr,
                              &finished_rendering_semaphores_[i]) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    return true;
}

bool VulkanTerrainPilot::createFences() {
    VkFenceCreateInfo fence_create_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    for (std::size_t i = 0; i < rendering_resources_.size(); ++i) {
        if (vkCreateFence(getVkDevice(),
                          &fence_create_info,
                          nullptr,
                          &rendering_resources_[i].getVkFence()) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not create a fence!");
            return false;
        }
    }
    return true;
}

bool VulkanTerrainPilot::createRenderingResources() {
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

bool VulkanTerrainPilot::allocateBufferMemory(VkBuffer buffer,
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

bool VulkanTerrainPilot::createBuffer(VkBufferUsageFlags usage,
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

bool VulkanTerrainPilot::createStagingBuffer() {
    // Rocky.raw decodes to 2048x2048 RGBA = 16,777,216 bytes once expanded
    // by getRawImageData() - by far the larger of this pilot's two staging
    // uses (the ~208KB vertex buffer is trivial next to it). Sized with
    // headroom rather than copied from another pilot's constant - see
    // copyTextureData()/copyBufferData()'s size guards below for what
    // silently overflowing this buffer did to the skybox pilot.
    staging_buffer_.setSize(20 * 1024 * 1024);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer_)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

bool VulkanTerrainPilot::createImage(std::uint32_t width,
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

bool VulkanTerrainPilot::allocateImageMemory(VkImage image,
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

bool VulkanTerrainPilot::createImageView(VkImage image,
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

    return vkCreateImageView(
                   getVkDevice(), &image_view_create_info, nullptr, image_view) ==
           VK_SUCCESS;
}

bool VulkanTerrainPilot::createSampler(VkSamplerAddressMode address_mode,
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

bool VulkanTerrainPilot::copyTextureData(char* texture_data,
                                         std::uint32_t data_size,
                                         std::uint32_t width,
                                         std::uint32_t height) {
    if (data_size > staging_buffer_.getSize()) {
        Logging::error(LOG_TAG,
                       "Texture data (",
                       data_size,
                       " bytes) does not fit in the staging buffer (",
                       staging_buffer_.getSize(),
                       " bytes)!");
        return false;
    }

    void* staging_buffer_memory_pointer;
    if (vkMapMemory(getVkDevice(),
                    staging_buffer_.getVkDeviceMemory(),
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
            .memory = staging_buffer_.getVkDeviceMemory(),
            .offset = 0,
            .size = VK_WHOLE_SIZE};
    vkFlushMappedMemoryRanges(getVkDevice(), 1, &flush_range);

    vkUnmapMemory(getVkDevice(), staging_buffer_.getVkDeviceMemory());

    VkCommandBufferBeginInfo command_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr};

    VkCommandBuffer command_buffer =
            rendering_resources_[0].getVkCommandBuffer();

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
             .image = texture_image_parameters_.getVkImage(),
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
                           staging_buffer_.getVkBuffer(),
                           texture_image_parameters_.getVkImage(),
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
            .image = texture_image_parameters_.getVkImage(),
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

bool VulkanTerrainPilot::createDepthResources() {
    std::uint32_t const width =
            getSwapchainParameters().getVkExtent2d().width;
    std::uint32_t const height =
            getSwapchainParameters().getVkExtent2d().height;

    VkImage vk_image;
    if (!createImage(width,
                     height,
                     c_depth_format,
                     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                     &vk_image)) {
        Logging::error(LOG_TAG, "Could not create depth image!");
        return false;
    }
    depth_image_parameters_.setVkImage(vk_image);

    VkDeviceMemory vk_device_memory;
    if (!allocateImageMemory(depth_image_parameters_.getVkImage(),
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &vk_device_memory)) {
        Logging::error(LOG_TAG, "Could not allocate memory for depth image!");
        return false;
    }
    depth_image_parameters_.setVkDeviceMemory(vk_device_memory);

    if (vkBindImageMemory(getVkDevice(),
                          depth_image_parameters_.getVkImage(),
                          depth_image_parameters_.getVkDeviceMemory(),
                          0) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not bind memory to depth image!");
        return false;
    }

    VkImageView vk_image_view;
    if (!createImageView(depth_image_parameters_.getVkImage(),
                         c_depth_format,
                         VK_IMAGE_ASPECT_DEPTH_BIT,
                         &vk_image_view)) {
        Logging::error(LOG_TAG, "Could not create depth image view!");
        return false;
    }
    depth_image_parameters_.setVkImageView(vk_image_view);

    return true;
}

bool VulkanTerrainPilot::destroyDepthResources() {
    if (depth_image_parameters_.getVkImageView() != VK_NULL_HANDLE) {
        vkDestroyImageView(getVkDevice(),
                           depth_image_parameters_.getVkImageView(),
                           nullptr);
        depth_image_parameters_.setVkImageView(VK_NULL_HANDLE);
    }
    if (depth_image_parameters_.getVkImage() != VK_NULL_HANDLE) {
        vkDestroyImage(
                getVkDevice(), depth_image_parameters_.getVkImage(), nullptr);
        depth_image_parameters_.setVkImage(VK_NULL_HANDLE);
    }
    if (depth_image_parameters_.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(),
                    depth_image_parameters_.getVkDeviceMemory(),
                    nullptr);
        depth_image_parameters_.setVkDeviceMemory(VK_NULL_HANDLE);
    }
    return true;
}

bool VulkanTerrainPilot::createTexture() {
    std::vector<char> texture_data =
            Tools::getRawImageData("Rocky.raw", 2048, 2048);
    if (texture_data.empty()) {
        return false;
    }

    VkImage vk_image;
    if (!createImage(2048,
                     2048,
                     c_texture_format,
                     VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                             VK_IMAGE_USAGE_SAMPLED_BIT,
                     &vk_image)) {
        Logging::error(LOG_TAG, "Could not create image!");
        return false;
    }
    texture_image_parameters_.setVkImage(vk_image);

    VkDeviceMemory vk_device_memory;
    if (!allocateImageMemory(texture_image_parameters_.getVkImage(),
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &vk_device_memory)) {
        Logging::error(LOG_TAG, "Could not allocate memory for image!");
        return false;
    }
    texture_image_parameters_.setVkDeviceMemory(vk_device_memory);

    if (vkBindImageMemory(getVkDevice(),
                          texture_image_parameters_.getVkImage(),
                          texture_image_parameters_.getVkDeviceMemory(),
                          0) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not bind memory to an image!");
        return false;
    }

    VkImageView vk_image_view;
    if (!createImageView(texture_image_parameters_.getVkImage(),
                         c_texture_format,
                         VK_IMAGE_ASPECT_COLOR_BIT,
                         &vk_image_view)) {
        Logging::error(LOG_TAG, "Could not create image view!");
        return false;
    }
    texture_image_parameters_.setVkImageView(vk_image_view);

    // REPEAT, like Tutorial09's ground texture: texcoords tile across the
    // terrain (see getVertexData()'s chunk_size-based UVs) rather than
    // being clamped into one stretched copy.
    VkSampler vk_sampler;
    if (!createSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT, &vk_sampler)) {
        Logging::error(LOG_TAG, "Could not create sampler!");
        return false;
    }
    texture_image_parameters_.setVkSampler(vk_sampler);

    if (!copyTextureData(texture_data.data(),
                         static_cast<std::uint32_t>(texture_data.size()),
                         2048,
                         2048)) {
        Logging::error(LOG_TAG,
                       "Could not upload texture data to device memory!");
        return false;
    }

    return true;
}

bool VulkanTerrainPilot::createUniformBuffer() {
    uniform_buffer_.setSize(sizeof(TerrainPilotUniformBufferData));
    // Host-visible/coherent, rewritten every frame as the orbit camera
    // moves - same tradeoff Tutorial09 makes for the same reason (see
    // updateUniformBufferData()/draw()).
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      uniform_buffer_)) {
        Logging::error(LOG_TAG, "Could not create uniform buffer!");
        return false;
    }

    return updateUniformBufferData();
}

TerrainPilotUniformBufferData VulkanTerrainPilot::getUniformBufferData()
        const {
    TerrainPilotUniformBufferData data{};
    data.model = Math::Mat4<float>(1.0f);  // static terrain, no rotation
    data.view = glm::lookAt(
            camera_.eye(), camera_.target(), Math::Vec3<float>(0.0f, 1.0f, 0.0f));

    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    data.projection = Tools::getPerspectiveProjectionMatrix(
            width / height, 45.0f, 0.1f, 100.0f);

    data.light_position = Math::Vec4<float>(15.0f, 25.0f, 15.0f, 1.0f);
    data.light_color = Math::Vec4<float>(1.0f, 1.0f, 1.0f, 1.0f);
    data.view_position = Math::Vec4<float>(camera_.eye(), 1.0f);

    return data;
}

bool VulkanTerrainPilot::updateUniformBufferData() {
    TerrainPilotUniformBufferData const uniform_data = getUniformBufferData();

    void* uniform_buffer_memory_pointer;
    if (vkMapMemory(getVkDevice(),
                    uniform_buffer_.getVkDeviceMemory(),
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &uniform_buffer_memory_pointer) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not map uniform buffer memory!");
        return false;
    }
    std::memcpy(uniform_buffer_memory_pointer,
                &uniform_data,
                uniform_buffer_.getSize());
    vkUnmapMemory(getVkDevice(), uniform_buffer_.getVkDeviceMemory());

    return true;
}

bool VulkanTerrainPilot::createDescriptorSetLayout() {
    // Binding numbers match Tutorial09's shaders exactly (binding 0 =
    // uniform buffer in vertex+fragment, binding 1 = sampler in the
    // fragment stage) since this pilot reuses those compiled shaders as-is.
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings = {
            {.binding = 0,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags =
                     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
             .pImmutableSamplers = nullptr},
            {.binding = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = 1,
             .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
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
    descriptor_set_parameters_.setVkDescriptorSetLayout(
            vk_descriptor_set_layout);

    return true;
}

bool VulkanTerrainPilot::createDescriptorPool() {
    std::vector<VkDescriptorPoolSize> pool_sizes = {
            {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1},
            {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = 1}};

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
    descriptor_set_parameters_.setVkDescriptorPool(vk_descriptor_pool);

    return true;
}

bool VulkanTerrainPilot::allocateDescriptorSet() {
    VkDescriptorSetLayout vk_descriptor_set_layout =
            descriptor_set_parameters_.getVkDescriptorSetLayout();
    VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = descriptor_set_parameters_.getVkDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_descriptor_set_layout};

    VkDescriptorSet vk_descriptor_set;
    if (vkAllocateDescriptorSets(getVkDevice(),
                                 &descriptor_set_allocate_info,
                                 &vk_descriptor_set) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate descriptor set!");
        return false;
    }
    descriptor_set_parameters_.setVkDescriptorSet(vk_descriptor_set);

    return true;
}

bool VulkanTerrainPilot::updateDescriptorSet() {
    VkDescriptorBufferInfo buffer_info = {
            .buffer = uniform_buffer_.getVkBuffer(),
            .offset = 0,
            .range = uniform_buffer_.getSize()};

    VkDescriptorImageInfo image_info = {
            .sampler = texture_image_parameters_.getVkSampler(),
            .imageView = texture_image_parameters_.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    std::vector<VkWriteDescriptorSet> descriptor_writes = {
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = descriptor_set_parameters_.getVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &buffer_info,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = descriptor_set_parameters_.getVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &image_info,
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

bool VulkanTerrainPilot::createRenderPass() {
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
             .finalLayout =
                     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}};

    VkAttachmentReference color_attachment_reference = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkAttachmentReference depth_attachment_reference = {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpass_descriptions[] = {
            {.flags = 0,
             .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
             .inputAttachmentCount = 0,
             .pInputAttachments = nullptr,
             .colorAttachmentCount = 1,
             .pColorAttachments = &color_attachment_reference,
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

    if (vkCreateRenderPass(getVkDevice(),
                           &render_pass_create_info,
                           nullptr,
                           &vk_render_pass_) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

bool VulkanTerrainPilot::createPipelineLayout() {
    VkDescriptorSetLayout vk_descriptor_set_layout =
            descriptor_set_parameters_.getVkDescriptorSetLayout();
    VkPipelineLayoutCreateInfo layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &vk_descriptor_set_layout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr};

    if (vkCreatePipelineLayout(getVkDevice(),
                               &layout_create_info,
                               nullptr,
                               &vk_pipeline_layout_) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create pipeline layout!");
        return false;
    }

    return true;
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
VulkanTerrainPilot::createShaderModule(const char* filename) {
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

bool VulkanTerrainPilot::createPipeline() {
    // Reused byte-for-byte from Tutorial09 (resources/09/Data/): a Phong-lit,
    // textured, depth-tested surface is exactly what this ported terrain
    // needs, and its vertex layout (position/normal/texcoord) matches
    // TerrainPilotVertexData exactly.
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module = createShaderModule("shader.09.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module = createShaderModule("shader.09.frag.spv");

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
              .stride = TerrainPilotVertexAttributeTraits::stride,
              .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};

    std::vector<VkVertexInputAttributeDescription>
            vertex_attribute_descriptions = {
                    {.location = 0,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset = offsetof(struct TerrainPilotVertexData,
                                        position)},
                    {.location = 1,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32_SFLOAT,
                     .offset =
                             offsetof(struct TerrainPilotVertexData, normal)},
                    {.location = 2,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32_SFLOAT,
                     .offset = offsetof(struct TerrainPilotVertexData,
                                        texcoord)}};

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

    // Non-indexed triangle list, six vertices per grid quad - matches
    // TerrainMaker::prepareData()'s own glDrawArrays(GL_TRIANGLES, ...)
    // layout (see getVertexData()) rather than Tutorial09's indexed one.
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
            // NONE: the winding of the ported v_i..v_z quad pattern hasn't
            // been verified against this project's usual CCW convention,
            // and the orbit camera can end up on either side of the grid -
            // same reasoning the skybox pilot used for its own cullMode.
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

    VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
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
            .pDepthStencilState = &depth_stencil_state_create_info,
            .pColorBlendState = &color_blend_state_create_info,
            .pDynamicState = &dynamic_state_create_info,
            .layout = vk_pipeline_layout_,
            .renderPass = vk_render_pass_,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};

    if (vkCreateGraphicsPipelines(getVkDevice(),
                                  VK_NULL_HANDLE,
                                  1,
                                  &pipeline_create_info,
                                  nullptr,
                                  &vk_graphics_pipeline_) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create graphics pipeline!");
        return false;
    }
    return true;
}

void VulkanTerrainPilot::prepTerrain() {
    terrain_heights_.assign(kGridSize, std::vector<int>(kGridSize, 0));
}

void VulkanTerrainPilot::terrainGen(int new_steps,
                                    int new_increase,
                                    float new_radius,
                                    int new_random_jump) {
    float current_x = static_cast<float>(kGridSize / 2);
    float current_y = static_cast<float>(kGridSize / 2);

    for (int current_step = 1; current_step < new_steps; ++current_step) {
        int const random_value = rand() % 100;

        if (random_value > new_random_jump) {
            switch (rand() % 4) {
                case 0:
                    current_x -= 1.0f;
                    break;
                case 1:
                    current_x += 1.0f;
                    break;
                case 2:
                    current_y -= 1.0f;
                    break;
                case 3:
                    current_y += 1.0f;
                    break;
                default:
                    break;
            }
            if (((current_x >= kGridSize) || (current_x < 0)) ||
                ((current_y >= kGridSize) || (current_y < 0))) {
                current_x = static_cast<float>(rand() % kGridSize);
                current_y = static_cast<float>(rand() % kGridSize);
            }
        } else {
            current_x = static_cast<float>(rand() % kGridSize);
            current_y = static_cast<float>(rand() % kGridSize);
        }

        int const x_min = static_cast<int>(current_x - new_radius);
        int const x_max = static_cast<int>(current_x + new_radius);
        int const y_min = static_cast<int>(current_y - new_radius);
        int const y_max = static_cast<int>(current_y + new_radius);
        for (int x = x_min; x < x_max; ++x) {
            for (int y = y_min; y < y_max; ++y) {
                float const distance = std::sqrt(
                        std::pow(static_cast<double>(current_x - x), 2) +
                        std::pow(static_cast<double>(current_y) - y, 2));
                if ((distance < new_radius) &&
                    ((x >= 0 && x < kGridSize) && (y >= 0 && y < kGridSize))) {
                    terrain_heights_[x][y] += new_increase;
                }
            }
        }
    }
}

void VulkanTerrainPilot::terrainSmoothe(int box_width) {
    std::vector<std::vector<int>> smoothed = terrain_heights_;
    for (int y = 0; y < kGridSize; ++y) {
        for (int x = 0; x < kGridSize; ++x) {
            int height_sum = 0;
            for (int i = y - (box_width / 2); i < y + (box_width / 2); ++i) {
                for (int j = x - (box_width / 2); j < x + (box_width / 2);
                     ++j) {
                    if ((i >= 0 && i < kGridSize) && (j >= 0 && j < kGridSize)) {
                        height_sum += terrain_heights_[i][j];
                    }
                }
            }
            smoothed[y][x] = height_sum / (box_width * box_width);
        }
    }
    terrain_heights_ = smoothed;

    // Zero the four grid edges - collapses TerrainMaker::terrainSmoothe()'s
    // four separate (but overlapping) edge-clearing conditions into the one
    // condition they're together equivalent to.
    for (int y = 0; y < kGridSize; ++y) {
        for (int x = 0; x < kGridSize; ++x) {
            if (x == 0 || y == 0 || x == kGridSize - 1 || y == kGridSize - 1) {
                terrain_heights_[y][x] = 0;
            }
        }
    }
}

void VulkanTerrainPilot::calcNormal(int x,
                                    int z,
                                    int flag,
                                    Math::Vec3<float>* normal) const {
    std::array<float, 3> v1 = {};
    std::array<float, 3> v2 = {};
    bool can_calculate = true;
    if (flag == 1) {
        if (((x - 1) >= 0) && ((z - 1) >= 0) && (x < kGridSize) &&
            (z < kGridSize)) {
            v1[0] = -static_cast<float>(kGridScale);
            v1[1] = static_cast<float>(terrain_heights_[x - 1][z - 1] -
                                       terrain_heights_[x][z]);
            v1[2] = -static_cast<float>(kGridScale);

            v2[0] = -static_cast<float>(kGridScale);
            v2[1] = static_cast<float>(terrain_heights_[x - 1][z] -
                                       terrain_heights_[x][z]);
            v2[2] = 0.0f;
        } else {
            can_calculate = false;
        }
    } else {
        if ((((x - 1) >= 0) && (x < kGridSize)) && ((z + 1) < kGridSize) &&
            (z > 0)) {
            v1[0] = -static_cast<float>(kGridScale);
            v1[1] = static_cast<float>(terrain_heights_[x - 1][z] -
                                       terrain_heights_[x][z]);
            v1[2] = 0.0f;

            v2[0] = 0.0f;
            v2[1] = static_cast<float>(terrain_heights_[x][z + 1] -
                                       terrain_heights_[x][z]);
            v2[2] = static_cast<float>(kGridScale);
        } else {
            can_calculate = false;
        }
    }

    if (can_calculate) {
        normal->x = v1[1] * v2[2] - v1[2] * v2[1];
        normal->y = v1[2] * v2[0] - v1[0] * v2[2];
        normal->z = v1[0] * v2[1] - v1[1] * v2[0];
        float const mag = std::sqrt((normal->x * normal->x) +
                                    (normal->y * normal->y) +
                                    (normal->z * normal->z));
        normal->x /= mag;
        normal->y /= mag;
        normal->z /= mag;
    } else {
        normal->x = 0.0f;
        normal->y = 1.0f;
        normal->z = 0.0f;
    }
}

void VulkanTerrainPilot::smoothShadeNormal(int x,
                                           int z,
                                           Math::Vec3<float>* normal) const {
    Math::Vec3<float> n0(0.0f, 0.0f, 0.0f);
    Math::Vec3<float> n1(0.0f, 0.0f, 0.0f);
    Math::Vec3<float> n2(0.0f, 0.0f, 0.0f);
    Math::Vec3<float> n3(0.0f, 0.0f, 0.0f);
    Math::Vec3<float> n4(0.0f, 0.0f, 0.0f);
    Math::Vec3<float> n5(0.0f, 0.0f, 0.0f);
    calcNormal(x, z - 1, 0, &n0);
    calcNormal(x, z, 0, &n1);
    calcNormal(x, z, 1, &n2);
    calcNormal(x + 1, z + 1, 1, &n3);
    calcNormal(x + 1, z, 1, &n4);
    calcNormal(x + 1, z, 0, &n5);

    normal->x = (n0.x + n1.x + n2.x + n3.x + n4.x + n5.x) / 6.0f;
    normal->y = (n0.y + n1.y + n2.y + n3.y + n4.y + n5.y) / 6.0f;
    normal->z = (n0.z + n1.z + n2.z + n3.z + n4.z + n5.z) / 6.0f;
}

const std::vector<TerrainPilotVertexData>&
VulkanTerrainPilot::getVertexData() {
    if (!vertex_data_.empty()) {
        return vertex_data_;
    }

    prepTerrain();
    terrainGen(kGenSteps, kGenIncrease, kGenRadius, kGenRandomJump);
    for (int i = -1; i < kSmoothingPasses; ++i) {
        terrainSmoothe(10);
    }

    int const chunk_size = kGridSize / 2;
    float const chunk_span = static_cast<float>(chunk_size - 1);
    float const half_extent =
            static_cast<float>(kGridSize - 1) * kGridScale / 2.0f;

    vertex_data_.reserve(static_cast<std::size_t>(kGridSize - 1) *
                         static_cast<std::size_t>(kGridSize - 1) * 6);

    for (int i = 0; i < kGridSize - 1; ++i) {
        for (int j = 0; j < kGridSize - 1; ++j) {
            auto make_position = [&](int px, int pz, int height_row,
                                     int height_col) {
                return Math::Vec4<float>(
                        static_cast<float>(px * kGridScale) - half_extent,
                        static_cast<float>(
                                terrain_heights_[height_row][height_col]),
                        static_cast<float>(pz * kGridScale) - half_extent,
                        1.0f);
            };

            // V_I
            Math::Vec2<float> t_i(
                    static_cast<float>(i % (chunk_size - 1)) / chunk_span,
                    static_cast<float>(j % (chunk_size - 1)) / chunk_span);
            Math::Vec3<float> n_i(0.0f, 0.0f, 0.0f);
            if (i != 0 && j != 0) {
                smoothShadeNormal(j, i, &n_i);
            }
            vertex_data_.push_back({make_position(j, i, i, j), n_i, t_i});

            // V_J
            Math::Vec2<float> t_j(
                    (static_cast<float>(i % (chunk_size - 1)) + 1) /
                            chunk_span,
                    static_cast<float>(j % (chunk_size - 1)) / chunk_span);
            Math::Vec3<float> n_j(0.0f, 0.0f, 0.0f);
            if (j != 0 && i != kGridSize - 2) {
                smoothShadeNormal(j, i + 1, &n_j);
            }
            vertex_data_.push_back(
                    {make_position(j, i + 1, i + 1, j), n_j, t_j});

            // V_K
            Math::Vec2<float> t_k(
                    static_cast<float>(i % (chunk_size - 1)) / chunk_span,
                    (static_cast<float>(j % (chunk_size - 1)) + 1) /
                            chunk_span);
            Math::Vec3<float> n_k(0.0f, 0.0f, 0.0f);
            if (i != 0 && j != kGridSize - 2) {
                smoothShadeNormal(j + 1, i, &n_k);
            }
            vertex_data_.push_back(
                    {make_position(j + 1, i, i, j + 1), n_k, t_k});

            // V_X (same position/UV pattern as V_J)
            vertex_data_.push_back(
                    {make_position(j, i + 1, i + 1, j), n_j, t_j});

            // V_Y
            Math::Vec2<float> t_y(
                    (static_cast<float>(i % (chunk_size - 1)) + 1) /
                            chunk_span,
                    (static_cast<float>(j % (chunk_size - 1)) + 1) /
                            chunk_span);
            Math::Vec3<float> n_y(0.0f, 0.0f, 0.0f);
            if (i != kGridSize - 2 && j != kGridSize - 2) {
                smoothShadeNormal(j + 1, i + 1, &n_y);
            }
            vertex_data_.push_back(
                    {make_position(j + 1, i + 1, i + 1, j + 1), n_y, t_y});

            // V_Z (same position/UV pattern as V_K)
            vertex_data_.push_back(
                    {make_position(j + 1, i, i, j + 1), n_k, t_k});
        }
    }

    return vertex_data_;
}

bool VulkanTerrainPilot::copyBufferData(BufferParameters& destination,
                                        const void* data,
                                        std::uint32_t data_size,
                                        VkAccessFlags dst_access_mask,
                                        VkPipelineStageFlags dst_stage_mask) {
    if (data_size > staging_buffer_.getSize()) {
        Logging::error(LOG_TAG,
                       "Buffer data (",
                       data_size,
                       " bytes) does not fit in the staging buffer (",
                       staging_buffer_.getSize(),
                       " bytes)!");
        return false;
    }

    void* staging_buffer_memory_pointer;
    if (vkMapMemory(getVkDevice(),
                    staging_buffer_.getVkDeviceMemory(),
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
            .memory = staging_buffer_.getVkDeviceMemory(),
            .offset = 0,
            .size = VK_WHOLE_SIZE};
    vkFlushMappedMemoryRanges(getVkDevice(), 1, &flush_range);

    vkUnmapMemory(getVkDevice(), staging_buffer_.getVkDeviceMemory());

    VkCommandBuffer command_buffer =
            rendering_resources_[0].getVkCommandBuffer();

    VkCommandBufferBeginInfo command_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr};

    vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

    VkBufferCopy buffer_copy_info = {
            .srcOffset = 0, .dstOffset = 0, .size = data_size};
    vkCmdCopyBuffer(command_buffer,
                    staging_buffer_.getVkBuffer(),
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

bool VulkanTerrainPilot::createVertexBuffer() {
    const std::vector<TerrainPilotVertexData>& vertex_data = getVertexData();
    vertex_count_ = static_cast<std::uint32_t>(vertex_data.size());

    vertex_buffer_.setSize(static_cast<std::uint32_t>(
            vertex_data.size() * sizeof(vertex_data[0])));
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      vertex_buffer_)) {
        Logging::error(LOG_TAG, "Could not create vertex buffer!");
        return false;
    }

    return copyBufferData(vertex_buffer_,
                          vertex_data.data(),
                          vertex_buffer_.getSize(),
                          VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                          VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
}

bool VulkanTerrainPilot::createFramebuffer(VkFramebuffer& framebuffer,
                                           VkImageView image_view) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(getVkDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    std::vector<VkImageView> attachments = {
            image_view, depth_image_parameters_.getVkImageView()};

    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = vk_render_pass_,
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

bool VulkanTerrainPilot::prepareFrame(VkCommandBuffer command_buffer,
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

    std::vector<VkClearValue> clear_values = {
            {.color = {.float32 = {0.4f, 0.55f, 0.8f, 1.0f}}},
            {.depthStencil = {.depth = 1.0f, .stencil = 0}}};

    VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = vk_render_pass_,
            .framebuffer = framebuffer,
            .renderArea = {.offset = {.x = 0, .y = 0},
                           .extent = getSwapchainParameters().getVkExtent2d()},
            .clearValueCount = static_cast<std::uint32_t>(clear_values.size()),
            .pClearValues = clear_values.data()};

    vkCmdBeginRenderPass(command_buffer,
                         &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(
            command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_graphics_pipeline_);

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

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(
            command_buffer, 0, 1, &vertex_buffer_.getVkBuffer(), &offset);

    VkDescriptorSet vk_descriptor_set =
            descriptor_set_parameters_.getVkDescriptorSet();
    vkCmdBindDescriptorSets(command_buffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vk_pipeline_layout_,
                            0,
                            1,
                            &vk_descriptor_set,
                            0,
                            nullptr);

    vkCmdDraw(command_buffer, vertex_count_, 1, 0, 0);

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

bool VulkanTerrainPilot::draw() {
    static std::size_t resource_index = 0;
    RenderingResourceParameters& current_rendering_resource =
            rendering_resources_[resource_index];
    VkSwapchainKHR swap_chain = getSwapchainParameters().getVkSwapchainKhr();
    std::uint32_t image_index;

    resource_index = (resource_index + 1) % resources_count_;

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
            finished_rendering_semaphores_[image_index];

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

void VulkanTerrainPilot::onMouseButton(int button,
                                       bool pressed,
                                       int pos_x,
                                       int pos_y) {
    camera_.onMouseButton(button, pressed, pos_x, pos_y);
}

void VulkanTerrainPilot::onMouseMove(int pos_x, int pos_y) {
    camera_.onMouseMove(pos_x, pos_y);
}

void VulkanTerrainPilot::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

bool VulkanTerrainPilot::childOnWindowSizeChanged() {
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
    return createVertexBuffer();
}

void VulkanTerrainPilot::childClear() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return;
    }
    vkDeviceWaitIdle(getVkDevice());

    for (std::size_t i = 0; i < rendering_resources_.size(); ++i) {
        if (rendering_resources_[i].getVkFramebuffer() != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(getVkDevice(),
                                 rendering_resources_[i].getVkFramebuffer(),
                                 nullptr);
            rendering_resources_[i].setVkFramebuffer(VK_NULL_HANDLE);
        }
        if (rendering_resources_[i].getVkCommandBuffer() != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(
                    getVkDevice(),
                    vk_command_pool_,
                    1,
                    &rendering_resources_[i].getVkCommandBuffer());
        }
        if (rendering_resources_[i].getImageAvailableVkSemaphore() !=
            VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(),
                    rendering_resources_[i].getImageAvailableVkSemaphore(),
                    nullptr);
        }
        if (rendering_resources_[i].getVkFence() != VK_NULL_HANDLE) {
            vkDestroyFence(
                    getVkDevice(), rendering_resources_[i].getVkFence(), nullptr);
        }
    }

    for (std::size_t i = 0; i < finished_rendering_semaphores_.size(); ++i) {
        if (finished_rendering_semaphores_[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(), finished_rendering_semaphores_[i], nullptr);
        }
    }
    finished_rendering_semaphores_.clear();

    if (vk_command_pool_ != VK_NULL_HANDLE) {
        vkDestroyCommandPool(getVkDevice(), vk_command_pool_, nullptr);
        vk_command_pool_ = VK_NULL_HANDLE;
    }

    destroyBuffer(vertex_buffer_);
    destroyBuffer(staging_buffer_);

    if (vk_graphics_pipeline_ != VK_NULL_HANDLE) {
        vkDestroyPipeline(getVkDevice(), vk_graphics_pipeline_, nullptr);
        vk_graphics_pipeline_ = VK_NULL_HANDLE;
    }

    if (vk_pipeline_layout_ != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(getVkDevice(), vk_pipeline_layout_, nullptr);
        vk_pipeline_layout_ = VK_NULL_HANDLE;
    }

    if (vk_render_pass_ != VK_NULL_HANDLE) {
        vkDestroyRenderPass(getVkDevice(), vk_render_pass_, nullptr);
        vk_render_pass_ = VK_NULL_HANDLE;
    }

    if (descriptor_set_parameters_.getVkDescriptorPool() != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(getVkDevice(),
                                descriptor_set_parameters_.getVkDescriptorPool(),
                                nullptr);
        descriptor_set_parameters_.setVkDescriptorPool(VK_NULL_HANDLE);
    }
    if (descriptor_set_parameters_.getVkDescriptorSetLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(
                getVkDevice(),
                descriptor_set_parameters_.getVkDescriptorSetLayout(),
                nullptr);
        descriptor_set_parameters_.setVkDescriptorSetLayout(VK_NULL_HANDLE);
    }

    destroyBuffer(uniform_buffer_);
    destroyDepthResources();

    if (texture_image_parameters_.getVkSampler() != VK_NULL_HANDLE) {
        vkDestroySampler(
                getVkDevice(), texture_image_parameters_.getVkSampler(), nullptr);
        texture_image_parameters_.setVkSampler(VK_NULL_HANDLE);
    }
    if (texture_image_parameters_.getVkImageView() != VK_NULL_HANDLE) {
        vkDestroyImageView(
                getVkDevice(), texture_image_parameters_.getVkImageView(), nullptr);
        texture_image_parameters_.setVkImageView(VK_NULL_HANDLE);
    }
    if (texture_image_parameters_.getVkImage() != VK_NULL_HANDLE) {
        vkDestroyImage(
                getVkDevice(), texture_image_parameters_.getVkImage(), nullptr);
        texture_image_parameters_.setVkImage(VK_NULL_HANDLE);
    }
    if (texture_image_parameters_.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(),
                    texture_image_parameters_.getVkDeviceMemory(),
                    nullptr);
        texture_image_parameters_.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

}  // namespace vulkan_graphix
