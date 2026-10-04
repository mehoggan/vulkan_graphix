#include "vulkan_graphix/Render/Renderer.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <utility>

#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/VulkanCommon.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_graphix::Render {

namespace vg = vulkan_graphix;
namespace vc = vulkan_graphix::VulkanCommon;

namespace {
using Vec2 = Math::Vec2<float>;
using Vec3 = Math::Vec3<float>;
using Vec4 = Math::Vec4<float>;
using Mat4 = Math::Mat4<float>;

constexpr VkFormat c_depth_format = VK_FORMAT_D32_SFLOAT;
constexpr VkDeviceSize c_transient_size = 16 * 1024 * 1024;
constexpr std::uint32_t c_max_textures = 4096;
}  // namespace

Rect Rect::fromBottomLeft(std::int32_t x,
                          std::int32_t y,
                          std::int32_t width,
                          std::int32_t height,
                          std::int32_t framebuffer_height) {
    return {x, framebuffer_height - (y + height), width, height};
}

// ************************************************************ //
// RenderContext                                                //
// ************************************************************ //
RenderContext::RenderContext(Renderer& renderer) : m_renderer(renderer) {}

void RenderContext::begin(VkCommandBuffer command_buffer, VkExtent2D extent) {
    m_command_buffer = command_buffer;
    m_extent = extent;
    m_bound_pipeline = VK_NULL_HANDLE;
    m_depth_test = true;
    m_bound_descriptor_set = VK_NULL_HANDLE;
    m_projection = Mat4(1.0f);
    m_view = Mat4(1.0f);
    setViewport({0,
                 0,
                 static_cast<std::int32_t>(extent.width),
                 static_cast<std::int32_t>(extent.height)});
}

VkCommandBuffer RenderContext::commandBuffer() const {
    return m_command_buffer;
}

VkExtent2D RenderContext::extent() const { return m_extent; }

VkRect2D RenderContext::toVkRect(const Rect& rect) const {
    // A Vulkan scissor may not extend past the framebuffer.
    const std::int32_t width = static_cast<std::int32_t>(m_extent.width);
    const std::int32_t height = static_cast<std::int32_t>(m_extent.height);
    const std::int32_t left = std::clamp(rect.x, 0, width);
    const std::int32_t right = std::clamp(rect.x + rect.width, 0, width);
    const std::int32_t top_edge = std::clamp(rect.y, 0, height);
    const std::int32_t bottom = std::clamp(rect.y + rect.height, 0, height);
    return VkRect2D{{left, top_edge},
                    {static_cast<std::uint32_t>(right - left),
                     static_cast<std::uint32_t>(bottom - top_edge)}};
}

void RenderContext::applyViewport(const Rect& rect) {
    const VkViewport viewport = {static_cast<float>(rect.x),
                                 static_cast<float>(rect.y),
                                 static_cast<float>(rect.width),
                                 static_cast<float>(rect.height),
                                 0.0f,
                                 1.0f};
    vg::vkCmdSetViewport(m_command_buffer, 0, 1, &viewport);
}

void RenderContext::setViewport(const Rect& rect) {
    m_viewport = rect;
    applyViewport(rect);
    setScissor(rect);
}

void RenderContext::setScissor(const Rect& rect) {
    m_scissor = rect;
    const VkRect2D scissor = toVkRect(rect);
    vg::vkCmdSetScissor(m_command_buffer, 0, 1, &scissor);
}

const Rect& RenderContext::viewport() const { return m_viewport; }

void RenderContext::setCamera(const Mat4& projection, const Mat4& view) {
    m_projection = projection;
    m_view = view;
}

const Mat4& RenderContext::projection() const { return m_projection; }

const Mat4& RenderContext::view() const { return m_view; }

void RenderContext::clearDepth() {
    const VkClearAttachment attachment = {
            VK_IMAGE_ASPECT_DEPTH_BIT, 1, {.depthStencil = {1.0f, 0}}};
    const VkClearRect rect = {toVkRect(m_scissor), 0, 1};
    if (rect.rect.extent.width == 0 || rect.rect.extent.height == 0) {
        return;
    }
    vg::vkCmdClearAttachments(m_command_buffer, 1, &attachment, 1, &rect);
}

void RenderContext::clearColorAndDepth(const Vec4& color) {
    std::array<VkClearAttachment, 2> attachments{};
    attachments[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    attachments[0].colorAttachment = 0;
    attachments[0].clearValue.color = {{color.r, color.g, color.b, color.a}};
    attachments[1].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    attachments[1].clearValue.depthStencil = {1.0f, 0};
    const VkClearRect rect = {toVkRect(m_scissor), 0, 1};
    if (rect.rect.extent.width == 0 || rect.rect.extent.height == 0) {
        return;
    }
    vg::vkCmdClearAttachments(m_command_buffer,
                              static_cast<std::uint32_t>(attachments.size()),
                              attachments.data(),
                              1,
                              &rect);
}

void RenderContext::setDepthTest(bool enabled) { m_depth_test = enabled; }

bool RenderContext::depthTest() const { return m_depth_test; }

void RenderContext::bindPipeline(PipelineHandle pipeline) {
    const VkPipeline handle = m_renderer.pipeline(pipeline, m_depth_test);
    if (m_bound_pipeline == handle) {
        return;
    }
    vg::vkCmdBindPipeline(
            m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, handle);
    m_bound_pipeline = handle;
}

void RenderContext::bindTexture(const Texture* texture) {
    const VkDescriptorSet descriptor_set =
            (texture != nullptr ? texture : &m_renderer.whiteTexture())
                    ->descriptorSet();
    if (descriptor_set == m_bound_descriptor_set) {
        return;
    }
    vg::vkCmdBindDescriptorSets(m_command_buffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_renderer.pipelineLayout(),
                                0,
                                1,
                                &descriptor_set,
                                0,
                                nullptr);
    m_bound_descriptor_set = descriptor_set;
}

void RenderContext::pushConstants(const Mat4& model, const Vec4& params) {
    const DrawConstants constants = {m_projection * m_view * model, params};
    vg::vkCmdPushConstants(
            m_command_buffer,
            m_renderer.pipelineLayout(),
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(DrawConstants),
            &constants);
}

void RenderContext::draw(UiMesh& mesh, const Mat4& model) {
    if (!m_renderer.m_ui_triangle_pipeline || !m_renderer.m_ui_line_pipeline) {
        return;
    }
    draw(mesh,
         *m_renderer.m_ui_triangle_pipeline,
         *m_renderer.m_ui_line_pipeline,
         model);
}

void RenderContext::drawRetained(const HostBuffer& buffer,
                                 const RetainedMeshBase& mesh,
                                 std::uint32_t triangle_count,
                                 std::uint32_t line_count,
                                 PipelineHandle triangle_pipeline,
                                 PipelineHandle line_pipeline,
                                 const Mat4& model) {
    const VkBuffer vk_buffer = buffer.buffer.getVkBuffer();
    if (vk_buffer == VK_NULL_HANDLE) {
        return;
    }
    const VkDeviceSize offset = 0;
    if (triangle_count != 0) {
        bindPipeline(triangle_pipeline);
        bindTexture(mesh.texture());
        pushConstants(model, mesh.params());
        vg::vkCmdBindVertexBuffers(
                m_command_buffer, 0, 1, &vk_buffer, &offset);
        vg::vkCmdDraw(m_command_buffer, triangle_count, 1, 0, 0);
    }
    if (line_count != 0) {
        bindPipeline(line_pipeline);
        bindTexture(mesh.texture());
        pushConstants(model, mesh.params());
        vg::vkCmdSetLineWidth(m_command_buffer,
                              m_renderer.clampLineWidth(mesh.lineWidth()));
        vg::vkCmdBindVertexBuffers(
                m_command_buffer, 0, 1, &vk_buffer, &offset);
        vg::vkCmdDraw(m_command_buffer, line_count, 1, triangle_count, 0);
    }
}

void RenderContext::drawTransientBytes(const void* data,
                                       std::size_t byte_count,
                                       std::uint32_t vertex_count,
                                       PipelineHandle pipeline,
                                       const Texture* texture,
                                       const Mat4& model,
                                       const Vec4& params,
                                       float line_width) {
    if (vertex_count == 0) {
        return;
    }
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceSize offset = 0;
    if (!m_renderer.allocateTransient(data, byte_count, &buffer, &offset)) {
        return;
    }
    bindPipeline(pipeline);
    bindTexture(texture);
    pushConstants(model, params);
    // Line width is dynamic state in every pipeline; triangles ignore it.
    vg::vkCmdSetLineWidth(m_command_buffer,
                          m_renderer.clampLineWidth(line_width));
    vg::vkCmdBindVertexBuffers(m_command_buffer, 0, 1, &buffer, &offset);
    vg::vkCmdDraw(m_command_buffer, vertex_count, 1, 0, 0);
}

void RenderContext::drawMesh(const Mesh& mesh,
                             PipelineHandle pipeline,
                             const Texture* texture,
                             const Mat4& model,
                             const Vec4& params) {
    if (mesh.vertexCount() == 0) {
        return;
    }
    bindPipeline(pipeline);
    bindTexture(texture);
    pushConstants(model, params);
    const VkBuffer buffer = mesh.buffer();
    const VkDeviceSize offset = 0;
    vg::vkCmdBindVertexBuffers(m_command_buffer, 0, 1, &buffer, &offset);
    vg::vkCmdDraw(m_command_buffer, mesh.vertexCount(), 1, 0, 0);
}

void RenderContext::drawText(const Font& font,
                             const Vec3& raster_position,
                             std::string_view text,
                             const Vec4& color,
                             const Mat4& model) {
    if (!m_renderer.m_text_pipeline || text.empty()) {
        return;
    }
    const Vec4 clip =
            m_projection * m_view * model * Vec4(raster_position, 1.0f);
    // A position outside the view volume draws nothing.
    if (clip.w <= 0.0f || std::abs(clip.x) > clip.w ||
        std::abs(clip.y) > clip.w || clip.z < 0.0f || clip.z > clip.w) {
        return;
    }
    const Vec3 device_coords = Vec3(clip) / clip.w;
    const Vec2 origin(static_cast<float>(m_viewport.x) +
                              (device_coords.x + 1.0f) * 0.5f *
                                      static_cast<float>(m_viewport.width),
                      static_cast<float>(m_viewport.y) +
                              (device_coords.y + 1.0f) * 0.5f *
                                      static_cast<float>(m_viewport.height));
    // Glyph quads in framebuffer pixels from the projected baseline origin,
    // converted straight to normalized device coordinates, so they are
    // drawn across the whole framebuffer (still clipped by the scissor).
    const float width = static_cast<float>(m_extent.width);
    const float height = static_cast<float>(m_extent.height);
    auto to_ndc = [&](const Vec2& pixel) {
        return Vec3(pixel.x / width * 2.0f - 1.0f,
                    pixel.y / height * 2.0f - 1.0f,
                    device_coords.z);
    };
    std::vector<UiVertex> vertices;
    for (const BitmapFontGlyphQuad& glyph :
         font.bitmap().layoutText(std::string(text), origin)) {
        const std::array<UiVertex, 4> corners = {
                UiVertex{to_ndc(glyph.top_left), color, glyph.uv_top_left},
                UiVertex{to_ndc(Vec2(glyph.top_left.x, glyph.bottom_right.y)),
                         color,
                         Vec2(glyph.uv_top_left.x, glyph.uv_bottom_right.y)},
                UiVertex{to_ndc(glyph.bottom_right),
                         color,
                         glyph.uv_bottom_right},
                UiVertex{to_ndc(Vec2(glyph.bottom_right.x, glyph.top_left.y)),
                         color,
                         Vec2(glyph.uv_bottom_right.x, glyph.uv_top_left.y)}};
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U}) {
            vertices.push_back(corners[corner]);
        }
    }
    if (vertices.empty()) {
        return;
    }
    const Mat4 saved_projection = m_projection;
    const Mat4 saved_view = m_view;
    m_projection = Mat4(1.0f);
    m_view = Mat4(1.0f);
    applyViewport({0,
                   0,
                   static_cast<std::int32_t>(m_extent.width),
                   static_cast<std::int32_t>(m_extent.height)});
    drawTransient(vertices, *m_renderer.m_text_pipeline, &font.atlas());
    applyViewport(m_viewport);
    m_projection = saved_projection;
    m_view = saved_view;
}

// ************************************************************ //
// Renderer                                                     //
// ************************************************************ //
Renderer* Renderer::s_instance = nullptr;

Renderer& Renderer::instance() { return *s_instance; }

bool Renderer::hasInstance() { return s_instance != nullptr; }

Renderer::Renderer() : m_context(*this) {}

Renderer::~Renderer() { shutdown(); }

Renderer::SwapchainInfo Renderer::swapchainInfo(const TutorialBase& base) {
    const SwapChainParameters& swapchain = base.getSwapchainParameters();
    SwapchainInfo info;
    info.swapchain = swapchain.getVkSwapchainKhr();
    info.format = swapchain.getVkFormat();
    info.extent = swapchain.getVkExtent2d();
    for (const ImageParameters& image : swapchain.getImageParameters()) {
        info.images.push_back(image.getVkImage());
        info.views.push_back(image.getVkImageView());
    }
    return info;
}

bool Renderer::initialize(const TutorialBase& base) {
    s_instance = this;
    m_device.device = base.getVkDevice();
    m_device.physical_device = base.getVkPhysicalDevice();
    m_device.graphics_queue = base.getGraphicsQueueParameters().getVkQueue();
    m_device.graphics_family =
            base.getGraphicsQueueParameters().getFamilyIndex();
    m_device.present_queue = base.getPresentQueueParameters().getVkQueue();
    m_swapchain = swapchainInfo(base);

    VkPhysicalDeviceFeatures features;
    vg::vkGetPhysicalDeviceFeatures(m_device.physical_device, &features);
    m_wireframe_supported = features.fillModeNonSolid == VK_TRUE;
    // Line widths are clamped to the device's range (as OpenGL's
    // glLineWidth() does); without wideLines (TutorialBase enables it when
    // available), only 1.0 is valid.
    if (features.wideLines == VK_TRUE) {
        VkPhysicalDeviceProperties properties;
        vg::vkGetPhysicalDeviceProperties(m_device.physical_device,
                                          &properties);
        m_line_width_range = {properties.limits.lineWidthRange[0],
                              properties.limits.lineWidthRange[1]};
    }

    if (!createRenderPass() || !createSwapchainResources() ||
        !createDescriptorResources() || !createFrameSlots()) {
        return false;
    }
    const std::vector<char> white(4, static_cast<char>(0xFF));
    m_white_texture =
            createTexture(white, 1, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    return m_white_texture != nullptr;
}

bool Renderer::createRenderPass() {
    std::array<VkAttachmentDescription, 2> attachments = {};
    attachments[0].format = m_swapchain.format;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[1].format = c_depth_format;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout =
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    const VkAttachmentReference color_reference = {
            0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    const VkAttachmentReference depth_reference = {
            1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_reference;
    subpass.pDepthStencilAttachment = &depth_reference;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                               VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.attachmentCount =
            static_cast<std::uint32_t>(attachments.size());
    create_info.pAttachments = attachments.data();
    create_info.subpassCount = 1;
    create_info.pSubpasses = &subpass;
    create_info.dependencyCount = 1;
    create_info.pDependencies = &dependency;
    return vg::vkCreateRenderPass(
                   m_device.device, &create_info, nullptr, &m_render_pass) ==
           VK_SUCCESS;
}

bool Renderer::createSwapchainResources() {
    const vc::ImageFactory factory(m_device.device, m_device.physical_device);
    VkImage depth_image = VK_NULL_HANDLE;
    if (!factory.createImage(m_swapchain.extent.width,
                             m_swapchain.extent.height,
                             c_depth_format,
                             VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                             &depth_image)) {
        return false;
    }
    m_depth_image.setVkImage(depth_image);
    VkDeviceMemory depth_memory = VK_NULL_HANDLE;
    if (!factory.allocateMemory(depth_image,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                &depth_memory) ||
        !factory.bindMemory(depth_image, depth_memory)) {
        return false;
    }
    m_depth_image.setVkDeviceMemory(depth_memory);
    VkImageView depth_view = VK_NULL_HANDLE;
    if (!factory.createImageView(depth_image,
                                 c_depth_format,
                                 VK_IMAGE_ASPECT_DEPTH_BIT,
                                 &depth_view)) {
        return false;
    }
    m_depth_image.setVkImageView(depth_view);

    m_framebuffers.assign(m_swapchain.views.size(), VK_NULL_HANDLE);
    m_render_finished.assign(m_swapchain.views.size(), VK_NULL_HANDLE);
    const vc::FrameResourceFactory frame_factory(m_device.device);
    for (std::size_t i = 0; i < m_swapchain.views.size(); ++i) {
        const std::array<VkImageView, 2> views = {m_swapchain.views[i],
                                                  depth_view};
        VkFramebufferCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = m_render_pass;
        create_info.attachmentCount = static_cast<std::uint32_t>(views.size());
        create_info.pAttachments = views.data();
        create_info.width = m_swapchain.extent.width;
        create_info.height = m_swapchain.extent.height;
        create_info.layers = 1;
        if (vg::vkCreateFramebuffer(m_device.device,
                                    &create_info,
                                    nullptr,
                                    &m_framebuffers[i]) != VK_SUCCESS ||
            !frame_factory.createSemaphore(&m_render_finished[i])) {
            return false;
        }
    }
    return true;
}

void Renderer::destroySwapchainResources() {
    for (VkFramebuffer framebuffer : m_framebuffers) {
        if (framebuffer != VK_NULL_HANDLE) {
            vg::vkDestroyFramebuffer(m_device.device, framebuffer, nullptr);
        }
    }
    m_framebuffers.clear();
    for (VkSemaphore semaphore : m_render_finished) {
        if (semaphore != VK_NULL_HANDLE) {
            vg::vkDestroySemaphore(m_device.device, semaphore, nullptr);
        }
    }
    m_render_finished.clear();
    vc::ImageFactory(m_device.device, m_device.physical_device)
            .destroy(m_depth_image);
    m_depth_image = ImageParameters();
}

void Renderer::releaseSwapchainResources() {
    vg::vkDeviceWaitIdle(m_device.device);
    destroySwapchainResources();
}

bool Renderer::onSwapchainRecreated(const TutorialBase& base) {
    vg::vkDeviceWaitIdle(m_device.device);
    destroySwapchainResources();
    m_swapchain = swapchainInfo(base);
    return createSwapchainResources();
}

bool Renderer::createDescriptorResources() {
    const VkDescriptorSetLayoutBinding binding = {
            0,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            1,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            nullptr};
    VkDescriptorSetLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = 1;
    layout_info.pBindings = &binding;
    if (vg::vkCreateDescriptorSetLayout(
                m_device.device, &layout_info, nullptr, &m_texture_layout) !=
        VK_SUCCESS) {
        return false;
    }

    const VkDescriptorPoolSize pool_size = {
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, c_max_textures};
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = c_max_textures;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    if (vg::vkCreateDescriptorPool(
                m_device.device, &pool_info, nullptr, &m_descriptor_pool) !=
        VK_SUCCESS) {
        return false;
    }

    const VkPushConstantRange push_range = {
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(DrawConstants)};
    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &m_texture_layout;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_range;
    return vg::vkCreatePipelineLayout(m_device.device,
                                      &pipeline_layout_info,
                                      nullptr,
                                      &m_pipeline_layout) == VK_SUCCESS;
}

std::optional<PipelineHandle> Renderer::createPipeline(
        const PipelineDescription& description) {
    std::array<VkPipeline, 2> variants = {VK_NULL_HANDLE, VK_NULL_HANDLE};
    if (!createPipelineVariant(description, true, &variants[0]) ||
        !createPipelineVariant(description, false, &variants[1])) {
        for (VkPipeline pipeline : variants) {
            if (pipeline != VK_NULL_HANDLE) {
                vg::vkDestroyPipeline(m_device.device, pipeline, nullptr);
            }
        }
        std::fprintf(stderr,
                     "vulkan_graphix: could not create pipeline %s/%s\n",
                     description.vertex_shader.c_str(),
                     description.fragment_shader.c_str());
        return std::nullopt;
    }
    m_pipelines.push_back(variants);
    return static_cast<PipelineHandle>(m_pipelines.size() - 1);
}

void Renderer::setUiPipelines(PipelineHandle triangles, PipelineHandle lines) {
    m_ui_triangle_pipeline = triangles;
    m_ui_line_pipeline = lines;
}

void Renderer::setTextPipeline(PipelineHandle text) { m_text_pipeline = text; }

bool Renderer::createPipelineVariant(const PipelineDescription& description,
                                     bool depth_test,
                                     VkPipeline* out) {
    auto vertex_module = vc::createShaderModule(
            m_device.device, description.vertex_shader.c_str());
    auto fragment_module = vc::createShaderModule(
            m_device.device, description.fragment_shader.c_str());
    if (!vertex_module || !fragment_module) {
        return false;
    }
    std::array<VkPipelineShaderStageCreateInfo, 2> stages = {};
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = vertex_module.get();
    stages[0].pName = "main";
    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = fragment_module.get();
    stages[1].pName = "main";

    const VertexLayout& layout = description.vertex_layout;
    const VkVertexInputBindingDescription binding = {
            0, layout.stride, VK_VERTEX_INPUT_RATE_VERTEX};
    VkPipelineVertexInputStateCreateInfo vertex_input = {};
    vertex_input.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input.vertexBindingDescriptionCount = 1;
    vertex_input.pVertexBindingDescriptions = &binding;
    vertex_input.vertexAttributeDescriptionCount =
            static_cast<std::uint32_t>(layout.attributes.size());
    vertex_input.pVertexAttributeDescriptions = layout.attributes.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
    input_assembly.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = description.topology;

    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    // No face culling.
    VkPipelineRasterizationStateCreateInfo rasterization = {};
    rasterization.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization.polygonMode =
            (description.polygon_mode == VK_POLYGON_MODE_FILL ||
             m_wireframe_supported)
                    ? description.polygon_mode
                    : VK_POLYGON_MODE_FILL;
    rasterization.cullMode = VK_CULL_MODE_NONE;
    rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample = {};
    multisample.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // LESS: OpenGL's default depth function.
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
    depth_stencil.sType =
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable =
            depth_test && description.depth_test ? VK_TRUE : VK_FALSE;
    depth_stencil.depthWriteEnable =
            depth_test && description.depth_write ? VK_TRUE : VK_FALSE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;

    // SRC_ALPHA / ONE_MINUS_SRC_ALPHA.
    VkPipelineColorBlendAttachmentState blend_attachment = {};
    blend_attachment.blendEnable = description.blend ? VK_TRUE : VK_FALSE;
    blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
    blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    VkPipelineColorBlendStateCreateInfo color_blend = {};
    color_blend.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend.attachmentCount = 1;
    color_blend.pAttachments = &blend_attachment;

    const std::array<VkDynamicState, 3> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_LINE_WIDTH};
    VkPipelineDynamicStateCreateInfo dynamic_state = {};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount =
            static_cast<std::uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    VkGraphicsPipelineCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.stageCount = static_cast<std::uint32_t>(stages.size());
    create_info.pStages = stages.data();
    create_info.pVertexInputState = &vertex_input;
    create_info.pInputAssemblyState = &input_assembly;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &rasterization;
    create_info.pMultisampleState = &multisample;
    create_info.pDepthStencilState = &depth_stencil;
    create_info.pColorBlendState = &color_blend;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = m_pipeline_layout;
    create_info.renderPass = m_render_pass;
    create_info.subpass = 0;
    return vg::vkCreateGraphicsPipelines(m_device.device,
                                         VK_NULL_HANDLE,
                                         1,
                                         &create_info,
                                         nullptr,
                                         out) == VK_SUCCESS;
}

bool Renderer::createFrameSlots() {
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = m_device.graphics_family;
    if (vg::vkCreateCommandPool(
                m_device.device, &pool_info, nullptr, &m_command_pool) !=
        VK_SUCCESS) {
        return false;
    }
    const vc::FrameResourceFactory factory(m_device.device);
    for (FrameSlot& slot : m_frames) {
        if (!factory.allocateCommandBuffers(
                    m_command_pool, 1, &slot.command_buffer) ||
            !factory.createSemaphore(&slot.image_available) ||
            !factory.createFence(true, &slot.in_flight)) {
            return false;
        }
        slot.transient = createHostBuffer(c_transient_size,
                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        if (slot.transient.mapped == nullptr) {
            return false;
        }
    }
    return true;
}

void Renderer::shutdown() {
    if (m_device.device == VK_NULL_HANDLE) {
        return;
    }
    vg::vkDeviceWaitIdle(m_device.device);
    // Everything the renderer itself still holds goes through the same
    // deferred-release path every texture/mesh uses...
    m_spheres.clear();
    m_white_texture.reset();
    m_texture_cache.clear();
    if (m_capture_buffer.buffer.getVkBuffer() != VK_NULL_HANDLE) {
        deferRelease(m_capture_buffer.buffer);
        m_capture_buffer = HostBuffer{};
    }
    for (FrameSlot& slot : m_frames) {
        if (slot.transient.buffer.getVkBuffer() != VK_NULL_HANDLE) {
            deferRelease(slot.transient.buffer);
            slot.transient = HostBuffer{};
        }
    }
    // ...and is then freed at once, since the device is idle.
    for (FrameSlot& slot : m_frames) {
        runReleases(slot);
        if (slot.in_flight != VK_NULL_HANDLE) {
            vg::vkDestroyFence(m_device.device, slot.in_flight, nullptr);
        }
        if (slot.image_available != VK_NULL_HANDLE) {
            vg::vkDestroySemaphore(
                    m_device.device, slot.image_available, nullptr);
        }
        slot = FrameSlot{};
    }
    for (auto& variants : m_pipelines) {
        for (VkPipeline pipeline : variants) {
            if (pipeline != VK_NULL_HANDLE) {
                vg::vkDestroyPipeline(m_device.device, pipeline, nullptr);
            }
        }
    }
    m_pipelines.clear();
    m_ui_triangle_pipeline.reset();
    m_ui_line_pipeline.reset();
    m_text_pipeline.reset();
    if (m_pipeline_layout != VK_NULL_HANDLE) {
        vg::vkDestroyPipelineLayout(
                m_device.device, m_pipeline_layout, nullptr);
    }
    if (m_descriptor_pool != VK_NULL_HANDLE) {
        vg::vkDestroyDescriptorPool(
                m_device.device, m_descriptor_pool, nullptr);
    }
    if (m_texture_layout != VK_NULL_HANDLE) {
        vg::vkDestroyDescriptorSetLayout(
                m_device.device, m_texture_layout, nullptr);
    }
    if (m_command_pool != VK_NULL_HANDLE) {
        vg::vkDestroyCommandPool(m_device.device, m_command_pool, nullptr);
    }
    destroySwapchainResources();
    if (m_render_pass != VK_NULL_HANDLE) {
        vg::vkDestroyRenderPass(m_device.device, m_render_pass, nullptr);
    }
    m_pipeline_layout = VK_NULL_HANDLE;
    m_descriptor_pool = VK_NULL_HANDLE;
    m_texture_layout = VK_NULL_HANDLE;
    m_command_pool = VK_NULL_HANDLE;
    m_render_pass = VK_NULL_HANDLE;
    m_device = DeviceInfo{};
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

void Renderer::runReleases(FrameSlot& slot) {
    std::vector<std::function<void()>> releases;
    releases.swap(slot.releases);
    for (auto& release : releases) {
        release();
    }
}

VkPipeline Renderer::pipeline(PipelineHandle handle, bool depth_test) const {
    return m_pipelines[handle][depth_test ? 0 : 1];
}

float Renderer::clampLineWidth(float width) const {
    return std::clamp(width, m_line_width_range[0], m_line_width_range[1]);
}

VkPipelineLayout Renderer::pipelineLayout() const { return m_pipeline_layout; }

VkExtent2D Renderer::extent() const { return m_swapchain.extent; }

std::uint64_t Renderer::frameNumber() const { return m_frame_number; }

RenderContext* Renderer::beginFrame(const Vec4& clear_color) {
    FrameSlot& slot = m_frames[m_frame_slot];
    vg::vkWaitForFences(
            m_device.device, 1, &slot.in_flight, VK_TRUE, UINT64_MAX);
    // Everything released while this slot's previous frame could still
    // have been reading it is now safe to free.
    runReleases(slot);
    slot.transient_offset = 0;

    const VkResult acquired = ::vkAcquireNextImageKHR(m_device.device,
                                                      m_swapchain.swapchain,
                                                      UINT64_MAX,
                                                      slot.image_available,
                                                      VK_NULL_HANDLE,
                                                      &m_image_index);
    if (acquired == VK_ERROR_OUT_OF_DATE_KHR) {
        return nullptr;
    }
    if (acquired != VK_SUCCESS && acquired != VK_SUBOPTIMAL_KHR) {
        return nullptr;
    }
    vg::vkResetFences(m_device.device, 1, &slot.in_flight);
    vg::vkResetCommandBuffer(slot.command_buffer, 0);

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vg::vkBeginCommandBuffer(slot.command_buffer, &begin_info);

    std::array<VkClearValue, 2> clear_values = {};
    clear_values[0].color = {
            {clear_color.r, clear_color.g, clear_color.b, clear_color.a}};
    clear_values[1].depthStencil = {1.0f, 0};
    VkRenderPassBeginInfo pass_info = {};
    pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass_info.renderPass = m_render_pass;
    pass_info.framebuffer = m_framebuffers[m_image_index];
    pass_info.renderArea = {{0, 0}, m_swapchain.extent};
    pass_info.clearValueCount =
            static_cast<std::uint32_t>(clear_values.size());
    pass_info.pClearValues = clear_values.data();
    vg::vkCmdBeginRenderPass(
            slot.command_buffer, &pass_info, VK_SUBPASS_CONTENTS_INLINE);

    m_context.begin(slot.command_buffer, m_swapchain.extent);
    return &m_context;
}

bool Renderer::endFrame() {
    FrameSlot& slot = m_frames[m_frame_slot];
    vg::vkCmdEndRenderPass(slot.command_buffer);

    const bool capturing = !m_capture_path.empty();
    if (capturing) {
        const VkDeviceSize size =
                static_cast<VkDeviceSize>(m_swapchain.extent.width) *
                m_swapchain.extent.height * 4;
        if (m_capture_buffer.buffer.getSize() < size) {
            if (m_capture_buffer.buffer.getVkBuffer() != VK_NULL_HANDLE) {
                deferRelease(m_capture_buffer.buffer);
            }
            m_capture_buffer =
                    createHostBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
        }
        const VkImage image = m_swapchain.images[m_image_index];
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        vg::vkCmdPipelineBarrier(slot.command_buffer,
                                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);
        VkBufferImageCopy region = {};
        region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
        region.imageExtent = {
                m_swapchain.extent.width, m_swapchain.extent.height, 1};
        vg::vkCmdCopyImageToBuffer(slot.command_buffer,
                                   image,
                                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                   m_capture_buffer.buffer.getVkBuffer(),
                                   1,
                                   &region);
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = 0;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        vg::vkCmdPipelineBarrier(slot.command_buffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);
    }
    vg::vkEndCommandBuffer(slot.command_buffer);

    const VkPipelineStageFlags wait_stage =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const VkSemaphore render_finished = m_render_finished[m_image_index];
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &slot.image_available;
    submit_info.pWaitDstStageMask = &wait_stage;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &slot.command_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &render_finished;
    if (vg::vkQueueSubmit(
                m_device.graphics_queue, 1, &submit_info, slot.in_flight) !=
        VK_SUCCESS) {
        return false;
    }
    if (capturing) {
        vg::vkWaitForFences(
                m_device.device, 1, &slot.in_flight, VK_TRUE, UINT64_MAX);
        writeCapture();
        m_capture_path.clear();
    }

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_finished;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &m_swapchain.swapchain;
    present_info.pImageIndices = &m_image_index;
    const VkResult presented =
            ::vkQueuePresentKHR(m_device.present_queue, &present_info);

    m_frame_slot = (m_frame_slot + 1) % c_frames_in_flight;
    ++m_frame_number;
    return presented == VK_SUCCESS;
}

void Renderer::requestCapture(std::string path) {
    m_capture_path = std::move(path);
}

bool Renderer::writeCapture() {
    std::FILE* file = std::fopen(m_capture_path.c_str(), "wb");
    if (file == nullptr) {
        return false;
    }
    const std::uint32_t width = m_swapchain.extent.width;
    const std::uint32_t height = m_swapchain.extent.height;
    std::fprintf(file, "P6\n%u %u\n255\n", width, height);
    const bool bgra = m_swapchain.format == VK_FORMAT_B8G8R8A8_UNORM ||
                      m_swapchain.format == VK_FORMAT_B8G8R8A8_SRGB;
    const auto* pixels =
            static_cast<const std::uint8_t*>(m_capture_buffer.mapped);
    std::vector<std::uint8_t> row_pixels(static_cast<std::size_t>(width) * 3);
    for (std::uint32_t y = 0; y < height; ++y) {
        for (std::uint32_t x = 0; x < width; ++x) {
            const std::uint8_t* pixel =
                    pixels + (static_cast<std::size_t>(y) * width + x) * 4;
            row_pixels[x * 3 + 0] = bgra ? pixel[2] : pixel[0];
            row_pixels[x * 3 + 1] = pixel[1];
            row_pixels[x * 3 + 2] = bgra ? pixel[0] : pixel[2];
        }
        std::fwrite(row_pixels.data(), 1, row_pixels.size(), file);
    }
    std::fclose(file);
    std::fprintf(stderr, "captured %s\n", m_capture_path.c_str());
    return true;
}

bool Renderer::allocateTransient(const void* data,
                                 VkDeviceSize size,
                                 VkBuffer* buffer,
                                 VkDeviceSize* offset) {
    FrameSlot& slot = m_frames[m_frame_slot];
    const VkDeviceSize aligned =
            (slot.transient_offset + 15) & ~VkDeviceSize{15};
    if (aligned + size > slot.transient.buffer.getSize()) {
        return false;
    }
    std::memcpy(
            static_cast<char*>(slot.transient.mapped) + aligned, data, size);
    slot.transient_offset = aligned + size;
    *buffer = slot.transient.buffer.getVkBuffer();
    *offset = aligned;
    return true;
}

HostBuffer Renderer::createHostBuffer(VkDeviceSize size,
                                      VkBufferUsageFlags usage) {
    HostBuffer host;
    host.buffer.setSize(static_cast<std::uint32_t>(size));
    const vc::BufferFactory factory(m_device.device, m_device.physical_device);
    if (!factory.create(usage,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        host.buffer)) {
        return HostBuffer{};
    }
    if (vg::vkMapMemory(m_device.device,
                        host.buffer.getVkDeviceMemory(),
                        0,
                        host.buffer.getSize(),
                        0,
                        &host.mapped) != VK_SUCCESS) {
        host.mapped = nullptr;
    }
    return host;
}

void Renderer::deferRelease(BufferParameters buffer) {
    if (buffer.getVkBuffer() == VK_NULL_HANDLE) {
        return;
    }
    const VkDevice device = m_device.device;
    const VkPhysicalDevice physical_device = m_device.physical_device;
    m_frames[m_frame_slot].releases.emplace_back([=]() mutable {
        // Unmapping is implicit in freeing the memory.
        vc::BufferFactory(device, physical_device).destroy(buffer);
    });
}

void Renderer::deferRelease(ImageParameters image,
                            VkDescriptorSet descriptor_set) {
    const VkDevice device = m_device.device;
    const VkPhysicalDevice physical_device = m_device.physical_device;
    const VkDescriptorPool pool = m_descriptor_pool;
    m_frames[m_frame_slot].releases.emplace_back([=]() mutable {
        if (descriptor_set != VK_NULL_HANDLE) {
            vg::vkFreeDescriptorSets(device, pool, 1, &descriptor_set);
        }
        vc::ImageFactory(device, physical_device).destroy(image);
    });
}

VkDescriptorSet Renderer::allocateTextureDescriptor(VkImageView view,
                                                    VkSampler sampler) {
    VkDescriptorSetAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.descriptorPool = m_descriptor_pool;
    allocate_info.descriptorSetCount = 1;
    allocate_info.pSetLayouts = &m_texture_layout;
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
    if (vg::vkAllocateDescriptorSets(m_device.device,
                                     &allocate_info,
                                     &descriptor_set) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }
    const VkDescriptorImageInfo image_info = {
            sampler, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptor_set;
    write.dstBinding = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = &image_info;
    vg::vkUpdateDescriptorSets(m_device.device, 1, &write, 0, nullptr);
    return descriptor_set;
}

std::shared_ptr<Texture> Renderer::createTexture(
        const std::vector<char>& pixels,
        std::uint32_t width,
        std::uint32_t height,
        VkSamplerAddressMode address_mode) {
    if (pixels.size() < static_cast<std::size_t>(width) * height * 4) {
        return nullptr;
    }
    const vc::BufferFactory buffer_factory(m_device.device,
                                           m_device.physical_device);
    BufferParameters staging;
    staging.setSize(static_cast<std::uint32_t>(pixels.size()));
    if (!buffer_factory.create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                               staging)) {
        return nullptr;
    }
    VkCommandBuffer upload_commands = VK_NULL_HANDLE;
    vc::FrameResourceFactory(m_device.device)
            .allocateCommandBuffers(m_command_pool, 1, &upload_commands);
    const vc::ImageFactory image_factory(m_device.device,
                                         m_device.physical_device);
    const vc::StagedUploader uploader(
            m_device.device, m_device.graphics_queue, upload_commands);
    ImageParameters image;
    const bool created =
            vc::createTextureFromPixels(image_factory,
                                        uploader,
                                        staging,
                                        width,
                                        height,
                                        pixels,
                                        address_mode,
                                        image,
                                        VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);
    vg::vkFreeCommandBuffers(
            m_device.device, m_command_pool, 1, &upload_commands);
    buffer_factory.destroy(staging);
    if (!created) {
        image_factory.destroy(image);
        return nullptr;
    }
    const VkDescriptorSet descriptor_set = allocateTextureDescriptor(
            image.getVkImageView(), image.getVkSampler());
    return std::make_shared<Texture>(image, descriptor_set, width, height);
}

std::shared_ptr<Texture> Renderer::loadRawTexture(
        const std::string& filename,
        std::uint32_t width,
        std::uint32_t height,
        VkSamplerAddressMode address_mode) {
    const std::string cache_key =
            filename + "#" + std::to_string(static_cast<int>(address_mode));
    if (auto cached = m_texture_cache[cache_key].lock()) {
        return cached;
    }
    const std::vector<char> pixels =
            vg::Tools::getRawImageData(filename, width, height);
    if (pixels.empty()) {
        std::fprintf(stderr,
                     "vulkan_graphix: could not read %s\n",
                     filename.c_str());
        return nullptr;
    }
    std::shared_ptr<Texture> texture =
            createTexture(pixels, width, height, address_mode);
    m_texture_cache[cache_key] = texture;
    return texture;
}

std::shared_ptr<Texture> Renderer::loadImageTexture(
        const std::string& filename, VkSamplerAddressMode address_mode) {
    const std::string cache_key =
            filename + "#" + std::to_string(static_cast<int>(address_mode));
    if (auto cached = m_texture_cache[cache_key].lock()) {
        return cached;
    }
    std::int32_t width = 0;
    std::int32_t height = 0;
    std::int32_t components = 0;
    std::int32_t data_size = 0;
    const std::vector<char> pixels = vg::Tools::getImageData(
            filename, 4, &width, &height, &components, &data_size);
    if (pixels.empty()) {
        std::fprintf(stderr,
                     "vulkan_graphix: could not read %s\n",
                     filename.c_str());
        return nullptr;
    }
    std::shared_ptr<Texture> texture =
            createTexture(pixels,
                          static_cast<std::uint32_t>(width),
                          static_cast<std::uint32_t>(height),
                          address_mode);
    m_texture_cache[cache_key] = texture;
    return texture;
}

const Texture& Renderer::whiteTexture() const { return *m_white_texture; }

std::unique_ptr<Font> Renderer::loadFont(const std::string& font_path,
                                         float pixel_height) {
    BitmapFont bitmap;
    if (!bitmap.load(font_path, pixel_height)) {
        std::fprintf(stderr,
                     "vulkan_graphix: could not load font %s\n",
                     font_path.c_str());
        return nullptr;
    }
    std::shared_ptr<Texture> atlas =
            createTexture(bitmap.atlasPixels(),
                          bitmap.atlasWidth(),
                          bitmap.atlasHeight(),
                          VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    if (!atlas) {
        return nullptr;
    }
    return std::make_unique<Font>(std::move(bitmap), std::move(atlas));
}

std::unique_ptr<Mesh> Renderer::createMeshFromBytes(
        const void* data, std::size_t byte_count, std::uint32_t vertex_count) {
    if (vertex_count == 0) {
        return std::make_unique<Mesh>(BufferParameters(), 0);
    }
    const VkDeviceSize size = byte_count;
    const vc::BufferFactory factory(m_device.device, m_device.physical_device);
    BufferParameters destination;
    destination.setSize(static_cast<std::uint32_t>(size));
    if (!factory.create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        destination)) {
        return nullptr;
    }
    BufferParameters staging;
    staging.setSize(static_cast<std::uint32_t>(size));
    if (!factory.create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        staging)) {
        factory.destroy(destination);
        return nullptr;
    }
    VkCommandBuffer upload_commands = VK_NULL_HANDLE;
    vc::FrameResourceFactory(m_device.device)
            .allocateCommandBuffers(m_command_pool, 1, &upload_commands);
    const bool uploaded =
            vc::StagedUploader(
                    m_device.device, m_device.graphics_queue, upload_commands)
                    .uploadToBuffer(staging,
                                    destination,
                                    data,
                                    static_cast<std::uint32_t>(size),
                                    VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                                    VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
    vg::vkFreeCommandBuffers(
            m_device.device, m_command_pool, 1, &upload_commands);
    factory.destroy(staging);
    if (!uploaded) {
        factory.destroy(destination);
        return nullptr;
    }
    return std::make_unique<Mesh>(destination, vertex_count);
}

const Mesh& Renderer::sphere(std::uint32_t slices, std::uint32_t stacks) {
    auto& mesh = m_spheres[{slices, stacks}];
    if (mesh) {
        return *mesh;
    }
    // freeglut's fghGenerateSphere(): slice angles run clockwise
    // (-2*pi*j/slices), stack angles from the +z pole (pi*i/stacks).
    const float pi = 3.14159265358979323846f;
    auto point = [&](std::uint32_t stack, std::uint32_t slice) {
        const float polar_angle =
                pi * static_cast<float>(stack) / static_cast<float>(stacks);
        const float theta = -2.0f * pi * static_cast<float>(slice % slices) /
                            static_cast<float>(slices);
        return Vec3(std::cos(theta) * std::sin(polar_angle),
                    std::sin(theta) * std::sin(polar_angle),
                    std::cos(polar_angle));
    };
    std::vector<MeshVertex> vertices;
    auto emit = [&](const Vec3& position) {
        vertices.push_back({position, position, Vec2(0.0f)});
    };
    for (std::uint32_t stack = 0; stack < stacks; ++stack) {
        for (std::uint32_t slice = 0; slice < slices; ++slice) {
            const Vec3 top_left = point(stack, slice);
            const Vec3 top_right = point(stack, slice + 1);
            const Vec3 bottom_left = point(stack + 1, slice);
            const Vec3 bottom_right = point(stack + 1, slice + 1);
            if (stack != 0) {
                emit(top_left);
                emit(bottom_left);
                emit(top_right);
            }
            if (stack + 1 != stacks) {
                emit(top_right);
                emit(bottom_left);
                emit(bottom_right);
            }
        }
    }
    mesh = createMesh(vertices);
    return *mesh;
}

}  // namespace vulkan_graphix::Render
