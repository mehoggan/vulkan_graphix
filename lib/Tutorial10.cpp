#include "vulkan_graphix/Tutorial10.h"

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>

#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/Math/CubicCurve.hpp"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix {

namespace {
constexpr VkFormat c_depth_format = VK_FORMAT_D32_SFLOAT;

constexpr float c_chordal_tolerance = 0.01f;

// The camera's initial view: a 3/4 vantage point rather than the default
// front-on one, so the curve's 3D shape reads clearly from the first frame.
constexpr float c_initial_camera_yaw = 0.78539816f;    // 45 degrees
constexpr float c_initial_camera_pitch = 0.78539816f;  // 45 degrees
constexpr float c_initial_camera_distance = 8.0f;

// The curve's control points: a single gentle bend, like a road segment
// seen on a map - mostly planar, one smooth arc rather than a full loop or
// wave, with only a slight elevation change for a subtle 3D feel. x is
// monotonic (the road's length) so the curve never doubles back on itself.
// Earlier versions of this curve swept a full spiral turn, then a full
// sine wave, and both read as too tangled/dramatic once projected to 2D.
std::vector<Math::Vec3<float>> const& getControlPoints() {
    static std::vector<Math::Vec3<float>> const points = [] {
        constexpr std::size_t point_count = 6;
        constexpr float x_start = -3.0f;
        constexpr float x_end = 3.0f;
        constexpr float bend_amplitude = 1.0f;
        constexpr float elevation_gain = 0.4f;
        constexpr float half_turn = 3.14159265358979323846f;

        std::vector<Math::Vec3<float>> result;
        result.reserve(point_count);
        for (std::size_t i = 0; i < point_count; ++i) {
            float const fraction = static_cast<float>(i) /
                                   static_cast<float>(point_count - 1);
            result.emplace_back(
                    x_start + fraction * (x_end - x_start),
                    elevation_gain * fraction,
                    bend_amplitude * std::sin(fraction * half_turn));
        }
        return result;
    }();
    return points;
}

// The Catmull-Rom spline through getControlPoints(), adaptively sampled
// per segment via Math::CubicCurve::computeSamplesAdaptive. Duplicates the
// first/last control points ("phantom" endpoints) so the curve actually
// passes through every point in getControlPoints(), including the first
// and last - true Catmull-Rom otherwise only interpolates the interior
// points, using the endpoints purely to shape the tangents at the ends of
// that interior span.
std::vector<Math::CurveSample3D<float>> const& getCurveSamples() {
    static std::vector<Math::CurveSample3D<float>> const samples = [] {
        std::vector<Math::Vec3<float>> const& control_points =
                getControlPoints();

        std::vector<Math::Vec3<float>> working_points;
        working_points.reserve(control_points.size() + 2);
        working_points.push_back(control_points.front());
        working_points.insert(working_points.end(),
                              control_points.begin(),
                              control_points.end());
        working_points.push_back(control_points.back());

        std::vector<Math::CurveSample3D<float>> result;
        std::size_t const segment_count = working_points.size() - 3;
        for (std::size_t segment = 0; segment < segment_count; ++segment) {
            Math::CubicCurve<float> const curve(
                    Math::CubicCurve<float>::Type::CatmullRom,
                    working_points[segment],
                    working_points[segment + 1],
                    working_points[segment + 2],
                    working_points[segment + 3]);
            std::vector<Math::CurveSample3D<float>> const segment_samples =
                    curve.computeSamplesAdaptive(c_chordal_tolerance);

            // Segment i's last sample and segment i+1's first sample are
            // the same point (the shared control point between them) -
            // skip it to avoid a duplicate vertex in the polyline.
            std::size_t const start_index = (segment == 0) ? 0 : 1;
            for (std::size_t i = start_index; i < segment_samples.size();
                 ++i) {
                result.push_back(segment_samples[i]);
            }
        }
        return result;
    }();
    return samples;
}
}  // namespace

// ************************************************************ //
// VulkanTutorial10Parameters                                   //
// ************************************************************ //
VulkanTutorial10Parameters::VulkanTutorial10Parameters()
        : m_vk_render_pass(VK_NULL_HANDLE)
        , m_vk_pipeline_layout(VK_NULL_HANDLE)
        , m_vk_line_pipeline(VK_NULL_HANDLE)
        , m_curve_vertex_count(0)
        , m_control_polygon_vertex_count(0)
        , m_vk_command_pool(VK_NULL_HANDLE)
        , m_rendering_resources(resources_count) {}

const VkRenderPass& VulkanTutorial10Parameters::getVkRenderPass() const {
    return m_vk_render_pass;
}
VkRenderPass& VulkanTutorial10Parameters::getVkRenderPass() {
    return m_vk_render_pass;
}
void VulkanTutorial10Parameters::setVkRenderPass(
        const VkRenderPass& vk_render_pass) {
    m_vk_render_pass = vk_render_pass;
}

const ImageParameters& VulkanTutorial10Parameters::getDepthImageParameters()
        const {
    return m_depth_image_parameters;
}
ImageParameters& VulkanTutorial10Parameters::getDepthImageParameters() {
    return m_depth_image_parameters;
}
void VulkanTutorial10Parameters::setDepthImageParameters(
        const ImageParameters& depth_image) {
    m_depth_image_parameters = depth_image;
}

const BufferParameters&
VulkanTutorial10Parameters::getUniformBufferParameters() const {
    return m_uniform_buffer;
}
BufferParameters& VulkanTutorial10Parameters::getUniformBufferParameters() {
    return m_uniform_buffer;
}
void VulkanTutorial10Parameters::setUniformBufferParameters(
        const BufferParameters& uniform_buffer) {
    m_uniform_buffer = uniform_buffer;
}

const DescriptorSetParameters&
VulkanTutorial10Parameters::getDescriptorSetParameters() const {
    return m_descriptor_set_parameters;
}
DescriptorSetParameters&
VulkanTutorial10Parameters::getDescriptorSetParameters() {
    return m_descriptor_set_parameters;
}
void VulkanTutorial10Parameters::setDescriptorSetParameters(
        const DescriptorSetParameters& descriptor_set_parameters) {
    m_descriptor_set_parameters = descriptor_set_parameters;
}

const VkPipelineLayout& VulkanTutorial10Parameters::getVkPipelineLayout()
        const {
    return m_vk_pipeline_layout;
}
VkPipelineLayout& VulkanTutorial10Parameters::getVkPipelineLayout() {
    return m_vk_pipeline_layout;
}
void VulkanTutorial10Parameters::setVkPipelineLayout(
        const VkPipelineLayout& vk_pipeline_layout) {
    m_vk_pipeline_layout = vk_pipeline_layout;
}

const VkPipeline& VulkanTutorial10Parameters::getVkLinePipeline() const {
    return m_vk_line_pipeline;
}
VkPipeline& VulkanTutorial10Parameters::getVkLinePipeline() {
    return m_vk_line_pipeline;
}
void VulkanTutorial10Parameters::setVkLinePipeline(
        const VkPipeline& vk_line_pipeline) {
    m_vk_line_pipeline = vk_line_pipeline;
}

const BufferParameters&
VulkanTutorial10Parameters::getCurveVertexBufferParameters() const {
    return m_curve_vertex_buffer;
}
BufferParameters&
VulkanTutorial10Parameters::getCurveVertexBufferParameters() {
    return m_curve_vertex_buffer;
}
void VulkanTutorial10Parameters::setCurveVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_curve_vertex_buffer = vertex_buffer;
}

std::uint32_t VulkanTutorial10Parameters::getCurveVertexCount() const {
    return m_curve_vertex_count;
}
void VulkanTutorial10Parameters::setCurveVertexCount(
        std::uint32_t vertex_count) {
    m_curve_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial10Parameters::getControlPolygonVertexBufferParameters() const {
    return m_control_polygon_vertex_buffer;
}
BufferParameters&
VulkanTutorial10Parameters::getControlPolygonVertexBufferParameters() {
    return m_control_polygon_vertex_buffer;
}
void VulkanTutorial10Parameters::setControlPolygonVertexBufferParameters(
        const BufferParameters& vertex_buffer) {
    m_control_polygon_vertex_buffer = vertex_buffer;
}

std::uint32_t VulkanTutorial10Parameters::getControlPolygonVertexCount()
        const {
    return m_control_polygon_vertex_count;
}
void VulkanTutorial10Parameters::setControlPolygonVertexCount(
        std::uint32_t vertex_count) {
    m_control_polygon_vertex_count = vertex_count;
}

const BufferParameters&
VulkanTutorial10Parameters::getStagingBufferParameters() const {
    return m_staging_buffer;
}
BufferParameters& VulkanTutorial10Parameters::getStagingBufferParameters() {
    return m_staging_buffer;
}
void VulkanTutorial10Parameters::setStagingBufferParameters(
        const BufferParameters& staging_buffer) {
    m_staging_buffer = staging_buffer;
}

const VkCommandPool& VulkanTutorial10Parameters::getVkCommandPool() const {
    return m_vk_command_pool;
}
VkCommandPool& VulkanTutorial10Parameters::getVkCommandPool() {
    return m_vk_command_pool;
}
void VulkanTutorial10Parameters::setVkCommandPool(
        const VkCommandPool& vk_command_pool) {
    m_vk_command_pool = vk_command_pool;
}

const std::vector<RenderingResourceParameters>&
VulkanTutorial10Parameters::getRenderingResources() const {
    return m_rendering_resources;
}
std::vector<RenderingResourceParameters>&
VulkanTutorial10Parameters::getRenderingResources() {
    return m_rendering_resources;
}
void VulkanTutorial10Parameters::setRenderingResources(
        const std::vector<RenderingResourceParameters>& rendering_resources) {
    m_rendering_resources = rendering_resources;
}

const std::vector<VkSemaphore>&
VulkanTutorial10Parameters::getFinishedRenderingSemaphores() const {
    return m_finished_rendering_semaphores;
}
std::vector<VkSemaphore>&
VulkanTutorial10Parameters::getFinishedRenderingSemaphores() {
    return m_finished_rendering_semaphores;
}
void VulkanTutorial10Parameters::setFinishedRenderingSemaphores(
        const std::vector<VkSemaphore>& finished_rendering_semaphores) {
    m_finished_rendering_semaphores = finished_rendering_semaphores;
}

// ************************************************************ //
// Tutorial10                                                   //
// ************************************************************ //
Tutorial10::Tutorial10()
        : m_camera(c_initial_camera_yaw,
                   c_initial_camera_pitch,
                   c_initial_camera_distance) {}

Tutorial10::~Tutorial10() { childClear(); }

void Tutorial10::onMouseButton(int button,
                               bool pressed,
                               int pos_x,
                               int pos_y) {
    m_camera.onMouseButton(button, pressed, pos_x, pos_y);
}

void Tutorial10::onMouseMove(int pos_x, int pos_y) {
    m_camera.onMouseMove(pos_x, pos_y);
}

bool Tutorial10::createCommandPool(std::uint32_t queue_family_index,
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

bool Tutorial10::allocateCommandBuffers(VkCommandPool pool,
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

bool Tutorial10::createCommandBuffers() {
    if (!createCommandPool(
                getGraphicsQueueParameters().getFamilyIndex(),
                &m_vulkan_tutorial10_parameters.getVkCommandPool())) {
        return false;
    }

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial10_parameters.getRenderingResources();
    for (std::size_t i = 0; i < rendering_resources.size(); ++i) {
        if (!allocateCommandBuffers(
                    m_vulkan_tutorial10_parameters.getVkCommandPool(),
                    1,
                    &rendering_resources[i].getVkCommandBuffer())) {
            return false;
        }
    }
    return true;
}

bool Tutorial10::createSemaphores() {
    VkSemaphoreCreateInfo semaphore_create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0};

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial10_parameters.getRenderingResources();
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
    // swapchain image, not by the rendering-resource slot. See the comment
    // in Tutorial07::createSemaphores() for the full rationale.
    std::vector<VkSemaphore>& finished_rendering_semaphores =
            m_vulkan_tutorial10_parameters.getFinishedRenderingSemaphores();
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

bool Tutorial10::createFences() {
    VkFenceCreateInfo fence_create_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial10_parameters.getRenderingResources();
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

bool Tutorial10::createRenderingResources() {
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

bool Tutorial10::allocateBufferMemory(VkBuffer buffer,
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

bool Tutorial10::createBuffer(VkBufferUsageFlags usage,
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

bool Tutorial10::createStagingBuffer() {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial10_parameters.getStagingBufferParameters();
    staging_buffer.setSize(2000000);
    if (!createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                      staging_buffer)) {
        Logging::error(LOG_TAG, "Could not create staging buffer!");
        return false;
    }

    return true;
}

bool Tutorial10::createImage(std::uint32_t width,
                             std::uint32_t height,
                             VkFormat format,
                             VkImageUsageFlags usage,
                             VkImage* image) {
    VkImageCreateInfo image_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent = {.width = width, .height = height, .depth = 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

    return vkCreateImage(getVkDevice(), &image_create_info, nullptr, image) ==
           VK_SUCCESS;
}

bool Tutorial10::allocateImageMemory(VkImage image,
                                     VkMemoryPropertyFlags property,
                                     VkDeviceMemory* memory) {
    VkMemoryRequirements image_memory_requirements;
    vkGetImageMemoryRequirements(
            getVkDevice(), image, &image_memory_requirements);

    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(getVkPhysicalDevice(),
                                        &memory_properties);

    for (std::uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((image_memory_requirements.memoryTypeBits & (1 << i)) &&
            ((memory_properties.memoryTypes[i].propertyFlags & property) ==
             property)) {
            VkMemoryAllocateInfo memory_allocate_info = {
                    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                    .pNext = nullptr,
                    .allocationSize = image_memory_requirements.size,
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

bool Tutorial10::createImageView(VkImage image,
                                 VkFormat format,
                                 VkImageAspectFlags aspect_mask,
                                 VkImageView* image_view) {
    VkImageViewCreateInfo image_view_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                           .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                           .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                           .a = VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange = {.aspectMask = aspect_mask,
                                 .baseMipLevel = 0,
                                 .levelCount = 1,
                                 .baseArrayLayer = 0,
                                 .layerCount = 1}};

    return vkCreateImageView(getVkDevice(),
                             &image_view_create_info,
                             nullptr,
                             image_view) == VK_SUCCESS;
}

bool Tutorial10::createDepthResources() {
    ImageParameters& depth_image =
            m_vulkan_tutorial10_parameters.getDepthImageParameters();

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

bool Tutorial10::destroyDepthResources() {
    ImageParameters& depth_image =
            m_vulkan_tutorial10_parameters.getDepthImageParameters();

    if (depth_image.getVkImageView() != VK_NULL_HANDLE) {
        vkDestroyImageView(
                getVkDevice(), depth_image.getVkImageView(), nullptr);
        depth_image.setVkImageView(VK_NULL_HANDLE);
    }
    if (depth_image.getVkImage() != VK_NULL_HANDLE) {
        vkDestroyImage(getVkDevice(), depth_image.getVkImage(), nullptr);
        depth_image.setVkImage(VK_NULL_HANDLE);
    }
    if (depth_image.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), depth_image.getVkDeviceMemory(), nullptr);
        depth_image.setVkDeviceMemory(VK_NULL_HANDLE);
    }
    return true;
}

bool Tutorial10::createUniformBuffer() {
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial10_parameters.getUniformBufferParameters();
    uniform_buffer.setSize(sizeof(Tutorial10UniformBufferData));
    // Host-visible/coherent (not device-local + staging) since this buffer
    // is rewritten every frame as the orbit camera moves; see
    // updateUniformBufferData()/draw() for how the CPU/GPU race that
    // creates is avoided (same approach as Tutorial08/09).
    if (!createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      uniform_buffer)) {
        Logging::error(LOG_TAG, "Could not create uniform buffer!");
        return false;
    }

    return updateUniformBufferData();
}

Tutorial10UniformBufferData Tutorial10::getUniformBufferData() const {
    Tutorial10UniformBufferData data{};
    data.view = glm::lookAt(m_camera.eye(),
                            m_camera.target(),
                            Math::Vec3<float>(0.0f, 1.0f, 0.0f));

    float const width =
            static_cast<float>(getSwapchainParameters().getVkExtent2d().width);
    float const height = static_cast<float>(
            getSwapchainParameters().getVkExtent2d().height);
    data.projection = Tools::getPerspectiveProjectionMatrix(
            width / height, 45.0f, 0.1f, 100.0f);

    return data;
}

bool Tutorial10::updateUniformBufferData() {
    Tutorial10UniformBufferData const uniform_data = getUniformBufferData();
    BufferParameters& uniform_buffer =
            m_vulkan_tutorial10_parameters.getUniformBufferParameters();

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

bool Tutorial10::createDescriptorSetLayout() {
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings = {
            {.binding = 0,
             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
             .pImmutableSamplers = nullptr}};

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount = static_cast<std::uint32_t>(layout_bindings.size()),
            .pBindings = layout_bindings.data()};

    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial10_parameters.getDescriptorSetParameters();

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

bool Tutorial10::createDescriptorPool() {
    std::vector<VkDescriptorPoolSize> pool_sizes = {
            {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1}};

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .maxSets = 1,
            .poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size()),
            .pPoolSizes = pool_sizes.data()};

    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial10_parameters.getDescriptorSetParameters();

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

bool Tutorial10::allocateDescriptorSet() {
    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial10_parameters.getDescriptorSetParameters();

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

bool Tutorial10::updateDescriptorSet() {
    const BufferParameters& uniform_buffer =
            m_vulkan_tutorial10_parameters.getUniformBufferParameters();
    const DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial10_parameters.getDescriptorSetParameters();

    VkDescriptorBufferInfo buffer_info = {
            .buffer = uniform_buffer.getVkBuffer(),
            .offset = 0,
            .range = uniform_buffer.getSize()};

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
             .pTexelBufferView = nullptr}};

    vkUpdateDescriptorSets(
            getVkDevice(),
            static_cast<std::uint32_t>(descriptor_writes.size()),
            descriptor_writes.data(),
            0,
            nullptr);
    return true;
}

bool Tutorial10::createRenderPass() {
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
                &m_vulkan_tutorial10_parameters.getVkRenderPass()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create render pass!");
        return false;
    }

    return true;
}

bool Tutorial10::createPipelineLayout() {
    VkDescriptorSetLayout vk_descriptor_set_layout =
            m_vulkan_tutorial10_parameters.getDescriptorSetParameters()
                    .getVkDescriptorSetLayout();

    VkPushConstantRange push_constant_range = {
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .offset = 0,
            .size = sizeof(LinePushConstants)};

    VkPipelineLayoutCreateInfo layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &vk_descriptor_set_layout,
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &push_constant_range};

    if (vkCreatePipelineLayout(
                getVkDevice(),
                &layout_create_info,
                nullptr,
                &m_vulkan_tutorial10_parameters.getVkPipelineLayout()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create pipeline layout!");
        return false;
    }

    return true;
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
Tutorial10::createShaderModule(const char* filename) {
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

bool Tutorial10::createLinePipeline() {
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            vertex_shader_module =
                    createShaderModule("shader.10_line.vert.spv");
    Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
            fragment_shader_module =
                    createShaderModule("shader.10_line.frag.spv");

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
              .stride = LineVertexAttributeTraits::stride,
              .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};

    std::vector<VkVertexInputAttributeDescription>
            vertex_attribute_descriptions = {
                    {.location = 0,
                     .binding = vertex_binding_descriptions[0].binding,
                     .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                     .offset = offsetof(struct LineVertexData, position)}};

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
            .topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
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
            // Per spec, cull mode only affects polygon (triangle)
            // rasterization and is simply ignored for line topology - set
            // to BACK anyway for consistency with the rest of the project's
            // pipelines rather than leaving it in an unspecified state.
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            // Ignored per spec: VK_DYNAMIC_STATE_LINE_WIDTH is set below,
            // via vkCmdSetLineWidth() in prepareFrame() - the curve and
            // control polygon draw with different widths from one pipeline.
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
                                                  VK_DYNAMIC_STATE_SCISSOR,
                                                  VK_DYNAMIC_STATE_LINE_WIDTH};

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
            .layout = m_vulkan_tutorial10_parameters.getVkPipelineLayout(),
            .renderPass = m_vulkan_tutorial10_parameters.getVkRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1};

    if (vkCreateGraphicsPipelines(
                getVkDevice(),
                VK_NULL_HANDLE,
                1,
                &pipeline_create_info,
                nullptr,
                &m_vulkan_tutorial10_parameters.getVkLinePipeline()) !=
        VK_SUCCESS) {
        Logging::error(LOG_TAG, "Could not create line graphics pipeline!");
        return false;
    }
    return true;
}

const std::vector<LineVertexData>& Tutorial10::getCurveVertexData() const {
    static const std::vector<LineVertexData> curve_vertices = [] {
        std::vector<Math::CurveSample3D<float>> const& samples =
                getCurveSamples();
        std::vector<LineVertexData> data;
        data.reserve(samples.size());
        for (Math::CurveSample3D<float> const& sample : samples) {
            data.push_back({Math::Vec4<float>(sample.position, 1.0f)});
        }
        return data;
    }();
    return curve_vertices;
}

const std::vector<LineVertexData>& Tutorial10::getControlPolygonVertexData()
        const {
    static const std::vector<LineVertexData> control_polygon_vertices = [] {
        std::vector<Math::Vec3<float>> const& control_points =
                getControlPoints();
        std::vector<LineVertexData> data;
        data.reserve(control_points.size());
        for (Math::Vec3<float> const& point : control_points) {
            data.push_back({Math::Vec4<float>(point, 1.0f)});
        }
        return data;
    }();
    return control_polygon_vertices;
}

float Tutorial10::getLineWidth() const {
    constexpr float c_desired_line_width = 4.0f;

    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(getVkPhysicalDevice(), &device_features);
    if (device_features.wideLines == VK_FALSE) {
        // Per spec, lineWidth must be exactly 1.0 when wideLines isn't
        // enabled - TutorialBase::createDevice() only enables it when the
        // physical device reports support.
        return 1.0f;
    }

    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(getVkPhysicalDevice(), &device_properties);
    return std::min(c_desired_line_width,
                    device_properties.limits.lineWidthRange[1]);
}

bool Tutorial10::copyBufferData(BufferParameters& destination,
                                const void* data,
                                std::uint32_t data_size,
                                VkAccessFlags dst_access_mask,
                                VkPipelineStageFlags dst_stage_mask) {
    BufferParameters& staging_buffer =
            m_vulkan_tutorial10_parameters.getStagingBufferParameters();

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

    std::memcpy(staging_buffer_memory_pointer, data, data_size);

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
            m_vulkan_tutorial10_parameters.getRenderingResources()[0]
                    .getVkCommandBuffer();

    vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

    VkBufferCopy buffer_copy_info = {
            .srcOffset = 0, .dstOffset = 0, .size = data_size};
    vkCmdCopyBuffer(command_buffer,
                    staging_buffer.getVkBuffer(),
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

bool Tutorial10::createCurveVertexBuffer() {
    const std::vector<LineVertexData>& vertex_data = getCurveVertexData();
    m_vulkan_tutorial10_parameters.setCurveVertexCount(
            static_cast<std::uint32_t>(vertex_data.size()));

    BufferParameters& vertex_buffer =
            m_vulkan_tutorial10_parameters.getCurveVertexBufferParameters();
    vertex_buffer.setSize(static_cast<std::uint32_t>(vertex_data.size() *
                                                     sizeof(vertex_data[0])));
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      vertex_buffer)) {
        Logging::error(LOG_TAG, "Could not create curve vertex buffer!");
        return false;
    }

    return copyBufferData(vertex_buffer,
                          vertex_data.data(),
                          vertex_buffer.getSize(),
                          VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                          VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
}

bool Tutorial10::createControlPolygonVertexBuffer() {
    const std::vector<LineVertexData>& vertex_data =
            getControlPolygonVertexData();
    m_vulkan_tutorial10_parameters.setControlPolygonVertexCount(
            static_cast<std::uint32_t>(vertex_data.size()));

    BufferParameters& vertex_buffer =
            m_vulkan_tutorial10_parameters
                    .getControlPolygonVertexBufferParameters();
    vertex_buffer.setSize(static_cast<std::uint32_t>(vertex_data.size() *
                                                     sizeof(vertex_data[0])));
    if (!createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      vertex_buffer)) {
        Logging::error(LOG_TAG,
                       "Could not create control polygon vertex buffer!");
        return false;
    }

    return copyBufferData(vertex_buffer,
                          vertex_data.data(),
                          vertex_buffer.getSize(),
                          VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                          VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
}

bool Tutorial10::createFramebuffer(VkFramebuffer& framebuffer,
                                   VkImageView image_view) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(getVkDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    VkImageView attachments[] = {
            image_view,
            m_vulkan_tutorial10_parameters.getDepthImageParameters()
                    .getVkImageView()};

    VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_vulkan_tutorial10_parameters.getVkRenderPass(),
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

bool Tutorial10::prepareFrame(VkCommandBuffer command_buffer,
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
            {.color = {.float32 = {0.06f, 0.06f, 0.09f, 0.0f}}},
            {.depthStencil = {.depth = 1.0f, .stencil = 0}}};

    VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = m_vulkan_tutorial10_parameters.getVkRenderPass(),
            .framebuffer = framebuffer,
            .renderArea = {.offset = {.x = 0, .y = 0},
                           .extent = getSwapchainParameters().getVkExtent2d()},
            .clearValueCount = 2,
            .pClearValues = clear_values};

    vkCmdBeginRenderPass(command_buffer,
                         &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

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

    VkDescriptorSet vk_descriptor_set =
            m_vulkan_tutorial10_parameters.getDescriptorSetParameters()
                    .getVkDescriptorSet();
    vkCmdBindDescriptorSets(
            command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vulkan_tutorial10_parameters.getVkPipelineLayout(),
            0,
            1,
            &vk_descriptor_set,
            0,
            nullptr);

    vkCmdBindPipeline(command_buffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_vulkan_tutorial10_parameters.getVkLinePipeline());

    VkDeviceSize offset = 0;

    // The curve itself: a real polyline through the adaptively-sampled
    // points, in a vibrant color.
    LinePushConstants const curve_push_constants = {
            Math::Vec4<float>(0.1f, 0.85f, 0.8f, 1.0f)};
    vkCmdPushConstants(command_buffer,
                       m_vulkan_tutorial10_parameters.getVkPipelineLayout(),
                       VK_SHADER_STAGE_FRAGMENT_BIT,
                       0,
                       sizeof(LinePushConstants),
                       &curve_push_constants);
    vkCmdBindVertexBuffers(
            command_buffer,
            0,
            1,
            &m_vulkan_tutorial10_parameters.getCurveVertexBufferParameters()
                     .getVkBuffer(),
            &offset);
    vkCmdSetLineWidth(command_buffer, getLineWidth());
    vkCmdDraw(command_buffer,
              m_vulkan_tutorial10_parameters.getCurveVertexCount(),
              1,
              0,
              0);

    // The control polygon: a muted gray reference line connecting the raw
    // control points in order.
    LinePushConstants const control_polygon_push_constants = {
            Math::Vec4<float>(0.55f, 0.58f, 0.62f, 1.0f)};
    vkCmdPushConstants(command_buffer,
                       m_vulkan_tutorial10_parameters.getVkPipelineLayout(),
                       VK_SHADER_STAGE_FRAGMENT_BIT,
                       0,
                       sizeof(LinePushConstants),
                       &control_polygon_push_constants);
    vkCmdBindVertexBuffers(command_buffer,
                           0,
                           1,
                           &m_vulkan_tutorial10_parameters
                                    .getControlPolygonVertexBufferParameters()
                                    .getVkBuffer(),
                           &offset);
    // Always exactly 1.0 - the thin end of what a line can be regardless
    // of device wideLines support - so it reads as background scaffolding
    // next to the thicker curve.
    vkCmdSetLineWidth(command_buffer, 1.0f);
    vkCmdDraw(command_buffer,
              m_vulkan_tutorial10_parameters.getControlPolygonVertexCount(),
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

bool Tutorial10::draw() {
    static std::size_t resource_index = 0;
    RenderingResourceParameters& current_rendering_resource =
            m_vulkan_tutorial10_parameters
                    .getRenderingResources()[resource_index];
    VkSwapchainKHR swap_chain = getSwapchainParameters().getVkSwapchainKhr();
    std::uint32_t image_index;

    resource_index =
            (resource_index + 1) % VulkanTutorial10Parameters::resources_count;

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
            m_vulkan_tutorial10_parameters
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

void Tutorial10::destroyBuffer(BufferParameters& buffer) {
    if (buffer.getVkBuffer() != VK_NULL_HANDLE) {
        vkDestroyBuffer(getVkDevice(), buffer.getVkBuffer(), nullptr);
        buffer.setVkBuffer(VK_NULL_HANDLE);
    }
    if (buffer.getVkDeviceMemory() != VK_NULL_HANDLE) {
        vkFreeMemory(getVkDevice(), buffer.getVkDeviceMemory(), nullptr);
        buffer.setVkDeviceMemory(VK_NULL_HANDLE);
    }
}

bool Tutorial10::childOnWindowSizeChanged() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return true;
    }
    vkDeviceWaitIdle(getVkDevice());

    // TutorialBase::onWindowSizeChanged() calls childClear() unconditionally
    // before this runs, which tears down every Vulkan object this tutorial
    // owns - not just the swapchain. Everything has to be rebuilt here, in
    // the same order tutorial10_main.cpp's initial setup uses. See
    // Tutorial04-09's identical fix for the full rationale.
    if (!createRenderingResources()) {
        return false;
    }
    if (!createStagingBuffer()) {
        return false;
    }
    if (!createDepthResources()) {
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
    if (!createLinePipeline()) {
        return false;
    }
    if (!createCurveVertexBuffer()) {
        return false;
    }
    return createControlPolygonVertexBuffer();
}

void Tutorial10::childClear() {
    if (getVkDevice() == VK_NULL_HANDLE) {
        return;
    }
    vkDeviceWaitIdle(getVkDevice());

    std::vector<RenderingResourceParameters>& rendering_resources =
            m_vulkan_tutorial10_parameters.getRenderingResources();
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
                    m_vulkan_tutorial10_parameters.getVkCommandPool(),
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
            m_vulkan_tutorial10_parameters.getFinishedRenderingSemaphores();
    for (std::size_t i = 0; i < finished_rendering_semaphores.size(); ++i) {
        if (finished_rendering_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(
                    getVkDevice(), finished_rendering_semaphores[i], nullptr);
        }
    }
    finished_rendering_semaphores.clear();

    if (m_vulkan_tutorial10_parameters.getVkCommandPool() != VK_NULL_HANDLE) {
        vkDestroyCommandPool(getVkDevice(),
                             m_vulkan_tutorial10_parameters.getVkCommandPool(),
                             nullptr);
        m_vulkan_tutorial10_parameters.getVkCommandPool() = VK_NULL_HANDLE;
    }

    destroyBuffer(
            m_vulkan_tutorial10_parameters.getCurveVertexBufferParameters());
    destroyBuffer(m_vulkan_tutorial10_parameters
                          .getControlPolygonVertexBufferParameters());
    destroyBuffer(m_vulkan_tutorial10_parameters.getStagingBufferParameters());

    if (m_vulkan_tutorial10_parameters.getVkLinePipeline() != VK_NULL_HANDLE) {
        vkDestroyPipeline(getVkDevice(),
                          m_vulkan_tutorial10_parameters.getVkLinePipeline(),
                          nullptr);
        m_vulkan_tutorial10_parameters.getVkLinePipeline() = VK_NULL_HANDLE;
    }

    if (m_vulkan_tutorial10_parameters.getVkPipelineLayout() !=
        VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
                getVkDevice(),
                m_vulkan_tutorial10_parameters.getVkPipelineLayout(),
                nullptr);
        m_vulkan_tutorial10_parameters.getVkPipelineLayout() = VK_NULL_HANDLE;
    }

    if (m_vulkan_tutorial10_parameters.getVkRenderPass() != VK_NULL_HANDLE) {
        vkDestroyRenderPass(getVkDevice(),
                            m_vulkan_tutorial10_parameters.getVkRenderPass(),
                            nullptr);
        m_vulkan_tutorial10_parameters.getVkRenderPass() = VK_NULL_HANDLE;
    }

    DescriptorSetParameters& descriptor_set =
            m_vulkan_tutorial10_parameters.getDescriptorSetParameters();
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

    destroyBuffer(m_vulkan_tutorial10_parameters.getUniformBufferParameters());

    destroyDepthResources();
}

}  // namespace vulkan_graphix
