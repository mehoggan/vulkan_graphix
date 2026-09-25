#include "vulkan_graphix/Tutorial15.h"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstddef>
#include <cstring>

#include "vulkan_graphix/UiGeometry.h"
#include "vulkan_graphix/VulkanCommon.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
constexpr VkFormat c_texture_format = VK_FORMAT_R8G8B8A8_UNORM;
}  // namespace

// ************************************************************ //
// VulkanTutorial15Parameters                                   //
// ************************************************************ //
VulkanTutorial15Parameters::VulkanTutorial15Parameters()
        : m_vk_render_pass(VK_NULL_HANDLE)
        , m_vk_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_graphics_pipeline(VK_NULL_HANDLE)
        , m_vertex_count(0)
        , m_vk_command_pool(VK_NULL_HANDLE)
        , m_rendering_resources(resources_count) {}

const VkRenderPass& VulkanTutorial15Parameters::getVkRenderPass() const {
    return m_vk_render_pass;
}
VkRenderPass& VulkanTutorial15Parameters::getVkRenderPass() {
    return m_vk_render_pass;
}
void VulkanTutorial15Parameters::setVkRenderPass(
        const VkRenderPass& vk_render_pass) {
    m_vk_render_pass = vk_render_pass;
}

const ImageParameters& VulkanTutorial15Parameters::getImageParameters()
        const {
    return m_image_parameters;
}
ImageParameters& VulkanTutorial15Parameters::getImageParameters() {
    return m_image_parameters;
}
void VulkanTutorial15Parameters::setImageParameters(
        const ImageParameters& image_parameters) {
    m_image_parameters = image_parameters;
}

const BufferParameters&
VulkanTutorial15Parameters::getUniformBufferParameters() const {
    return m_uniform_buffer;
}
BufferParameters& VulkanTutorial15Parameters::getUniformBufferParameters() {
    return m_uniform_buffer;
}
void VulkanTutorial15Parameters::setUniformBufferParameters(
        const BufferParameters& uniform_buffer) {
    m_uniform_buffer = uniform_buffer;
}

const DescriptorSetParameters&
VulkanTutorial15Parameters::getDescriptorSetParameters() const {
    return m_descriptor_set_parameters;
}
DescriptorSetParameters&
VulkanTutorial15Parameters::getDescriptorSetParameters() {
    return m_descriptor_set_parameters;
}
void VulkanTutorial15Parameters::setDescriptorSetParameters(
        const DescriptorSetParameters& descriptor_set_parameters) {
    m_descriptor_set_parameters = descriptor_set_parameters;
}

const VkPipelineLayout& VulkanTutorial15Parameters::getVkPipelineLayout()
        const {
    return m_vk_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial15Parameters::getVkPipelineLayout() {
    return m_vk_pipeline_layout;
}
void VulkanTutorial15Parameters::setVkPipelineLayout(
        const VkPipelineLayout& vk_pipeline_layout) {
    m_vk_pipeline_layout = vk_pipeline_layout;
}

const VkPipeline& VulkanTutorial15Parameters::getVkGraphicsPipeline() const {
    return m_vk_graphics_pipeline;
}
VkPipeline& VulkanTutorial15Parameters::getVkGraphicsPipeline() {
    return m_vk_graphics_pipeline;
}
void VulkanTutorial15Parameters::setVkGraphicsPipeline(
        const VkPipeline& vk_graphics_pipeline) {
    m_vk_graphics_pipeline = vk_graphics_pipeline;
}

const BufferParameters& VulkanTutorial15Parameters::getVertexBufferParameters()
        const {
    return m_vertex_buffer;
}
BufferParameters& VulkanTutorial15Parameters::getVertexBufferParameters() {
    return m_vertex_buffer;
}
void VulkanTutorial15Parameters::setVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_vertex_buffer = vertex_buffer;
}

std::uint32_t VulkanTutorial15Parameters::getVertexCount() const {
    return m_vertex_count;
}
void VulkanTutorial15Parameters::setVertexCount(std::uint32_t vertex_count) {
    m_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial15Parameters::getStagingBufferParameters() const {
    return m_staging_buffer;
}
BufferParameters& VulkanTutorial15Parameters::getStagingBufferParameters() {
    return m_staging_buffer;
}
void VulkanTutorial15Parameters::setStagingBufferParameters(
        const BufferParameters& staging_buffer) {
    m_staging_buffer = staging_buffer;
}

const VkCommandPool& VulkanTutorial15Parameters::getVkCommandPool() const {
    return m_vk_command_pool;
}
VkCommandPool& VulkanTutorial15Parameters::getVkCommandPool() {
    return m_vk_command_pool;
}
void VulkanTutorial15Parameters::setVkCommandPool(
        const VkCommandPool& vk_command_pool) {
    m_vk_command_pool = vk_command_pool;
}

const std::vector<RenderingResourceParameters>&
VulkanTutorial15Parameters::getRenderingResources() const {
    return m_rendering_resources;
}
std::vector<RenderingResourceParameters>&
VulkanTutorial15Parameters::getRenderingResources() {
    return m_rendering_resources;
}
void VulkanTutorial15Parameters::setRenderingResources(
        const std::vector<RenderingResourceParameters>& rendering_resources) {
    m_rendering_resources = rendering_resources;
}

const std::vector<VkSemaphore>&
VulkanTutorial15Parameters::getFinishedRenderingSemaphores() const {
    return m_finished_rendering_semaphores;
}
std::vector<VkSemaphore>&
VulkanTutorial15Parameters::getFinishedRenderingSemaphores() {
    return m_finished_rendering_semaphores;
}
void VulkanTutorial15Parameters::setFinishedRenderingSemaphores(
        const std::vector<VkSemaphore>& finished_rendering_semaphores) {
    m_finished_rendering_semaphores = finished_rendering_semaphores;
}

// ************************************************************ //
// Tutorial15                                                   //
// ************************************************************ //
Tutorial15::Tutorial15() : m_button_pressed(false), m_click_count(0) {}

Tutorial15::~Tutorial15() { childClear(); }

bool Tutorial15::createCommandPool(std::uint32_t queue_family_index,
                                   VkCommandPool* pool) {
    if (!VulkanCommon::FrameResourceFactory(getVkDevice())
                 .createCommandPool(queue_family_index, pool)) {
        Logging::error(LOG_TAG, "Could not create command pool!");
        return false;
    }
    return true;
}

bool Tutorial15::allocateCommandBuffers(VkCommandPool pool,
                                        std::uint32_t count,
                                        VkCommandBuffer* command_buffers) {
    if (!VulkanCommon::FrameResourceFactory(getVkDevice())
                 .allocateCommandBuffers(pool, count, command_buffers)) {
        Logging::error(LOG_TAG, "Could not allocate command buffer!");
        return false;
    }
    return true;
}

bool Tutorial15::createCommandBuffers() {
    if (!createCommandPool(
                getGraphicsQueueParameters().getFamilyIndex(),
                &m_vulkan_tutorial15_parameters.getVkCommandPool())) {
        return false;
    }

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial15_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!allocateCommandBuffers(
                    m_vulkan_tutorial15_parameters.getVkCommandPool(),
                    1,
                    &rendering_resources[i].getVkCommandBuffer())) {
            return false;
        }
    }
    return true;
}

bool Tutorial15::createSemaphores() {
    VulkanCommon::FrameResourceFactory factory(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial15_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!factory.createSemaphore(
                    &rendering_resources[i].getImageAvailableVkSemaphore())) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    std::vector<VkSemaphore>& finished_rendering_semaphores =
            m_vulkan_tutorial15_parameters.getFinishedRenderingSemaphores();
    finished_rendering_semaphores.assign(
            getSwapchainParameters().getImageParameters().size(),
            VK_NULL_HANDLE);
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (!factory.createSemaphore(&finished_rendering_semaphores[i])) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    return true;
}

bool Tutorial15::createFences() {
    VulkanCommon::FrameResourceFactory factory(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial15_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!factory.createFence(true,
                                 &rendering_resources[i].getVkFence())) {
            Logging::error(LOG_TAG, "Could not create a fence!");
            return false;
        }
    }
    return true;
}

bool Tutorial15::createRenderingResources() {
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

bool Tutorial15::createBuffer(VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags memory_property,
                              BufferParameters& buffer) {
    if (!VulkanCommon::BufferFactory(getVkDevice(), getVkPhysicalDevice())
                 .create(usage, memory_property, buffer)) {
        Logging::error(LOG_TAG, "Could not create buffer!");
        return false;
    }
    return true;
}

bool Tutorial15::createStagingBuffer() {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial15_parameters.getStagingBufferParameters();
    // Only ever used once, to upload the font atlas (512x512 RGBA =
    // 1,048,576 bytes) - sized with headroom rather than copied from
    // another tutorial's constant. See copyTextureData()'s size guard
    // below for what silently overflowing this buffer did to the
    // original vulkan_earth migration pilots.
    staging_buffer.setSize(2 * 1024 * 1024);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

bool Tutorial15::createImage(std::uint32_t width,
                             std::uint32_t height,
                             VkImage* image) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .createImage(width,
                        height,
                        c_texture_format,
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                VK_IMAGE_USAGE_SAMPLED_BIT,
                        image);
}

bool Tutorial15::allocateImageMemory(VkImage image,
                                     VkMemoryPropertyFlags property,
                                     VkDeviceMemory* memory) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .allocateMemory(image, property, memory);
}

bool Tutorial15::createImageView() {
    ImageParameters& image_parameters =
            m_vulkan_tutorial15_parameters.getImageParameters();
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .createImageView(image_parameters.getVkImage(),
                             c_texture_format,
                             VK_IMAGE_ASPECT_COLOR_BIT,
                             &image_parameters.getVkImageView());
}

bool Tutorial15::createSampler(VkSampler* sampler) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .createSampler(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, sampler);
}

bool Tutorial15::copyTextureData(char* texture_data,
                                 std::uint32_t data_size,
                                 std::uint32_t width,
                                 std::uint32_t height) {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial15_parameters.getStagingBufferParameters();
    ImageParameters& image_parameters =
            m_vulkan_tutorial15_parameters.getImageParameters();

    if (!VulkanCommon::StagedUploader(
                getVkDevice(),
                getGraphicsQueueParameters().getVkQueue(),
                m_vulkan_tutorial15_parameters.getRenderingResources()[0]
                        .getVkCommandBuffer())
                 .uploadToImage(staging_buffer,
                                image_parameters.getVkImage(),
                                texture_data,
                                data_size,
                                width,
                                height)) {
        Logging::error(LOG_TAG, "Could not upload texture data!");
        return false;
    }

    return true;
}

bool Tutorial15::createFontAtlas() {
    if (!m_font.load(kFontPath, kFontPixelHeight)) {
        Logging::error(LOG_TAG,
                       "Could not load font \"",
                       kFontPath,
                       "\" - is the fonts-dejavu-core package installed?");
        return false;
    }

    ImageParameters& image_parameters =
            m_vulkan_tutorial15_parameters.getImageParameters();

    VkImage vk_image;
    if (!createImage(m_font.atlasWidth(), m_font.atlasHeight(), &vk_image)) {
        Logging::error(LOG_TAG, "Could not create image!");
        return false;
    }
    image_parameters.setVkImage(vk_image);

    VkDeviceMemory vk_device_memory;
    if (!allocateImageMemory(image_parameters.getVkImage(),
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &vk_device_memory)) {
        Logging::error(LOG_TAG, "Could not allocate memory for image!");
        return false;
    }
    image_parameters.setVkDeviceMemory(vk_device_memory);

    if (vkBindImageMemory(getVkDevice(),
                          image_parameters.getVkImage(),
                          image_parameters.getVkDeviceMemory(),
                          0) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not bind memory to an image!");
        return false;
    }

    if (!createImageView()) {
        Logging::error(LOG_TAG, "Could not create image view!");
        return false;
    }

    VkSampler vk_sampler;
    if (!createSampler(&vk_sampler)) {
        Logging::error(LOG_TAG, "Could not create sampler!");
        return false;
    }
    image_parameters.setVkSampler(vk_sampler);

    // Local, non-const copy: copyTextureData() takes char* (matching
    // every other tutorial's texture-upload signature), and
    // BitmapFont::atlasPixels() returns a const reference.
    std::vector<char> atlas_pixels = m_font.atlasPixels();
    if (!copyTextureData(atlas_pixels.data(),
                         static_cast<std::uint32_t>(atlas_pixels.size()),
                         m_font.atlasWidth(),
                         m_font.atlasHeight())) {
        Logging::error(LOG_TAG,
                       "Could not upload texture data to device memory!");
        return false;
    }

    return true;
}

bool Tutorial15::createUniformBuffer() {
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial15_parameters.getUniformBufferParameters();
    uniform_buffer.setSize(sizeof(Math::Mat4<float>));
    // Host-visible/coherent: unlike the camera-driven tutorials this
    // doesn't change every frame (the projection only depends on window
    // size), so it's written once here and again in
    // childOnWindowSizeChanged() - never per-frame in draw().
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      uniform_buffer)) {
        Logging::error(LOG_TAG, "Could not create uniform buffer!");
        return false;
    }

    return updateUniformBufferData();
}

Math::Mat4<float> Tutorial15::getUniformBufferData() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    // Top-left-origin, y-down screen convention (matches the mouse
    // coordinates onMouseButton() receives): world (0,0) is the window's
    // top-left corner, world (width,height) is bottom-right. Vulkan's
    // own NDC is already y-down (unlike OpenGL's), so this maps directly
    // with no extra sign flip - top=0 -> NDC y=-1 (top), bottom=height ->
    // NDC y=+1 (bottom).
    return Tools::getOrthographicProjectionMatrix(
            0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

bool Tutorial15::updateUniformBufferData() {
    Math::Mat4<float> const uniform_data = getUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial15_parameters.getUniformBufferParameters();

    void* uniform_buffer_memory_pointer;
    if (vkMapMemory(getVkDevice(),
                    uniform_buffer.getVkDeviceMemory(),
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &uniform_buffer_memory_pointer) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not map uniform buffer memory!");
        return false;
    }
    std::memcpy(uniform_buffer_memory_pointer,
                &uniform_data,
                uniform_buffer.getSize());
    vkUnmapMemory(getVkDevice(), uniform_buffer.getVkDeviceMemory());

    return true;
}

bool Tutorial15::createDescriptorSetLayout() {
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings = {
            {.binding = 0,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
             .pImmutableSamplers = nullptr},
            {.binding = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = 1,
             .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
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
    m_vulkan_tutorial15_parameters.getDescriptorSetParameters()
            .setVkDescriptorSetLayout(vk_descriptor_set_layout);

    return true;
}

bool Tutorial15::createDescriptorPool() {
    std::vector<VkDescriptorPoolSize> pool_sizes = {
            {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1},
            {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = 1}};

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
    m_vulkan_tutorial15_parameters.getDescriptorSetParameters()
            .setVkDescriptorPool(vk_descriptor_pool);

    return true;
}

bool Tutorial15::allocateDescriptorSet() {
    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial15_parameters.getDescriptorSetParameters();
    VkDescriptorSetLayout vk_descriptor_set_layout =
            descriptor_set.getVkDescriptorSetLayout();
    VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = descriptor_set.getVkDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_descriptor_set_layout};

    VkDescriptorSet vk_descriptor_set;
    if (vkAllocateDescriptorSets(getVkDevice(),
                                 &descriptor_set_allocate_info,
                                 &vk_descriptor_set) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate descriptor set!");
        return false;
    }
    descriptor_set.setVkDescriptorSet(vk_descriptor_set);

    return true;
}

bool Tutorial15::updateDescriptorSet() {
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial15_parameters.getUniformBufferParameters();
    ImageParameters& image_parameters =
            m_vulkan_tutorial15_parameters.getImageParameters();
    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial15_parameters.getDescriptorSetParameters();

    VkDescriptorBufferInfo buffer_info = {
            .buffer = uniform_buffer.getVkBuffer(),
            .offset = 0,
            .range = uniform_buffer.getSize()};

    VkDescriptorImageInfo image_info = {
            .sampler = image_parameters.getVkSampler(),
            .imageView = image_parameters.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    std::vector<VkWriteDescriptorSet> descriptor_writes = {
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = descriptor_set.getVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &buffer_info,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = descriptor_set.getVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &image_info,
             .pBufferInfo = nullptr,
             .pTexelBufferView = nullptr}};

    vkUpdateDescriptorSets(
            getVkDevice(),
            static_cast<std::uint32_t>(descriptor_writes.size()),
            descriptor_writes.data(),
            0,
            nullptr);
    return true;
}

bool Tutorial15::createRenderPass() {
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

    if (vkCreateRenderPass(
                getVkDevice(),
                &render_pass_create_info,
                nullptr,
                &m_vulkan_tutorial15_parameters.getVkRenderPass()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

bool Tutorial15::createPipelineLayout() {
    VkDescriptorSetLayout vk_descriptor_set_layout =
            m_vulkan_tutorial15_parameters.getDescriptorSetParameters()
                    .getVkDescriptorSetLayout();
    VkPipelineLayoutCreateInfo layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &vk_descriptor_set_layout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr};

    if (vkCreatePipelineLayout(
                getVkDevice(),
                &layout_create_info,
                nullptr,
                &m_vulkan_tutorial15_parameters.getVkPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create pipeline layout!");
        return false;
    }

    return true;
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
Tutorial15::createShaderModule(const char* filename) {
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

bool Tutorial15::createPipeline() {
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module = createShaderModule("shader.15.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module = createShaderModule("shader.15.frag.spv");

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
              .stride = Tutorial15VertexAttributeTraits::stride,
              .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};

    std::vector<VkVertexInputAttributeDescription>
            vertex_attribute_descriptions = {
                    {.location = 0,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset =
                             offsetof(struct Tutorial15VertexData, position)},
                    {.location = 1,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32_SFLOAT,
                     .offset =
                             offsetof(struct Tutorial15VertexData, texcoord)},
                    {.location = 2,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset = offsetof(struct Tutorial15VertexData, color)}};

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

    // Non-indexed triangle list, 6 vertices/quad - matches Tutorial12's
    // non-indexed precedent; simplest fit for a small, per-frame-rebuilt
    // CPU vertex list (see updateVertexBufferData()).
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
            // 2D UI - winding doesn't matter, so no culling.
            .cullMode = VK_CULL_MODE_NONE,
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

    // Alpha blending on - glyph edges are anti-aliased via the atlas's
    // coverage alpha, same as Tutorial14's translucent sphere.
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
            .layout = m_vulkan_tutorial15_parameters.getVkPipelineLayout(),
            .renderPass = m_vulkan_tutorial15_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};

    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info,
                nullptr,
                &m_vulkan_tutorial15_parameters.getVkGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create graphics pipeline!");
        return false;
    }
    return true;
}

Math::Vec2<float> Tutorial15::getButtonSize() const {
    return Math::Vec2<float>(180.0f, 56.0f);
}

Math::Vec2<float> Tutorial15::getButtonTopLeft() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    Math::Vec2<float> const size = getButtonSize();
    return Math::Vec2<float>(width * 0.5f - size.x * 0.5f,
                             height * 0.5f - size.y * 0.5f);
}

std::string Tutorial15::getButtonLabel() const {
    if (m_click_count == 0) {
        return "Click Me";
    }
    return "Clicked: " + std::to_string(m_click_count);
}

void Tutorial15::appendGlyphQuad(
        std::vector<Tutorial15VertexData>& vertex_data,
        const BitmapFontGlyphQuad& glyph,
        Math::Vec4<float> color) const {
    Math::Vec2<float> const& top_left = glyph.top_left;
    Math::Vec2<float> const& bottom_right = glyph.bottom_right;
    Math::Vec2<float> const& uv_top_left = glyph.uv_top_left;
    Math::Vec2<float> const& uv_bottom_right = glyph.uv_bottom_right;

    Tutorial15VertexData const top_left_vertex{
            Math::Vec4<float>(top_left.x, top_left.y, 0.0f, 1.0f),
            uv_top_left,
            color};
    Tutorial15VertexData const bottom_left_vertex{
            Math::Vec4<float>(top_left.x, bottom_right.y, 0.0f, 1.0f),
            Math::Vec2<float>(uv_top_left.x, uv_bottom_right.y),
            color};
    Tutorial15VertexData const bottom_right_vertex{
            Math::Vec4<float>(bottom_right.x, bottom_right.y, 0.0f, 1.0f),
            uv_bottom_right,
            color};
    Tutorial15VertexData const top_right_vertex{
            Math::Vec4<float>(bottom_right.x, top_left.y, 0.0f, 1.0f),
            Math::Vec2<float>(uv_bottom_right.x, uv_top_left.y),
            color};

    vertex_data.push_back(top_left_vertex);
    vertex_data.push_back(bottom_left_vertex);
    vertex_data.push_back(bottom_right_vertex);
    vertex_data.push_back(top_left_vertex);
    vertex_data.push_back(bottom_right_vertex);
    vertex_data.push_back(top_right_vertex);
}

void Tutorial15::appendColoredQuad(
        std::vector<Tutorial15VertexData>& vertex_data,
        const std::array<Math::Vec2<float>, 4>& corners,
        Math::Vec4<float> color) const {
    Math::Vec2<float> const solid_uv = m_font.solidTexelUv();
    std::array<Tutorial15VertexData, 4> quad_vertices;
    for (std::size_t i = 0; i < corners.size(); ++i) {
        quad_vertices[i] = Tutorial15VertexData{
                Math::Vec4<float>(corners[i].x, corners[i].y, 0.0f, 1.0f),
                solid_uv,
                color};
    }
    vertex_data.push_back(quad_vertices[0]);
    vertex_data.push_back(quad_vertices[1]);
    vertex_data.push_back(quad_vertices[2]);
    vertex_data.push_back(quad_vertices[0]);
    vertex_data.push_back(quad_vertices[2]);
    vertex_data.push_back(quad_vertices[3]);
}

void Tutorial15::appendText(std::vector<Tutorial15VertexData>& vertex_data,
                            const std::string& text,
                            Math::Vec2<float> origin,
                            Math::Vec4<float> color) const {
    std::vector<BitmapFontGlyphQuad> const glyphs =
            m_font.layoutText(text, origin);
    for (BitmapFontGlyphQuad const& glyph : glyphs) {
        appendGlyphQuad(vertex_data, glyph, color);
    }
}

std::vector<Tutorial15VertexData> Tutorial15::buildUiVertexData() const {
    std::vector<Tutorial15VertexData> vertex_data;
    vertex_data.reserve(kMaxVertexCount);

    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    Math::Vec4<float> const text_color(0.05f, 0.05f, 0.05f, 1.0f);

    std::string const title = "Tutorial 15 - Text & UI";
    float const title_width = m_font.textWidth(title);
    Math::Vec2<float> const title_origin(width * 0.5f - title_width * 0.5f,
                                         140.0f);
    appendText(vertex_data, title, title_origin, text_color);

    Math::Vec2<float> const button_top_left = getButtonTopLeft();
    Math::Vec2<float> const button_size = getButtonSize();
    Math::Vec4<float> const button_color(0.3f, 0.5f, 0.75f, 1.0f);
    std::vector<UiGeometry::ColoredQuad> const bevel =
            UiGeometry::buildButtonBevel(
                    button_top_left, button_size, button_color, m_button_pressed);
    for (UiGeometry::ColoredQuad const& quad : bevel) {
        appendColoredQuad(vertex_data, quad.corners, quad.color);
    }

    std::string const label = getButtonLabel();
    float const label_width = m_font.textWidth(label);
    Math::Vec2<float> const label_origin(
            button_top_left.x + button_size.x * 0.5f - label_width * 0.5f,
            button_top_left.y + button_size.y * 0.5f +
                    kFontPixelHeight * 0.3f);
    appendText(vertex_data, label, label_origin, text_color);

    return vertex_data;
}

bool Tutorial15::updateVertexBufferData() {
    std::vector<Tutorial15VertexData> const vertex_data = buildUiVertexData();
    if (vertex_data.size() > kMaxVertexCount) {
        Logging::error(LOG_TAG,
                       "UI vertex data (",
                       vertex_data.size(),
                       " vertices) exceeds kMaxVertexCount (",
                       kMaxVertexCount,
                       ")!");
        return false;
    }
    m_vulkan_tutorial15_parameters.setVertexCount(
            static_cast<std::uint32_t>(vertex_data.size()));

    BufferParameters& vertex_buffer =
            m_vulkan_tutorial15_parameters.getVertexBufferParameters();
    void* vertex_buffer_memory_pointer;
    if (vkMapMemory(getVkDevice(),
                    vertex_buffer.getVkDeviceMemory(),
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &vertex_buffer_memory_pointer) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not map vertex buffer memory!");
        return false;
    }
    if (!vertex_data.empty()) {
        std::memcpy(vertex_buffer_memory_pointer,
                    vertex_data.data(),
                    vertex_data.size() * sizeof(Tutorial15VertexData));
    }
    vkUnmapMemory(getVkDevice(), vertex_buffer.getVkDeviceMemory());

    return true;
}

bool Tutorial15::createVertexBuffer() {
    BufferParameters& vertex_buffer =
            m_vulkan_tutorial15_parameters.getVertexBufferParameters();
    vertex_buffer.setSize(static_cast<std::uint32_t>(
            kMaxVertexCount * sizeof(Tutorial15VertexData)));
    // Host-visible/coherent, not device-local+staged: rebuilt and
    // re-uploaded every frame from small CPU-side UI state (see
    // updateVertexBufferData()) - the same per-frame-map technique every
    // tutorial already uses for its uniform buffer, extended to vertex
    // data here since this content is small and dynamic rather than
    // static/upload-once.
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      vertex_buffer)) {
        Logging::error(LOG_TAG, "Could not create vertex buffer!");
        return false;
    }

    return updateVertexBufferData();
}

bool Tutorial15::createFramebuffer(VkFramebuffer& framebuffer,
                                   VkImageView image_view) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(getVkDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_vulkan_tutorial15_parameters.getVkRenderPass(),
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

bool Tutorial15::prepareFrame(VkCommandBuffer command_buffer,
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

    VkClearValue clear_value = {
            .color = {.float32 = {0.92f, 0.92f, 0.9f, 1.0f}}};

    VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = m_vulkan_tutorial15_parameters.getVkRenderPass(),
            .framebuffer = framebuffer,
            .renderArea = {.offset = {.x = 0, .y = 0},
                           .extent = getSwapchainParameters().getVkExtent2d()},
            .clearValueCount = 1,
            .pClearValues = &clear_value};

    vkCmdBeginRenderPass(command_buffer,
                         &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial15_parameters.getVkGraphicsPipeline());

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
            &m_vulkan_tutorial15_parameters.getVertexBufferParameters()
                     .getVkBuffer(),
            &offset);

    VkDescriptorSet vk_descriptor_set =
            m_vulkan_tutorial15_parameters.getDescriptorSetParameters()
                    .getVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial15_parameters.getVkPipelineLayout(),
            0,
            1,
            &vk_descriptor_set,
            0,
            nullptr);

    vkCmdDraw(
            command_buffer,
            m_vulkan_tutorial15_parameters.getVertexCount(),
            1,
            0,
            0);

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

bool Tutorial15::draw() {
    static std::size_t resource_index = 0;
    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial15_parameters.getRenderingResources();
    RenderingResourceParameters& current_rendering_resource =
            rendering_resources[resource_index];
    VkSwapchainKHR swap_chain = getSwapchainParameters().getVkSwapchainKhr();
    std::uint32_t image_index;

    resource_index = (resource_index + 1) %
                     VulkanTutorial15Parameters::resources_count;

    if (vkWaitForFences(getVkDevice(),
                        1,
                        &current_rendering_resource.getVkFence(),
                        VK_FALSE,
                        1000000000) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Waiting for fence takes too long!");
        return false;
    }
    vkResetFences(getVkDevice(), 1, &current_rendering_resource.getVkFence());

    // The vertex buffer is a single, shared, host-visible allocation
    // rewritten every frame from current UI state (mouse clicks can
    // change it at any time) - vkDeviceWaitIdle() avoids racing a
    // previous frame's in-flight command buffer that might still be
    // reading it, same reasoning every camera-driven tutorial already
    // applies to its own per-frame uniform buffer write.
    vkDeviceWaitIdle(getVkDevice());
    if (!updateVertexBufferData()) {
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
            m_vulkan_tutorial15_parameters.getFinishedRenderingSemaphores()
                    [image_index];

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

void Tutorial15::onMouseButton(int button,
                               bool pressed,
                               int pos_x,
                               int pos_y) {
    constexpr int c_left_button = 1;
    if (button != c_left_button) {
        return;
    }

    if (!pressed) {
        // Matches ControlItemButton::mouseClickEvent()'s own behavior:
        // the visual press state always clears on release, regardless
        // of where the release happened.
        m_button_pressed = false;
        return;
    }

    Math::Vec2<float> const top_left = getButtonTopLeft();
    Math::Vec2<float> const size = getButtonSize();
    float const x = static_cast<float>(pos_x);
    float const y = static_cast<float>(pos_y);
    if (x >= top_left.x && x <= top_left.x + size.x && y >= top_left.y &&
        y <= top_left.y + size.y) {
        // Matches the original: the click registers on press, not on
        // release.
        m_button_pressed = true;
        ++m_click_count;
    } else {
        m_button_pressed = false;
    }
}

void Tutorial15::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

bool Tutorial15::childOnWindowSizeChanged() {
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
    if (!createFontAtlas()) {
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
    return createVertexBuffer();
}

void Tutorial15::childClear() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return;
    }
    vkDeviceWaitIdle(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial15_parameters.getRenderingResources();
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
                    m_vulkan_tutorial15_parameters.getVkCommandPool(),
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
            vkDestroyFence(
                    getVkDevice(), rendering_resources[i].getVkFence(), nullptr);
        }
    }

    std::vector<VkSemaphore>& finished_rendering_semaphores =
            m_vulkan_tutorial15_parameters.getFinishedRenderingSemaphores();
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (finished_rendering_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(), finished_rendering_semaphores[i], nullptr);
        }
    }
    finished_rendering_semaphores.clear();

    if (m_vulkan_tutorial15_parameters.getVkCommandPool() != VK_NULL_HANDLE) {
        vkDestroyCommandPool(
                getVkDevice(),
                m_vulkan_tutorial15_parameters.getVkCommandPool(),
                nullptr);
        m_vulkan_tutorial15_parameters.setVkCommandPool(VK_NULL_HANDLE);
    }

    destroyBuffer(m_vulkan_tutorial15_parameters.getVertexBufferParameters());
    destroyBuffer(m_vulkan_tutorial15_parameters.getStagingBufferParameters());

    if (m_vulkan_tutorial15_parameters.getVkGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial15_parameters.getVkGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial15_parameters.setVkGraphicsPipeline(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial15_parameters.getVkPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial15_parameters.getVkPipelineLayout(),
                nullptr);
        m_vulkan_tutorial15_parameters.setVkPipelineLayout(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial15_parameters.getVkRenderPass() != VK_NULL_HANDLE) {
        vkDestroyRenderPass(
                getVkDevice(),
                m_vulkan_tutorial15_parameters.getVkRenderPass(),
                nullptr);
        m_vulkan_tutorial15_parameters.setVkRenderPass(VK_NULL_HANDLE);
    }

    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial15_parameters.getDescriptorSetParameters();
    if (descriptor_set.getVkDescriptorPool() != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(
                getVkDevice(), descriptor_set.getVkDescriptorPool(), nullptr);
        descriptor_set.setVkDescriptorPool(VK_NULL_HANDLE);
    }
    if (descriptor_set.getVkDescriptorSetLayout() != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(getVkDevice(),
                                     descriptor_set.getVkDescriptorSetLayout(),
                                     nullptr);
        descriptor_set.setVkDescriptorSetLayout(VK_NULL_HANDLE);
    }

    destroyBuffer(m_vulkan_tutorial15_parameters.getUniformBufferParameters());

    VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .destroy(m_vulkan_tutorial15_parameters.getImageParameters());
}

}  // namespace vulkan_graphix
