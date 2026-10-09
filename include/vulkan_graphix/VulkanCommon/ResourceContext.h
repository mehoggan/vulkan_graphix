#ifndef VULKAN_GRAPHIX_VULKANCOMMON_RESOURCECONTEXT_H
#define VULKAN_GRAPHIX_VULKANCOMMON_RESOURCECONTEXT_H

// Creates the GPU objects a renderer built on TutorialBase needs - device-
// local vertex/index buffers uploaded through a staging copy, host-visible
// uniform buffers, textures from files or pixels, depth images, render
// passes, pipelines, descriptor objects - and remembers each one, so a
// single releaseAll() (newest first) replaces a hand-written teardown of
// every handle. Uploads use a command pool and buffer of the context's own
// and a staging buffer sized to each upload.

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/Tutorial/TutorialBase.h"
#include "vulkan_graphix/VulkanCommon/Descriptors.h"
#include "vulkan_graphix/VulkanCommon/Pipeline.h"

namespace vulkan_graphix::VulkanCommon {

class ResourceContext {
public:
  ResourceContext() = default;
  ~ResourceContext();

  ResourceContext(const ResourceContext&) = delete;
  ResourceContext& operator=(const ResourceContext&) = delete;

  // Binds to base's device and graphics queue.
  bool initialize(const TutorialBase& base);
  // Waits for the device to go idle, then destroys everything created
  // through this context, newest first. The context can be reused.
  void releaseAll();

  VkDevice device() const;

  // A device-local buffer holding bytes (usage gains TRANSFER_DST).
  bool createDeviceLocalBuffer(
      std::span<const std::byte> bytes,
      VkBufferUsageFlags usage,
      BufferParameters& out);
  template <typename T>
  bool createDeviceLocalBuffer(
      const std::vector<T>& data,
      VkBufferUsageFlags usage,
      BufferParameters& out) {
    return createDeviceLocalBuffer(
        std::as_bytes(std::span<const T>(data)), usage, out);
  }
  // Host-visible and coherent, size bytes; fill it with writeBuffer() -
  // for data rewritten every frame (uniforms, per-frame UI geometry).
  bool createHostVisibleBuffer(
      std::uint32_t size, VkBufferUsageFlags usage, BufferParameters& out);
  bool createUniformBuffer(std::uint32_t size, BufferParameters& out);
  bool writeBuffer(
      const BufferParameters& buffer, std::span<const std::byte> bytes) const;
  template <typename T>
  bool writeBuffer(
      const BufferParameters& buffer, const std::vector<T>& values) const {
    return writeBuffer(buffer, std::as_bytes(std::span<const T>(values)));
  }
  template <typename T>
  bool writeBuffer(const BufferParameters& buffer, const T& value) const {
    static_assert(
        std::is_trivially_copyable_v<T>,
        "writeBuffer() copies a value's bytes");
    return writeBuffer(
        buffer,
        std::span<const std::byte>(
            reinterpret_cast<const std::byte*>(&value), sizeof(T)));
  }

  // An RGBA8 texture with its view and sampler.
  bool createTexture(
      std::uint32_t width,
      std::uint32_t height,
      const std::vector<char>& rgba_pixels,
      VkSamplerAddressMode address_mode,
      ImageParameters& out);
  // A .png/.jpg (Tools::getImageData()) as an RGBA8 texture.
  bool loadTexture(
      const std::string& filename,
      VkSamplerAddressMode address_mode,
      ImageParameters& out);
  // A raw RGB file (Tools::getRawImageData()) as an RGBA8 texture.
  bool loadRawTexture(
      const std::string& filename,
      std::uint32_t width,
      std::uint32_t height,
      VkSamplerAddressMode address_mode,
      ImageParameters& out);
  // A depth attachment (image, memory, view) of extent.
  bool createDepthImage(
      VkExtent2D extent, VkFormat format, ImageParameters& out);

  bool createRenderPass(
      const RenderPassDescription& description, VkRenderPass* out);
  bool createPipelineLayout(
      const std::vector<VkDescriptorSetLayout>& set_layouts,
      const std::vector<VkPushConstantRange>& push_constant_ranges,
      VkPipelineLayout* out);
  bool createGraphicsPipeline(
      const GraphicsPipelineDescription& description, VkPipeline* out);
  bool createDescriptorSetLayout(
      const std::vector<DescriptorBinding>& bindings,
      VkDescriptorSetLayout* out);
  bool createDescriptorPool(
      const std::vector<DescriptorBinding>& bindings,
      std::uint32_t set_count,
      VkDescriptorPool* out);

private:
  bool ensureUploadCommandBuffer();
  void track(std::function<void()> release);

  VkDevice m_device = VK_NULL_HANDLE;
  VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
  VkQueue m_graphics_queue = VK_NULL_HANDLE;
  std::uint32_t m_graphics_family = 0;
  VkCommandPool m_upload_pool = VK_NULL_HANDLE;
  VkCommandBuffer m_upload_command_buffer = VK_NULL_HANDLE;
  std::vector<std::function<void()>> m_releases;
};

}  // namespace vulkan_graphix::VulkanCommon

#endif  // VULKAN_GRAPHIX_VULKANCOMMON_RESOURCECONTEXT_H
