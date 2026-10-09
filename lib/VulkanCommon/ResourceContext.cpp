#include "vulkan_graphix/VulkanCommon/ResourceContext.h"

#include <cstring>
#include <utility>

#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/VulkanCommon.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix::VulkanCommon {

ResourceContext::~ResourceContext() { releaseAll(); }

bool ResourceContext::initialize(const TutorialBase& base) {
  releaseAll();
  m_device = base.getVkDevice();
  m_physical_device = base.getVkPhysicalDevice();
  m_graphics_queue = base.getGraphicsQueueParameters().getVkQueue();
  m_graphics_family = base.getGraphicsQueueParameters().getFamilyIndex();
  return m_device != VK_NULL_HANDLE;
}

void ResourceContext::releaseAll() {
  if (m_device == VK_NULL_HANDLE) {
    return;
  }
  vkDeviceWaitIdle(m_device);
  while (!m_releases.empty()) {
    m_releases.back()();
    m_releases.pop_back();
  }
  m_upload_pool = VK_NULL_HANDLE;
  m_upload_command_buffer = VK_NULL_HANDLE;
}

VkDevice ResourceContext::device() const { return m_device; }

void ResourceContext::track(std::function<void()> release) {
  m_releases.push_back(std::move(release));
}

bool ResourceContext::ensureUploadCommandBuffer() {
  if (m_upload_command_buffer != VK_NULL_HANDLE) {
    return true;
  }
  const FrameResourceFactory factory(m_device);
  if (!factory.createCommandPool(m_graphics_family, &m_upload_pool)) {
    return false;
  }
  const VkDevice device = m_device;
  const VkCommandPool pool = m_upload_pool;
  track([device, pool] { vkDestroyCommandPool(device, pool, nullptr); });
  return factory.allocateCommandBuffers(
      m_upload_pool, 1, &m_upload_command_buffer);
}

bool ResourceContext::createDeviceLocalBuffer(
    std::span<const std::byte> bytes,
    VkBufferUsageFlags usage,
    BufferParameters& out) {
  if (!ensureUploadCommandBuffer()) {
    return false;
  }
  const BufferFactory factory(m_device, m_physical_device);
  out.setSize(static_cast<std::uint32_t>(bytes.size()));
  if (!factory.create(
          usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
          out)) {
    return false;
  }
  const VkDevice device = m_device;
  const VkPhysicalDevice physical_device = m_physical_device;
  BufferParameters created = out;
  track([device, physical_device, created]() mutable {
    BufferFactory(device, physical_device).destroy(created);
  });

  BufferParameters staging;
  staging.setSize(static_cast<std::uint32_t>(bytes.size()));
  if (!factory.create(
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
          staging)) {
    return false;
  }
  VkAccessFlags access = 0;
  if ((usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) != 0) {
    access |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
  }
  if ((usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) != 0) {
    access |= VK_ACCESS_INDEX_READ_BIT;
  }
  if ((usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) != 0) {
    access |= VK_ACCESS_UNIFORM_READ_BIT;
  }
  const bool uploaded =
      StagedUploader(m_device, m_graphics_queue, m_upload_command_buffer)
          .uploadToBuffer(
              staging,
              out,
              bytes.data(),
              static_cast<std::uint32_t>(bytes.size()),
              access,
              VK_PIPELINE_STAGE_VERTEX_INPUT_BIT |
                  VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
  factory.destroy(staging);
  return uploaded;
}

bool ResourceContext::createHostVisibleBuffer(
    std::uint32_t size, VkBufferUsageFlags usage, BufferParameters& out) {
  const BufferFactory factory(m_device, m_physical_device);
  out.setSize(size);
  if (!factory.create(
          usage,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
          out)) {
    return false;
  }
  const VkDevice device = m_device;
  const VkPhysicalDevice physical_device = m_physical_device;
  BufferParameters created = out;
  track([device, physical_device, created]() mutable {
    BufferFactory(device, physical_device).destroy(created);
  });
  return true;
}

bool ResourceContext::createUniformBuffer(
    std::uint32_t size, BufferParameters& out) {
  return createHostVisibleBuffer(
      size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, out);
}

bool ResourceContext::writeBuffer(
    const BufferParameters& buffer, std::span<const std::byte> bytes) const {
  if (bytes.size() > buffer.getSize()) {
    return false;
  }
  void* mapped = nullptr;
  if (vkMapMemory(
          m_device,
          buffer.getVkDeviceMemory(),
          0,
          VK_WHOLE_SIZE,
          0,
          &mapped) != VK_SUCCESS) {
    return false;
  }
  std::memcpy(mapped, bytes.data(), bytes.size());
  vkUnmapMemory(m_device, buffer.getVkDeviceMemory());
  return true;
}

bool ResourceContext::createTexture(
    std::uint32_t width,
    std::uint32_t height,
    const std::vector<char>& rgba_pixels,
    VkSamplerAddressMode address_mode,
    ImageParameters& out) {
  if (!ensureUploadCommandBuffer()) {
    return false;
  }
  const ImageFactory image_factory(m_device, m_physical_device);
  const BufferFactory buffer_factory(m_device, m_physical_device);
  BufferParameters staging;
  staging.setSize(static_cast<std::uint32_t>(rgba_pixels.size()));
  if (!buffer_factory.create(
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
          staging)) {
    return false;
  }
  const bool created = VulkanCommon::createTextureFromPixels(
      image_factory,
      StagedUploader(m_device, m_graphics_queue, m_upload_command_buffer),
      staging,
      width,
      height,
      rgba_pixels,
      address_mode,
      out);
  buffer_factory.destroy(staging);
  // Whatever got created is released even if a later step failed.
  const VkDevice device = m_device;
  const VkPhysicalDevice physical_device = m_physical_device;
  ImageParameters image = out;
  track([device, physical_device, image]() mutable {
    ImageFactory(device, physical_device).destroy(image);
  });
  return created;
}

bool ResourceContext::loadTexture(
    const std::string& filename,
    VkSamplerAddressMode address_mode,
    ImageParameters& out) {
  std::int32_t width = 0;
  std::int32_t height = 0;
  std::int32_t data_size = 0;
  const std::vector<char> pixels =
      Tools::getImageData(filename, 4, &width, &height, nullptr, &data_size);
  if (pixels.empty()) {
    return false;
  }
  return createTexture(
      static_cast<std::uint32_t>(width),
      static_cast<std::uint32_t>(height),
      pixels,
      address_mode,
      out);
}

bool ResourceContext::loadRawTexture(
    const std::string& filename,
    std::uint32_t width,
    std::uint32_t height,
    VkSamplerAddressMode address_mode,
    ImageParameters& out) {
  const std::vector<char> pixels =
      Tools::getRawImageData(filename, width, height);
  if (pixels.empty()) {
    return false;
  }
  return createTexture(width, height, pixels, address_mode, out);
}

bool ResourceContext::createDepthImage(
    VkExtent2D extent, VkFormat format, ImageParameters& out) {
  const bool created = VulkanCommon::createDepthImage(
      ImageFactory(m_device, m_physical_device), extent, format, out);
  const VkDevice device = m_device;
  const VkPhysicalDevice physical_device = m_physical_device;
  ImageParameters tracked = out;
  track([device, physical_device, tracked]() mutable {
    ImageFactory(device, physical_device).destroy(tracked);
  });
  return created;
}

bool ResourceContext::createRenderPass(
    const RenderPassDescription& description, VkRenderPass* out) {
  if (!VulkanCommon::createRenderPass(m_device, description, out)) {
    return false;
  }
  const VkDevice device = m_device;
  const VkRenderPass render_pass = *out;
  track([device, render_pass] {
    vkDestroyRenderPass(device, render_pass, nullptr);
  });
  return true;
}

bool ResourceContext::createPipelineLayout(
    const std::vector<VkDescriptorSetLayout>& set_layouts,
    const std::vector<VkPushConstantRange>& push_constant_ranges,
    VkPipelineLayout* out) {
  if (!VulkanCommon::createPipelineLayout(
          m_device, set_layouts, push_constant_ranges, out)) {
    return false;
  }
  const VkDevice device = m_device;
  const VkPipelineLayout layout = *out;
  track(
      [device, layout] { vkDestroyPipelineLayout(device, layout, nullptr); });
  return true;
}

bool ResourceContext::createGraphicsPipeline(
    const GraphicsPipelineDescription& description, VkPipeline* out) {
  if (!VulkanCommon::createGraphicsPipeline(m_device, description, out)) {
    return false;
  }
  const VkDevice device = m_device;
  const VkPipeline pipeline = *out;
  track([device, pipeline] { vkDestroyPipeline(device, pipeline, nullptr); });
  return true;
}

bool ResourceContext::createDescriptorSetLayout(
    const std::vector<DescriptorBinding>& bindings,
    VkDescriptorSetLayout* out) {
  if (!VulkanCommon::createDescriptorSetLayout(m_device, bindings, out)) {
    return false;
  }
  const VkDevice device = m_device;
  const VkDescriptorSetLayout layout = *out;
  track([device, layout] {
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
  });
  return true;
}

bool ResourceContext::createDescriptorPool(
    const std::vector<DescriptorBinding>& bindings,
    std::uint32_t set_count,
    VkDescriptorPool* out) {
  if (!VulkanCommon::createDescriptorPool(
          m_device, bindings, set_count, out)) {
    return false;
  }
  const VkDevice device = m_device;
  const VkDescriptorPool pool = *out;
  track([device, pool] { vkDestroyDescriptorPool(device, pool, nullptr); });
  return true;
}

}  // namespace vulkan_graphix::VulkanCommon
