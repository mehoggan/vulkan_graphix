#ifndef VULKAN_GRAPHIX_VULKANCOMMON_H
#define VULKAN_GRAPHIX_VULKANCOMMON_H

// Shared Vulkan object-creation boilerplate factored out of Tutorial03
// through Tutorial22, which each independently reimplemented buffer/image/
// sampler creation, staging-buffer uploads, command pool/semaphore/fence
// setup, and shader module loading - almost always byte-for-byte identical.
// `TutorialBase` itself only handles instance/device/swapchain bring-up
// (see TutorialBase.h) and has none of this; every tutorial still owns its
// own `BufferParameters`/`ImageParameters` members and calls these classes
// as cheap temporaries from its own thin private create*() wrappers, so
// every tutorial's PUBLIC method names/signatures - and the existing
// fault-injection/integration tests that call them by name - are
// unaffected. These classes hold only a device handle (plus, where a
// memory-type lookup is needed, a physical device handle) - deliberately
// lightweight, meant to be constructed inline at each call site rather
// than stored as a tutorial member, since a tutorial is default-
// constructed before prepareVulkan() runs and getVkDevice() isn't valid
// yet at that point.
//
// None of these log on failure (matching Tools::'s own existing
// convention for free/shared code - see e.g. Tools::getBinaryFileContents,
// which just returns an empty vector rather than logging): a tutorial's
// own thin wrapper method keeps its own Logging::error(LOG_TAG, ...) call,
// since only LoggedClass<T> subclasses (like TutorialBase) have a LOG_TAG.

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/TutorialBase.h"
#include "vulkan_graphix/Tools.h"

namespace vulkan_graphix::VulkanCommon {

// VkBuffer + VkDeviceMemory creation/destruction.
class BufferFactory {
public:
    BufferFactory(VkDevice device, VkPhysicalDevice physical_device);

    // Reads the buffer's size from buffer.getSize(), which the caller must
    // set before calling this (matches every existing tutorial's own
    // createBuffer() convention).
    bool create(VkBufferUsageFlags usage,
               VkMemoryPropertyFlags memory_property,
               BufferParameters& out) const;
    void destroy(BufferParameters& buffer) const;

private:
    bool allocateMemory(VkBuffer buffer,
                        VkMemoryPropertyFlags property,
                        VkDeviceMemory* memory) const;

    VkDevice m_device;
    VkPhysicalDevice m_physical_device;
};

// VkImage + VkImageView + VkSampler + VkDeviceMemory creation/destruction.
// mipLevels/arrayLayers/samples/tiling/sharingMode/initialLayout on
// createImage(), and viewType/components/subresourceRange on
// createImageView(), and every VkSamplerCreateInfo field but the address
// mode on createSampler(), are never parameterized anywhere in the
// existing tutorials - always the same values - so they stay hardcoded
// here exactly as they were in every tutorial's own version.
class ImageFactory {
public:
    ImageFactory(VkDevice device, VkPhysicalDevice physical_device);

    bool createImage(std::uint32_t width,
                     std::uint32_t height,
                     VkFormat format,
                     VkImageUsageFlags usage,
                     VkImage* out) const;
    bool createImageView(VkImage image,
                         VkFormat format,
                         VkImageAspectFlags aspect_mask,
                         VkImageView* out) const;
    bool createSampler(VkSamplerAddressMode address_mode,
                       VkSampler* out) const;

    // Public (unlike BufferFactory's own allocateMemory, which stays
    // private - createBuffer() always does create+allocate+bind as one
    // atomic unit with nothing interleaved) because several tutorials
    // need a view and/or sampler created *between* allocating an
    // image's memory and binding it, so they can't go through a single
    // combined call the way BufferFactory::create() does.
    bool allocateMemory(VkImage image,
                        VkMemoryPropertyFlags property,
                        VkDeviceMemory* memory) const;

    // Also public for the same reason as allocateMemory() above - the
    // view/sampler created between allocating and binding need the image
    // handle, but binding itself is always the same one call.
    bool bindMemory(VkImage image, VkDeviceMemory memory) const;

    // No equivalent existed anywhere before this - every tutorial's own
    // childClear() repeated the same 4-field (sampler->view->image->
    // memory) defensive-destroy block inline, once per image member.
    void destroy(ImageParameters& image) const;

private:
    VkDevice m_device;
    VkPhysicalDevice m_physical_device;
};

// Staged (staging-buffer + a caller-owned one-shot command buffer) upload
// of pixel data into a device-local image, or raw bytes into a device-
// local buffer - the copyTextureData()/copyBufferData() pattern shared by
// every multi-texture/multi-mesh tutorial. Fully synchronous (submits and
// vkDeviceWaitIdle()s before returning), matching every existing call
// site - never pipelined/double-buffered. staging_buffer is a per-call
// parameter, not constructor state, since every tutorial reuses one
// staging buffer sequentially across many unrelated uploads.
class StagedUploader {
public:
    StagedUploader(VkDevice device,
                   VkQueue graphics_queue,
                   VkCommandBuffer upload_command_buffer);

    bool uploadToImage(BufferParameters& staging_buffer,
                       VkImage image,
                       const char* data,
                       std::uint32_t data_size,
                       std::uint32_t width,
                       std::uint32_t height) const;
    bool uploadToBuffer(BufferParameters& staging_buffer,
                        BufferParameters& destination,
                        const void* data,
                        std::uint32_t data_size,
                        VkAccessFlags dst_access_mask,
                        VkPipelineStageFlags dst_stage_mask) const;

private:
    VkDevice m_device;
    VkQueue m_graphics_queue;
    VkCommandBuffer m_upload_command_buffer;
};

// Command pool/command buffer allocation and per-frame sync object
// (semaphore/fence) creation - the primitives every tutorial's own
// createCommandPool()/allocateCommandBuffers()/createSemaphores()/
// createFences() build on. The looping/storage logic in those methods
// (how many semaphores, indexed by which tutorial-owned array) stays in
// each tutorial, since that's real per-tutorial orchestration, not
// duplicated boilerplate - only the single-object creation call here.
class FrameResourceFactory {
public:
    explicit FrameResourceFactory(VkDevice device);

    bool createCommandPool(std::uint32_t queue_family_index,
                           VkCommandPool* out) const;
    bool allocateCommandBuffers(VkCommandPool pool,
                                std::uint32_t count,
                                VkCommandBuffer* out) const;
    bool createSemaphore(VkSemaphore* out) const;
    bool createFence(bool signaled, VkFence* out) const;

private:
    VkDevice m_device;
};

// Composes ImageFactory + StagedUploader for the create-image-view-
// sampler-then-upload-pixels pattern several tutorials (17/18/19/21/22)
// each already have their own near-identical private
// createTextureFromPixels() for.
bool createTextureFromPixels(const ImageFactory& image_factory,
                             const StagedUploader& uploader,
                             BufferParameters& staging_buffer,
                             std::uint32_t width,
                             std::uint32_t height,
                             const std::vector<char>& pixels,
                             VkSamplerAddressMode address_mode,
                             ImageParameters& out);

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
createShaderModule(VkDevice device, const char* filename);

}  // namespace vulkan_graphix::VulkanCommon

#endif  // VULKAN_GRAPHIX_VULKANCOMMON_H
