#include "vulkan_graphix/Tutorial05.h"

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <cstring>

#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

VulkanTutorial05Parameters::VulkanTutorial05Parameters()
        : m_vk_render_pass(VK_NULL_HANDLE)
        , m_vk_graphics_pipeline(VK_NULL_HANDLE)
        , m_vk_command_pool(VK_NULL_HANDLE)
        , m_rendering_resources(resources_count) {}

const VkRenderPass& VulkanTutorial05Parameters::getVkRenderPass() const {
    return m_vk_render_pass;
}
VkRenderPass& VulkanTutorial05Parameters::getVkRenderPass() {
    return m_vk_render_pass;
}
void VulkanTutorial05Parameters::setVkRenderPass(
        const VkRenderPass& vk_render_pass) {
    m_vk_render_pass = vk_render_pass;
}

const VkPipeline& VulkanTutorial05Parameters::getVkGraphicsPipeline() const {
    return m_vk_graphics_pipeline;
}
VkPipeline& VulkanTutorial05Parameters::getVkGraphicsPipeline() {
    return m_vk_graphics_pipeline;
}
void VulkanTutorial05Parameters::setVkGraphicsPipeline(
        const VkPipeline& vk_graphics_pipeline) {
    m_vk_graphics_pipeline = vk_graphics_pipeline;
}

const BufferParameters& VulkanTutorial05Parameters::getVertexBufferParameters()
        const {
    return m_vertex_buffer;
}
BufferParameters& VulkanTutorial05Parameters::getVertexBufferParameters() {
    return m_vertex_buffer;
}
void VulkanTutorial05Parameters::setVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_vertex_buffer = vertex_buffer;
}

const BufferParameters&
VulkanTutorial05Parameters::getStagingBufferParameters() const {
    return m_staging_buffer;
}
BufferParameters& VulkanTutorial05Parameters::getStagingBufferParameters() {
    return m_staging_buffer;
}
void VulkanTutorial05Parameters::setStagingBufferParameters(
        const BufferParameters& staging_buffer) {
    m_staging_buffer = staging_buffer;
}

const VkCommandPool& VulkanTutorial05Parameters::getVkCommandPool() const {
    return m_vk_command_pool;
}
VkCommandPool& VulkanTutorial05Parameters::getVkCommandPool() {
    return m_vk_command_pool;
}
void VulkanTutorial05Parameters::setVkCommandPool(
        const VkCommandPool& vk_command_pool) {
    m_vk_command_pool = vk_command_pool;
}

const std::vector<RenderingResourceParameters>&
VulkanTutorial05Parameters::getRenderingResources() const {
    return m_rendering_resources;
}
std::vector<RenderingResourceParameters>&
VulkanTutorial05Parameters::getRenderingResources() {
    return m_rendering_resources;
}
void VulkanTutorial05Parameters::setRenderingResources(
        const std::vector<RenderingResourceParameters>& rendering_resources) {
    m_rendering_resources = rendering_resources;
}

const std::vector<VkSemaphore>&
VulkanTutorial05Parameters::getFinishedRenderingSemaphores() const {
    return m_finished_rendering_semaphores;
}
std::vector<VkSemaphore>&
VulkanTutorial05Parameters::getFinishedRenderingSemaphores() {
    return m_finished_rendering_semaphores;
}
void VulkanTutorial05Parameters::setFinishedRenderingSemaphores(
        const std::vector<VkSemaphore>& finished_rendering_semaphores) {
    m_finished_rendering_semaphores = finished_rendering_semaphores;
}

Tutorial05::Tutorial05() = default;

Tutorial05::~Tutorial05() { childClear(); }

bool Tutorial05::createRenderPass() {
    VkAttachmentDescription attachment_descriptions[] = {
            {.flags = 0,
             .format = getSwapchainParameters().getVkFormat(),
             .samples = VK_SAMPLE_COUNT_1_BIT,
             .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
             .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
             .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
             .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
             .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
             .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR}};

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

    std::vector<VkSubpassDependency> dependencies = {
            {.srcSubpass = VK_SUBPASS_EXTERNAL,
             .dstSubpass = 0,
             .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
             .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
             .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
             .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
             .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT},
            {.srcSubpass = 0,
             .dstSubpass = VK_SUBPASS_EXTERNAL,
             .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
             .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
             .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
             .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
             .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT}};

    VkRenderPassCreateInfo render_pass_create_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .attachmentCount = 1,
            .pAttachments = attachment_descriptions,
            .subpassCount = 1,
            .pSubpasses = subpass_descriptions,
            .dependencyCount = static_cast<std::uint32_t>(dependencies.size()),
            .pDependencies = dependencies.data()};

    if (vkCreateRenderPass(
                getVkDevice(),
                &render_pass_create_info,
                nullptr,
                &m_vulkan_tutorial05_parameters.getVkRenderPass()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
Tutorial05::createShaderModule(const char* filename) {
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

Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>
Tutorial05::createPipelineLayout() {
    VkPipelineLayoutCreateInfo layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr};

    VkPipelineLayout pipeline_layout;
    if (vkCreatePipelineLayout(getVkDevice(),
                               &layout_create_info,
                               nullptr,
                               &pipeline_layout) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create pipeline layout!");
        return Tools::AutoDeleter<VkPipelineLayout,
                                  PFN_vkDestroyPipelineLayout>();
    }

    return Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>(
            pipeline_layout, vkDestroyPipelineLayout, getVkDevice());
}

bool Tutorial05::createPipeline() {
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module = createShaderModule("shader.05.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module = createShaderModule("shader.05.frag.spv");

    if (!vertex_shader_module || !fragment_shader_module) {
        return false;
    }

    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos = {
            // Vertex shader
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_VERTEX_BIT,
             .module = vertex_shader_module.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr},
            // Fragment shader
            {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
             .module = fragment_shader_module.get(),
             .pName = "main",
             .pSpecializationInfo = nullptr}};

    std::vector<VkVertexInputBindingDescription> vertex_binding_descriptions =
            {{.binding = 0,
              .stride = VertexAttributeTraits::stride,
              .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};

    std::vector<VkVertexInputAttributeDescription>
            vertex_attribute_descriptions = {
                    {.location = 0,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset =
                             offsetof(struct Tutorial05VertexData, position)},
                    {.location = 1,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset = offsetof(struct Tutorial05VertexData, color)}};

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

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
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
            .cullMode = VK_CULL_MODE_BACK_BIT,
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

    Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>
            pipeline_layout = createPipelineLayout();
    if (!pipeline_layout) {
        return false;
    }

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
            .layout = pipeline_layout.get(),
            .renderPass = m_vulkan_tutorial05_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};

    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info,
                nullptr,
                &m_vulkan_tutorial05_parameters.getVkGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create graphics pipeline!");
        return false;
    }
    return true;
}

bool Tutorial05::allocateBufferMemory(VkBuffer buffer,
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

bool Tutorial05::createBuffer(VkBufferUsageFlags usage,
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

bool Tutorial05::createVertexBuffer() {
    const std::vector<Tutorial05VertexData>& vertex_data = getVertexData();

    BufferParameters& vertex_buffer =
            m_vulkan_tutorial05_parameters.getVertexBufferParameters();
    vertex_buffer.setSize(static_cast<std::uint32_t>(vertex_data.size() *
                                                     sizeof(vertex_data[0])));
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      vertex_buffer)) {
        Logging::error(LOG_TAG, "Could not create vertex buffer!");
        return false;
    }

    return true;
}

bool Tutorial05::createStagingBuffer() {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial05_parameters.getStagingBufferParameters();
    staging_buffer.setSize(4000);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

const std::vector<Tutorial05VertexData>& Tutorial05::getVertexData() const {
    static const std::vector<Tutorial05VertexData> vertex_data = {
            {Math::Vec4<float>(-0.7f, -0.7f, 0.0f, 1.0f),
             Math::Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f)},
            {Math::Vec4<float>(-0.7f, 0.7f, 0.0f, 1.0f),
             Math::Vec4<float>(0.0f, 1.0f, 0.0f, 0.0f)},
            {Math::Vec4<float>(0.7f, -0.7f, 0.0f, 1.0f),
             Math::Vec4<float>(0.0f, 0.0f, 1.0f, 0.0f)},
            {Math::Vec4<float>(0.7f, 0.7f, 0.0f, 1.0f),
             Math::Vec4<float>(0.3f, 0.3f, 0.3f, 0.0f)}};

    return vertex_data;
}

bool Tutorial05::copyVertexData() {
    const std::vector<Tutorial05VertexData>& vertex_data = getVertexData();
    BufferParameters& vertex_buffer =
            m_vulkan_tutorial05_parameters.getVertexBufferParameters();
    BufferParameters& staging_buffer =
            m_vulkan_tutorial05_parameters.getStagingBufferParameters();

    void* staging_buffer_memory_pointer;
    if (vkMapMemory(getVkDevice(),
                    staging_buffer.getVkDeviceMemory(),
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &staging_buffer_memory_pointer) != VK_SUCCESS) {
        Logging::error(
                LOG_TAG,
                "Could not map memory and upload data to a staging buffer!");
        return false;
    }

    std::memcpy(staging_buffer_memory_pointer,
                vertex_data.data(),
                vertex_buffer.getSize());

    VkMappedMemoryRange flush_range = {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .pNext = nullptr,
            .memory = staging_buffer.getVkDeviceMemory(),
            .offset = 0,
            .size = VK_WHOLE_SIZE};
    vkFlushMappedMemoryRanges(getVkDevice(), 1, &flush_range);

    vkUnmapMemory(getVkDevice(), staging_buffer.getVkDeviceMemory());

    VkCommandBufferBeginInfo command_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr};

    VkCommandBuffer command_buffer =
            m_vulkan_tutorial05_parameters.getRenderingResources()[0]
                    .getVkCommandBuffer();

    vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

    VkBufferCopy buffer_copy_info = {
            .srcOffset = 0, .dstOffset = 0, .size = vertex_buffer.getSize()};
    vkCmdCopyBuffer(command_buffer,
                    staging_buffer.getVkBuffer(),
                    vertex_buffer.getVkBuffer(),
                    1,
                    &buffer_copy_info);

    VkBufferMemoryBarrier buffer_memory_barrier = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = vertex_buffer.getVkBuffer(),
            .offset = 0,
            .size = VK_WHOLE_SIZE};
    vkCmdPipelineBarrier(command_buffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
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

bool Tutorial05::createCommandPool(std::uint32_t queue_family_index,
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

bool Tutorial05::allocateCommandBuffers(VkCommandPool pool,
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

bool Tutorial05::createCommandBuffers() {
    if (!createCommandPool(
                getGraphicsQueueParameters().getFamilyIndex(),
                &m_vulkan_tutorial05_parameters.getVkCommandPool())) {
        return false;
    }

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial05_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!allocateCommandBuffers(
                    m_vulkan_tutorial05_parameters.getVkCommandPool(),
                    1,
                    &rendering_resources[i].getVkCommandBuffer())) {
            return false;
        }
    }
    return true;
}

bool Tutorial05::createSemaphores() {
    VkSemaphoreCreateInfo semaphore_create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0};

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial05_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (vkCreateSemaphore(
                    getVkDevice(),
                    &semaphore_create_info,
                    nullptr,
                    &rendering_resources[i].getImageAvailableVkSemaphore()) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    // A "finished rendering" semaphore must be indexed by the acquired
    // swapchain image, not by the rendering-resource slot: the fence below
    // only guarantees the GPU has finished waiting on/submitting with a
    // resource slot's semaphores, not that the presentation engine has
    // finished consuming the *previous* signal on that same semaphore
    // object, since present completion isn't tracked by any fence here. See
    // https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html
    std::vector<VkSemaphore>& finished_rendering_semaphores =
            m_vulkan_tutorial05_parameters.getFinishedRenderingSemaphores();
    finished_rendering_semaphores.assign(
            getSwapchainParameters().getImageParameters().size(),
            VK_NULL_HANDLE);
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (vkCreateSemaphore(getVkDevice(),
                              &semaphore_create_info,
                              nullptr,
                              &finished_rendering_semaphores[i]) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    return true;
}

bool Tutorial05::createFences() {
    VkFenceCreateInfo fence_create_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial05_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (vkCreateFence(getVkDevice(),
                          &fence_create_info,
                          nullptr,
                          &rendering_resources[i].getVkFence()) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not create a fence!");
            return false;
        }
    }
    return true;
}

bool Tutorial05::createRenderingResources() {
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

bool Tutorial05::createFramebuffer(VkFramebuffer& framebuffer,
                                   VkImageView image_view) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(getVkDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_vulkan_tutorial05_parameters.getVkRenderPass(),
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

bool Tutorial05::prepareFrame(VkCommandBuffer command_buffer,
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

    if (getPresentQueueParameters().getVkQueue() !=
        getGraphicsQueueParameters().getVkQueue()) {
        VkImageMemoryBarrier barrier_from_present_to_draw = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .srcQueueFamilyIndex =
                        getPresentQueueParameters().getFamilyIndex(),
                .dstQueueFamilyIndex =
                        getGraphicsQueueParameters().getFamilyIndex(),
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
    }

    VkClearValue clear_value = {
            .color = {.float32 = {1.0f, 0.8f, 0.4f, 0.0f}}};

    VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = m_vulkan_tutorial05_parameters.getVkRenderPass(),
            .framebuffer = framebuffer,
            .renderArea = {.offset = {.x = 0, .y = 0},
                           .extent = getSwapchainParameters().getVkExtent2d()},
            .clearValueCount = 1,
            .pClearValues = &clear_value};

    vkCmdBeginRenderPass(command_buffer,
                         &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command_buffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_vulkan_tutorial05_parameters.getVkGraphicsPipeline());

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
            command_buffer,
            0,
            1,
            &m_vulkan_tutorial05_parameters.getVertexBufferParameters()
                     .getVkBuffer(),
            &offset);

    vkCmdDraw(command_buffer, 4, 1, 0, 0);

    vkCmdEndRenderPass(command_buffer);

    if (getGraphicsQueueParameters().getVkQueue() !=
        getPresentQueueParameters().getVkQueue()) {
        VkImageMemoryBarrier barrier_from_draw_to_present = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .srcQueueFamilyIndex =
                        getGraphicsQueueParameters().getFamilyIndex(),
                .dstQueueFamilyIndex =
                        getPresentQueueParameters().getFamilyIndex(),
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
    }

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not record command buffer!");
        return false;
    }
    return true;
}

bool Tutorial05::draw() {
    static std::size_t resource_index = 0;
    RenderingResourceParameters& current_rendering_resource =
            m_vulkan_tutorial05_parameters
                    .getRenderingResources()[resource_index];
    VkSwapchainKHR swap_chain = getSwapchainParameters().getVkSwapchainKhr();
    std::uint32_t image_index;

    resource_index =
            (resource_index + 1) % VulkanTutorial05Parameters::resources_count;

    if (vkWaitForFences(getVkDevice(),
                        1,
                        &current_rendering_resource.getVkFence(),
                        VK_FALSE,
                        1000000000) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Waiting for fence takes too long!");
        return false;
    }
    vkResetFences(getVkDevice(), 1, &current_rendering_resource.getVkFence());

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
            m_vulkan_tutorial05_parameters
                    .getFinishedRenderingSemaphores()[image_index];

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

void Tutorial05::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

void Tutorial05::childClear() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return;
    }
    vkDeviceWaitIdle(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial05_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (rendering_resources[i].getVkFramebuffer() != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(getVkDevice(),
                                 rendering_resources[i].getVkFramebuffer(),
                                 nullptr);
            rendering_resources[i].setVkFramebuffer(VK_NULL_HANDLE);
        }
        if (rendering_resources[i].getVkCommandBuffer() != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(
                    getVkDevice(),
                    m_vulkan_tutorial05_parameters.getVkCommandPool(),
                    1,
                    &rendering_resources[i].getVkCommandBuffer());
        }
        if (rendering_resources[i].getImageAvailableVkSemaphore() !=
            VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(),
                    rendering_resources[i].getImageAvailableVkSemaphore(),
                    nullptr);
        }
        if (rendering_resources[i].getVkFence() != VK_NULL_HANDLE) {
            vkDestroyFence(getVkDevice(),
                           rendering_resources[i].getVkFence(),
                           nullptr);
        }
    }

    std::vector<VkSemaphore>& finished_rendering_semaphores =
            m_vulkan_tutorial05_parameters.getFinishedRenderingSemaphores();
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (finished_rendering_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(), finished_rendering_semaphores[i], nullptr);
        }
    }
    finished_rendering_semaphores.clear();

    if (m_vulkan_tutorial05_parameters.getVkCommandPool() != VK_NULL_HANDLE) {
        vkDestroyCommandPool(getVkDevice(),
                             m_vulkan_tutorial05_parameters.getVkCommandPool(),
                             nullptr);
        m_vulkan_tutorial05_parameters.getVkCommandPool() = VK_NULL_HANDLE;
    }

    destroyBuffer(m_vulkan_tutorial05_parameters.getVertexBufferParameters());
    destroyBuffer(m_vulkan_tutorial05_parameters.getStagingBufferParameters());

    if (m_vulkan_tutorial05_parameters.getVkGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial05_parameters.getVkGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial05_parameters.getVkGraphicsPipeline() =
                VK_NULL_HANDLE;
    }

    if (m_vulkan_tutorial05_parameters.getVkRenderPass() != VK_NULL_HANDLE) {
        vkDestroyRenderPass(getVkDevice(),
                            m_vulkan_tutorial05_parameters.getVkRenderPass(),
                            nullptr);
        m_vulkan_tutorial05_parameters.getVkRenderPass() = VK_NULL_HANDLE;
    }
}

bool Tutorial05::childOnWindowSizeChanged() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return true;
    }
    vkDeviceWaitIdle(getVkDevice());

    // TutorialBase::onWindowSizeChanged() calls childClear() unconditionally
    // before this runs, which tears down every Vulkan object this tutorial
    // owns (fences, semaphores, command pool, pipeline, render pass, vertex
    // and staging buffers) - not just the swapchain. Everything has to be
    // rebuilt here, in the same order as Tutorial05Main.cpp's initial
    // setup (rendering resources first, since copyVertexData() borrows
    // rendering_resources[0]'s command buffer).
    if (!createRenderingResources()) {
        return false;
    }
    if (!createRenderPass()) {
        return false;
    }
    if (!createPipeline()) {
        return false;
    }
    if (!createVertexBuffer()) {
        return false;
    }
    if (!createStagingBuffer()) {
        return false;
    }
    return copyVertexData();
}

}  // namespace vulkan_graphix
