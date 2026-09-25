#include "vulkan_graphix/Tutorial09.h"

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <cstring>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vulkan_graphix/Math/TessellationOps.hpp"
#include "vulkan_graphix/Math/TessellationTypes.hpp"
#include "vulkan_graphix/Math/Triangle.hpp"
#include "vulkan_graphix/VulkanCommon.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
constexpr VkFormat c_depth_format = VK_FORMAT_D32_SFLOAT;
constexpr VkFormat c_texture_format = VK_FORMAT_R8G8B8A8_UNORM;

constexpr float c_terrain_half_size = 5.0f;
constexpr std::size_t c_terrain_subdivision_count = 6;
constexpr float c_texture_tile_repeat = 8.0f;

// The terrain mesh, built once from two large triangles spanning the
// [-half, half] square in the XZ plane, subdivided via the existing
// tessellation module. Uses the *vector* overload of
// tessellateTrianglesByMidpointSubdivision so both triangles share one
// vertex-dedup map - required since they share an edge (the square's
// diagonal); the singular, single-triangle overload would duplicate every
// vertex along that seam if called once per triangle.
Math::TessellatedTriangleData<float, std::uint32_t> const&
getTerrainTessellation() {
    static Math::TessellatedTriangleData<float,
                                         std::uint32_t> const data = [] {
        using Math::Vec3;
        Vec3<float> const corner_00(
                -c_terrain_half_size, 0.0f, -c_terrain_half_size);
        Vec3<float> const corner_10(
                c_terrain_half_size, 0.0f, -c_terrain_half_size);
        Vec3<float> const corner_11(
                c_terrain_half_size, 0.0f, c_terrain_half_size);
        Vec3<float> const corner_01(
                -c_terrain_half_size, 0.0f, c_terrain_half_size);

        std::vector<Math::Triangle<float>> const tris = {
                Math::Triangle<float>(corner_00, corner_10, corner_11),
                Math::Triangle<float>(corner_00, corner_11, corner_01)};

        Math::TessellatedTriangleData<float, std::uint32_t> result;
        std::uint32_t index = 0;
        Math::tessellateTrianglesByMidpointSubdivision<float, std::uint32_t>(
                tris, c_terrain_subdivision_count, index, result);
        return result;
    }();
    return data;
}
}  // namespace

// ************************************************************ //
// VulkanTutorial09Parameters                                   //
// ************************************************************ //
VulkanTutorial09Parameters::VulkanTutorial09Parameters()
        : m_vk_render_pass(VK_NULL_HANDLE)
        , m_vk_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_graphics_pipeline(VK_NULL_HANDLE)
        , m_index_count(0)
        , m_vk_command_pool(VK_NULL_HANDLE)
        , m_rendering_resources(resources_count) {}

const VkRenderPass& VulkanTutorial09Parameters::getVkRenderPass() const {
    return m_vk_render_pass;
}
VkRenderPass& VulkanTutorial09Parameters::getVkRenderPass() {
    return m_vk_render_pass;
}
void VulkanTutorial09Parameters::setVkRenderPass(
        const VkRenderPass& vk_render_pass) {
    m_vk_render_pass = vk_render_pass;
}

const ImageParameters& VulkanTutorial09Parameters::getDepthImageParameters()
        const {
    return m_depth_image_parameters;
}
ImageParameters& VulkanTutorial09Parameters::getDepthImageParameters() {
    return m_depth_image_parameters;
}
void VulkanTutorial09Parameters::setDepthImageParameters(
        const ImageParameters& depth_image) {
    m_depth_image_parameters = depth_image;
}

const ImageParameters& VulkanTutorial09Parameters::getTextureImageParameters()
        const {
    return m_texture_image_parameters;
}
ImageParameters& VulkanTutorial09Parameters::getTextureImageParameters() {
    return m_texture_image_parameters;
}
void VulkanTutorial09Parameters::setTextureImageParameters(
        const ImageParameters& texture_image) {
    m_texture_image_parameters = texture_image;
}

const BufferParameters&
VulkanTutorial09Parameters::getUniformBufferParameters() const {
    return m_uniform_buffer;
}
BufferParameters& VulkanTutorial09Parameters::getUniformBufferParameters() {
    return m_uniform_buffer;
}
void VulkanTutorial09Parameters::setUniformBufferParameters(
        const BufferParameters& uniform_buffer) {
    m_uniform_buffer = uniform_buffer;
}

const DescriptorSetParameters&
VulkanTutorial09Parameters::getDescriptorSetParameters() const {
    return m_descriptor_set_parameters;
}
DescriptorSetParameters&
VulkanTutorial09Parameters::getDescriptorSetParameters() {
    return m_descriptor_set_parameters;
}
void VulkanTutorial09Parameters::setDescriptorSetParameters(
        const DescriptorSetParameters& descriptor_set_parameters) {
    m_descriptor_set_parameters = descriptor_set_parameters;
}

const VkPipelineLayout& VulkanTutorial09Parameters::getVkPipelineLayout()
        const {
    return m_vk_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial09Parameters::getVkPipelineLayout() {
    return m_vk_pipeline_layout;
}
void VulkanTutorial09Parameters::setVkPipelineLayout(
        const VkPipelineLayout& vk_pipeline_layout) {
    m_vk_pipeline_layout = vk_pipeline_layout;
}

const VkPipeline& VulkanTutorial09Parameters::getVkGraphicsPipeline() const {
    return m_vk_graphics_pipeline;
}
VkPipeline& VulkanTutorial09Parameters::getVkGraphicsPipeline() {
    return m_vk_graphics_pipeline;
}
void VulkanTutorial09Parameters::setVkGraphicsPipeline(
        const VkPipeline& vk_graphics_pipeline) {
    m_vk_graphics_pipeline = vk_graphics_pipeline;
}

const BufferParameters& VulkanTutorial09Parameters::getVertexBufferParameters()
        const {
    return m_vertex_buffer;
}
BufferParameters& VulkanTutorial09Parameters::getVertexBufferParameters() {
    return m_vertex_buffer;
}
void VulkanTutorial09Parameters::setVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_vertex_buffer = vertex_buffer;
}

const BufferParameters& VulkanTutorial09Parameters::getIndexBufferParameters()
        const {
    return m_index_buffer;
}
BufferParameters& VulkanTutorial09Parameters::getIndexBufferParameters() {
    return m_index_buffer;
}
void VulkanTutorial09Parameters::setIndexBufferParameters(
        const BufferParameters& index_buffer) {
    m_index_buffer = index_buffer;
}

std::uint32_t VulkanTutorial09Parameters::getIndexCount() const {
    return m_index_count;
}
void VulkanTutorial09Parameters::setIndexCount(std::uint32_t index_count) {
    m_index_count = index_count;
}

const BufferParameters&
VulkanTutorial09Parameters::getStagingBufferParameters() const {
    return m_staging_buffer;
}
BufferParameters& VulkanTutorial09Parameters::getStagingBufferParameters() {
    return m_staging_buffer;
}
void VulkanTutorial09Parameters::setStagingBufferParameters(
        const BufferParameters& staging_buffer) {
    m_staging_buffer = staging_buffer;
}

const VkCommandPool& VulkanTutorial09Parameters::getVkCommandPool() const {
    return m_vk_command_pool;
}
VkCommandPool& VulkanTutorial09Parameters::getVkCommandPool() {
    return m_vk_command_pool;
}
void VulkanTutorial09Parameters::setVkCommandPool(
        const VkCommandPool& vk_command_pool) {
    m_vk_command_pool = vk_command_pool;
}

const std::vector<RenderingResourceParameters>&
VulkanTutorial09Parameters::getRenderingResources() const {
    return m_rendering_resources;
}
std::vector<RenderingResourceParameters>&
VulkanTutorial09Parameters::getRenderingResources() {
    return m_rendering_resources;
}
void VulkanTutorial09Parameters::setRenderingResources(
        const std::vector<RenderingResourceParameters>& rendering_resources) {
    m_rendering_resources = rendering_resources;
}

const std::vector<VkSemaphore>&
VulkanTutorial09Parameters::getFinishedRenderingSemaphores() const {
    return m_finished_rendering_semaphores;
}
std::vector<VkSemaphore>&
VulkanTutorial09Parameters::getFinishedRenderingSemaphores() {
    return m_finished_rendering_semaphores;
}
void VulkanTutorial09Parameters::setFinishedRenderingSemaphores(
        const std::vector<VkSemaphore>& finished_rendering_semaphores) {
    m_finished_rendering_semaphores = finished_rendering_semaphores;
}

// ************************************************************ //
// Tutorial09                                                   //
// ************************************************************ //
Tutorial09::Tutorial09() = default;

Tutorial09::~Tutorial09() { childClear(); }

void Tutorial09::onMouseButton(int button,
                               bool pressed,
                               int pos_x,
                               int pos_y) {
    m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial09::onMouseMove(int pos_x, int pos_y) {
    m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial09::createCommandPool(std::uint32_t queue_family_index,
                                   VkCommandPool* pool) {
    if (!VulkanCommon::FrameResourceFactory(getVkDevice())
                 .createCommandPool(queue_family_index, pool)) {
        Logging::error(LOG_TAG, "Could not create command pool!");
        return false;
    }
    return true;
}

bool Tutorial09::allocateCommandBuffers(VkCommandPool pool,
                                        std::uint32_t count,
                                        VkCommandBuffer* command_buffers) {
    if (!VulkanCommon::FrameResourceFactory(getVkDevice())
                 .allocateCommandBuffers(pool, count, command_buffers)) {
        Logging::error(LOG_TAG, "Could not allocate command buffer!");
        return false;
    }
    return true;
}

bool Tutorial09::createCommandBuffers() {
    if (!createCommandPool(
                getGraphicsQueueParameters().getFamilyIndex(),
                &m_vulkan_tutorial09_parameters.getVkCommandPool())) {
        return false;
    }

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial09_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!allocateCommandBuffers(
                    m_vulkan_tutorial09_parameters.getVkCommandPool(),
                    1,
                    &rendering_resources[i].getVkCommandBuffer())) {
            return false;
        }
    }
    return true;
}

bool Tutorial09::createSemaphores() {
    VulkanCommon::FrameResourceFactory const frame_resource_factory(
            getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial09_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!frame_resource_factory.createSemaphore(
                    &rendering_resources[i].getImageAvailableVkSemaphore())) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    // A "finished rendering" semaphore must be indexed by the acquired
    // swapchain image, not by the rendering-resource slot. See the comment
    // in Tutorial07::createSemaphores() for the full rationale.
    std::vector<VkSemaphore>& finished_rendering_semaphores =
            m_vulkan_tutorial09_parameters.getFinishedRenderingSemaphores();
    finished_rendering_semaphores.assign(
            getSwapchainParameters().getImageParameters().size(),
            VK_NULL_HANDLE);
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (!frame_resource_factory.createSemaphore(
                    &finished_rendering_semaphores[i])) {
            Logging::error(LOG_TAG, "Could not create semaphores!");
            return false;
        }
    }

    return true;
}

bool Tutorial09::createFences() {
    VulkanCommon::FrameResourceFactory const frame_resource_factory(
            getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial09_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!frame_resource_factory.createFence(
                    /*signaled=*/true, &rendering_resources[i].getVkFence())) {
            Logging::error(LOG_TAG, "Could not create a fence!");
            return false;
        }
    }
    return true;
}

bool Tutorial09::createRenderingResources() {
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

bool Tutorial09::createBuffer(VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags memory_property,
                              BufferParameters& buffer) {
    if (!VulkanCommon::BufferFactory(getVkDevice(), getVkPhysicalDevice())
                 .create(usage, memory_property, buffer)) {
        Logging::error(LOG_TAG, "Could not create buffer!");
        return false;
    }
    return true;
}

bool Tutorial09::createStagingBuffer() {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial09_parameters.getStagingBufferParameters();
    staging_buffer.setSize(4000000);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

bool Tutorial09::createImage(std::uint32_t width,
                             std::uint32_t height,
                             VkFormat format,
                             VkImageUsageFlags usage,
                             VkImage* image) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .createImage(width, height, format, usage, image);
}

bool Tutorial09::allocateImageMemory(VkImage image,
                                     VkMemoryPropertyFlags property,
                                     VkDeviceMemory* memory) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .allocateMemory(image, property, memory);
}

bool Tutorial09::createImageView(VkImage image,
                                 VkFormat format,
                                 VkImageAspectFlags aspect_mask,
                                 VkImageView* image_view) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .createImageView(image, format, aspect_mask, image_view);
}

bool Tutorial09::createDepthResources() {
    ImageParameters& depth_image =
            m_vulkan_tutorial09_parameters.getDepthImageParameters();

    VkImage vk_image;
    if (!createImage(getSwapchainParameters().getVkExtent2d().width,
                     getSwapchainParameters().getVkExtent2d().height,
                     c_depth_format,
                     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                     &vk_image)) {
        Logging::error(LOG_TAG, "Could not create depth image!");
        return false;
    }
    depth_image.setVkImage(vk_image);

    VkDeviceMemory vk_device_memory;
    if (!allocateImageMemory(depth_image.getVkImage(),
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &vk_device_memory)) {
        Logging::error(LOG_TAG, "Could not allocate memory for depth image!");
        return false;
    }
    depth_image.setVkDeviceMemory(vk_device_memory);

    if (vkBindImageMemory(getVkDevice(),
                          depth_image.getVkImage(),
                          depth_image.getVkDeviceMemory(),
                          0) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not bind memory to depth image!");
        return false;
    }

    VkImageView vk_image_view;
    if (!createImageView(depth_image.getVkImage(),
                         c_depth_format,
                         VK_IMAGE_ASPECT_DEPTH_BIT,
                         &vk_image_view)) {
        Logging::error(LOG_TAG, "Could not create depth image view!");
        return false;
    }
    depth_image.setVkImageView(vk_image_view);

    return true;
}

bool Tutorial09::destroyDepthResources() {
    VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .destroy(m_vulkan_tutorial09_parameters.getDepthImageParameters());
    return true;
}

bool Tutorial09::createSampler(VkSamplerAddressMode address_mode,
                               VkSampler* sampler) {
    return VulkanCommon::ImageFactory(getVkDevice(), getVkPhysicalDevice())
            .createSampler(address_mode, sampler);
}

bool Tutorial09::copyTextureData(char* texture_data,
                                 std::uint32_t data_size,
                                 std::uint32_t width,
                                 std::uint32_t height) {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial09_parameters.getStagingBufferParameters();
    ImageParameters& image_parameters =
            m_vulkan_tutorial09_parameters.getTextureImageParameters();

    if (!VulkanCommon::StagedUploader(
                getVkDevice(),
                getGraphicsQueueParameters().getVkQueue(),
                m_vulkan_tutorial09_parameters.getRenderingResources()[0]
                        .getVkCommandBuffer())
                 .uploadToImage(staging_buffer,
                               image_parameters.getVkImage(),
                               texture_data,
                               data_size,
                               width,
                               height)) {
        Logging::error(LOG_TAG,
                       "Could not map memory and upload texture data to a "
                       "staging buffer!");
        return false;
    }

    return true;
}

bool Tutorial09::createTexture() {
    int width = 0, height = 0, data_size = 0;
    std::vector<char> texture_data = Tools::getImageData(
            "texture.09.png", 4, &width, &height, nullptr, &data_size);
    if (texture_data.empty()) {
        return false;
    }

    ImageParameters& image_parameters =
            m_vulkan_tutorial09_parameters.getTextureImageParameters();

    VkImage vk_image;
    if (!createImage(
                static_cast<std::uint32_t>(width),
                static_cast<std::uint32_t>(height),
                c_texture_format,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                &vk_image)) {
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

    VkImageView vk_image_view;
    if (!createImageView(image_parameters.getVkImage(),
                         c_texture_format,
                         VK_IMAGE_ASPECT_COLOR_BIT,
                         &vk_image_view)) {
        Logging::error(LOG_TAG, "Could not create image view!");
        return false;
    }
    image_parameters.setVkImageView(vk_image_view);

    // REPEAT (not Tutorial06/07's CLAMP_TO_EDGE): texcoords intentionally
    // exceed [0,1] so the ground texture tiles across the terrain instead
    // of being stretched into one blurry copy.
    VkSampler vk_sampler;
    if (!createSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT, &vk_sampler)) {
        Logging::error(LOG_TAG, "Could not create sampler!");
        return false;
    }
    image_parameters.setVkSampler(vk_sampler);

    if (!copyTextureData(texture_data.data(),
                         static_cast<std::uint32_t>(data_size),
                         static_cast<std::uint32_t>(width),
                         static_cast<std::uint32_t>(height))) {
        Logging::error(LOG_TAG,
                       "Could not upload texture data to device memory!");
        return false;
    }

    return true;
}

bool Tutorial09::createUniformBuffer() {
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial09_parameters.getUniformBufferParameters();
    uniform_buffer.setSize(sizeof(Tutorial09UniformBufferData));
    // Host-visible/coherent (not device-local + staging) since this buffer
    // is rewritten every frame as the orbit camera moves; see
    // updateUniformBufferData()/draw() for how the CPU/GPU race that
    // creates is avoided.
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      uniform_buffer)) {
        Logging::error(LOG_TAG, "Could not create uniform buffer!");
        return false;
    }

    return updateUniformBufferData();
}

Tutorial09UniformBufferData Tutorial09::getUniformBufferData() const {
    Tutorial09UniformBufferData data{};
    data.model = Math::Mat4<float>(1.0f);  // static terrain, no rotation
    data.view = glm::lookAt(m_camera.eye(),
                            m_camera.target(),
                            Math::Vec3<float>(0.0f, 1.0f, 0.0f));

    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    data.projection = Tools::getPerspectiveProjectionMatrix(
            width / height, 45.0f, 0.1f, 100.0f);

    data.light_position = Math::Vec4<float>(5.0f, 8.0f, 5.0f, 1.0f);
    data.light_color = Math::Vec4<float>(1.0f, 1.0f, 1.0f, 1.0f);
    data.view_position = Math::Vec4<float>(m_camera.eye(), 1.0f);

    return data;
}

bool Tutorial09::updateUniformBufferData() {
    Tutorial09UniformBufferData const uniform_data = getUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial09_parameters.getUniformBufferParameters();

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

bool Tutorial09::createDescriptorSetLayout() {
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings = {
            {.binding = 0,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags =
                     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
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

    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial09_parameters.getDescriptorSetParameters();

    VkDescriptorSetLayout vk_descriptor_set_layout;
    if (vkCreateDescriptorSetLayout(getVkDevice(),
                                    &descriptor_set_layout_create_info,
                                    nullptr,
                                    &vk_descriptor_set_layout) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create descriptor set layout!");
        return false;
    }
    descriptor_set.setVkDescriptorSetLayout(vk_descriptor_set_layout);

    return true;
}

bool Tutorial09::createDescriptorPool() {
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

    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial09_parameters.getDescriptorSetParameters();

    VkDescriptorPool vk_descriptor_pool;
    if (vkCreateDescriptorPool(getVkDevice(),
                               &descriptor_pool_create_info,
                               nullptr,
                               &vk_descriptor_pool) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create descriptor pool!");
        return false;
    }
    descriptor_set.setVkDescriptorPool(vk_descriptor_pool);

    return true;
}

bool Tutorial09::allocateDescriptorSet() {
    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial09_parameters.getDescriptorSetParameters();

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

bool Tutorial09::updateDescriptorSet() {
    const BufferParameters& uniform_buffer =
            m_vulkan_tutorial09_parameters.getUniformBufferParameters();
    const ImageParameters& image_parameters =
            m_vulkan_tutorial09_parameters.getTextureImageParameters();
    const DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial09_parameters.getDescriptorSetParameters();

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

bool Tutorial09::createRenderPass() {
    VkAttachmentDescription attachment_descriptions[] = {
            {.flags = 0,
             .format = getSwapchainParameters().getVkFormat(),
             .samples = VK_SAMPLE_COUNT_1_BIT,
             .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
             .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
             .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
             .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
             .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
             .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
            {.flags = 0,
             .format = c_depth_format,
             .samples = VK_SAMPLE_COUNT_1_BIT,
             .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
             .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
             .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
             .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
             .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
             .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}};

    VkAttachmentReference color_attachment_references[] = {
            {.attachment = 0,
             .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}};

    VkAttachmentReference depth_attachment_reference = {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpass_descriptions[] = {
            {.flags = 0,
             .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
             .inputAttachmentCount = 0,
             .pInputAttachments = nullptr,
             .colorAttachmentCount = 1,
             .pColorAttachments = color_attachment_references,
             .pResolveAttachments = nullptr,
             .pDepthStencilAttachment = &depth_attachment_reference,
             .preserveAttachmentCount = 0,
             .pPreserveAttachments = nullptr}};

    VkRenderPassCreateInfo render_pass_create_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .attachmentCount = 2,
            .pAttachments = attachment_descriptions,
            .subpassCount = 1,
            .pSubpasses = subpass_descriptions,
            .dependencyCount = 0,
            .pDependencies = nullptr};

    if (vkCreateRenderPass(
                getVkDevice(),
                &render_pass_create_info,
                nullptr,
                &m_vulkan_tutorial09_parameters.getVkRenderPass()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

bool Tutorial09::createPipelineLayout() {
    VkDescriptorSetLayout vk_descriptor_set_layout =
            m_vulkan_tutorial09_parameters.getDescriptorSetParameters()
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
                &m_vulkan_tutorial09_parameters.getVkPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create pipeline layout!");
        return false;
    }

    return true;
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
Tutorial09::createShaderModule(const char* filename) {
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

bool Tutorial09::createPipeline() {
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module = createShaderModule("shader.09.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module = createShaderModule("shader.09.frag.spv");

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
              .stride = VertexAttributeTraits::stride,
              .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};

    std::vector<VkVertexInputAttributeDescription>
            vertex_attribute_descriptions = {
                    {.location = 0,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset =
                             offsetof(struct Tutorial09VertexData, position)},
                    {.location = 1,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32_SFLOAT,
                     .offset = offsetof(struct Tutorial09VertexData, normal)},
                    {.location = 2,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32_SFLOAT,
                     .offset =
                             offsetof(struct Tutorial09VertexData, texcoord)}};

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
            // Disabled on purpose: the terrain is a single flat plane, and
            // the user should be able to orbit the camera underneath it
            // and still see it shaded (see shader.09.frag's normal-flip).
            .cullMode = VK_CULL_MODE_NONE,
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

    VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
            .sType =
                    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
            .front = {},
            .back = {},
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f};

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
            .pDepthStencilState = &depth_stencil_state_create_info,
            .pColorBlendState = &color_blend_state_create_info,
            .pDynamicState = &dynamic_state_create_info,
            .layout = m_vulkan_tutorial09_parameters.getVkPipelineLayout(),
            .renderPass = m_vulkan_tutorial09_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};

    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info,
                nullptr,
                &m_vulkan_tutorial09_parameters.getVkGraphicsPipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create graphics pipeline!");
        return false;
    }
    return true;
}

const std::vector<Tutorial09VertexData>& Tutorial09::getVertexData() const {
    static const std::vector<Tutorial09VertexData> vertex_data = [] {
        Math::TessellatedTriangleData<float, std::uint32_t> const& tess =
                getTerrainTessellation();

        std::vector<Tutorial09VertexData> data;
        data.reserve(tess.points().size());
        for (Math::Vec3<float> const& point : tess.points()) {
            float const texcoord_u = (point.x + c_terrain_half_size) /
                                     (2.0f * c_terrain_half_size) *
                                     c_texture_tile_repeat;
            float const texcoord_v = (point.z + c_terrain_half_size) /
                                     (2.0f * c_terrain_half_size) *
                                     c_texture_tile_repeat;
            data.push_back({Math::Vec4<float>(point, 1.0f),
                            Math::Vec3<float>(0.0f, 1.0f, 0.0f),
                            Math::Vec2<float>(texcoord_u, texcoord_v)});
        }
        return data;
    }();

    return vertex_data;
}

const std::vector<std::uint32_t>& Tutorial09::getIndexData() const {
    return getTerrainTessellation().indices();
}

bool Tutorial09::copyBufferData(BufferParameters& destination,
                                const void* data,
                                std::uint32_t data_size,
                                VkAccessFlags dst_access_mask,
                                VkPipelineStageFlags dst_stage_mask) {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial09_parameters.getStagingBufferParameters();

    if (!VulkanCommon::StagedUploader(
                getVkDevice(),
                getGraphicsQueueParameters().getVkQueue(),
                m_vulkan_tutorial09_parameters.getRenderingResources()[0]
                        .getVkCommandBuffer())
                 .uploadToBuffer(staging_buffer,
                                destination,
                                data,
                                data_size,
                                dst_access_mask,
                                dst_stage_mask)) {
        Logging::error(
                LOG_TAG,
                "Could not map memory and upload data to a staging buffer!");
        return false;
    }

    return true;
}

bool Tutorial09::createVertexBuffer() {
    const std::vector<Tutorial09VertexData>& vertex_data = getVertexData();

    BufferParameters& vertex_buffer =
            m_vulkan_tutorial09_parameters.getVertexBufferParameters();
    vertex_buffer.setSize(static_cast<std::uint32_t>(vertex_data.size() *
                                                     sizeof(vertex_data[0])));
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      vertex_buffer)) {
        Logging::error(LOG_TAG, "Could not create vertex buffer!");
        return false;
    }

    return copyBufferData(vertex_buffer,
                          vertex_data.data(),
                          vertex_buffer.getSize(),
                          VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                          VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
}

bool Tutorial09::createIndexBuffer() {
    const std::vector<std::uint32_t>& index_data = getIndexData();
    m_vulkan_tutorial09_parameters.setIndexCount(
            static_cast<std::uint32_t>(index_data.size()));

    BufferParameters& index_buffer =
            m_vulkan_tutorial09_parameters.getIndexBufferParameters();
    index_buffer.setSize(static_cast<std::uint32_t>(index_data.size() *
                                                    sizeof(index_data[0])));
    if (!createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      index_buffer)) {
        Logging::error(LOG_TAG, "Could not create index buffer!");
        return false;
    }

    return copyBufferData(index_buffer,
                          index_data.data(),
                          index_buffer.getSize(),
                          VK_ACCESS_INDEX_READ_BIT,
                          VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
}

bool Tutorial09::createFramebuffer(VkFramebuffer& framebuffer,
                                   VkImageView image_view) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(getVkDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    VkImageView attachments[] = {
            image_view,
            m_vulkan_tutorial09_parameters.getDepthImageParameters()
                    .getVkImageView()};

    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_vulkan_tutorial09_parameters.getVkRenderPass(),
            .attachmentCount = 2,
            .pAttachments = attachments,
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

bool Tutorial09::prepareFrame(VkCommandBuffer command_buffer,
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

    VkClearValue clear_values[] = {
            {.color = {.float32 = {0.35f, 0.55f, 0.85f, 0.0f}}},
            {.depthStencil = {.depth = 1.0f, .stencil = 0}}};

    VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = m_vulkan_tutorial09_parameters.getVkRenderPass(),
            .framebuffer = framebuffer,
            .renderArea = {.offset = {.x = 0, .y = 0},
                           .extent = getSwapchainParameters().getVkExtent2d()},
            .clearValueCount = 2,
            .pClearValues = clear_values};

    vkCmdBeginRenderPass(command_buffer,
                         &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command_buffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_vulkan_tutorial09_parameters.getVkGraphicsPipeline());

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
            &m_vulkan_tutorial09_parameters.getVertexBufferParameters()
                     .getVkBuffer(),
            &offset);

    vkCmdBindIndexBuffer(
            command_buffer,
            m_vulkan_tutorial09_parameters.getIndexBufferParameters()
                    .getVkBuffer(),
            0,
            VK_INDEX_TYPE_UINT32);

    VkDescriptorSet vk_descriptor_set =
            m_vulkan_tutorial09_parameters.getDescriptorSetParameters()
                    .getVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial09_parameters.getVkPipelineLayout(),
            0,
            1,
            &vk_descriptor_set,
            0,
            nullptr);

    vkCmdDrawIndexed(command_buffer,
                     m_vulkan_tutorial09_parameters.getIndexCount(),
                     1,
                     0,
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

bool Tutorial09::draw() {
    static std::size_t resource_index = 0;
    RenderingResourceParameters& current_rendering_resource =
            m_vulkan_tutorial09_parameters
                    .getRenderingResources()[resource_index];
    VkSwapchainKHR swap_chain = getSwapchainParameters().getVkSwapchainKhr();
    std::uint32_t image_index;

    resource_index =
            (resource_index + 1) % VulkanTutorial09Parameters::resources_count;

    if (vkWaitForFences(getVkDevice(),
                        1,
                        &current_rendering_resource.getVkFence(),
                        VK_FALSE,
                        1000000000) != VK_SUCCESS) {
        Logging::error(LOG_TAG, "Waiting for fence takes too long!");
        return false;
    }
    vkResetFences(getVkDevice(), 1, &current_rendering_resource.getVkFence());

    // The uniform buffer is a single, shared, host-visible allocation (not
    // one per rendering resource), rewritten every frame from the orbit
    // camera's current state. Waiting for the whole device to go idle
    // before touching it keeps the CPU write from racing any in-flight
    // frame that might still be reading it on the GPU. See Tutorial08's
    // draw() for the same tradeoff spelled out in more detail.
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
            m_vulkan_tutorial09_parameters
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

void Tutorial09::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

bool Tutorial09::childOnWindowSizeChanged() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return true;
    }
    vkDeviceWaitIdle(getVkDevice());

    // TutorialBase::onWindowSizeChanged() calls childClear() unconditionally
    // before this runs, which tears down every Vulkan object this tutorial
    // owns - not just the swapchain. Everything has to be rebuilt here, in
    // the same order Tutorial09Main.cpp's initial setup uses. See
    // Tutorial04-08's identical fix for the full rationale.
    if (!createRenderingResources()) {
        return false;
    }
    if (!createStagingBuffer()) {
        return false;
    }
    if (!createDepthResources()) {
        return false;
    }
    if (!createTexture()) {
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

void Tutorial09::childClear() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return;
    }
    vkDeviceWaitIdle(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial09_parameters.getRenderingResources();
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
                    m_vulkan_tutorial09_parameters.getVkCommandPool(),
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
            m_vulkan_tutorial09_parameters.getFinishedRenderingSemaphores();
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (finished_rendering_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(), finished_rendering_semaphores[i], nullptr);
        }
    }
    finished_rendering_semaphores.clear();

    if (m_vulkan_tutorial09_parameters.getVkCommandPool() != VK_NULL_HANDLE) {
        vkDestroyCommandPool(getVkDevice(),
                             m_vulkan_tutorial09_parameters.getVkCommandPool(),
                             nullptr);
        m_vulkan_tutorial09_parameters.getVkCommandPool() = VK_NULL_HANDLE;
    }

    destroyBuffer(m_vulkan_tutorial09_parameters.getVertexBufferParameters());
    destroyBuffer(m_vulkan_tutorial09_parameters.getIndexBufferParameters());
    destroyBuffer(m_vulkan_tutorial09_parameters.getStagingBufferParameters());

    if (m_vulkan_tutorial09_parameters.getVkGraphicsPipeline() !=
        VK_NULL_HANDLE) {
        vkDestroyPipeline(
                getVkDevice(),
                m_vulkan_tutorial09_parameters.getVkGraphicsPipeline(),
                nullptr);
        m_vulkan_tutorial09_parameters.getVkGraphicsPipeline() =
                VK_NULL_HANDLE;
    }

    if (m_vulkan_tutorial09_parameters.getVkPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial09_parameters.getVkPipelineLayout(),
                nullptr);
        m_vulkan_tutorial09_parameters.getVkPipelineLayout() = VK_NULL_HANDLE;
    }

    if (m_vulkan_tutorial09_parameters.getVkRenderPass() != VK_NULL_HANDLE) {
        vkDestroyRenderPass(getVkDevice(),
                            m_vulkan_tutorial09_parameters.getVkRenderPass(),
                            nullptr);
        m_vulkan_tutorial09_parameters.getVkRenderPass() = VK_NULL_HANDLE;
    }

    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial09_parameters.getDescriptorSetParameters();
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

    destroyBuffer(m_vulkan_tutorial09_parameters.getUniformBufferParameters());

    ImageParameters& texture_image =
            m_vulkan_tutorial09_parameters.getTextureImageParameters();
    if (texture_image.getVkSampler() != VK_NULL_HANDLE) {
        vkDestroySampler(getVkDevice(), texture_image.getVkSampler(), nullptr);
        texture_image.setVkSampler(VK_NULL_HANDLE);
    }
    if (texture_image.getVkImageView() != VK_NULL_HANDLE) {
        vkDestroyImageView(
                getVkDevice(), texture_image.getVkImageView(), nullptr);
        texture_image.setVkImageView(VK_NULL_HANDLE);
    }
    if (texture_image.getVkImage() != VK_NULL_HANDLE) {
        vkDestroyImage(getVkDevice(), texture_image.getVkImage(), nullptr);
        texture_image.setVkImage(VK_NULL_HANDLE);
    }
    if (texture_image.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(
                getVkDevice(), texture_image.getVkDeviceMemory(), nullptr);
        texture_image.setVkDeviceMemory(VK_NULL_HANDLE);
    }

    destroyDepthResources();
}

}  // namespace vulkan_graphix
