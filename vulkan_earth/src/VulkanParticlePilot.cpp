#include "VulkanParticlePilot.h"

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <cstring>

#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/Math/Sphere.hpp"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

VulkanParticlePilot::VulkanParticlePilot()
        : vk_render_pass_(VK_NULL_HANDLE)
        , vk_pipeline_layout_(VK_NULL_HANDLE)
        , vk_graphics_pipeline_(VK_NULL_HANDLE)
        , index_count_(0)
        , vk_command_pool_(VK_NULL_HANDLE)
        , rendering_resources_(resources_count_)
        // The sphere itself has radius 1.0 (see getVertexData()); 3.5 gives
        // a clear view of it and its translucency against the clear color.
        , camera_(0.6f, 0.3f, 3.5f) {}

VulkanParticlePilot::~VulkanParticlePilot() { childClear(); }

bool VulkanParticlePilot::createCommandPool(std::uint32_t queue_family_index,
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

bool VulkanParticlePilot::allocateCommandBuffers(
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

bool VulkanParticlePilot::createCommandBuffers() {
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

bool VulkanParticlePilot::createSemaphores() {
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

bool VulkanParticlePilot::createFences() {
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

bool VulkanParticlePilot::createRenderingResources() {
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

bool VulkanParticlePilot::allocateBufferMemory(
        VkBuffer buffer, VkMemoryPropertyFlags property, VkDeviceMemory* memory) {
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

bool VulkanParticlePilot::createBuffer(VkBufferUsageFlags usage,
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

bool VulkanParticlePilot::createStagingBuffer() {
    // The icosphere's vertex+index data is tiny (a few hundred vertices at
    // most for a sane level-of-detail) - 1MB is generous headroom rather
    // than copied from another pilot's constant sized for a much larger
    // texture upload this pilot doesn't have. See copyBufferData()'s size
    // guard below for what silently overflowing this buffer did to the
    // skybox pilot.
    staging_buffer_.setSize(1024 * 1024);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer_)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

bool VulkanParticlePilot::createUniformBuffer() {
    uniform_buffer_.setSize(sizeof(ParticlePilotUniformBufferData));
    // Host-visible/coherent, rewritten every frame as the orbit camera
    // moves - same tradeoff Tutorial08/Tutorial09 make for the same reason.
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      uniform_buffer_)) {
        Logging::error(LOG_TAG, "Could not create uniform buffer!");
        return false;
    }

    return updateUniformBufferData();
}

ParticlePilotUniformBufferData VulkanParticlePilot::getUniformBufferData()
        const {
    ParticlePilotUniformBufferData data{};
    data.model = Math::Mat4<float>(1.0f);  // static particle, no rotation
    data.view = glm::lookAt(
            camera_.eye(), camera_.target(), Math::Vec3<float>(0.0f, 1.0f, 0.0f));

    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    data.projection = Tools::getPerspectiveProjectionMatrix(
            width / height, 45.0f, 0.1f, 100.0f);

    data.light_position = Math::Vec4<float>(5.0f, 8.0f, 5.0f, 1.0f);
    data.light_color = Math::Vec4<float>(1.0f, 1.0f, 1.0f, 1.0f);
    data.view_position = Math::Vec4<float>(camera_.eye(), 1.0f);

    return data;
}

bool VulkanParticlePilot::updateUniformBufferData() {
    ParticlePilotUniformBufferData const uniform_data =
            getUniformBufferData();

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
                &uniform_data,
                uniform_buffer_.getSize());
    vkUnmapMemory(getVkDevice(), uniform_buffer_.getVkDeviceMemory());

    return true;
}

bool VulkanParticlePilot::createDescriptorSetLayout() {
    // Just the one uniform buffer binding - no texture, matching the
    // original's flat, untextured glutSolidSphere() look and Tutorial08's
    // own (texture-free) descriptor layout.
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings = {
            {.binding = 0,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags =
                     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
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

bool VulkanParticlePilot::createDescriptorPool() {
    std::vector<VkDescriptorPoolSize> pool_sizes = {
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

bool VulkanParticlePilot::allocateDescriptorSet() {
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

bool VulkanParticlePilot::updateDescriptorSet() {
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

bool VulkanParticlePilot::createRenderPass() {
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

bool VulkanParticlePilot::createPipelineLayout() {
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
VulkanParticlePilot::createShaderModule(const char* filename) {
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

bool VulkanParticlePilot::createPipeline() {
    // Vertex shader reused verbatim from Tutorial08 (position/normal ->
    // world position/normal, same UBO shape); fragment shader is new
    // (resources/particle_pilot/Data/shader.frag) since Tutorial08's own
    // hardcodes an opaque, non-alpha-blended color - see VulkanParticlePilot.h.
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module =
                    createShaderModule("shader.particle.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module =
                    createShaderModule("shader.particle.frag.spv");

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
              .stride = ParticlePilotVertexAttributeTraits::stride,
              .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};

    std::vector<VkVertexInputAttributeDescription>
            vertex_attribute_descriptions = {
                    {.location = 0,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset = offsetof(struct ParticlePilotVertexData,
                                        position)},
                    {.location = 1,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32_SFLOAT,
                     .offset =
                             offsetof(struct ParticlePilotVertexData, normal)}};

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
            // BACK_BIT/CLOCKWISE, not this project's usual CCW convention -
            // Math::Sphere/Icosahedron wind their triangles clockwise as
            // seen from outside; Tutorial08 uses the exact same pairing for
            // the exact same reason (see lib/Tutorial08.cpp).
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
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

    // The only one of this session's pilots that needs blending - matches
    // Particle::render()'s own glColor4f(r, g, b, 0.4) translucency.
    // Standard alpha-over blend factors; no depth attachment (see
    // VulkanParticlePilot.h - back-face culling alone is enough for a
    // single convex icosphere, so there's no self-occlusion to resolve).
    VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
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

const std::vector<ParticlePilotVertexData>&
VulkanParticlePilot::getVertexData() const {
    // Same Math::Sphere usage as Tutorial08::getVertexData() - an icosphere
    // (radius 1.0, subdivision level 3), no texcoords since the original
    // glutSolidSphere() draw is untextured too.
    static const std::vector<ParticlePilotVertexData> vertex_data = [] {
        Math::Sphere<float, std::uint32_t> const sphere(
                1.0f, static_cast<std::uint8_t>(3));

        std::vector<ParticlePilotVertexData> data;
        data.reserve(sphere.points().size());
        for (std::size_t i = 0; i < sphere.points().size(); ++i) {
            Math::Vec3<float> const& point = sphere.points()[i];
            Math::Vec3<float> const& normal = sphere.normals()[i];
            data.push_back({Math::Vec4<float>(point, 1.0f), normal});
        }
        return data;
    }();

    return vertex_data;
}

const std::vector<std::uint32_t>& VulkanParticlePilot::getIndexData() const {
    static const std::vector<std::uint32_t> index_data = [] {
        Math::Sphere<float, std::uint32_t> const sphere(
                1.0f, static_cast<std::uint8_t>(3));
        return sphere.indices();
    }();
    return index_data;
}

bool VulkanParticlePilot::copyBufferData(BufferParameters& destination,
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

bool VulkanParticlePilot::createVertexBuffer() {
    const std::vector<ParticlePilotVertexData>& vertex_data = getVertexData();

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

bool VulkanParticlePilot::createIndexBuffer() {
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

bool VulkanParticlePilot::createFramebuffer(VkFramebuffer& framebuffer,
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

bool VulkanParticlePilot::prepareFrame(
        VkCommandBuffer command_buffer,
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

    // A mid-gray clear color makes the sphere's alpha=0.4 translucency
    // visually obvious - too close to black/white would hide the blend.
    VkClearValue clear_value = {
            .color = {.float32 = {0.35f, 0.35f, 0.4f, 1.0f}}};

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

bool VulkanParticlePilot::draw() {
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

void VulkanParticlePilot::onMouseButton(int button,
                                        bool pressed,
                                        int pos_x,
                                        int pos_y) {
    camera_.onMouseButton(button, pressed, pos_x, pos_y);
}

void VulkanParticlePilot::onMouseMove(int pos_x, int pos_y) {
    camera_.onMouseMove(pos_x, pos_y);
}

void VulkanParticlePilot::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

bool VulkanParticlePilot::childOnWindowSizeChanged() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return true;
    }
    vkDeviceWaitIdle(getVkDevice());

    if (!createRenderingResources()) {
        return false;
    }
    if (!createStagingBuffer()) {
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

void VulkanParticlePilot::childClear() {
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
}

}  // namespace vulkan_graphix
