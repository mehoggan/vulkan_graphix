#include "VulkanSkyboxPilot.h"

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <cstring>

#include <glm/gtc/type_ptr.hpp>

#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

VulkanSkyboxPilot::VulkanSkyboxPilot()
        : vk_render_pass_(VK_NULL_HANDLE)
        , vk_pipeline_layout_(VK_NULL_HANDLE)
        , vk_graphics_pipeline_(VK_NULL_HANDLE)
        , index_count_(0)
        , vk_command_pool_(VK_NULL_HANDLE)
        , rendering_resources_(resources_count_)
        // Start outside the cube (half-extent 2.0, see getVertexData()) for
        // a clear initial view; drag to orbit, including in past the faces
        // to look around from inside - cullMode is NONE (see
        // createPipeline()) specifically so that works without any
        // winding-order fuss.
        , camera_(0.6f, 0.4f, 6.0f) {}

VulkanSkyboxPilot::~VulkanSkyboxPilot() { childClear(); }

bool VulkanSkyboxPilot::createCommandPool(std::uint32_t queue_family_index,
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

bool VulkanSkyboxPilot::allocateCommandBuffers(
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

bool VulkanSkyboxPilot::createCommandBuffers() {
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

bool VulkanSkyboxPilot::createSemaphores() {
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

bool VulkanSkyboxPilot::createFences() {
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

bool VulkanSkyboxPilot::createRenderingResources() {
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

bool VulkanSkyboxPilot::allocateBufferMemory(VkBuffer buffer,
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

bool VulkanSkyboxPilot::createBuffer(VkBufferUsageFlags usage,
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

bool VulkanSkyboxPilot::createStagingBuffer() {
    // Tutorial07 hardcodes 1000000 bytes here, sized for its own
    // texture.07.png (512x462 RGBA = 946176 bytes - just under). SkyBox.jpg
    // is 1024x1024 RGBA = 4194304 bytes, well over that; sized explicitly
    // for it here (with headroom) rather than copying Tutorial07's constant
    // without checking it against this pilot's own asset - see
    // copyTextureData()'s size guard for what silently overflowing this
    // buffer actually did (a real, reproducible i915 GPU hang a few
    // seconds into every run).
    staging_buffer_.setSize(5 * 1024 * 1024);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer_)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

bool VulkanSkyboxPilot::createImage(std::uint32_t width,
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

bool VulkanSkyboxPilot::allocateImageMemory(VkImage image,
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

bool VulkanSkyboxPilot::createImageView() {
    VkImageViewCreateInfo image_view_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image_parameters_.getVkImage(),
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
                             &image_parameters_.getVkImageView()) ==
           VK_SUCCESS;
}

bool VulkanSkyboxPilot::createSampler(VkSampler* sampler) {
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

bool VulkanSkyboxPilot::copyTextureData(char* texture_data,
                                        std::uint32_t data_size,
                                        std::uint32_t width,
                                        std::uint32_t height) {
    if (data_size > staging_buffer_.getSize()) {
        // Silently memcpy-ing past the staging buffer's mapped allocation
        // here is exactly what caused a real, reproducible i915 GPU hang
        // a few seconds into every run before this check existed - fail
        // loudly instead.
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
             .image = image_parameters_.getVkImage(),
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
                           image_parameters_.getVkImage(),
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
            .image = image_parameters_.getVkImage(),
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

bool VulkanSkyboxPilot::createTexture() {
    int width = 0, height = 0, data_size = 0;
    // SkyBox.jpg is the same 1024x1024 sky texture SkyboxFactory.cpp loads
    // as SkyBox.raw, decodable directly via stb_image - no new raw-image
    // loader needed for this pilot (see the migration plan's "Future work"
    // section for the broader .raw-asset question).
    std::vector<char> texture_data = Tools::getImageData(
            "SkyBox.jpg", 4, &width, &height, nullptr, &data_size);
    if (texture_data.empty()) {
        return false;
    }

    VkImage vk_image;
    if (!createImage(static_cast<std::uint32_t>(width),
                     static_cast<std::uint32_t>(height),
                     &vk_image)) {
        Logging::error(LOG_TAG, "Could not create image!");
        return false;
    }
    image_parameters_.setVkImage(vk_image);

    VkDeviceMemory vk_device_memory;
    if (!allocateImageMemory(image_parameters_.getVkImage(),
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &vk_device_memory)) {
        Logging::error(LOG_TAG, "Could not allocate memory for image!");
        return false;
    }
    image_parameters_.setVkDeviceMemory(vk_device_memory);

    if (vkBindImageMemory(getVkDevice(),
                          image_parameters_.getVkImage(),
                          image_parameters_.getVkDeviceMemory(),
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
    image_parameters_.setVkSampler(vk_sampler);

    if (!copyTextureData(texture_data.data(),
                         static_cast<std::uint32_t>(data_size),
                         static_cast<std::uint32_t>(width),
                         static_cast<std::uint32_t>(height))) {
        Logging::error(LOG_TAG,
                       "Could not upload texture data to device memory!");
        return false;
    }

    return true;
}

bool VulkanSkyboxPilot::createUniformBuffer() {
    uniform_buffer_.setSize(16 * sizeof(float));
    // Host-visible/coherent (not device-local + staged): rewritten every
    // frame as the orbit camera moves, same tradeoff Tutorial09 makes for
    // the same reason (see updateUniformBufferData()/draw()).
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      uniform_buffer_)) {
        Logging::error(LOG_TAG, "Could not create uniform buffer!");
        return false;
    }

    return updateUniformBufferData();
}

Math::Mat4<float> VulkanSkyboxPilot::getUniformBufferData() const {
    // Reused Tutorial07 shaders name this uniform "u_ProjectionMatrix" but
    // just do gl_Position = u_ProjectionMatrix * i_Position - it's really a
    // full model-view-projection slot, which is exactly what's needed here.
    Math::Mat4<float> const model(1.0f);
    Math::Mat4<float> const view = glm::lookAt(
            camera_.eye(), camera_.target(), Math::Vec3<float>(0.0f, 1.0f, 0.0f));

    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    Math::Mat4<float> const projection =
            Tools::getPerspectiveProjectionMatrix(
                    width / height, 45.0f, 0.1f, 100.0f);

    return projection * view * model;
}

bool VulkanSkyboxPilot::updateUniformBufferData() {
    Math::Mat4<float> const uniform_data = getUniformBufferData();

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
                glm::value_ptr(uniform_data),
                uniform_buffer_.getSize());
    vkUnmapMemory(getVkDevice(), uniform_buffer_.getVkDeviceMemory());

    return true;
}

bool VulkanSkyboxPilot::createDescriptorSetLayout() {
    // Binding numbers match Tutorial07's shaders exactly (binding 0 =
    // sampler in the fragment stage, binding 1 = uniform buffer in the
    // vertex stage) since this pilot reuses those compiled shaders as-is.
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
    descriptor_set_parameters_.setVkDescriptorSetLayout(
            vk_descriptor_set_layout);

    return true;
}

bool VulkanSkyboxPilot::createDescriptorPool() {
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
    descriptor_set_parameters_.setVkDescriptorPool(vk_descriptor_pool);

    return true;
}

bool VulkanSkyboxPilot::allocateDescriptorSet() {
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

bool VulkanSkyboxPilot::updateDescriptorSet() {
    VkDescriptorImageInfo image_info = {
            .sampler = image_parameters_.getVkSampler(),
            .imageView = image_parameters_.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    VkDescriptorBufferInfo buffer_info = {
            .buffer = uniform_buffer_.getVkBuffer(),
            .offset = 0,
            .range = uniform_buffer_.getSize()};

    std::vector<VkWriteDescriptorSet> descriptor_writes = {
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = descriptor_set_parameters_.getVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &image_info,
             .pBufferInfo = nullptr,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = descriptor_set_parameters_.getVkDescriptorSet(),
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

bool VulkanSkyboxPilot::createRenderPass() {
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

    if (vkCreateRenderPass(getVkDevice(),
                           &render_pass_create_info,
                           nullptr,
                           &vk_render_pass_) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

bool VulkanSkyboxPilot::createPipelineLayout() {
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
VulkanSkyboxPilot::createShaderModule(const char* filename) {
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

bool VulkanSkyboxPilot::createPipeline() {
    // Reused byte-for-byte from Tutorial07 (resources/07/Data/): its vertex
    // shader does gl_Position = u_ProjectionMatrix * i_Position on a
    // {vec4 position; vec2 texcoord;} vertex, and its fragment shader is a
    // single texture(sampler, uv) lookup - exactly an unlit textured
    // surface, which is all a skybox is. No new GLSL needed for this pilot.
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module = createShaderModule("shader.07.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module = createShaderModule("shader.07.frag.spv");

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
              .stride = SkyboxPilotVertexAttributeTraits::stride,
              .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};

    std::vector<VkVertexInputAttributeDescription>
            vertex_attribute_descriptions = {
                    {.location = 0,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset = offsetof(struct SkyboxPilotVertexData,
                                        position)},
                    {.location = 1,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32_SFLOAT,
                     .offset = offsetof(struct SkyboxPilotVertexData,
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

    // Indexed triangle list (24 unique vertices + 36 indices), unlike
    // Tutorial07's 4-vertex triangle strip - see getVertexData()/
    // getIndexData().
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
            // NONE, not BACK_BIT: the camera can end up either outside or
            // inside this cube (drag the orbit camera in past a face), and
            // both need to render without fighting over SkyboxFactory's
            // original triangle winding. A real skybox is conventionally
            // rendered this way (or with inverted culling) for exactly this
            // reason.
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

const std::vector<SkyboxPilotVertexData>& VulkanSkyboxPilot::getVertexData()
        const {
    // Replicates SkyboxFactory::draw()'s six hardcoded GL_QUADS faces
    // (vulkan_earth/src/SkyboxFactory.cpp:219-288) exactly - same per-face
    // vertex order and (0,1)/(0,0)/(1,0)/(1,1) UV corners on every face, no
    // atlas - just at a small, sane scale instead of the game's *100 world
    // units, and as 24 unique indexed vertices instead of six independent
    // GL_QUADS draws. Y is asymmetric on purpose, matching the original:
    // the top is half as far from center as the bottom/sides.
    static const float x_min = -2.0f, x_max = 2.0f;
    static const float y_min = -2.0f, y_max = 1.0f;  // 2.0f * 0.5f
    static const float z_min = -2.0f, z_max = 2.0f;

    static const std::vector<SkyboxPilotVertexData> vertex_data = {
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

    return vertex_data;
}

const std::vector<std::uint32_t>& VulkanSkyboxPilot::getIndexData() const {
    // Two triangles per face (0,1,2 and 0,2,3), matching GL_QUADS' implicit
    // fan triangulation of the same four corners.
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

bool VulkanSkyboxPilot::copyBufferData(BufferParameters& destination,
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

bool VulkanSkyboxPilot::createVertexBuffer() {
    const std::vector<SkyboxPilotVertexData>& vertex_data = getVertexData();

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

bool VulkanSkyboxPilot::createIndexBuffer() {
    const std::vector<std::uint32_t>& index_data = getIndexData();
    index_count_ = static_cast<std::uint32_t>(index_data.size());

    index_buffer_.setSize(static_cast<std::uint32_t>(
            index_data.size() * sizeof(index_data[0])));
    if (!createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      index_buffer_)) {
        Logging::error(LOG_TAG, "Could not create index buffer!");
        return false;
    }

    return copyBufferData(index_buffer_,
                          index_data.data(),
                          index_buffer_.getSize(),
                          VK_ACCESS_INDEX_READ_BIT,
                          VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
}

bool VulkanSkyboxPilot::createFramebuffer(VkFramebuffer& framebuffer,
                                          VkImageView image_view) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(getVkDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = vk_render_pass_,
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

bool VulkanSkyboxPilot::prepareFrame(VkCommandBuffer command_buffer,
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
            .color = {.float32 = {0.0f, 0.0f, 0.0f, 0.0f}}};

    VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = vk_render_pass_,
            .framebuffer = framebuffer,
            .renderArea = {.offset = {.x = 0, .y = 0},
                           .extent = getSwapchainParameters().getVkExtent2d()},
            .clearValueCount = 1,
            .pClearValues = &clear_value};

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

    vkCmdBindIndexBuffer(
            command_buffer, index_buffer_.getVkBuffer(), 0, VK_INDEX_TYPE_UINT32);

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

    vkCmdDrawIndexed(command_buffer, index_count_, 1, 0, 0, 0);

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

bool VulkanSkyboxPilot::draw() {
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

    // The uniform buffer is a single, shared, host-visible allocation
    // rewritten every frame from the orbit camera's current state - the
    // same tradeoff Tutorial09 makes for the same reason (see the comment
    // in Tutorial09::draw()).
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

void VulkanSkyboxPilot::onMouseButton(int button,
                                      bool pressed,
                                      int pos_x,
                                      int pos_y) {
    camera_.onMouseButton(button, pressed, pos_x, pos_y);
}

void VulkanSkyboxPilot::onMouseMove(int pos_x, int pos_y) {
    camera_.onMouseMove(pos_x, pos_y);
}

void VulkanSkyboxPilot::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

bool VulkanSkyboxPilot::childOnWindowSizeChanged() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return true;
    }
    vkDeviceWaitIdle(getVkDevice());

    // TutorialBase::onWindowSizeChanged() calls childClear() unconditionally
    // before this runs, tearing down every Vulkan object this pilot owns,
    // not just the swapchain - everything is rebuilt here in the same order
    // as vk_skybox_pilot_main.cpp's initial setup.
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
    if (!createVertexBuffer()) {
        return false;
    }
    return createIndexBuffer();
}

void VulkanSkyboxPilot::childClear() {
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
    destroyBuffer(index_buffer_);
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

    if (image_parameters_.getVkSampler() != VK_NULL_HANDLE) {
        vkDestroySampler(getVkDevice(), image_parameters_.getVkSampler(), nullptr);
        image_parameters_.setVkSampler(VK_NULL_HANDLE);
    }
    if (image_parameters_.getVkImageView() != VK_NULL_HANDLE) {
        vkDestroyImageView(
                getVkDevice(), image_parameters_.getVkImageView(), nullptr);
        image_parameters_.setVkImageView(VK_NULL_HANDLE);
    }
    if (image_parameters_.getVkImage() != VK_NULL_HANDLE) {
        vkDestroyImage(getVkDevice(), image_parameters_.getVkImage(), nullptr);
        image_parameters_.setVkImage(VK_NULL_HANDLE);
    }
    if (image_parameters_.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(
                getVkDevice(), image_parameters_.getVkDeviceMemory(), nullptr);
        image_parameters_.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

}  // namespace vulkan_graphix
