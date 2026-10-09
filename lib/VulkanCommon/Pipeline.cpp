#include "vulkan_graphix/VulkanCommon/Pipeline.h"

#include <array>
#include <cstdint>

#include "vulkan_graphix/VulkanCommon.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix::VulkanCommon {

bool createRenderPass(
    VkDevice device,
    const RenderPassDescription& description,
    VkRenderPass* out) {
  std::vector<VkAttachmentDescription> attachments = {
      {.flags = 0,
       .format = description.m_color_format,
       .samples = VK_SAMPLE_COUNT_1_BIT,
       .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
       .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
       .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
       .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
       .initialLayout = description.m_color_initial_layout,
       .finalLayout = description.m_color_final_layout}};
  if (description.m_depth_format) {
    attachments.push_back(
        {.flags = 0,
         .format = *description.m_depth_format,
         .samples = VK_SAMPLE_COUNT_1_BIT,
         .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
         .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
         .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
         .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
         .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL});
  }

  const VkAttachmentReference color_reference = {
      .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
  const VkAttachmentReference depth_reference = {
      .attachment = 1,
      .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
  const VkSubpassDescription subpass = {
      .flags = 0,
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .inputAttachmentCount = 0,
      .pInputAttachments = nullptr,
      .colorAttachmentCount = 1,
      .pColorAttachments = &color_reference,
      .pResolveAttachments = nullptr,
      .pDepthStencilAttachment =
          description.m_depth_format ? &depth_reference : nullptr,
      .preserveAttachmentCount = 0,
      .pPreserveAttachments = nullptr};

  const VkRenderPassCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .attachmentCount = static_cast<std::uint32_t>(attachments.size()),
      .pAttachments = attachments.data(),
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount =
          static_cast<std::uint32_t>(description.m_dependencies.size()),
      .pDependencies = description.m_dependencies.empty()
          ? nullptr
          : description.m_dependencies.data()};
  return vkCreateRenderPass(device, &create_info, nullptr, out) == VK_SUCCESS;
}

bool createPipelineLayout(
    VkDevice device,
    const std::vector<VkDescriptorSetLayout>& set_layouts,
    const std::vector<VkPushConstantRange>& push_constant_ranges,
    VkPipelineLayout* out) {
  const VkPipelineLayoutCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .setLayoutCount = static_cast<std::uint32_t>(set_layouts.size()),
      .pSetLayouts = set_layouts.empty() ? nullptr : set_layouts.data(),
      .pushConstantRangeCount =
          static_cast<std::uint32_t>(push_constant_ranges.size()),
      .pPushConstantRanges = push_constant_ranges.empty()
          ? nullptr
          : push_constant_ranges.data()};
  return vkCreatePipelineLayout(device, &create_info, nullptr, out) ==
      VK_SUCCESS;
}

VkPipelineColorBlendAttachmentState opaqueBlend() {
  return {
      .blendEnable = VK_FALSE,
      .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .alphaBlendOp = VK_BLEND_OP_ADD,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
}

VkPipelineColorBlendAttachmentState alphaBlend() {
  VkPipelineColorBlendAttachmentState blend = opaqueBlend();
  blend.blendEnable = VK_TRUE;
  blend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  blend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  return blend;
}

bool createGraphicsPipeline(
    VkDevice device,
    const GraphicsPipelineDescription& description,
    VkPipeline* out) {
  auto vertex_module =
      createShaderModule(device, description.m_vertex_shader.c_str());
  auto fragment_module =
      createShaderModule(device, description.m_fragment_shader.c_str());
  if (!vertex_module || !fragment_module) {
    return false;
  }
  const std::array<VkPipelineShaderStageCreateInfo, 2> stages = {{
      {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
       .pNext = nullptr,
       .flags = 0,
       .stage = VK_SHADER_STAGE_VERTEX_BIT,
       .module = vertex_module.get(),
       .pName = "main",
       .pSpecializationInfo = nullptr},
      {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
       .pNext = nullptr,
       .flags = 0,
       .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
       .module = fragment_module.get(),
       .pName = "main",
       .pSpecializationInfo = nullptr},
  }};

  const VkVertexInputBindingDescription binding = {
      .binding = 0,
      .stride = description.m_vertex_stride,
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
  const VkPipelineVertexInputStateCreateInfo vertex_input = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &binding,
      .vertexAttributeDescriptionCount =
          static_cast<std::uint32_t>(description.m_vertex_attributes.size()),
      .pVertexAttributeDescriptions = description.m_vertex_attributes.data()};

  const VkPipelineInputAssemblyStateCreateInfo input_assembly = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .topology = description.m_topology,
      .primitiveRestartEnable = VK_FALSE};

  const VkPipelineViewportStateCreateInfo viewport_state = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .viewportCount = 1,
      .pViewports = nullptr,
      .scissorCount = 1,
      .pScissors = nullptr};

  const VkPipelineRasterizationStateCreateInfo rasterization = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = description.m_polygon_mode,
      .cullMode = description.m_cull_mode,
      .frontFace = description.m_front_face,
      .depthBiasEnable = VK_FALSE,
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f,
      .lineWidth = 1.0f};

  const VkPipelineMultisampleStateCreateInfo multisample = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 1.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE};

  VkPipelineDepthStencilStateCreateInfo depth_stencil = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .depthTestEnable = VK_FALSE,
      .depthWriteEnable = VK_FALSE,
      .depthCompareOp = VK_COMPARE_OP_ALWAYS,
      .depthBoundsTestEnable = VK_FALSE,
      .stencilTestEnable = VK_FALSE,
      .front = {},
      .back = {},
      .minDepthBounds = 0.0f,
      .maxDepthBounds = 1.0f};
  if (description.m_depth) {
    depth_stencil.depthTestEnable =
        description.m_depth->m_test ? VK_TRUE : VK_FALSE;
    depth_stencil.depthWriteEnable =
        description.m_depth->m_write ? VK_TRUE : VK_FALSE;
    depth_stencil.depthCompareOp = description.m_depth->m_compare;
  }

  const VkPipelineColorBlendStateCreateInfo color_blend = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,
      .attachmentCount = 1,
      .pAttachments = &description.m_blend,
      .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};

  const VkPipelineDynamicStateCreateInfo dynamic_state = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .dynamicStateCount =
          static_cast<std::uint32_t>(description.m_dynamic_states.size()),
      .pDynamicStates = description.m_dynamic_states.data()};

  const VkGraphicsPipelineCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stageCount = static_cast<std::uint32_t>(stages.size()),
      .pStages = stages.data(),
      .pVertexInputState = &vertex_input,
      .pInputAssemblyState = &input_assembly,
      .pTessellationState = nullptr,
      .pViewportState = &viewport_state,
      .pRasterizationState = &rasterization,
      .pMultisampleState = &multisample,
      .pDepthStencilState = description.m_depth ? &depth_stencil : nullptr,
      .pColorBlendState = &color_blend,
      .pDynamicState = &dynamic_state,
      .layout = description.m_layout,
      .renderPass = description.m_render_pass,
      .subpass = 0,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1};
  return vkCreateGraphicsPipelines(
             device, VK_NULL_HANDLE, 1, &create_info, nullptr, out) ==
      VK_SUCCESS;
}

}  // namespace vulkan_graphix::VulkanCommon
