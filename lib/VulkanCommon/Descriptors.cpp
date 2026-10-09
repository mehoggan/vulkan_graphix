#include "vulkan_graphix/VulkanCommon/Descriptors.h"

#include <map>

#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix::VulkanCommon {

bool createDescriptorSetLayout(
    VkDevice device,
    const std::vector<DescriptorBinding>& bindings,
    VkDescriptorSetLayout* out) {
  std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
  layout_bindings.reserve(bindings.size());
  for (const DescriptorBinding& binding : bindings) {
    layout_bindings.push_back(
        {.binding = binding.m_binding,
         .descriptorType = binding.m_type,
         .descriptorCount = 1,
         .stageFlags = binding.m_stages,
         .pImmutableSamplers = nullptr});
  }
  const VkDescriptorSetLayoutCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .bindingCount = static_cast<std::uint32_t>(layout_bindings.size()),
      .pBindings = layout_bindings.data()};
  return vkCreateDescriptorSetLayout(device, &create_info, nullptr, out) ==
      VK_SUCCESS;
}

bool createDescriptorPool(
    VkDevice device,
    const std::vector<DescriptorBinding>& bindings,
    std::uint32_t set_count,
    VkDescriptorPool* out) {
  std::map<VkDescriptorType, std::uint32_t> counts;
  for (const DescriptorBinding& binding : bindings) {
    counts[binding.m_type] += set_count;
  }
  std::vector<VkDescriptorPoolSize> pool_sizes;
  pool_sizes.reserve(counts.size());
  for (const auto& [type, count] : counts) {
    pool_sizes.push_back({.type = type, .descriptorCount = count});
  }
  const VkDescriptorPoolCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .maxSets = set_count,
      .poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size()),
      .pPoolSizes = pool_sizes.data()};
  return vkCreateDescriptorPool(device, &create_info, nullptr, out) ==
      VK_SUCCESS;
}

bool allocateDescriptorSet(
    VkDevice device,
    VkDescriptorPool pool,
    VkDescriptorSetLayout layout,
    VkDescriptorSet* out) {
  const VkDescriptorSetAllocateInfo allocate_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .pNext = nullptr,
      .descriptorPool = pool,
      .descriptorSetCount = 1,
      .pSetLayouts = &layout};
  return vkAllocateDescriptorSets(device, &allocate_info, out) == VK_SUCCESS;
}

void writeImageDescriptor(
    VkDevice device,
    VkDescriptorSet set,
    std::uint32_t binding,
    const ImageParameters& image) {
  const VkDescriptorImageInfo image_info = {
      .sampler = image.getVkSampler(),
      .imageView = image.getVkImageView(),
      .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
  const VkWriteDescriptorSet write = {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = set,
      .dstBinding = binding,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .pImageInfo = &image_info,
      .pBufferInfo = nullptr,
      .pTexelBufferView = nullptr};
  vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void writeUniformBufferDescriptor(
    VkDevice device,
    VkDescriptorSet set,
    std::uint32_t binding,
    const BufferParameters& buffer) {
  const VkDescriptorBufferInfo buffer_info = {
      .buffer = buffer.getVkBuffer(), .offset = 0, .range = buffer.getSize()};
  const VkWriteDescriptorSet write = {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = set,
      .dstBinding = binding,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .pImageInfo = nullptr,
      .pBufferInfo = &buffer_info,
      .pTexelBufferView = nullptr};
  vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

}  // namespace vulkan_graphix::VulkanCommon
