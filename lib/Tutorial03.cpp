#include "vulkan_graphix/Tutorial03.h"

#include <vulkan/vulkan_core.h>

#include "vulkan_graphix/VulkanCommon.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

VulkanTutorial03Parameters::VulkanTutorial03Parameters()
        : m_vk_render_pass(VK_NULL_HANDLE)
        , m_vk_framebuffers({})
        , m_vk_pipeline(VK_NULL_HANDLE)
        , m_image_available_vk_semaphore(VK_NULL_HANDLE)
        , m_rendering_finished_semaphores({})
        , m_vk_fence(VK_NULL_HANDLE)
        , m_vk_command_pool(VK_NULL_HANDLE)
        , m_vk_command_buffers({}) {}

const VkRenderPass& VulkanTutorial03Parameters::getVkRenderPass() const {
    return m_vk_render_pass;
}
VkRenderPass& VulkanTutorial03Parameters::getVkRenderPass() {
    return m_vk_render_pass;
}
void VulkanTutorial03Parameters::setVkRenderPass(
        const VkRenderPass& vk_render_pass) {
    m_vk_render_pass = vk_render_pass;
}

const std::vector<VkFramebuffer>&
VulkanTutorial03Parameters::getVkFramebuffers() const {
    return m_vk_framebuffers;
}
std::vector<VkFramebuffer>& VulkanTutorial03Parameters::getVkFramebuffers() {
    return m_vk_framebuffers;
}
void VulkanTutorial03Parameters::setVkFramebuffers(
        const std::vector<VkFramebuffer>& vk_framebuffers) {
    m_vk_framebuffers = vk_framebuffers;
}

const VkPipeline& VulkanTutorial03Parameters::getVkPipeline() const {
    return m_vk_pipeline;
}
VkPipeline& VulkanTutorial03Parameters::getVkPipeline() {
    return m_vk_pipeline;
}
void VulkanTutorial03Parameters::setVkPipeline(const VkPipeline& vk_pipeline) {
    m_vk_pipeline = vk_pipeline;
}

const VkSemaphore& VulkanTutorial03Parameters::getImageAvailableVkSemaphore()
        const {
    return m_image_available_vk_semaphore;
}
VkSemaphore& VulkanTutorial03Parameters::getImageAvailableVkSemaphore() {
    return m_image_available_vk_semaphore;
}
void VulkanTutorial03Parameters::setImageAvailableVkSemaphore(
        const VkSemaphore& vk_semaphore) {
    m_image_available_vk_semaphore = vk_semaphore;
}

const std::vector<VkSemaphore>&
VulkanTutorial03Parameters::getRenderingFinishedSemaphores() const {
    return m_rendering_finished_semaphores;
}
std::vector<VkSemaphore>&
VulkanTutorial03Parameters::getRenderingFinishedSemaphores() {
    return m_rendering_finished_semaphores;
}
void VulkanTutorial03Parameters::setRenderingFinishedSemaphores(
        const std::vector<VkSemaphore>& rendering_finished_semaphores) {
    m_rendering_finished_semaphores = rendering_finished_semaphores;
}

const VkFence& VulkanTutorial03Parameters::getVkFence() const {
    return m_vk_fence;
}
VkFence& VulkanTutorial03Parameters::getVkFence() { return m_vk_fence; }
void VulkanTutorial03Parameters::setVkFence(const VkFence& vk_fence) {
    m_vk_fence = vk_fence;
}

const VkCommandPool& VulkanTutorial03Parameters::getVkCommandPool() const {
    return m_vk_command_pool;
}
VkCommandPool& VulkanTutorial03Parameters::getVkCommandPool() {
    return m_vk_command_pool;
}
void VulkanTutorial03Parameters::setVkCommandPool(
        const VkCommandPool& vk_command_pool) {
    m_vk_command_pool = vk_command_pool;
}

const std::vector<VkCommandBuffer>&
VulkanTutorial03Parameters::getVkCommandBuffers() const {
    return m_vk_command_buffers;
}
std::vector<VkCommandBuffer>&
VulkanTutorial03Parameters::getVkCommandBuffers() {
    return m_vk_command_buffers;
}
void VulkanTutorial03Parameters::setVkCommandBuffers(
        const std::vector<VkCommandBuffer>& vk_command_buffers) {
    m_vk_command_buffers = vk_command_buffers;
}

Tutorial03::Tutorial03() = default;

Tutorial03::~Tutorial03() {
    childClear();

    if (getVkDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(getVkDevice());

        if (m_vulkan_tutorial03_parameters.getImageAvailableVkSemaphore() !=
            VK_NULL_HANDLE) {
            vkDestroySemaphore(getVkDevice(),
                               m_vulkan_tutorial03_parameters
                                       .getImageAvailableVkSemaphore(),
                               nullptr);
        }

        std::vector<VkSemaphore>& rendering_finished_semaphores =
                m_vulkan_tutorial03_parameters
                        .getRenderingFinishedSemaphores();
        for (std::size_t i = 0; i < rendering_finished_semaphores.size();
             ++i) {
            if (rendering_finished_semaphores[i] != VK_NULL_HANDLE) {
                vkDestroySemaphore(getVkDevice(),
                                   rendering_finished_semaphores[i],
                                   nullptr);
            }
        }
        rendering_finished_semaphores.clear();

        if (m_vulkan_tutorial03_parameters.getVkFence() != VK_NULL_HANDLE) {
            vkDestroyFence(getVkDevice(),
                           m_vulkan_tutorial03_parameters.getVkFence(),
                           nullptr);
        }
    }
}

bool Tutorial03::createRenderPass() {
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
            {.attachment = 0,  // uint32_t                       attachment
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

    if (vkCreateRenderPass(
                getVkDevice(),
                &render_pass_create_info,
                nullptr,
                &m_vulkan_tutorial03_parameters.getVkRenderPass()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

bool Tutorial03::createFramebuffers() {
    const std::vector<ImageParameters>& swap_chain_images =
            getSwapchainParameters().getImageParameters();
    m_vulkan_tutorial03_parameters.getVkFramebuffers().resize(
            swap_chain_images.size());

    for (size_t i = 0; i < swap_chain_images.size(); ++i) {
        VkFramebufferCreateInfo framebuffer_create_info = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .renderPass = m_vulkan_tutorial03_parameters.getVkRenderPass(),
                .attachmentCount = 1,
                .pAttachments = &swap_chain_images[i].getVkImageView(),
                .width = getSwapchainParameters().getVkExtent2d().width,
                .height = getSwapchainParameters().getVkExtent2d().height,
                .layers = 1};

        if (vkCreateFramebuffer(
                    getVkDevice(),
                    &framebuffer_create_info,
                    nullptr,
                    &m_vulkan_tutorial03_parameters.getVkFramebuffers()[i]) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not create a framebuffer!");
            return false;
        }
    }
    return true;
}

bool Tutorial03::createPipeline() {
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module = createShaderModule("shader.03.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module = createShaderModule("shader.03.frag.spv");

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

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = nullptr};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE};

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

    VkPipelineViewportStateCreateInfo viewport_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor};

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
            .pDynamicState = nullptr,
            .layout = pipeline_layout.get(),
            .renderPass = m_vulkan_tutorial03_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};

    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info,
                nullptr,
                &m_vulkan_tutorial03_parameters.getVkPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create graphics pipeline!");
        return false;
    }
    return true;
}

bool Tutorial03::createSemaphores() {
    VulkanCommon::FrameResourceFactory const frame_resource_factory(
            getVkDevice());

    if (!frame_resource_factory.createSemaphore(
                &m_vulkan_tutorial03_parameters
                         .getImageAvailableVkSemaphore())) {
        Logging::error(LOG_TAG, "Could not create semaphores!");
        return false;
    }

    // A "rendering finished" semaphore must be indexed by the acquired
    // swapchain image, not reused as a single semaphore every frame: the
    // fence below only guarantees the GPU has finished this submit, not
    // that the presentation engine has finished consuming the *previous*
    // signal on this same semaphore object, since present completion isn't
    // tracked by any fence here. See
    // https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html
    std::vector<VkSemaphore>& rendering_finished_semaphores =
            m_vulkan_tutorial03_parameters.getRenderingFinishedSemaphores();
    rendering_finished_semaphores.assign(
            getSwapchainParameters().getImageParameters().size(),
            VK_NULL_HANDLE);
    for (std::size_t i = 0; i < rendering_finished_semaphores.size(); ++i) {
        if (!frame_resource_factory.createSemaphore(
                    &rendering_finished_semaphores[i])) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    if (!frame_resource_factory.createFence(
                /*signaled=*/true, &m_vulkan_tutorial03_parameters.getVkFence())) {
        Logging::error(LOG_TAG, "Could not create a fence!");
        return false;
    }

    return true;
}

bool Tutorial03::createCommandBuffers() {
    if (!createCommandPool(
                getGraphicsQueueParameters().getFamilyIndex(),
                &m_vulkan_tutorial03_parameters.getVkCommandPool())) {
        Logging::error(LOG_TAG, "Could not create command pool!");
        return false;
    }

    std::uint32_t image_count = static_cast<std::uint32_t>(
            getSwapchainParameters().getImageParameters().size());
    m_vulkan_tutorial03_parameters.setVkCommandBuffers(
            std::vector<VkCommandBuffer>(image_count, VK_NULL_HANDLE));

    if (!allocateCommandBuffers(
                m_vulkan_tutorial03_parameters.getVkCommandPool(),
                image_count,
                m_vulkan_tutorial03_parameters.getVkCommandBuffers().data())) {
        Logging::error(LOG_TAG, "Could not allocate command buffers!");
        return false;
    }
    return true;
}

bool Tutorial03::recordCommandBuffers() {
    VkCommandBufferBeginInfo graphics_command_buffer_begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
            .pInheritanceInfo = nullptr};

    VkImageSubresourceRange image_subresource_range = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1};

    VkClearValue clear_value = {
            .color = {.float32 = {1.0f, 0.8f, 0.4f, 0.0f}}};

    const std::vector<ImageParameters>& swap_chain_images =
            getSwapchainParameters().getImageParameters();

    for (size_t i = 0;
         i < m_vulkan_tutorial03_parameters.getVkCommandBuffers().size();
         ++i) {
        vkBeginCommandBuffer(
                m_vulkan_tutorial03_parameters.getVkCommandBuffers()[i],
                &graphics_command_buffer_begin_info);

        if (getPresentQueueParameters().getVkQueue() !=
            getGraphicsQueueParameters().getVkQueue()) {
            VkImageMemoryBarrier barrier_from_present_to_draw = {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    .srcQueueFamilyIndex =
                            getPresentQueueParameters().getFamilyIndex(),
                    .dstQueueFamilyIndex =
                            getGraphicsQueueParameters().getFamilyIndex(),
                    .image = swap_chain_images[i].getVkImage(),
                    .subresourceRange = image_subresource_range};
            vkCmdPipelineBarrier(
                    m_vulkan_tutorial03_parameters.getVkCommandBuffers()[i],
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

        VkRenderPassBeginInfo render_pass_begin_info = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = nullptr,
                .renderPass = m_vulkan_tutorial03_parameters.getVkRenderPass(),
                .framebuffer =
                        m_vulkan_tutorial03_parameters.getVkFramebuffers()[i],
                .renderArea =
                        {.offset = {.x = 0, .y = 0},
                         .extent = getSwapchainParameters().getVkExtent2d()},
                .clearValueCount = 1,
                .pClearValues = &clear_value};

        vkCmdBeginRenderPass(
                m_vulkan_tutorial03_parameters.getVkCommandBuffers()[i],
                &render_pass_begin_info,
                VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(
                m_vulkan_tutorial03_parameters.getVkCommandBuffers()[i],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_vulkan_tutorial03_parameters.getVkPipeline());

        vkCmdDraw(m_vulkan_tutorial03_parameters.getVkCommandBuffers()[i],
                  3,
                  1,
                  0,
                  0);

        vkCmdEndRenderPass(
                m_vulkan_tutorial03_parameters.getVkCommandBuffers()[i]);

        if (getGraphicsQueueParameters().getVkQueue() !=
            getPresentQueueParameters().getVkQueue()) {
            VkImageMemoryBarrier barrier_from_draw_to_present = {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
                    .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    .srcQueueFamilyIndex =
                            getGraphicsQueueParameters().getFamilyIndex(),
                    .dstQueueFamilyIndex =
                            getPresentQueueParameters().getFamilyIndex(),
                    .image = swap_chain_images[i].getVkImage(),
                    .subresourceRange = image_subresource_range};
            vkCmdPipelineBarrier(
                    m_vulkan_tutorial03_parameters.getVkCommandBuffers()[i],
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
        if (vkEndCommandBuffer(
                    m_vulkan_tutorial03_parameters.getVkCommandBuffers()[i]) !=
            VK_SUCCESS) {
            Logging::error(LOG_TAG, "Could not record command buffer!");
            return false;
        }
    }
    return true;
}

bool Tutorial03::draw() {
    VkSwapchainKHR swap_chain = getSwapchainParameters().getVkSwapchainKhr();
    std::uint32_t image_index;

    if (vkWaitForFences(getVkDevice(),
                        1,
                        &m_vulkan_tutorial03_parameters.getVkFence(),
                        VK_FALSE,
                        1000000000) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Waiting for fence takes too long!");
        return false;
    }
    vkResetFences(
            getVkDevice(), 1, &m_vulkan_tutorial03_parameters.getVkFence());

    VkResult result = vkAcquireNextImageKHR(
            getVkDevice(),
            swap_chain,
            UINT64_MAX,
            m_vulkan_tutorial03_parameters.getImageAvailableVkSemaphore(),
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

    VkSemaphore& rendering_finished_semaphore =
            m_vulkan_tutorial03_parameters
                    .getRenderingFinishedSemaphores()[image_index];

    VkPipelineStageFlags wait_dst_stage_mask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &m_vulkan_tutorial03_parameters
                                        .getImageAvailableVkSemaphore(),
            .pWaitDstStageMask = &wait_dst_stage_mask,
            .commandBufferCount = 1,
            .pCommandBuffers = &m_vulkan_tutorial03_parameters
                                        .getVkCommandBuffers()[image_index],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &rendering_finished_semaphore};

    if (vkQueueSubmit(getGraphicsQueueParameters().getVkQueue(),
                      1,
                      &submit_info,
                      m_vulkan_tutorial03_parameters.getVkFence()) !=
        VK_SUCCESS) {
        return false;
    }

    VkPresentInfoKHR present_info = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &rendering_finished_semaphore,
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

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
Tutorial03::createShaderModule(const char* filename) {
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule> module =
            VulkanCommon::createShaderModule(getVkDevice(), filename);
    if (!module) {
        Logging::error(LOG_TAG,
                       "Could not create shader module from a \"",
                       filename,
                       "\" file!");
    }
    return module;
}

Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>
Tutorial03::createPipelineLayout() {
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

bool Tutorial03::createCommandPool(std::uint32_t queue_family_index,
                                   VkCommandPool* pool) {
    VkCommandPoolCreateInfo cmd_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = queue_family_index};

    return vkCreateCommandPool(
                   getVkDevice(), &cmd_pool_create_info, nullptr, pool) ==
           VK_SUCCESS;
}

bool Tutorial03::allocateCommandBuffers(VkCommandPool pool,
                                        std::uint32_t count,
                                        VkCommandBuffer* command_buffers) {
    return VulkanCommon::FrameResourceFactory(getVkDevice())
            .allocateCommandBuffers(pool, count, command_buffers);
}

void Tutorial03::childClear() {
    if (getVkDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(getVkDevice());

        if ((!m_vulkan_tutorial03_parameters.getVkCommandBuffers().empty()) &&
            (m_vulkan_tutorial03_parameters.getVkCommandBuffers()[0] !=
             VK_NULL_HANDLE)) {
            vkFreeCommandBuffers(
                    getVkDevice(),
                    m_vulkan_tutorial03_parameters.getVkCommandPool(),
                    static_cast<std::uint32_t>(m_vulkan_tutorial03_parameters
                                                       .getVkCommandBuffers()
                                                       .size()),
                    m_vulkan_tutorial03_parameters.getVkCommandBuffers()
                            .data());
            m_vulkan_tutorial03_parameters.getVkCommandBuffers().clear();
        }

        if (m_vulkan_tutorial03_parameters.getVkCommandPool() !=
            VK_NULL_HANDLE) {
            vkDestroyCommandPool(
                    getVkDevice(),
                    m_vulkan_tutorial03_parameters.getVkCommandPool(),
                    nullptr);
            m_vulkan_tutorial03_parameters.getVkCommandPool() = VK_NULL_HANDLE;
        }

        if (m_vulkan_tutorial03_parameters.getVkPipeline() != VK_NULL_HANDLE) {
            vkDestroyPipeline(getVkDevice(),
                              m_vulkan_tutorial03_parameters.getVkPipeline(),
                              nullptr);
            m_vulkan_tutorial03_parameters.getVkPipeline() = VK_NULL_HANDLE;
        }

        if (m_vulkan_tutorial03_parameters.getVkRenderPass() !=
            VK_NULL_HANDLE) {
            vkDestroyRenderPass(
                    getVkDevice(),
                    m_vulkan_tutorial03_parameters.getVkRenderPass(),
                    nullptr);
            m_vulkan_tutorial03_parameters.getVkRenderPass() = VK_NULL_HANDLE;
        }

        for (size_t i = 0;
             i < m_vulkan_tutorial03_parameters.getVkFramebuffers().size();
             ++i) {
            if (m_vulkan_tutorial03_parameters.getVkFramebuffers()[i] !=
                VK_NULL_HANDLE) {
                vkDestroyFramebuffer(
                        getVkDevice(),
                        m_vulkan_tutorial03_parameters.getVkFramebuffers()[i],
                        nullptr);
                m_vulkan_tutorial03_parameters.getVkFramebuffers()[i] =
                        VK_NULL_HANDLE;
            }
        }
        m_vulkan_tutorial03_parameters.getVkFramebuffers().clear();
    }
}

bool Tutorial03::childOnWindowSizeChanged() {
    if (!createRenderPass()) {
        return false;
    }
    if (!createFramebuffers()) {
        return false;
    }
    if (!createPipeline()) {
        return false;
    }
    if (!createCommandBuffers()) {
        return false;
    }
    if (!recordCommandBuffers()) {
        return false;
    }

    return true;
}

}  // namespace vulkan_graphix
