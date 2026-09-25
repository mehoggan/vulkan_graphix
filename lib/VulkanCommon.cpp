#include "vulkan_graphix/VulkanCommon.h"

#include <cstring>

#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix::VulkanCommon {

namespace {

// Shared by BufferFactory::allocateMemory() and ImageFactory::
// allocateMemory() - identical first-fit memory-type scan, differing only
// in how the VkMemoryRequirements are obtained.
bool findAndAllocateMemory(VkDevice device,
                           VkPhysicalDevice physical_device,
                           VkMemoryRequirements const& requirements,
                           VkMemoryPropertyFlags property,
                           VkDeviceMemory* memory) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

    for (std::uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((requirements.memoryTypeBits & (1 << i)) &&
            ((memory_properties.memoryTypes[i].propertyFlags & property) ==
             property)) {
            VkMemoryAllocateInfo memory_allocate_info = {
                    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                    .pNext = nullptr,
                    .allocationSize = requirements.size,
                    .memoryTypeIndex = i};

            if (vkAllocateMemory(
                        device, &memory_allocate_info, nullptr, memory) ==
                VK_SUCCESS) {
                return true;
            }
        }
    }
    return false;
}

}  // namespace

// ************************************************************ //
// BufferFactory                                                //
// ************************************************************ //
BufferFactory::BufferFactory(VkDevice device, VkPhysicalDevice physical_device)
        : m_device(device), m_physical_device(physical_device) {}

bool BufferFactory::allocateMemory(VkBuffer buffer,
                                   VkMemoryPropertyFlags property,
                                   VkDeviceMemory* memory) const {
    VkMemoryRequirements buffer_memory_requirements;
    vkGetBufferMemoryRequirements(
            m_device, buffer, &buffer_memory_requirements);
    return findAndAllocateMemory(m_device,
                                 m_physical_device,
                                 buffer_memory_requirements,
                                 property,
                                 memory);
}

bool BufferFactory::create(VkBufferUsageFlags usage,
                           VkMemoryPropertyFlags memory_property,
                           BufferParameters& out) const {
    VkBufferCreateInfo buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = out.getSize(),
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr};

    VkBuffer vk_buffer;
    if (vkCreateBuffer(m_device, &buffer_create_info, nullptr, &vk_buffer) !=
        VK_SUCCESS) {
        return false;
    }
    out.setVkBuffer(vk_buffer);

    VkDeviceMemory vk_device_memory;
    if (!allocateMemory(out.getVkBuffer(), memory_property, &vk_device_memory)) {
        return false;
    }
    out.setVkDeviceMemory(vk_device_memory);

    return vkBindBufferMemory(
                   m_device, out.getVkBuffer(), out.getVkDeviceMemory(), 0) ==
           VK_SUCCESS;
}

void BufferFactory::destroy(BufferParameters& buffer) const {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_device, buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(m_device, buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

// ************************************************************ //
// ImageFactory                                                 //
// ************************************************************ //
ImageFactory::ImageFactory(VkDevice device, VkPhysicalDevice physical_device)
        : m_device(device), m_physical_device(physical_device) {}

bool ImageFactory::allocateMemory(VkImage image,
                                  VkMemoryPropertyFlags property,
                                  VkDeviceMemory* memory) const {
    VkMemoryRequirements image_memory_requirements;
    vkGetImageMemoryRequirements(m_device, image, &image_memory_requirements);
    return findAndAllocateMemory(m_device,
                                 m_physical_device,
                                 image_memory_requirements,
                                 property,
                                 memory);
}

bool ImageFactory::bindMemory(VkImage image, VkDeviceMemory memory) const {
    return vkBindImageMemory(m_device, image, memory, 0) == VK_SUCCESS;
}

bool ImageFactory::createImage(std::uint32_t width,
                               std::uint32_t height,
                               VkFormat format,
                               VkImageUsageFlags usage,
                               VkImage* out) const {
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

    return vkCreateImage(m_device, &image_create_info, nullptr, out) ==
           VK_SUCCESS;
}

bool ImageFactory::createImageView(VkImage image,
                                   VkFormat format,
                                   VkImageAspectFlags aspect_mask,
                                   VkImageView* out) const {
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

    return vkCreateImageView(m_device, &image_view_create_info, nullptr, out) ==
           VK_SUCCESS;
}

bool ImageFactory::createSampler(VkSamplerAddressMode address_mode,
                                 VkSampler* out) const {
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

    return vkCreateSampler(m_device, &sampler_create_info, nullptr, out) ==
           VK_SUCCESS;
}

void ImageFactory::destroy(ImageParameters& image) const {
    if (image.getVkSampler() != VK_NULL_HANDLE) {
        vkDestroySampler(m_device, image.getVkSampler(), nullptr);
        image.setVkSampler(VK_NULL_HANDLE);
    }
    if (image.getVkImageView() != VK_NULL_HANDLE) {
        vkDestroyImageView(m_device, image.getVkImageView(), nullptr);
        image.setVkImageView(VK_NULL_HANDLE);
    }
    if (image.getVkImage() != VK_NULL_HANDLE) {
        vkDestroyImage(m_device, image.getVkImage(), nullptr);
        image.setVkImage(VK_NULL_HANDLE);
    }
    if (image.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(m_device, image.getVkDeviceMemory(), nullptr);
        image.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

// ************************************************************ //
// StagedUploader                                               //
// ************************************************************ //
StagedUploader::StagedUploader(VkDevice device,
                               VkQueue graphics_queue,
                               VkCommandBuffer upload_command_buffer)
        : m_device(device)
        , m_graphics_queue(graphics_queue)
        , m_upload_command_buffer(upload_command_buffer) {}

bool StagedUploader::uploadToImage(BufferParameters& staging_buffer,
                                   VkImage image,
                                   const char* data,
                                   std::uint32_t data_size,
                                   std::uint32_t width,
                                   std::uint32_t height) const {
    if (data_size > staging_buffer.getSize()) {
        return false;
    }

    void* staging_buffer_memory_pointer;
    if (vkMapMemory(m_device,
                    staging_buffer.getVkDeviceMemory(),
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &staging_buffer_memory_pointer) != VK_SUCCESS) {
        return false;
    }

    std::memcpy(staging_buffer_memory_pointer, data, data_size);

    VkMappedMemoryRange flush_range = {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .pNext = nullptr,
            .memory = staging_buffer.getVkDeviceMemory(),
            .offset = 0,
            .size = VK_WHOLE_SIZE};
    vkFlushMappedMemoryRanges(m_device, 1, &flush_range);

    vkUnmapMemory(m_device, staging_buffer.getVkDeviceMemory());

    VkCommandBufferBeginInfo command_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr};
    vkBeginCommandBuffer(m_upload_command_buffer, &command_buffer_begin_info);

    VkImageSubresourceRange image_subresource_range = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1};

    VkImageMemoryBarrier barrier_to_transfer_dst = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = image_subresource_range};
    vkCmdPipelineBarrier(m_upload_command_buffer,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier_to_transfer_dst);

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
    vkCmdCopyBufferToImage(m_upload_command_buffer,
                           staging_buffer.getVkBuffer(),
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &buffer_image_copy_info);

    VkImageMemoryBarrier barrier_to_shader_read = {
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
    vkCmdPipelineBarrier(m_upload_command_buffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier_to_shader_read);

    vkEndCommandBuffer(m_upload_command_buffer);

    VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = nullptr,
                                .waitSemaphoreCount = 0,
                                .pWaitSemaphores = nullptr,
                                .pWaitDstStageMask = nullptr,
                                .commandBufferCount = 1,
                                .pCommandBuffers = &m_upload_command_buffer,
                                .signalSemaphoreCount = 0,
                                .pSignalSemaphores = nullptr};
    if (vkQueueSubmit(m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE) !=
        VK_SUCCESS) {
        return false;
    }

    vkDeviceWaitIdle(m_device);

    return true;
}

bool StagedUploader::uploadToBuffer(BufferParameters& staging_buffer,
                                    BufferParameters& destination,
                                    const void* data,
                                    std::uint32_t data_size,
                                    VkAccessFlags dst_access_mask,
                                    VkPipelineStageFlags dst_stage_mask) const {
    if (data_size > staging_buffer.getSize()) {
        return false;
    }

    void* staging_buffer_memory_pointer;
    if (vkMapMemory(m_device,
                    staging_buffer.getVkDeviceMemory(),
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &staging_buffer_memory_pointer) != VK_SUCCESS) {
        return false;
    }

    std::memcpy(staging_buffer_memory_pointer, data, data_size);

    VkMappedMemoryRange flush_range = {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .pNext = nullptr,
            .memory = staging_buffer.getVkDeviceMemory(),
            .offset = 0,
            .size = VK_WHOLE_SIZE};
    vkFlushMappedMemoryRanges(m_device, 1, &flush_range);

    vkUnmapMemory(m_device, staging_buffer.getVkDeviceMemory());

    VkCommandBufferBeginInfo command_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr};
    vkBeginCommandBuffer(m_upload_command_buffer, &command_buffer_begin_info);

    VkBufferCopy buffer_copy_info = {
            .srcOffset = 0, .dstOffset = 0, .size = data_size};
    vkCmdCopyBuffer(m_upload_command_buffer,
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
    vkCmdPipelineBarrier(m_upload_command_buffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         dst_stage_mask,
                         0,
                         0,
                         nullptr,
                         1,
                         &buffer_memory_barrier,
                         0,
                         nullptr);

    vkEndCommandBuffer(m_upload_command_buffer);

    VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = nullptr,
                                .waitSemaphoreCount = 0,
                                .pWaitSemaphores = nullptr,
                                .pWaitDstStageMask = nullptr,
                                .commandBufferCount = 1,
                                .pCommandBuffers = &m_upload_command_buffer,
                                .signalSemaphoreCount = 0,
                                .pSignalSemaphores = nullptr};
    if (vkQueueSubmit(m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE) !=
        VK_SUCCESS) {
        return false;
    }

    vkDeviceWaitIdle(m_device);

    return true;
}

// ************************************************************ //
// FrameResourceFactory                                         //
// ************************************************************ //
FrameResourceFactory::FrameResourceFactory(VkDevice device)
        : m_device(device) {}

bool FrameResourceFactory::createCommandPool(std::uint32_t queue_family_index,
                                             VkCommandPool* out) const {
    VkCommandPoolCreateInfo cmd_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
                     VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = queue_family_index};

    return vkCreateCommandPool(m_device, &cmd_pool_create_info, nullptr, out) ==
           VK_SUCCESS;
}

bool FrameResourceFactory::allocateCommandBuffers(VkCommandPool pool,
                                                  std::uint32_t count,
                                                  VkCommandBuffer* out) const {
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = count};

    return vkAllocateCommandBuffers(
                   m_device, &command_buffer_allocate_info, out) == VK_SUCCESS;
}

bool FrameResourceFactory::createSemaphore(VkSemaphore* out) const {
    VkSemaphoreCreateInfo semaphore_create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0};

    return vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, out) ==
           VK_SUCCESS;
}

bool FrameResourceFactory::createFence(bool signaled, VkFence* out) const {
    VkFenceCreateInfo fence_create_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = signaled
                             ? static_cast<VkFenceCreateFlags>(
                                       VK_FENCE_CREATE_SIGNALED_BIT)
                             : 0u};

    return vkCreateFence(m_device, &fence_create_info, nullptr, out) ==
           VK_SUCCESS;
}

// ************************************************************ //
// Free functions                                               //
// ************************************************************ //
bool createTextureFromPixels(const ImageFactory& image_factory,
                             const StagedUploader& uploader,
                             BufferParameters& staging_buffer,
                             std::uint32_t width,
                             std::uint32_t height,
                             const std::vector<char>& pixels,
                             VkSamplerAddressMode address_mode,
                             ImageParameters& out) {
    // Every existing tutorial's own createTextureFromPixels() hardcodes
    // this format/usage combination - it's always an uploaded RGBA color
    // texture, never anything else, so it stays hardcoded here too rather
    // than becoming two more parameters every call site would need to
    // repeat identically.
    constexpr VkFormat c_texture_format = VK_FORMAT_R8G8B8A8_UNORM;

    VkImage vk_image;
    if (!image_factory.createImage(width,
                                   height,
                                   c_texture_format,
                                   VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                           VK_IMAGE_USAGE_SAMPLED_BIT,
                                   &vk_image)) {
        return false;
    }
    out.setVkImage(vk_image);

    VkDeviceMemory vk_device_memory;
    if (!image_factory.allocateMemory(out.getVkImage(),
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                      &vk_device_memory)) {
        return false;
    }
    out.setVkDeviceMemory(vk_device_memory);

    if (!image_factory.bindMemory(out.getVkImage(),
                                  out.getVkDeviceMemory())) {
        return false;
    }

    VkSampler vk_sampler;
    if (!image_factory.createSampler(address_mode, &vk_sampler)) {
        return false;
    }
    out.setVkSampler(vk_sampler);

    VkImageView vk_image_view;
    if (!image_factory.createImageView(out.getVkImage(),
                                       c_texture_format,
                                       VK_IMAGE_ASPECT_COLOR_BIT,
                                       &vk_image_view)) {
        return false;
    }
    out.setVkImageView(vk_image_view);

    return uploader.uploadToImage(staging_buffer,
                                  out.getVkImage(),
                                  pixels.data(),
                                  static_cast<std::uint32_t>(pixels.size()),
                                  width,
                                  height);
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
createShaderModule(VkDevice device, const char* filename) {
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
    if (vkCreateShaderModule(device,
                             &shader_module_create_info,
                             nullptr,
                             &shader_module) != VK_SUCCESS) {
        return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>();
    }

    return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>(
            shader_module, vkDestroyShaderModule, device);
}

}  // namespace vulkan_graphix::VulkanCommon
