#include "vulkan_graphix/Tutorial17.h"

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>

#include "vulkan_graphix/UiGeometry.h"
#include "vulkan_graphix/VulkanCommon.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
struct ItemDisplayData {
    const char* icon_file;
    const char* short_name;
    const char* description;
    int price;
    int remaining;
};

// Real data from vulkan_earth/src/ItemXxx.cpp's own constructors, not
// fabricated - only short_name (this tutorial's own per-cell label, taken
// from each description's own lead-in before its colon) isn't a field
// Item itself stores.
std::array<ItemDisplayData, kInventoryItemCount> const& getItemDisplayData() {
    static std::array<ItemDisplayData, kInventoryItemCount> const data = {{
            {"ItemAntiAcid.raw",
             "Anti-Acid",
             "Anti-Acid:     Cures acid status.",
             70,
             3},
            {"ItemBigRepair.raw",
             "Big Repair",
             "Big Repair:     Heals 700 damage (uses 1 turn).",
             100,
             3},
            {"ItemCloak.raw",
             "Cloak",
             "Cloak:     Makes the player's tank disappear (users 1 turn).",
             100,
             2},
            {"ItemDoubleAction.raw",
             "Double Action",
             "Double Action:     Allows the player to perform an action "
             "twice in one turn.",
             120,
             1},
            {"ItemExtraBattery.raw",
             "Extra Battery",
             "Extra Battery:     Recovers from the damage of EMP.",
             50,
             3},
            {"ItemFloat.raw",
             "Float",
             "Float:     Allows the player to float (uses 1 turn).",
             80,
             2},
            {"ItemShield.raw",
             "Shield",
             "Shield:     Neutralize the damage taken for 5 times (uses 1 "
             "turn).",
             150,
             1},
            {"ItemSmallRepair.raw",
             "Small Repair",
             "Small Repair:     Heals 200 damage.",
             50,
             3},
    }};
    return data;
}
}  // namespace

// ************************************************************ //
// VulkanTutorial17Parameters                                   //
// ************************************************************ //
VulkanTutorial17Parameters::VulkanTutorial17Parameters()
        : m_vk_render_pass(VK_NULL_HANDLE)
        , m_vk_descriptor_set_layout(VK_NULL_HANDLE)
        , m_vk_descriptor_pool(VK_NULL_HANDLE)
        , m_vk_font_descriptor_set(VK_NULL_HANDLE)
        , m_vk_icon_descriptor_set(VK_NULL_HANDLE)
        , m_vk_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_graphics_pipeline(VK_NULL_HANDLE)
        , m_text_vertex_count(0)
        , m_icon_vertex_count(0)
        , m_vk_command_pool(VK_NULL_HANDLE)
        , m_rendering_resources(resources_count) {}

const VkRenderPass& VulkanTutorial17Parameters::getVkRenderPass() const {
    return m_vk_render_pass;
}
VkRenderPass& VulkanTutorial17Parameters::getVkRenderPass() {
    return m_vk_render_pass;
}
void VulkanTutorial17Parameters::setVkRenderPass(
        const VkRenderPass& vk_render_pass) {
    m_vk_render_pass = vk_render_pass;
}

const ImageParameters& VulkanTutorial17Parameters::getFontImageParameters()
        const {
    return m_font_image_parameters;
}
ImageParameters& VulkanTutorial17Parameters::getFontImageParameters() {
    return m_font_image_parameters;
}
void VulkanTutorial17Parameters::setFontImageParameters(
        const ImageParameters& image_parameters) {
    m_font_image_parameters = image_parameters;
}

const ImageParameters& VulkanTutorial17Parameters::getIconImageParameters()
        const {
    return m_icon_image_parameters;
}
ImageParameters& VulkanTutorial17Parameters::getIconImageParameters() {
    return m_icon_image_parameters;
}
void VulkanTutorial17Parameters::setIconImageParameters(
        const ImageParameters& image_parameters) {
    m_icon_image_parameters = image_parameters;
}

const BufferParameters&
VulkanTutorial17Parameters::getUniformBufferParameters() const {
    return m_uniform_buffer;
}
BufferParameters& VulkanTutorial17Parameters::getUniformBufferParameters() {
    return m_uniform_buffer;
}
void VulkanTutorial17Parameters::setUniformBufferParameters(
        const BufferParameters& uniform_buffer) {
    m_uniform_buffer = uniform_buffer;
}

const VkDescriptorSetLayout&
VulkanTutorial17Parameters::getVkDescriptorSetLayout() const {
    return m_vk_descriptor_set_layout;
}
VkDescriptorSetLayout& VulkanTutorial17Parameters::getVkDescriptorSetLayout() {
    return m_vk_descriptor_set_layout;
}
void VulkanTutorial17Parameters::setVkDescriptorSetLayout(
        const VkDescriptorSetLayout& other) {
    m_vk_descriptor_set_layout = other;
}

const VkDescriptorPool& VulkanTutorial17Parameters::getVkDescriptorPool()
        const {
    return m_vk_descriptor_pool;
}
VkDescriptorPool& VulkanTutorial17Parameters::getVkDescriptorPool() {
    return m_vk_descriptor_pool;
}
void VulkanTutorial17Parameters::setVkDescriptorPool(
        const VkDescriptorPool& other) {
    m_vk_descriptor_pool = other;
}

const VkDescriptorSet& VulkanTutorial17Parameters::getFontVkDescriptorSet()
        const {
    return m_vk_font_descriptor_set;
}
VkDescriptorSet& VulkanTutorial17Parameters::getFontVkDescriptorSet() {
    return m_vk_font_descriptor_set;
}
void VulkanTutorial17Parameters::setFontVkDescriptorSet(
        const VkDescriptorSet& other) {
    m_vk_font_descriptor_set = other;
}

const VkDescriptorSet& VulkanTutorial17Parameters::getIconVkDescriptorSet()
        const {
    return m_vk_icon_descriptor_set;
}
VkDescriptorSet& VulkanTutorial17Parameters::getIconVkDescriptorSet() {
    return m_vk_icon_descriptor_set;
}
void VulkanTutorial17Parameters::setIconVkDescriptorSet(
        const VkDescriptorSet& other) {
    m_vk_icon_descriptor_set = other;
}

const VkPipelineLayout& VulkanTutorial17Parameters::getVkPipelineLayout()
        const {
    return m_vk_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial17Parameters::getVkPipelineLayout() {
    return m_vk_pipeline_layout;
}
void VulkanTutorial17Parameters::setVkPipelineLayout(
        const VkPipelineLayout& vk_pipeline_layout) {
    m_vk_pipeline_layout = vk_pipeline_layout;
}

const VkPipeline& VulkanTutorial17Parameters::getVkGraphicsPipeline() const {
    return m_vk_graphics_pipeline;
}
VkPipeline& VulkanTutorial17Parameters::getVkGraphicsPipeline() {
    return m_vk_graphics_pipeline;
}
void VulkanTutorial17Parameters::setVkGraphicsPipeline(
        const VkPipeline& vk_graphics_pipeline) {
    m_vk_graphics_pipeline = vk_graphics_pipeline;
}

const BufferParameters& VulkanTutorial17Parameters::getVertexBufferParameters()
        const {
    return m_vertex_buffer;
}
BufferParameters& VulkanTutorial17Parameters::getVertexBufferParameters() {
    return m_vertex_buffer;
}
void VulkanTutorial17Parameters::setVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_vertex_buffer = vertex_buffer;
}

std::uint32_t VulkanTutorial17Parameters::getTextVertexCount() const {
    return m_text_vertex_count;
}
void VulkanTutorial17Parameters::setTextVertexCount(
        std::uint32_t vertex_count) {
    m_text_vertex_count = vertex_count;
}
std::uint32_t VulkanTutorial17Parameters::getIconVertexCount() const {
    return m_icon_vertex_count;
}
void VulkanTutorial17Parameters::setIconVertexCount(
        std::uint32_t vertex_count) {
    m_icon_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial17Parameters::getStagingBufferParameters() const {
    return m_staging_buffer;
}
BufferParameters& VulkanTutorial17Parameters::getStagingBufferParameters() {
    return m_staging_buffer;
}
void VulkanTutorial17Parameters::setStagingBufferParameters(
        const BufferParameters& staging_buffer) {
    m_staging_buffer = staging_buffer;
}

const VkCommandPool& VulkanTutorial17Parameters::getVkCommandPool() const {
    return m_vk_command_pool;
}
VkCommandPool& VulkanTutorial17Parameters::getVkCommandPool() {
    return m_vk_command_pool;
}
void VulkanTutorial17Parameters::setVkCommandPool(
        const VkCommandPool& vk_command_pool) {
    m_vk_command_pool = vk_command_pool;
}

const std::vector<RenderingResourceParameters>&
VulkanTutorial17Parameters::getRenderingResources() const {
    return m_rendering_resources;
}
std::vector<RenderingResourceParameters>&
VulkanTutorial17Parameters::getRenderingResources() {
    return m_rendering_resources;
}
void VulkanTutorial17Parameters::setRenderingResources(
        const std::vector<RenderingResourceParameters>& rendering_resources) {
    m_rendering_resources = rendering_resources;
}

const std::vector<VkSemaphore>&
VulkanTutorial17Parameters::getFinishedRenderingSemaphores() const {
    return m_finished_rendering_semaphores;
}
std::vector<VkSemaphore>&
VulkanTutorial17Parameters::getFinishedRenderingSemaphores() {
    return m_finished_rendering_semaphores;
}
void VulkanTutorial17Parameters::setFinishedRenderingSemaphores(
        const std::vector<VkSemaphore>& finished_rendering_semaphores) {
    m_finished_rendering_semaphores = finished_rendering_semaphores;
}

// ************************************************************ //
// Tutorial17                                                   //
// ************************************************************ //
Tutorial17::Tutorial17() : m_selected_index(0) {}

Tutorial17::~Tutorial17() { childClear(); }

bool Tutorial17::createCommandPool(std::uint32_t queue_family_index,
                                   VkCommandPool* pool) {
    if (!VulkanCommon::FrameResourceFactory(getVkDevice())
                 .createCommandPool(queue_family_index, pool)) {
        Logging::error(LOG_TAG, "Could not create command pool!");
        return false;
    }
    return true;
}

bool Tutorial17::allocateCommandBuffers(VkCommandPool pool,
                                        std::uint32_t count,
                                        VkCommandBuffer* command_buffers) {
    if (!VulkanCommon::FrameResourceFactory(getVkDevice())
                 .allocateCommandBuffers(pool, count, command_buffers)) {
        Logging::error(LOG_TAG, "Could not allocate command buffer!");
        return false;
    }
    return true;
}

bool Tutorial17::createCommandBuffers() {
    if (!createCommandPool(
                getGraphicsQueueParameters().getFamilyIndex(),
                &m_vulkan_tutorial17_parameters.getVkCommandPool())) {
        return false;
    }

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial17_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!allocateCommandBuffers(
                    m_vulkan_tutorial17_parameters.getVkCommandPool(),
                    1,
                    &rendering_resources[i].getVkCommandBuffer())) {
            return false;
        }
    }
    return true;
}

bool Tutorial17::createSemaphores() {
    VulkanCommon::FrameResourceFactory factory(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial17_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!factory.createSemaphore(
                    &rendering_resources[i].getImageAvailableVkSemaphore())) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    std::vector<VkSemaphore>& finished_rendering_semaphores =
            m_vulkan_tutorial17_parameters.getFinishedRenderingSemaphores();
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

bool Tutorial17::createFences() {
    VulkanCommon::FrameResourceFactory factory(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial17_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!factory.createFence(true,
                                 &rendering_resources[i].getVkFence())) {
            Logging::error(LOG_TAG, "Could not create a fence!");
            return false;
        }
    }
    return true;
}

bool Tutorial17::createRenderingResources() {
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

bool Tutorial17::createBuffer(VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags memory_property,
                              BufferParameters& buffer) {
    if (!VulkanCommon::BufferFactory(getVkDevice(), getVkPhysicalDevice())
                 .create(usage, memory_property, buffer)) {
        Logging::error(LOG_TAG, "Could not create buffer!");
        return false;
    }
    return true;
}

bool Tutorial17::createStagingBuffer() {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial17_parameters.getStagingBufferParameters();
    // Reused sequentially for two uploads: the font atlas (512x512 RGBA =
    // 1,048,576 bytes) and the larger icon atlas (kIconAtlasWidth x
    // kIconAtlasHeight RGBA = 2,097,152 bytes) - sized with headroom over
    // the larger one rather than copied from another tutorial's constant.
    // See VulkanCommon::StagedUploader's size guard for what silently
    // overflowing this buffer did to the original migration pilots.
    staging_buffer.setSize(4 * 1024 * 1024);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

bool Tutorial17::createTextureFromPixels(std::uint32_t width,
                                         std::uint32_t height,
                                         const std::vector<char>& pixels,
                                         ImageParameters& out_image_parameters) {
    if (!VulkanCommon::createTextureFromPixels(
                VulkanCommon::ImageFactory(getVkDevice(),
                                           getVkPhysicalDevice()),
                VulkanCommon::StagedUploader(
                        getVkDevice(),
                        getGraphicsQueueParameters().getVkQueue(),
                        m_vulkan_tutorial17_parameters.getRenderingResources()
                                [0]
                                        .getVkCommandBuffer()),
                m_vulkan_tutorial17_parameters.getStagingBufferParameters(),
                width,
                height,
                pixels,
                VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                out_image_parameters)) {
        Logging::error(LOG_TAG, "Could not create texture from pixels!");
        return false;
    }
    return true;
}

bool Tutorial17::createFontAtlas() {
    if (!m_font.load(kFontPath, kFontPixelHeight)) {
        Logging::error(LOG_TAG,
                       "Could not load font \"",
                       kFontPath,
                       "\" - is the fonts-dejavu-core package installed?");
        return false;
    }

    return createTextureFromPixels(
            m_font.atlasWidth(),
            m_font.atlasHeight(),
            m_font.atlasPixels(),
            m_vulkan_tutorial17_parameters.getFontImageParameters());
}

std::vector<char> Tutorial17::buildIconAtlasPixels() const {
    std::vector<char> atlas(
            static_cast<std::size_t>(kIconAtlasWidth) * kIconAtlasHeight * 4,
            0);

    std::array<ItemDisplayData, kInventoryItemCount> const& items =
            getItemDisplayData();
    for (std::size_t index = 0; index < items.size(); ++index) {
        std::vector<char> const icon_pixels =
                Tools::getRawImageData(items[index].icon_file, kIconSize, kIconSize);
        if (icon_pixels.empty()) {
            Logging::error(LOG_TAG,
                           "Could not load icon \"",
                           items[index].icon_file,
                           "\"!");
            return {};
        }

        std::uint32_t const column =
                static_cast<std::uint32_t>(index) % kIconAtlasCols;
        std::uint32_t const grid_row =
                static_cast<std::uint32_t>(index) / kIconAtlasCols;
        std::uint32_t const dest_x = column * kIconSize;
        std::uint32_t const dest_y = grid_row * kIconSize;

        for (std::uint32_t y = 0; y < kIconSize; ++y) {
            const char* src_row = icon_pixels.data() +
                                  static_cast<std::size_t>(y) * kIconSize * 4;
            char* dest_row =
                    atlas.data() +
                    (static_cast<std::size_t>(dest_y + y) * kIconAtlasWidth +
                     dest_x) *
                            4;
            std::memcpy(dest_row, src_row, static_cast<std::size_t>(kIconSize) * 4);
        }
    }

    return atlas;
}

bool Tutorial17::createIconAtlas() {
    std::vector<char> const pixels = buildIconAtlasPixels();
    if (pixels.empty()) {
        return false;
    }

    return createTextureFromPixels(
            kIconAtlasWidth,
            kIconAtlasHeight,
            pixels,
            m_vulkan_tutorial17_parameters.getIconImageParameters());
}

bool Tutorial17::createUniformBuffer() {
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial17_parameters.getUniformBufferParameters();
    uniform_buffer.setSize(sizeof(Math::Mat4<float>));
    // Host-visible/coherent: doesn't change every frame (the projection
    // only depends on window size), written once here and again in
    // childOnWindowSizeChanged() - matches Tutorial15's own reasoning.
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      uniform_buffer)) {
        Logging::error(LOG_TAG, "Could not create uniform buffer!");
        return false;
    }

    return updateUniformBufferData();
}

Math::Mat4<float> Tutorial17::getUniformBufferData() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    // Top-left-origin, y-down screen convention, same as Tutorial15.
    return Tools::getOrthographicProjectionMatrix(
            0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

bool Tutorial17::updateUniformBufferData() {
    Math::Mat4<float> const uniform_data = getUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial17_parameters.getUniformBufferParameters();

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

bool Tutorial17::createDescriptorSetLayout() {
    // Same binding shape as Tutorial15's shaders (0 = UBO in the vertex
    // stage, 1 = combined sampler in the fragment stage) - this tutorial's
    // shader.17.vert/frag are textually identical. One layout, two sets
    // allocated from it (see this tutorial's header comment).
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
    m_vulkan_tutorial17_parameters.setVkDescriptorSetLayout(
            vk_descriptor_set_layout);

    return true;
}

bool Tutorial17::createDescriptorPool() {
    // Two sets' worth of each descriptor type (font set + icon set).
    std::vector<VkDescriptorPoolSize> pool_sizes = {
            {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 2},
            {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .descriptorCount = 2}};

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .maxSets = 2,
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
    m_vulkan_tutorial17_parameters.setVkDescriptorPool(vk_descriptor_pool);

    return true;
}

bool Tutorial17::allocateDescriptorSets() {
    VkDescriptorSetLayout vk_descriptor_set_layout =
            m_vulkan_tutorial17_parameters.getVkDescriptorSetLayout();
    VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool =
                    m_vulkan_tutorial17_parameters.getVkDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &vk_descriptor_set_layout};

    VkDescriptorSet vk_font_descriptor_set;
    if (vkAllocateDescriptorSets(getVkDevice(),
                                 &descriptor_set_allocate_info,
                                 &vk_font_descriptor_set) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate font descriptor set!");
        return false;
    }
    m_vulkan_tutorial17_parameters.setFontVkDescriptorSet(
            vk_font_descriptor_set);

    VkDescriptorSet vk_icon_descriptor_set;
    if (vkAllocateDescriptorSets(getVkDevice(),
                                 &descriptor_set_allocate_info,
                                 &vk_icon_descriptor_set) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not allocate icon descriptor set!");
        return false;
    }
    m_vulkan_tutorial17_parameters.setIconVkDescriptorSet(
            vk_icon_descriptor_set);

    return true;
}

bool Tutorial17::updateDescriptorSets() {
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial17_parameters.getUniformBufferParameters();
    VkDescriptorBufferInfo buffer_info = {
            .buffer = uniform_buffer.getVkBuffer(),
            .offset = 0,
            .range = uniform_buffer.getSize()};

    ImageParameters& font_image =
            m_vulkan_tutorial17_parameters.getFontImageParameters();
    VkDescriptorImageInfo font_image_info = {
            .sampler = font_image.getVkSampler(),
            .imageView = font_image.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    ImageParameters& icon_image =
            m_vulkan_tutorial17_parameters.getIconImageParameters();
    VkDescriptorImageInfo icon_image_info = {
            .sampler = icon_image.getVkSampler(),
            .imageView = icon_image.getVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    std::vector<VkWriteDescriptorSet> descriptor_writes = {
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial17_parameters.getFontVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &buffer_info,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial17_parameters.getFontVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &font_image_info,
             .pBufferInfo = nullptr,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial17_parameters.getIconVkDescriptorSet(),
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .pImageInfo = nullptr,
             .pBufferInfo = &buffer_info,
             .pTexelBufferView = nullptr},
            {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = m_vulkan_tutorial17_parameters.getIconVkDescriptorSet(),
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             .pImageInfo = &icon_image_info,
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

bool Tutorial17::createRenderPass() {
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
                &m_vulkan_tutorial17_parameters.getVkRenderPass()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

bool Tutorial17::createPipelineLayout() {
    VkDescriptorSetLayout vk_descriptor_set_layout =
            m_vulkan_tutorial17_parameters.getVkDescriptorSetLayout();
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
                &m_vulkan_tutorial17_parameters.getVkPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create pipeline layout!");
        return false;
    }

    return true;
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
Tutorial17::createShaderModule(const char* filename) {
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

bool Tutorial17::createPipeline() {
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module = createShaderModule("shader.17.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module = createShaderModule("shader.17.frag.spv");

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
              .stride = Tutorial17VertexAttributeTraits::stride,
              .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};

    std::vector<VkVertexInputAttributeDescription>
            vertex_attribute_descriptions = {
                    {.location = 0,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset =
                             offsetof(struct Tutorial17VertexData, position)},
                    {.location = 1,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32_SFLOAT,
                     .offset =
                             offsetof(struct Tutorial17VertexData, texcoord)},
                    {.location = 2,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset = offsetof(struct Tutorial17VertexData, color)}};

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

    // Alpha blending on - glyph edges are anti-aliased via the font
    // atlas's coverage alpha (same as Tutorial15), and it costs nothing
    // for the fully-opaque icon quads drawn in the second pass.
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
            .layout = m_vulkan_tutorial17_parameters.getVkPipelineLayout(),
            .renderPass = m_vulkan_tutorial17_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};

    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info,
                nullptr,
                &m_vulkan_tutorial17_parameters.getVkGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create graphics pipeline!");
        return false;
    }
    return true;
}

Math::Vec2<float> Tutorial17::getPanelTopLeft() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    return Math::Vec2<float>(width * 0.05f, height * 0.05f);
}

Math::Vec2<float> Tutorial17::getPanelSize() const {
    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    return Math::Vec2<float>(width * 0.9f, height * 0.9f);
}

Math::Vec2<float> Tutorial17::getGridTopLeft() const {
    Math::Vec2<float> const panel_top_left = getPanelTopLeft();
    return Math::Vec2<float>(panel_top_left.x + 20.0f,
                             panel_top_left.y + 90.0f);
}

Math::Vec2<float> Tutorial17::getGridSize() const {
    Math::Vec2<float> const panel_size = getPanelSize();
    return Math::Vec2<float>(panel_size.x - 40.0f, 190.0f);
}

Math::Vec2<float> Tutorial17::getCellSize() const {
    Math::Vec2<float> const grid_size = getGridSize();
    constexpr float c_cell_gap = 10.0f;
    float const cell_width =
            (grid_size.x - static_cast<float>(kIconAtlasCols - 1) * c_cell_gap) /
            static_cast<float>(kIconAtlasCols);
    float const cell_height =
            (grid_size.y - static_cast<float>(kIconAtlasRows - 1) * c_cell_gap) /
            static_cast<float>(kIconAtlasRows);
    return Math::Vec2<float>(cell_width, cell_height);
}

Math::Vec2<float> Tutorial17::getCellTopLeft(std::size_t index) const {
    Math::Vec2<float> const grid_top_left = getGridTopLeft();
    Math::Vec2<float> const cell_size = getCellSize();
    constexpr float c_cell_gap = 10.0f;
    std::uint32_t const column =
            static_cast<std::uint32_t>(index) % kIconAtlasCols;
    std::uint32_t const grid_row =
            static_cast<std::uint32_t>(index) / kIconAtlasCols;
    return Math::Vec2<float>(
            grid_top_left.x +
                    static_cast<float>(column) * (cell_size.x + c_cell_gap),
            grid_top_left.y +
                    static_cast<float>(grid_row) * (cell_size.y + c_cell_gap));
}

float Tutorial17::getDescriptionTop() const {
    Math::Vec2<float> const grid_top_left = getGridTopLeft();
    Math::Vec2<float> const grid_size = getGridSize();
    return grid_top_left.y + grid_size.y + 30.0f;
}

Math::Vec2<float> Tutorial17::getIconUvMin(std::size_t index) const {
    std::uint32_t const column =
            static_cast<std::uint32_t>(index) % kIconAtlasCols;
    std::uint32_t const grid_row =
            static_cast<std::uint32_t>(index) / kIconAtlasCols;
    return Math::Vec2<float>(
            static_cast<float>(column) / static_cast<float>(kIconAtlasCols),
            static_cast<float>(grid_row) / static_cast<float>(kIconAtlasRows));
}

Math::Vec2<float> Tutorial17::getIconUvMax(std::size_t index) const {
    std::uint32_t const column =
            static_cast<std::uint32_t>(index) % kIconAtlasCols;
    std::uint32_t const grid_row =
            static_cast<std::uint32_t>(index) / kIconAtlasCols;
    return Math::Vec2<float>(
            static_cast<float>(column + 1) / static_cast<float>(kIconAtlasCols),
            static_cast<float>(grid_row + 1) /
                    static_cast<float>(kIconAtlasRows));
}

std::vector<std::string> Tutorial17::wrapText(const std::string& text,
                                              float max_width) const {
    std::vector<std::string> lines;
    std::string current_line;
    std::size_t word_start = 0;
    for (std::size_t i = 0; i <= text.size(); ++i) {
        if (i < text.size() && text[i] != ' ') {
            continue;
        }
        std::string const word = text.substr(word_start, i - word_start);
        word_start = i + 1;
        if (word.empty()) {
            continue;
        }

        std::string const candidate =
                current_line.empty() ? word : current_line + " " + word;
        if (!current_line.empty() && m_font.textWidth(candidate) > max_width) {
            lines.push_back(current_line);
            current_line = word;
        } else {
            current_line = candidate;
        }
    }
    if (!current_line.empty()) {
        lines.push_back(current_line);
    }
    return lines;
}

void Tutorial17::appendGlyphQuad(
        std::vector<Tutorial17VertexData>& vertex_data,
        const BitmapFontGlyphQuad& glyph,
        Math::Vec4<float> color) const {
    Math::Vec2<float> const& top_left = glyph.top_left;
    Math::Vec2<float> const& bottom_right = glyph.bottom_right;
    Math::Vec2<float> const& uv_top_left = glyph.uv_top_left;
    Math::Vec2<float> const& uv_bottom_right = glyph.uv_bottom_right;

    Tutorial17VertexData const top_left_vertex{
            Math::Vec4<float>(top_left.x, top_left.y, 0.0f, 1.0f),
            uv_top_left,
            color};
    Tutorial17VertexData const bottom_left_vertex{
            Math::Vec4<float>(top_left.x, bottom_right.y, 0.0f, 1.0f),
            Math::Vec2<float>(uv_top_left.x, uv_bottom_right.y),
            color};
    Tutorial17VertexData const bottom_right_vertex{
            Math::Vec4<float>(bottom_right.x, bottom_right.y, 0.0f, 1.0f),
            uv_bottom_right,
            color};
    Tutorial17VertexData const top_right_vertex{
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

void Tutorial17::appendColoredQuad(
        std::vector<Tutorial17VertexData>& vertex_data,
        const std::array<Math::Vec2<float>, 4>& corners,
        Math::Vec4<float> color) const {
    Math::Vec2<float> const solid_uv = m_font.solidTexelUv();
    std::array<Tutorial17VertexData, 4> quad_vertices;
    for (std::size_t i = 0; i < corners.size(); ++i) {
        quad_vertices[i] = Tutorial17VertexData{
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

void Tutorial17::appendText(std::vector<Tutorial17VertexData>& vertex_data,
                            const std::string& text,
                            Math::Vec2<float> origin,
                            Math::Vec4<float> color) const {
    std::vector<BitmapFontGlyphQuad> const glyphs =
            m_font.layoutText(text, origin);
    for (BitmapFontGlyphQuad const& glyph : glyphs) {
        appendGlyphQuad(vertex_data, glyph, color);
    }
}

void Tutorial17::appendImageQuad(std::vector<Tutorial17VertexData>& vertex_data,
                                 Math::Vec2<float> top_left,
                                 Math::Vec2<float> size,
                                 Math::Vec2<float> uv_min,
                                 Math::Vec2<float> uv_max) const {
    Math::Vec4<float> const white(1.0f, 1.0f, 1.0f, 1.0f);
    Math::Vec2<float> const bottom_right(top_left.x + size.x,
                                         top_left.y + size.y);

    Tutorial17VertexData const top_left_vertex{
            Math::Vec4<float>(top_left.x, top_left.y, 0.0f, 1.0f),
            uv_min,
            white};
    Tutorial17VertexData const bottom_left_vertex{
            Math::Vec4<float>(top_left.x, bottom_right.y, 0.0f, 1.0f),
            Math::Vec2<float>(uv_min.x, uv_max.y),
            white};
    Tutorial17VertexData const bottom_right_vertex{
            Math::Vec4<float>(bottom_right.x, bottom_right.y, 0.0f, 1.0f),
            uv_max,
            white};
    Tutorial17VertexData const top_right_vertex{
            Math::Vec4<float>(bottom_right.x, top_left.y, 0.0f, 1.0f),
            Math::Vec2<float>(uv_max.x, uv_min.y),
            white};

    vertex_data.push_back(top_left_vertex);
    vertex_data.push_back(bottom_left_vertex);
    vertex_data.push_back(bottom_right_vertex);
    vertex_data.push_back(top_left_vertex);
    vertex_data.push_back(bottom_right_vertex);
    vertex_data.push_back(top_right_vertex);
}

std::vector<Tutorial17VertexData> Tutorial17::buildTextPassVertexData()
        const {
    std::vector<Tutorial17VertexData> vertex_data;
    vertex_data.reserve(kMaxVertexCount);

    Math::Vec2<float> const panel_top_left = getPanelTopLeft();
    Math::Vec2<float> const panel_size = getPanelSize();
    Math::Vec4<float> const panel_color(0.75f, 0.75f, 0.75f, 1.0f);
    // Same five-quad bevel ControlItemGrid::draw() itself uses for the
    // grid panel, tinted the same neutral gray.
    std::vector<UiGeometry::ColoredQuad> const panel_bevel =
            UiGeometry::buildButtonBevel(
                    panel_top_left, panel_size, panel_color, false);
    for (UiGeometry::ColoredQuad const& quad : panel_bevel) {
        appendColoredQuad(vertex_data, quad.corners, quad.color);
    }

    Math::Vec4<float> const text_color(0.05f, 0.05f, 0.05f, 1.0f);
    std::string const title = "Tutorial 17 - Inventory";
    appendText(vertex_data,
              title,
              Math::Vec2<float>(panel_top_left.x + 20.0f,
                                panel_top_left.y + 30.0f),
              text_color);

    std::string const explain = "(Click an item to see its description)";
    appendText(vertex_data,
              explain,
              Math::Vec2<float>(panel_top_left.x + 20.0f,
                                panel_top_left.y + 58.0f),
              text_color);

    std::array<ItemDisplayData, kInventoryItemCount> const& items =
            getItemDisplayData();
    Math::Vec2<float> const cell_size = getCellSize();
    for (std::size_t index = 0; index < items.size(); ++index) {
        Math::Vec2<float> const cell_top_left = getCellTopLeft(index);

        if (index == m_selected_index) {
            // A highlight quad behind the icon/label - the icon pass
            // draws on top of this afterward, so it reads as a border
            // around the selected cell (matches ControlItemGrid's own
            // active-cell highlight).
            Math::Vec4<float> const highlight_color(0.95f, 0.82f, 0.25f, 1.0f);
            std::array<Math::Vec2<float>, 4> const highlight_corners = {
                    cell_top_left,
                    Math::Vec2<float>(cell_top_left.x,
                                      cell_top_left.y + cell_size.y),
                    Math::Vec2<float>(cell_top_left.x + cell_size.x,
                                      cell_top_left.y + cell_size.y),
                    Math::Vec2<float>(cell_top_left.x + cell_size.x,
                                      cell_top_left.y)};
            appendColoredQuad(vertex_data, highlight_corners, highlight_color);
        }

        std::string const label = "x " + std::to_string(items[index].remaining);
        float const label_width = m_font.textWidth(label);
        appendText(vertex_data,
                  label,
                  Math::Vec2<float>(
                          cell_top_left.x + cell_size.x * 0.5f -
                                  label_width * 0.5f,
                          cell_top_left.y + cell_size.y - 6.0f),
                  text_color);
    }

    float const description_top = getDescriptionTop();
    Math::Vec2<float> const panel_bottom_right(
            panel_top_left.x + panel_size.x, panel_top_left.y + panel_size.y);
    float const max_description_width =
            panel_bottom_right.x - (panel_top_left.x + 20.0f) - 20.0f;
    std::vector<std::string> const description_lines = wrapText(
            items[m_selected_index].description, max_description_width);
    for (std::size_t line = 0; line < description_lines.size(); ++line) {
        appendText(vertex_data,
                  description_lines[line],
                  Math::Vec2<float>(panel_top_left.x + 20.0f,
                                    description_top +
                                            static_cast<float>(line) *
                                                    m_font.lineHeight()),
                  text_color);
    }

    return vertex_data;
}

std::vector<Tutorial17VertexData> Tutorial17::buildIconPassVertexData()
        const {
    std::vector<Tutorial17VertexData> vertex_data;
    vertex_data.reserve(kInventoryItemCount * 6);

    Math::Vec2<float> const cell_size = getCellSize();
    // Icon inset within its cell, leaving room for the "x N" label
    // drawn along the cell's bottom edge in the text pass.
    float const icon_size =
            std::min(cell_size.x, cell_size.y - 24.0f) * 0.8f;
    for (std::size_t index = 0; index < kInventoryItemCount; ++index) {
        Math::Vec2<float> const cell_top_left = getCellTopLeft(index);
        Math::Vec2<float> const icon_top_left(
                cell_top_left.x + cell_size.x * 0.5f - icon_size * 0.5f,
                cell_top_left.y + 6.0f);
        appendImageQuad(vertex_data,
                        icon_top_left,
                        Math::Vec2<float>(icon_size, icon_size),
                        getIconUvMin(index),
                        getIconUvMax(index));
    }

    return vertex_data;
}

bool Tutorial17::updateVertexBufferData() {
    std::vector<Tutorial17VertexData> const text_vertex_data =
            buildTextPassVertexData();
    std::vector<Tutorial17VertexData> const icon_vertex_data =
            buildIconPassVertexData();

    if (text_vertex_data.size() + icon_vertex_data.size() > kMaxVertexCount) {
        Logging::error(LOG_TAG,
                       "Inventory vertex data (",
                       text_vertex_data.size() + icon_vertex_data.size(),
                       " vertices) exceeds kMaxVertexCount (",
                       kMaxVertexCount,
                       ")!");
        return false;
    }
    m_vulkan_tutorial17_parameters.setTextVertexCount(
            static_cast<std::uint32_t>(text_vertex_data.size()));
    m_vulkan_tutorial17_parameters.setIconVertexCount(
            static_cast<std::uint32_t>(icon_vertex_data.size()));

    BufferParameters& vertex_buffer =
            m_vulkan_tutorial17_parameters.getVertexBufferParameters();
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
    char* dest = static_cast<char*>(vertex_buffer_memory_pointer);
    if (!text_vertex_data.empty()) {
        std::memcpy(dest,
                    text_vertex_data.data(),
                    text_vertex_data.size() * sizeof(Tutorial17VertexData));
        dest += text_vertex_data.size() * sizeof(Tutorial17VertexData);
    }
    if (!icon_vertex_data.empty()) {
        std::memcpy(dest,
                    icon_vertex_data.data(),
                    icon_vertex_data.size() * sizeof(Tutorial17VertexData));
    }
    vkUnmapMemory(getVkDevice(), vertex_buffer.getVkDeviceMemory());

    return true;
}

bool Tutorial17::createVertexBuffer() {
    BufferParameters& vertex_buffer =
            m_vulkan_tutorial17_parameters.getVertexBufferParameters();
    vertex_buffer.setSize(static_cast<std::uint32_t>(
            kMaxVertexCount * sizeof(Tutorial17VertexData)));
    // Host-visible/coherent, rebuilt every frame from small CPU-side UI
    // state - same pattern as Tutorial15's own vertex buffer.
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      vertex_buffer)) {
        Logging::error(LOG_TAG, "Could not create vertex buffer!");
        return false;
    }

    return updateVertexBufferData();
}

bool Tutorial17::createFramebuffer(VkFramebuffer& framebuffer,
                                   VkImageView image_view) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(getVkDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_vulkan_tutorial17_parameters.getVkRenderPass(),
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

bool Tutorial17::prepareFrame(VkCommandBuffer command_buffer,
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
            .renderPass = m_vulkan_tutorial17_parameters.getVkRenderPass(),
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
            m_vulkan_tutorial17_parameters.getVkGraphicsPipeline());

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

    VkBuffer vk_vertex_buffer =
            m_vulkan_tutorial17_parameters.getVertexBufferParameters()
                    .getVkBuffer();

    // Pass 1: every flat-color/text quad (panel bevel, title/explain/
    // descript, per-cell labels, selection highlight), bound against the
    // font atlas descriptor set.
    VkDeviceSize text_offset = 0;
    vkCmdBindVertexBuffers(
            command_buffer, 0, 1, &vk_vertex_buffer, &text_offset);
    VkDescriptorSet vk_font_descriptor_set =
            m_vulkan_tutorial17_parameters.getFontVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial17_parameters.getVkPipelineLayout(),
            0,
            1,
            &vk_font_descriptor_set,
            0,
            nullptr);
    vkCmdDraw(command_buffer,
             m_vulkan_tutorial17_parameters.getTextVertexCount(),
             1,
             0,
             0);

    // Pass 2: the 8 icon quads, drawn on top against the icon atlas
    // descriptor set, from the second range of the same vertex buffer.
    VkDeviceSize const icon_offset =
            static_cast<VkDeviceSize>(
                    m_vulkan_tutorial17_parameters.getTextVertexCount()) *
            sizeof(Tutorial17VertexData);
    vkCmdBindVertexBuffers(
            command_buffer, 0, 1, &vk_vertex_buffer, &icon_offset);
    VkDescriptorSet vk_icon_descriptor_set =
            m_vulkan_tutorial17_parameters.getIconVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial17_parameters.getVkPipelineLayout(),
            0,
            1,
            &vk_icon_descriptor_set,
            0,
            nullptr);
    vkCmdDraw(command_buffer,
             m_vulkan_tutorial17_parameters.getIconVertexCount(),
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

bool Tutorial17::draw() {
    static std::size_t resource_index = 0;
    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial17_parameters.getRenderingResources();
    RenderingResourceParameters& current_rendering_resource =
            rendering_resources[resource_index];
    VkSwapchainKHR swap_chain = getSwapchainParameters().getVkSwapchainKhr();
    std::uint32_t image_index;

    resource_index = (resource_index + 1) %
                     VulkanTutorial17Parameters::resources_count;

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
    // rewritten every frame (mouse clicks can change the selection at
    // any time) - vkDeviceWaitIdle() avoids racing a previous frame's
    // in-flight command buffer, same reasoning Tutorial15 applies to its
    // own per-frame vertex buffer write.
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
            m_vulkan_tutorial17_parameters.getFinishedRenderingSemaphores()
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

void Tutorial17::onMouseButton(int button,
                               bool pressed,
                               int pos_x,
                               int pos_y) {
    constexpr int c_left_button = 1;
    if (button != c_left_button || !pressed) {
        return;
    }

    float const x = static_cast<float>(pos_x);
    float const y = static_cast<float>(pos_y);
    Math::Vec2<float> const cell_size = getCellSize();
    for (std::size_t index = 0; index < kInventoryItemCount; ++index) {
        Math::Vec2<float> const cell_top_left = getCellTopLeft(index);
        if (x >= cell_top_left.x && x <= cell_top_left.x + cell_size.x &&
            y >= cell_top_left.y && y <= cell_top_left.y + cell_size.y) {
            m_selected_index = index;
            return;
        }
    }
}

void Tutorial17::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

bool Tutorial17::childOnWindowSizeChanged() {
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
    if (!createIconAtlas()) {
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
    if (!allocateDescriptorSets()) {
        return false;
    }
    if (!updateDescriptorSets()) {
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

void Tutorial17::childClear() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return;
    }
    vkDeviceWaitIdle(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial17_parameters.getRenderingResources();
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
                    m_vulkan_tutorial17_parameters.getVkCommandPool(),
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
            m_vulkan_tutorial17_parameters.getFinishedRenderingSemaphores();
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (finished_rendering_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(), finished_rendering_semaphores[i], nullptr);
        }
    }
    finished_rendering_semaphores.clear();

    if (m_vulkan_tutorial17_parameters.getVkCommandPool() != VK_NULL_HANDLE) {
        vkDestroyCommandPool(
                getVkDevice(),
                m_vulkan_tutorial17_parameters.getVkCommandPool(),
                nullptr);
        m_vulkan_tutorial17_parameters.setVkCommandPool(VK_NULL_HANDLE);
    }

    destroyBuffer(m_vulkan_tutorial17_parameters.getVertexBufferParameters());
    destroyBuffer(m_vulkan_tutorial17_parameters.getStagingBufferParameters());

    if (m_vulkan_tutorial17_parameters.getVkGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial17_parameters.getVkGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial17_parameters.setVkGraphicsPipeline(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial17_parameters.getVkPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial17_parameters.getVkPipelineLayout(),
                nullptr);
        m_vulkan_tutorial17_parameters.setVkPipelineLayout(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial17_parameters.getVkRenderPass() != VK_NULL_HANDLE) {
        vkDestroyRenderPass(
                getVkDevice(),
                m_vulkan_tutorial17_parameters.getVkRenderPass(),
                nullptr);
        m_vulkan_tutorial17_parameters.setVkRenderPass(VK_NULL_HANDLE);
    }

    if (m_vulkan_tutorial17_parameters.getVkDescriptorPool() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(
                getVkDevice(),
                m_vulkan_tutorial17_parameters.getVkDescriptorPool(),
                nullptr);
        m_vulkan_tutorial17_parameters.setVkDescriptorPool(VK_NULL_HANDLE);
    }
    if (m_vulkan_tutorial17_parameters.getVkDescriptorSetLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(
                getVkDevice(),
                m_vulkan_tutorial17_parameters.getVkDescriptorSetLayout(),
                nullptr);
        m_vulkan_tutorial17_parameters.setVkDescriptorSetLayout(
                VK_NULL_HANDLE);
    }

    destroyBuffer(m_vulkan_tutorial17_parameters.getUniformBufferParameters());

    VulkanCommon::ImageFactory image_factory(getVkDevice(),
                                             getVkPhysicalDevice());
    image_factory.destroy(
            m_vulkan_tutorial17_parameters.getFontImageParameters());
    image_factory.destroy(
            m_vulkan_tutorial17_parameters.getIconImageParameters());
}

}  // namespace vulkan_graphix
