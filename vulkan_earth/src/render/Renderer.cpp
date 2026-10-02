#include "vulkan_earth/render/Renderer.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <utility>

#include "vulkan_earth/render/Camera.h"
#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/VulkanCommon.h"
#include "vulkan_graphix/VulkanFunctions.h"

namespace vulkan_earth::render {

namespace vg = vulkan_graphix;
namespace vc = vulkan_graphix::VulkanCommon;

namespace {
constexpr VkFormat c_depth_format = VK_FORMAT_D32_SFLOAT;
constexpr VkDeviceSize c_transient_size = 16 * 1024 * 1024;
constexpr std::uint32_t c_max_textures = 4096;
// glBitmap()'s raster position nudge (Mesa's, matching SGI's GL), so a
// position a hair under a pixel boundary still lands on it.
constexpr float c_bitmap_epsilon = 0.0001f;

GpuBuffer toGpuBuffer(vg::BufferParameters const& parameters) {
    GpuBuffer buffer;
    buffer.buffer = parameters.getVkBuffer();
    buffer.memory = parameters.getVkDeviceMemory();
    buffer.size = parameters.getSize();
    return buffer;
}

vg::BufferParameters toBufferParameters(GpuBuffer const& buffer) {
    vg::BufferParameters parameters;
    parameters.setVkBuffer(buffer.buffer);
    parameters.setVkDeviceMemory(buffer.memory);
    parameters.setSize(static_cast<std::uint32_t>(buffer.size));
    return parameters;
}

struct PipelineConfig {
    char const* vertex_shader;
    char const* fragment_shader;
    bool ui_vertex;
    VkPrimitiveTopology topology;
    bool depth_test;
    bool depth_write;
    bool blend;
    VkPolygonMode polygon_mode;
};

PipelineConfig pipelineConfig(PipelineId id) {
    constexpr VkPrimitiveTopology triangles =
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    constexpr VkPolygonMode fill = VK_POLYGON_MODE_FILL;
    switch (id) {
        case PipelineId::UiTriangles:
            return {"shaders/ui.vert.spv",
                    "shaders/ui.frag.spv",
                    true,
                    triangles,
                    true,
                    true,
                    true,
                    fill};
        case PipelineId::UiLines:
            return {"shaders/ui.vert.spv",
                    "shaders/ui.frag.spv",
                    true,
                    VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
                    true,
                    true,
                    true,
                    fill};
        case PipelineId::Text:
            return {"shaders/text.vert.spv",
                    "shaders/text.frag.spv",
                    true,
                    triangles,
                    true,
                    true,
                    false,
                    fill};
        case PipelineId::Mesh:
            return {"shaders/mesh.vert.spv",
                    "shaders/mesh.frag.spv",
                    false,
                    triangles,
                    true,
                    true,
                    true,
                    fill};
        case PipelineId::MeshBackground:
            return {"shaders/mesh.vert.spv",
                    "shaders/mesh.frag.spv",
                    false,
                    triangles,
                    false,
                    false,
                    true,
                    fill};
        case PipelineId::Terrain:
            return {"shaders/terrain.vert.spv",
                    "shaders/terrain.frag.spv",
                    false,
                    triangles,
                    true,
                    true,
                    true,
                    fill};
        case PipelineId::FlatColorWireframe:
            return {"shaders/flat.vert.spv",
                    "shaders/flat.frag.spv",
                    false,
                    triangles,
                    true,
                    true,
                    true,
                    VK_POLYGON_MODE_LINE};
        case PipelineId::Water:
            return {"shaders/water.vert.spv",
                    "shaders/water.frag.spv",
                    false,
                    triangles,
                    true,
                    true,
                    true,
                    fill};
        case PipelineId::FlatColor:
            return {"shaders/flat.vert.spv",
                    "shaders/flat.frag.spv",
                    false,
                    triangles,
                    true,
                    true,
                    true,
                    fill};
        case PipelineId::Count:
            break;
    }
    return {};
}
}  // namespace

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
    GlRect const full = {0,
                         0,
                         static_cast<std::int32_t>(extent.width),
                         static_cast<std::int32_t>(extent.height)};
    setViewport(full);
}

VkCommandBuffer RenderContext::commandBuffer() const {
    return m_command_buffer;
}

VkExtent2D RenderContext::extent() const { return m_extent; }

VkRect2D RenderContext::toVkRect(GlRect const& rect) const {
    // GL's origin is the bottom-left, Vulkan's the top-left; clamp to the
    // framebuffer since a Vulkan scissor may not extend past it.
    std::int32_t const width = static_cast<std::int32_t>(m_extent.width);
    std::int32_t const height = static_cast<std::int32_t>(m_extent.height);
    std::int32_t const left = std::clamp(rect.x, 0, width);
    std::int32_t const right = std::clamp(rect.x + rect.width, 0, width);
    std::int32_t const top =
            std::clamp(height - (rect.y + rect.height), 0, height);
    std::int32_t const bottom = std::clamp(height - rect.y, 0, height);
    return VkRect2D{{left, top},
                    {static_cast<std::uint32_t>(right - left),
                     static_cast<std::uint32_t>(bottom - top)}};
}

void RenderContext::applyViewport(GlRect const& rect) {
    VkViewport const viewport = {
            static_cast<float>(rect.x),
            static_cast<float>(static_cast<std::int32_t>(m_extent.height) -
                               (rect.y + rect.height)),
            static_cast<float>(rect.width),
            static_cast<float>(rect.height),
            0.0f,
            1.0f};
    vg::vkCmdSetViewport(m_command_buffer, 0, 1, &viewport);
}

void RenderContext::setViewport(GlRect const& rect) {
    m_viewport = rect;
    applyViewport(rect);
    setScissor(rect);
}

void RenderContext::setScissor(GlRect const& rect) {
    m_scissor = rect;
    VkRect2D const scissor = toVkRect(rect);
    vg::vkCmdSetScissor(m_command_buffer, 0, 1, &scissor);
}

GlRect const& RenderContext::viewport() const { return m_viewport; }

void RenderContext::setCamera(Mat4 const& projection, Mat4 const& view) {
    m_projection = projection;
    m_view = view;
}

Mat4 const& RenderContext::projection() const { return m_projection; }

Mat4 const& RenderContext::view() const { return m_view; }

void RenderContext::clearDepth() {
    VkClearAttachment const attachment = {
            VK_IMAGE_ASPECT_DEPTH_BIT, 1, {.depthStencil = {1.0f, 0}}};
    VkClearRect const rect = {toVkRect(m_scissor), 0, 1};
    if (rect.rect.extent.width == 0 || rect.rect.extent.height == 0) {
        return;
    }
    vg::vkCmdClearAttachments(m_command_buffer, 1, &attachment, 1, &rect);
}

void RenderContext::clearColorAndDepth(Vec4 const& color) {
    std::array<VkClearAttachment, 2> attachments{};
    attachments[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    attachments[0].colorAttachment = 0;
    attachments[0].clearValue.color = {{color.r, color.g, color.b, color.a}};
    attachments[1].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    attachments[1].clearValue.depthStencil = {1.0f, 0};
    VkClearRect const rect = {toVkRect(m_scissor), 0, 1};
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

void RenderContext::bindPipeline(PipelineId pipeline) {
    VkPipeline const handle = m_renderer.pipeline(pipeline, m_depth_test);
    if (m_bound_pipeline == handle) {
        return;
    }
    vg::vkCmdBindPipeline(
            m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, handle);
    m_bound_pipeline = handle;
}

void RenderContext::bindTexture(Texture const* texture) {
    VkDescriptorSet const descriptor_set =
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

void RenderContext::pushConstants(Mat4 const& model, Vec4 const& params) {
    DrawConstants const constants = {m_projection * m_view * model, params};
    vg::vkCmdPushConstants(
            m_command_buffer,
            m_renderer.pipelineLayout(),
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(DrawConstants),
            &constants);
}

void RenderContext::draw(UiMesh& mesh, Mat4 const& model) {
    GpuBuffer const& buffer = mesh.upload();
    if (buffer.buffer == VK_NULL_HANDLE) {
        return;
    }
    Vec4 const params(mesh.replaceTexEnv() ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f);
    auto const triangle_count =
            static_cast<std::uint32_t>(mesh.triangles().size());
    auto const line_count = static_cast<std::uint32_t>(mesh.lines().size());
    VkDeviceSize const offset = 0;
    if (triangle_count != 0) {
        bindPipeline(PipelineId::UiTriangles);
        bindTexture(mesh.texture());
        pushConstants(model, params);
        vg::vkCmdBindVertexBuffers(
                m_command_buffer, 0, 1, &buffer.buffer, &offset);
        vg::vkCmdDraw(m_command_buffer, triangle_count, 1, 0, 0);
    }
    if (line_count != 0) {
        bindPipeline(PipelineId::UiLines);
        bindTexture(mesh.texture());
        pushConstants(model, params);
        vg::vkCmdSetLineWidth(m_command_buffer,
                              m_renderer.clampLineWidth(mesh.lineWidth()));
        vg::vkCmdBindVertexBuffers(
                m_command_buffer, 0, 1, &buffer.buffer, &offset);
        vg::vkCmdDraw(m_command_buffer, line_count, 1, triangle_count, 0);
    }
}

void RenderContext::drawTransient(std::vector<UiVertex> const& vertices,
                                  PipelineId pipeline,
                                  Texture const* texture,
                                  Mat4 const& model,
                                  float line_width) {
    if (vertices.empty()) {
        return;
    }
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceSize offset = 0;
    if (!m_renderer.allocateTransient(vertices.data(),
                                      vertices.size() * sizeof(UiVertex),
                                      &buffer,
                                      &offset)) {
        return;
    }
    bindPipeline(pipeline);
    bindTexture(texture);
    pushConstants(model, Vec4(0.0f));
    if (pipeline == PipelineId::UiLines) {
        vg::vkCmdSetLineWidth(m_command_buffer,
                              m_renderer.clampLineWidth(line_width));
    }
    vg::vkCmdBindVertexBuffers(m_command_buffer, 0, 1, &buffer, &offset);
    vg::vkCmdDraw(m_command_buffer,
                  static_cast<std::uint32_t>(vertices.size()),
                  1,
                  0,
                  0);
}

void RenderContext::drawMesh(StaticMesh const& mesh,
                             PipelineId pipeline,
                             Texture const* texture,
                             Mat4 const& model,
                             Vec4 const& params) {
    if (mesh.vertexCount() == 0) {
        return;
    }
    if (pipeline == PipelineId::FlatColorWireframe &&
        !m_renderer.m_wireframe_supported) {
        pipeline = PipelineId::FlatColor;
    }
    bindPipeline(pipeline);
    bindTexture(texture);
    pushConstants(model, params);
    VkBuffer const buffer = mesh.buffer();
    VkDeviceSize const offset = 0;
    vg::vkCmdBindVertexBuffers(m_command_buffer, 0, 1, &buffer, &offset);
    vg::vkCmdDraw(m_command_buffer, mesh.vertexCount(), 1, 0, 0);
}

void RenderContext::emitGlyphs(GlutFont const& font,
                               float window_x,
                               float window_y_top_down,
                               float depth,
                               std::string_view text,
                               Vec4 const& color) {
    // Glyph cells in window pixels, positioned so each glyph's raster
    // origin lands on the (advancing) raster position, converted straight
    // to normalized device coordinates for the text pipeline (whose
    // viewport is the whole framebuffer during these draws).
    float const width = static_cast<float>(m_extent.width);
    float const height = static_cast<float>(m_extent.height);
    float const atlas_width =
            static_cast<float>(GlutFont::c_columns * GlutFont::c_cell_size);
    float const atlas_height =
            static_cast<float>(GlutFont::c_rows * GlutFont::c_cell_size);
    auto const cell = static_cast<float>(GlutFont::c_cell_size);
    auto to_ndc = [&](float pixel_x, float pixel_y) {
        return Vec3(pixel_x / width * 2.0f - 1.0f,
                    pixel_y / height * 2.0f - 1.0f,
                    depth);
    };
    std::vector<UiVertex> vertices;
    vertices.reserve(text.size() * 6);
    float pen_x = window_x;
    for (char character : text) {
        std::int32_t const index =
                static_cast<std::int32_t>(
                        static_cast<std::uint8_t>(character)) -
                GlutFont::c_first_char;
        if (index >= 0 && index < GlutFont::c_columns * GlutFont::c_rows &&
            character != ' ') {
            // Snapped to whole pixels exactly as glBitmap() does (Mesa:
            // IFLOOR(raster_position + 0.0001 - origin), in GL's bottom-up
            // window coordinates; the glyph origins are whole pixels).
            float const left = std::floor(pen_x + c_bitmap_epsilon) -
                               static_cast<float>(GlutFont::c_origin_x);
            float const top = (height - std::floor(height - window_y_top_down +
                                                   c_bitmap_epsilon)) -
                              static_cast<float>(GlutFont::c_origin_y);
            float const u0 = static_cast<float>(index % GlutFont::c_columns) *
                             cell / atlas_width;
            float const v0 = static_cast<float>(index / GlutFont::c_columns) *
                             cell / atlas_height;
            float const u1 = u0 + cell / atlas_width;
            float const v1 = v0 + cell / atlas_height;
            std::array<UiVertex, 4> const corners = {
                    UiVertex{to_ndc(left, top), color, Vec2(u0, v0)},
                    UiVertex{to_ndc(left, top + cell), color, Vec2(u0, v1)},
                    UiVertex{to_ndc(left + cell, top + cell),
                             color,
                             Vec2(u1, v1)},
                    UiVertex{to_ndc(left + cell, top), color, Vec2(u1, v0)}};
            for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U}) {
                vertices.push_back(corners[corner]);
            }
        }
        pen_x += static_cast<float>(font.advance(character));
    }
    if (vertices.empty()) {
        return;
    }
    // glBitmap output is clipped by the scissor, not the viewport: draw
    // across the whole framebuffer, then restore the caller's viewport.
    GlRect const full = {0,
                         0,
                         static_cast<std::int32_t>(m_extent.width),
                         static_cast<std::int32_t>(m_extent.height)};
    applyViewport(full);
    drawTransient(vertices, PipelineId::Text, &font.atlas());
    applyViewport(m_viewport);
}

void RenderContext::drawBitmapText(GlutFont const& font,
                                   Vec3 const& raster_position,
                                   std::string_view text,
                                   Vec4 const& color,
                                   Mat4 const& model) {
    Vec4 const clip =
            m_projection * m_view * model * Vec4(raster_position, 1.0f);
    // An invalid (clipped) raster position draws nothing, as in GL.
    if (clip.w <= 0.0f || std::abs(clip.x) > clip.w ||
        std::abs(clip.y) > clip.w || clip.z < 0.0f || clip.z > clip.w) {
        return;
    }
    Vec3 const ndc = Vec3(clip) / clip.w;
    // Vulkan's (y-flipped) projection already puts +y downward.
    float const window_x =
            static_cast<float>(m_viewport.x) +
            (ndc.x + 1.0f) * 0.5f * static_cast<float>(m_viewport.width);
    float const viewport_top =
            static_cast<float>(static_cast<std::int32_t>(m_extent.height) -
                               (m_viewport.y + m_viewport.height));
    float const window_y =
            viewport_top +
            (ndc.y + 1.0f) * 0.5f * static_cast<float>(m_viewport.height);
    emitGlyphs(font, window_x, window_y, ndc.z, text, color);
}

void RenderContext::drawBitmapTextAtWindow(GlutFont const& font,
                                           Vec2 const& window_position,
                                           float depth,
                                           std::string_view text,
                                           Vec4 const& color) {
    float const window_y =
            static_cast<float>(m_extent.height) - window_position.y;
    emitGlyphs(font, window_position.x, window_y, depth, text, color);
}

// ************************************************************ //
// Renderer                                                     //
// ************************************************************ //
Renderer* Renderer::s_instance = nullptr;

Renderer& Renderer::instance() { return *s_instance; }

bool Renderer::hasInstance() { return s_instance != nullptr; }

Renderer::Renderer() : m_context(*this) {}

Renderer::~Renderer() { shutdown(); }

bool Renderer::initialize(DeviceInfo const& device,
                          SwapchainInfo const& swapchain) {
    s_instance = this;
    m_device = device;
    m_swapchain = swapchain;

    VkPhysicalDeviceFeatures features;
    vg::vkGetPhysicalDeviceFeatures(m_device.physical_device, &features);
    m_wireframe_supported = features.fillModeNonSolid == VK_TRUE;
    // glLineWidth() clamps to the implementation's range; so does this.
    // Without wideLines (TutorialBase enables it when available), only
    // 1.0 is valid.
    if (features.wideLines == VK_TRUE) {
        VkPhysicalDeviceProperties properties;
        vg::vkGetPhysicalDeviceProperties(m_device.physical_device,
                                          &properties);
        m_line_width_range = {properties.limits.lineWidthRange[0],
                              properties.limits.lineWidthRange[1]};
    }

    if (!createRenderPass() || !createSwapchainResources() ||
        !createDescriptorResources() || !createPipelines() ||
        !createFrameSlots()) {
        return false;
    }
    std::vector<char> const white(4, static_cast<char>(0xFF));
    m_white_texture =
            createTexture(white, 1, 1, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    return m_white_texture != nullptr && loadFonts();
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

    VkAttachmentReference const color_reference = {
            0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkAttachmentReference const depth_reference = {
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
    vc::ImageFactory const factory(m_device.device, m_device.physical_device);
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
    vc::FrameResourceFactory const frame_factory(m_device.device);
    for (std::size_t i = 0; i < m_swapchain.views.size(); ++i) {
        std::array<VkImageView, 2> const views = {m_swapchain.views[i],
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
    m_depth_image = vg::ImageParameters();
}

void Renderer::releaseSwapchainResources() {
    vg::vkDeviceWaitIdle(m_device.device);
    destroySwapchainResources();
}

bool Renderer::onSwapchainRecreated(SwapchainInfo const& swapchain) {
    vg::vkDeviceWaitIdle(m_device.device);
    destroySwapchainResources();
    m_swapchain = swapchain;
    return createSwapchainResources();
}

bool Renderer::createDescriptorResources() {
    VkDescriptorSetLayoutBinding const binding = {
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

    VkDescriptorPoolSize const pool_size = {
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

    VkPushConstantRange const push_range = {
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

bool Renderer::createPipelines() {
    for (std::uint32_t i = 0;
         i < static_cast<std::uint32_t>(PipelineId::Count);
         ++i) {
        auto const id = static_cast<PipelineId>(i);
        if (id == PipelineId::FlatColorWireframe && !m_wireframe_supported) {
            continue;
        }
        if (!createPipeline(id, true) || !createPipeline(id, false)) {
            std::fprintf(
                    stderr, "vulkan_earth: could not create pipeline %u\n", i);
            return false;
        }
    }
    return true;
}

bool Renderer::createPipeline(PipelineId id, bool depth_test) {
    PipelineConfig config = pipelineConfig(id);
    if (!depth_test) {
        config.depth_test = false;
        config.depth_write = false;
    }
    auto vertex_module =
            vc::createShaderModule(m_device.device, config.vertex_shader);
    auto fragment_module =
            vc::createShaderModule(m_device.device, config.fragment_shader);
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

    VkVertexInputBindingDescription const binding = {
            0,
            config.ui_vertex ? sizeof(UiVertex) : sizeof(MeshVertex),
            VK_VERTEX_INPUT_RATE_VERTEX};
    std::array<VkVertexInputAttributeDescription, 3> attributes = {};
    if (config.ui_vertex) {
        attributes[0] = {0,
                         0,
                         VK_FORMAT_R32G32B32_SFLOAT,
                         offsetof(UiVertex, position)};
        attributes[1] = {1,
                         0,
                         VK_FORMAT_R32G32B32A32_SFLOAT,
                         offsetof(UiVertex, color)};
        attributes[2] = {
                2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(UiVertex, texcoord)};
    } else {
        attributes[0] = {0,
                         0,
                         VK_FORMAT_R32G32B32_SFLOAT,
                         offsetof(MeshVertex, position)};
        attributes[1] = {1,
                         0,
                         VK_FORMAT_R32G32B32_SFLOAT,
                         offsetof(MeshVertex, normal)};
        attributes[2] = {
                2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(MeshVertex, texcoord)};
    }
    VkPipelineVertexInputStateCreateInfo vertex_input = {};
    vertex_input.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input.vertexBindingDescriptionCount = 1;
    vertex_input.pVertexBindingDescriptions = &binding;
    vertex_input.vertexAttributeDescriptionCount =
            static_cast<std::uint32_t>(attributes.size());
    vertex_input.pVertexAttributeDescriptions = attributes.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
    input_assembly.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = config.topology;

    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    // No face culling: the game never enabled GL_CULL_FACE.
    VkPipelineRasterizationStateCreateInfo rasterization = {};
    rasterization.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization.polygonMode = config.polygon_mode;
    rasterization.cullMode = VK_CULL_MODE_NONE;
    rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample = {};
    multisample.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // glDepthFunc's default, GL_LESS.
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
    depth_stencil.sType =
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = config.depth_test ? VK_TRUE : VK_FALSE;
    depth_stencil.depthWriteEnable = config.depth_write ? VK_TRUE : VK_FALSE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA), enabled for the
    // whole game at startup (initStuff()).
    VkPipelineColorBlendAttachmentState blend_attachment = {};
    blend_attachment.blendEnable = config.blend ? VK_TRUE : VK_FALSE;
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

    std::array<VkDynamicState, 3> const dynamic_states = {
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
    return vg::vkCreateGraphicsPipelines(
                   m_device.device,
                   VK_NULL_HANDLE,
                   1,
                   &create_info,
                   nullptr,
                   &(depth_test
                             ? m_pipelines
                             : m_pipelines_no_depth)[static_cast<std::size_t>(
                           id)]) == VK_SUCCESS;
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
    vc::FrameResourceFactory const factory(m_device.device);
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

bool Renderer::loadFonts() {
    std::array<std::pair<FontId, char const*>, 2> const fonts = {
            std::pair{FontId::TimesRoman24, "fonts/glut_times_roman_24.raw"},
            std::pair{FontId::Fixed9By15, "fonts/glut_9_by_15.raw"}};
    for (auto const& [id, path] : fonts) {
        std::shared_ptr<Texture> atlas =
                loadRawTexture(path,
                               GlutFont::c_columns * GlutFont::c_cell_size,
                               GlutFont::c_rows * GlutFont::c_cell_size,
                               VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        if (!atlas) {
            std::fprintf(stderr, "vulkan_earth: missing font %s\n", path);
            return false;
        }
        m_fonts.push_back(std::make_unique<GlutFont>(id, std::move(atlas)));
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
    m_fonts.clear();
    m_spheres.clear();
    m_white_texture.reset();
    m_texture_cache.clear();
    if (m_capture_buffer.buffer != VK_NULL_HANDLE) {
        deferRelease(m_capture_buffer);
        m_capture_buffer = GpuBuffer{};
    }
    for (FrameSlot& slot : m_frames) {
        if (slot.transient.buffer != VK_NULL_HANDLE) {
            deferRelease(slot.transient);
            slot.transient = GpuBuffer{};
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
    for (auto* pipelines : {&m_pipelines, &m_pipelines_no_depth}) {
        for (VkPipeline& pipeline : *pipelines) {
            if (pipeline != VK_NULL_HANDLE) {
                vg::vkDestroyPipeline(m_device.device, pipeline, nullptr);
                pipeline = VK_NULL_HANDLE;
            }
        }
    }
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

VkPipeline Renderer::pipeline(PipelineId id, bool depth_test) const {
    return (depth_test ? m_pipelines
                       : m_pipelines_no_depth)[static_cast<std::size_t>(id)];
}

float Renderer::clampLineWidth(float width) const {
    return std::clamp(width, m_line_width_range[0], m_line_width_range[1]);
}

VkPipelineLayout Renderer::pipelineLayout() const { return m_pipeline_layout; }

VkExtent2D Renderer::extent() const { return m_swapchain.extent; }

std::uint64_t Renderer::frameNumber() const { return m_frame_number; }

RenderContext* Renderer::beginFrame(Vec4 const& clear_color) {
    FrameSlot& slot = m_frames[m_frame_slot];
    vg::vkWaitForFences(
            m_device.device, 1, &slot.in_flight, VK_TRUE, UINT64_MAX);
    // Everything released while this slot's previous frame could still
    // have been reading it is now safe to free.
    runReleases(slot);
    slot.transient_offset = 0;

    VkResult const acquired = ::vkAcquireNextImageKHR(m_device.device,
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

    bool const capturing = !m_capture_path.empty();
    if (capturing) {
        VkDeviceSize const size =
                static_cast<VkDeviceSize>(m_swapchain.extent.width) *
                m_swapchain.extent.height * 4;
        if (m_capture_buffer.size < size) {
            if (m_capture_buffer.buffer != VK_NULL_HANDLE) {
                deferRelease(m_capture_buffer);
            }
            m_capture_buffer =
                    createHostBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
        }
        VkImage const image = m_swapchain.images[m_image_index];
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
                                   m_capture_buffer.buffer,
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

    VkPipelineStageFlags const wait_stage =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSemaphore const render_finished = m_render_finished[m_image_index];
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
    VkResult const presented =
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
    std::uint32_t const width = m_swapchain.extent.width;
    std::uint32_t const height = m_swapchain.extent.height;
    std::fprintf(file, "P6\n%u %u\n255\n", width, height);
    bool const bgra = m_swapchain.format == VK_FORMAT_B8G8R8A8_UNORM ||
                      m_swapchain.format == VK_FORMAT_B8G8R8A8_SRGB;
    auto const* pixels =
            static_cast<std::uint8_t const*>(m_capture_buffer.mapped);
    std::vector<std::uint8_t> row(static_cast<std::size_t>(width) * 3);
    for (std::uint32_t y = 0; y < height; ++y) {
        for (std::uint32_t x = 0; x < width; ++x) {
            std::uint8_t const* pixel =
                    pixels + (static_cast<std::size_t>(y) * width + x) * 4;
            row[x * 3 + 0] = bgra ? pixel[2] : pixel[0];
            row[x * 3 + 1] = pixel[1];
            row[x * 3 + 2] = bgra ? pixel[0] : pixel[2];
        }
        std::fwrite(row.data(), 1, row.size(), file);
    }
    std::fclose(file);
    std::fprintf(stderr, "captured %s\n", m_capture_path.c_str());
    return true;
}

bool Renderer::allocateTransient(void const* data,
                                 VkDeviceSize size,
                                 VkBuffer* buffer,
                                 VkDeviceSize* offset) {
    FrameSlot& slot = m_frames[m_frame_slot];
    VkDeviceSize const aligned =
            (slot.transient_offset + 15) & ~VkDeviceSize{15};
    if (aligned + size > slot.transient.size) {
        return false;
    }
    std::memcpy(
            static_cast<char*>(slot.transient.mapped) + aligned, data, size);
    slot.transient_offset = aligned + size;
    *buffer = slot.transient.buffer;
    *offset = aligned;
    return true;
}

GpuBuffer Renderer::createHostBuffer(VkDeviceSize size,
                                     VkBufferUsageFlags usage) {
    vg::BufferParameters parameters;
    parameters.setSize(static_cast<std::uint32_t>(size));
    vc::BufferFactory const factory(m_device.device, m_device.physical_device);
    if (!factory.create(usage,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        parameters)) {
        return GpuBuffer{};
    }
    GpuBuffer buffer = toGpuBuffer(parameters);
    if (vg::vkMapMemory(m_device.device,
                        buffer.memory,
                        0,
                        buffer.size,
                        0,
                        &buffer.mapped) != VK_SUCCESS) {
        buffer.mapped = nullptr;
    }
    return buffer;
}

void Renderer::deferRelease(GpuBuffer buffer) {
    if (buffer.buffer == VK_NULL_HANDLE) {
        return;
    }
    VkDevice const device = m_device.device;
    VkPhysicalDevice const physical_device = m_device.physical_device;
    m_frames[m_frame_slot].releases.emplace_back([=]() {
        if (buffer.mapped != nullptr) {
            vg::vkUnmapMemory(device, buffer.memory);
        }
        vg::BufferParameters parameters = toBufferParameters(buffer);
        vc::BufferFactory(device, physical_device).destroy(parameters);
    });
}

void Renderer::deferRelease(vg::ImageParameters image,
                            VkDescriptorSet descriptor_set) {
    VkDevice const device = m_device.device;
    VkPhysicalDevice const physical_device = m_device.physical_device;
    VkDescriptorPool const pool = m_descriptor_pool;
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
    VkDescriptorImageInfo const image_info = {
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
        std::vector<char> const& pixels,
        std::uint32_t width,
        std::uint32_t height,
        VkSamplerAddressMode address_mode) {
    if (pixels.size() < static_cast<std::size_t>(width) * height * 4) {
        return nullptr;
    }
    vc::BufferFactory const buffer_factory(m_device.device,
                                           m_device.physical_device);
    vg::BufferParameters staging;
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
    vc::ImageFactory const image_factory(m_device.device,
                                         m_device.physical_device);
    vc::StagedUploader const uploader(
            m_device.device, m_device.graphics_queue, upload_commands);
    vg::ImageParameters image;
    bool const created =
            vc::createTextureFromPixels(image_factory,
                                        uploader,
                                        staging,
                                        width,
                                        height,
                                        pixels,
                                        address_mode,
                                        image,
                                        // GL_CLAMP's border on a
                                        // GL_RGB texture: black,
                                        // alpha 1.
                                        VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);
    vg::vkFreeCommandBuffers(
            m_device.device, m_command_pool, 1, &upload_commands);
    buffer_factory.destroy(staging);
    if (!created) {
        image_factory.destroy(image);
        return nullptr;
    }
    VkDescriptorSet const descriptor_set = allocateTextureDescriptor(
            image.getVkImageView(), image.getVkSampler());
    return std::make_shared<Texture>(image, descriptor_set, width, height);
}

std::shared_ptr<Texture> Renderer::loadRawTexture(
        std::string const& filename,
        std::uint32_t width,
        std::uint32_t height,
        VkSamplerAddressMode address_mode) {
    std::string const key =
            filename + "#" + std::to_string(static_cast<int>(address_mode));
    if (auto cached = m_texture_cache[key].lock()) {
        return cached;
    }
    std::vector<char> const pixels =
            vg::Tools::getRawImageData(filename, width, height);
    if (pixels.empty()) {
        std::fprintf(
                stderr, "vulkan_earth: could not read %s\n", filename.c_str());
        return nullptr;
    }
    std::shared_ptr<Texture> texture =
            createTexture(pixels, width, height, address_mode);
    m_texture_cache[key] = texture;
    return texture;
}

std::shared_ptr<Texture> Renderer::loadImageTexture(
        std::string const& filename, VkSamplerAddressMode address_mode) {
    std::string const key =
            filename + "#" + std::to_string(static_cast<int>(address_mode));
    if (auto cached = m_texture_cache[key].lock()) {
        return cached;
    }
    std::int32_t width = 0;
    std::int32_t height = 0;
    std::int32_t components = 0;
    std::int32_t data_size = 0;
    std::vector<char> const pixels = vg::Tools::getImageData(
            filename, 4, &width, &height, &components, &data_size);
    if (pixels.empty()) {
        std::fprintf(
                stderr, "vulkan_earth: could not read %s\n", filename.c_str());
        return nullptr;
    }
    std::shared_ptr<Texture> texture =
            createTexture(pixels,
                          static_cast<std::uint32_t>(width),
                          static_cast<std::uint32_t>(height),
                          address_mode);
    m_texture_cache[key] = texture;
    return texture;
}

Texture const& Renderer::whiteTexture() const { return *m_white_texture; }

std::unique_ptr<StaticMesh> Renderer::createMesh(
        std::vector<MeshVertex> const& vertices) {
    if (vertices.empty()) {
        return std::make_unique<StaticMesh>(GpuBuffer{}, 0);
    }
    VkDeviceSize const size = vertices.size() * sizeof(MeshVertex);
    vc::BufferFactory const factory(m_device.device, m_device.physical_device);
    vg::BufferParameters destination;
    destination.setSize(static_cast<std::uint32_t>(size));
    if (!factory.create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        destination)) {
        return nullptr;
    }
    vg::BufferParameters staging;
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
    bool const uploaded =
            vc::StagedUploader(
                    m_device.device, m_device.graphics_queue, upload_commands)
                    .uploadToBuffer(staging,
                                    destination,
                                    vertices.data(),
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
    return std::make_unique<StaticMesh>(
            toGpuBuffer(destination),
            static_cast<std::uint32_t>(vertices.size()));
}

StaticMesh const& Renderer::sphere(std::uint32_t slices,
                                   std::uint32_t stacks) {
    auto& mesh = m_spheres[{slices, stacks}];
    if (mesh) {
        return *mesh;
    }
    // freeglut's fghGenerateSphere(): slice angles run clockwise
    // (-2*pi*j/slices), stack angles from the +z pole (pi*i/stacks).
    float const pi = 3.14159265358979323846f;
    auto point = [&](std::uint32_t stack, std::uint32_t slice) {
        float const phi =
                pi * static_cast<float>(stack) / static_cast<float>(stacks);
        float const theta = -2.0f * pi * static_cast<float>(slice % slices) /
                            static_cast<float>(slices);
        return Vec3(std::cos(theta) * std::sin(phi),
                    std::sin(theta) * std::sin(phi),
                    std::cos(phi));
    };
    std::vector<MeshVertex> vertices;
    auto emit = [&](Vec3 const& position) {
        vertices.push_back({position, position, Vec2(0.0f)});
    };
    for (std::uint32_t stack = 0; stack < stacks; ++stack) {
        for (std::uint32_t slice = 0; slice < slices; ++slice) {
            Vec3 const top_left = point(stack, slice);
            Vec3 const top_right = point(stack, slice + 1);
            Vec3 const bottom_left = point(stack + 1, slice);
            Vec3 const bottom_right = point(stack + 1, slice + 1);
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

GlutFont const& Renderer::font(FontId id) const {
    for (auto const& font : m_fonts) {
        if (font->id() == id) {
            return *font;
        }
    }
    return *m_fonts.front();
}

std::int32_t windowWidth() {
    return static_cast<std::int32_t>(Renderer::instance().extent().width);
}

std::int32_t windowHeight() {
    return static_cast<std::int32_t>(Renderer::instance().extent().height);
}

void resetToFullWindow(RenderContext& context) {
    std::int32_t win_width = windowWidth();
    std::int32_t win_height = windowHeight();
    context.setViewport({0, 0, win_width, win_height});
    context.setCamera(
            camera::perspective(60.0,
                                static_cast<float>(win_width) /
                                        static_cast<float>(win_height),
                                1.0,
                                1000000.0),
            Mat4(1.0f));
}

void beginOverlayPanel(RenderContext& context,
                       GlRect const& viewport,
                       std::int32_t width,
                       std::int32_t height) {
    context.setViewport(viewport);
    context.clearColorAndDepth(Vec4(0.75, 0.75, 0.75, 1));
    std::int32_t distance =
            static_cast<std::int32_t>(windowHeight() / 4 * tan(1.04719755));
    context.setCamera(camera::perspective(60.0,
                                          (static_cast<float>(width) /
                                           (1.5 * static_cast<float>(height))),
                                          1,
                                          199999999),
                      camera::lookAt(Vec3(0, 0, distance),
                                     Vec3(0, 0, 0),
                                     Vec3(0.0f, 1.0f, 0.0f)));
}

}  // namespace vulkan_earth::render
