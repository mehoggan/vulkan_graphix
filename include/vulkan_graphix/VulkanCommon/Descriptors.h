#ifndef VULKAN_GRAPHIX_VULKANCOMMON_DESCRIPTORS_H
#define VULKAN_GRAPHIX_VULKANCOMMON_DESCRIPTORS_H

// Descriptor set layouts, pools, sets, and the two kinds of writes the
// renderers here use (a combined image sampler, a uniform buffer), from a
// list of bindings instead of hand-filled VkXxxCreateInfo structs.

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_graphix/Tutorial/TutorialBase.h"

namespace vulkan_graphix::VulkanCommon {

// One descriptor (count 1) at a binding number.
struct DescriptorBinding {
  std::uint32_t m_binding;
  VkDescriptorType m_type;
  VkShaderStageFlags m_stages;
};

bool createDescriptorSetLayout(
    VkDevice device,
    const std::vector<DescriptorBinding>& bindings,
    VkDescriptorSetLayout* out);

// A pool with room for set_count sets laid out as bindings.
bool createDescriptorPool(
    VkDevice device,
    const std::vector<DescriptorBinding>& bindings,
    std::uint32_t set_count,
    VkDescriptorPool* out);

bool allocateDescriptorSet(
    VkDevice device,
    VkDescriptorPool pool,
    VkDescriptorSetLayout layout,
    VkDescriptorSet* out);

// image's view and sampler, read as SHADER_READ_ONLY_OPTIMAL.
void writeImageDescriptor(
    VkDevice device,
    VkDescriptorSet set,
    std::uint32_t binding,
    const ImageParameters& image);

// All of buffer (its getSize() bytes).
void writeUniformBufferDescriptor(
    VkDevice device,
    VkDescriptorSet set,
    std::uint32_t binding,
    const BufferParameters& buffer);

}  // namespace vulkan_graphix::VulkanCommon

#endif  // VULKAN_GRAPHIX_VULKANCOMMON_DESCRIPTORS_H
