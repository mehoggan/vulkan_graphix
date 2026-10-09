#ifndef VULKAN_GRAPHIX_VULKANCOMMON_PIPELINE_H
#define VULKAN_GRAPHIX_VULKANCOMMON_PIPELINE_H

// Render passes, pipeline layouts, and graphics pipelines from plain
// descriptions - the VkXxxCreateInfo boilerplate every renderer here used
// to spell out field by field. Each description's defaults are what the
// tutorials and Render::Renderer use unless they say otherwise, so a
// caller sets only what's different about its own pipeline.

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace vulkan_graphix::VulkanCommon {

// One subpass drawing into a color attachment (cleared, stored) and, if
// m_depth_format is set, a depth attachment (cleared, not stored).
struct RenderPassDescription {
  VkFormat m_color_format = VK_FORMAT_UNDEFINED;
  std::optional<VkFormat> m_depth_format;
  // The tutorials transition the swapchain image themselves (FrameLoop's
  // barriers), so the pass keeps it in COLOR_ATTACHMENT_OPTIMAL; the
  // Render module lets the pass do it (UNDEFINED -> PRESENT_SRC_KHR).
  VkImageLayout m_color_initial_layout =
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  VkImageLayout m_color_final_layout =
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  std::vector<VkSubpassDependency> m_dependencies;
};

bool createRenderPass(
    VkDevice device,
    const RenderPassDescription& description,
    VkRenderPass* out);

bool createPipelineLayout(
    VkDevice device,
    const std::vector<VkDescriptorSetLayout>& set_layouts,
    const std::vector<VkPushConstantRange>& push_constant_ranges,
    VkPipelineLayout* out);

// Blending off (factors ONE/ZERO, ignored).
VkPipelineColorBlendAttachmentState opaqueBlend();
// Color blended SRC_ALPHA / ONE_MINUS_SRC_ALPHA ("over"); the written alpha
// is the source's own (ONE/ZERO).
VkPipelineColorBlendAttachmentState alphaBlend();

struct DepthState {
  bool m_test = true;
  bool m_write = true;
  VkCompareOp m_compare = VK_COMPARE_OP_LESS;
};

// A vertex + fragment shader pipeline with one vertex binding, one viewport
// and scissor (dynamic), single-sampled, one color attachment.
struct GraphicsPipelineDescription {
  // SPIR-V files, relative to the working directory.
  std::string m_vertex_shader;
  std::string m_fragment_shader;
  std::uint32_t m_vertex_stride = 0;
  std::vector<VkVertexInputAttributeDescription> m_vertex_attributes;
  VkPrimitiveTopology m_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  VkPolygonMode m_polygon_mode = VK_POLYGON_MODE_FILL;
  VkCullModeFlags m_cull_mode = VK_CULL_MODE_NONE;
  VkFrontFace m_front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  // No depth/stencil state at all when unset (a pass without a depth
  // attachment).
  std::optional<DepthState> m_depth;
  VkPipelineColorBlendAttachmentState m_blend = opaqueBlend();
  std::vector<VkDynamicState> m_dynamic_states = {
      VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineLayout m_layout = VK_NULL_HANDLE;
  VkRenderPass m_render_pass = VK_NULL_HANDLE;
};

bool createGraphicsPipeline(
    VkDevice device,
    const GraphicsPipelineDescription& description,
    VkPipeline* out);

}  // namespace vulkan_graphix::VulkanCommon

#endif  // VULKAN_GRAPHIX_VULKANCOMMON_PIPELINE_H
