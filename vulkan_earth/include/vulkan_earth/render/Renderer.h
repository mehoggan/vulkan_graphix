#ifndef VULKAN_EARTH_RENDER_RENDERER_H
#define VULKAN_EARTH_RENDER_RENDERER_H

// vulkan_earth's Vulkan renderer - the replacement for the OpenGL/GLUT/
// GLEW stack the game was originally written against.
//
// Design:
// - Device/swapchain bring-up stays in libvulkan_graphix's TutorialBase
//   (the game's App derives from it); Renderer owns everything per-game on
//   top: one render pass (color + D32 depth), framebuffers, two frames in
//   flight, the pipelines (RenderTypes.h's PipelineId), a descriptor pool
//   for textures, and the GLUT fonts.
// - Game objects own their GPU resources: textures and static meshes are
//   created once; a UI control keeps a UiMesh it rebuilds only when it
//   changes. The one per-frame stream is a transient ring buffer per
//   frame-in-flight, for geometry that genuinely changes every frame
//   (text placed at a projected raster position, HUD bars, particles).
// - Destroying a Texture/StaticMesh/UiMesh hands its Vulkan objects to
//   deferRelease(), which frees them once every in-flight frame that
//   might still read them has completed.
// - beginFrame() hands out a RenderContext: the per-frame recording API
//   every game object's draw() takes - camera matrices built the same
//   way the GL code built them (see Camera.h), GL-convention viewports,
//   sub-viewport clears, and draws.
// - requestCapture() copies a finished frame back to a PPM file, for
//   verifying the port against frames captured from the original GL game
//   (and VE_SCRIPT-driven runs, see App).

#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "vulkan_earth/render/Font.h"
#include "vulkan_earth/render/Mesh.h"
#include "vulkan_earth/render/RenderTypes.h"
#include "vulkan_earth/render/Texture.h"

namespace vulkan_earth::render {

class Renderer;

struct DeviceInfo {
    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkQueue graphics_queue = VK_NULL_HANDLE;
    std::uint32_t graphics_family = 0;
    VkQueue present_queue = VK_NULL_HANDLE;
};

struct SwapchainInfo {
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkExtent2D extent = {0, 0};
    std::vector<VkImage> images;
    std::vector<VkImageView> views;
};

// A rectangle in GL window convention: origin at the bottom-left.
struct GlRect {
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
};

class RenderContext {
public:
    explicit RenderContext(Renderer& renderer);

    VkCommandBuffer commandBuffer() const;
    VkExtent2D extent() const;

    // glViewport() + matching glScissor(), GL convention.
    void setViewport(GlRect const& rect);
    // glScissor() alone (the viewport is unchanged).
    void setScissor(GlRect const& rect);
    GlRect const& viewport() const;

    // The projection and view (modelview without a model transform) the
    // GL code set up; every subsequent draw's mvp is projection * view *
    // model.
    void setCamera(Mat4 const& projection, Mat4 const& view);
    Mat4 const& projection() const;
    Mat4 const& view() const;

    // glEnable()/glDisable(GL_DEPTH_TEST) together with glDepthMask(): when
    // off, every later draw neither tests nor writes depth (the game's HUD
    // and minimap overlays).
    void setDepthTest(bool enabled);
    bool depthTest() const;

    // glClear() of depth, or color+depth, within the current scissor.
    void clearDepth();
    void clearColorAndDepth(Vec4 const& color);

    void draw(UiMesh& mesh, Mat4 const& model = Mat4(1.0f));
    // One-frame geometry through the transient ring (triangles or lines).
    void drawTransient(std::vector<UiVertex> const& vertices,
                       PipelineId pipeline,
                       Texture const* texture,
                       Mat4 const& model = Mat4(1.0f),
                       float line_width = 1.0f);
    void drawMesh(StaticMesh const& mesh,
                  PipelineId pipeline,
                  Texture const* texture,
                  Mat4 const& model,
                  Vec4 const& params);
    // glRasterPos3f(raster_position) under model, then
    // glutBitmapCharacter() for each character: glyphs advance in window
    // pixels from the projected raster position, are drawn at its depth,
    // and nothing is drawn if it falls outside the view volume.
    void drawBitmapText(GlutFont const& font,
                        Vec3 const& raster_position,
                        std::string_view text,
                        Vec4 const& color,
                        Mat4 const& model = Mat4(1.0f));
    // glRasterPos2f() in window pixels (origin bottom-left) followed by
    // glutBitmapCharacter(), at depth depth (0 near, 1 far).
    void drawBitmapTextAtWindow(GlutFont const& font,
                                Vec2 const& window_position,
                                float depth,
                                std::string_view text,
                                Vec4 const& color);

private:
    friend class Renderer;
    void begin(VkCommandBuffer command_buffer, VkExtent2D extent);
    void bindPipeline(PipelineId pipeline);
    void bindTexture(Texture const* texture);
    void pushConstants(Mat4 const& model, Vec4 const& params);
    void applyViewport(GlRect const& rect);
    VkRect2D toVkRect(GlRect const& rect) const;
    void emitGlyphs(GlutFont const& font,
                    float window_x,
                    float window_y_top_down,
                    float depth,
                    std::string_view text,
                    Vec4 const& color);

    Renderer& m_renderer;
    VkCommandBuffer m_command_buffer = VK_NULL_HANDLE;
    VkExtent2D m_extent = {0, 0};
    GlRect m_viewport = {0, 0, 0, 0};
    GlRect m_scissor = {0, 0, 0, 0};
    Mat4 m_projection = Mat4(1.0f);
    Mat4 m_view = Mat4(1.0f);
    VkPipeline m_bound_pipeline = VK_NULL_HANDLE;
    bool m_depth_test = true;
    VkDescriptorSet m_bound_descriptor_set = VK_NULL_HANDLE;
};

class Renderer {
public:
    static constexpr std::uint32_t c_frames_in_flight = 2;

    // The process-wide renderer (the GL context's role): set by App while
    // it owns a live Renderer.
    static Renderer& instance();
    static bool hasInstance();

    Renderer();
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool initialize(DeviceInfo const& device, SwapchainInfo const& swapchain);
    // Frees everything tied to the old swapchain's images (framebuffers,
    // depth buffer) - call before the swapchain is rebuilt.
    void releaseSwapchainResources();
    bool onSwapchainRecreated(SwapchainInfo const& swapchain);
    void shutdown();

    // Waits for this frame slot, acquires a swapchain image, and begins
    // the render pass with color cleared to clear_color and depth to 1.
    // Returns nullptr if the swapchain must be recreated first.
    RenderContext* beginFrame(Vec4 const& clear_color);
    // Ends and submits the frame and presents it. Returns false if the
    // swapchain must be recreated.
    bool endFrame();

    // The next completed frame is written to path as a binary PPM.
    void requestCapture(std::string path);

    VkExtent2D extent() const;
    std::uint64_t frameNumber() const;

    // RGBA8 pixels, width * height * 4 bytes.
    std::shared_ptr<Texture> createTexture(std::vector<char> const& pixels,
                                           std::uint32_t width,
                                           std::uint32_t height,
                                           VkSamplerAddressMode address_mode);
    // The game's headerless RGB .raw textures, loaded once per
    // (filename, address mode) and shared.
    std::shared_ptr<Texture> loadRawTexture(std::string const& filename,
                                            std::uint32_t width,
                                            std::uint32_t height,
                                            VkSamplerAddressMode address_mode);
    // .jpg/.png textures (stb_image), shared the same way.
    std::shared_ptr<Texture> loadImageTexture(
            std::string const& filename, VkSamplerAddressMode address_mode);
    Texture const& whiteTexture() const;

    std::unique_ptr<StaticMesh> createMesh(
            std::vector<MeshVertex> const& vertices);
    GlutFont const& font(FontId id) const;
    // glutSolidSphere(1, slices, stacks)'s geometry - a unit sphere in
    // freeglut's own tessellation and triangle order (top fan, stacks top
    // to bottom, bottom fan), created once per (slices, stacks). Draw it
    // scaled by the radius.
    StaticMesh const& sphere(std::uint32_t slices, std::uint32_t stacks);

    // Host-visible, coherent, persistently mapped.
    GpuBuffer createHostBuffer(VkDeviceSize size, VkBufferUsageFlags usage);
    // Frees buffer / image (+ its descriptor set) once no in-flight frame
    // can still be using it.
    void deferRelease(GpuBuffer buffer);
    void deferRelease(vulkan_graphix::ImageParameters image,
                      VkDescriptorSet descriptor_set);

private:
    friend class RenderContext;

    struct FrameSlot {
        VkCommandBuffer command_buffer = VK_NULL_HANDLE;
        VkSemaphore image_available = VK_NULL_HANDLE;
        VkFence in_flight = VK_NULL_HANDLE;
        GpuBuffer transient;
        VkDeviceSize transient_offset = 0;
        std::vector<std::function<void()>> releases;
    };

    bool createRenderPass();
    bool createSwapchainResources();
    void destroySwapchainResources();
    bool createPipelines();
    bool createPipeline(PipelineId id, bool depth_test);
    bool createDescriptorResources();
    bool createFrameSlots();
    bool loadFonts();
    void runReleases(FrameSlot& slot);
    // id's pipeline, or its variant with depth testing and writing off.
    VkPipeline pipeline(PipelineId id, bool depth_test) const;
    VkPipelineLayout pipelineLayout() const;
    float clampLineWidth(float width) const;
    // Copies size bytes into the current frame's transient ring; returns
    // the buffer offset, or false when the ring is full.
    bool allocateTransient(void const* data,
                           VkDeviceSize size,
                           VkBuffer* buffer,
                           VkDeviceSize* offset);
    bool writeCapture();
    VkDescriptorSet allocateTextureDescriptor(VkImageView view,
                                              VkSampler sampler);

    static Renderer* s_instance;

    DeviceInfo m_device;
    SwapchainInfo m_swapchain;
    VkRenderPass m_render_pass = VK_NULL_HANDLE;
    vulkan_graphix::ImageParameters m_depth_image;
    std::vector<VkFramebuffer> m_framebuffers;
    std::vector<VkSemaphore> m_render_finished;
    VkCommandPool m_command_pool = VK_NULL_HANDLE;
    std::array<FrameSlot, c_frames_in_flight> m_frames;
    std::uint32_t m_frame_slot = 0;
    std::uint32_t m_image_index = 0;
    std::uint64_t m_frame_number = 0;
    VkDescriptorSetLayout m_texture_layout = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptor_pool = VK_NULL_HANDLE;
    VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
    std::array<VkPipeline, static_cast<std::size_t>(PipelineId::Count)>
            m_pipelines{};
    std::array<VkPipeline, static_cast<std::size_t>(PipelineId::Count)>
            m_pipelines_no_depth{};
    bool m_wireframe_supported = false;
    std::array<float, 2> m_line_width_range = {1.0f, 1.0f};
    std::shared_ptr<Texture> m_white_texture;
    std::map<std::string, std::weak_ptr<Texture>> m_texture_cache;
    std::vector<std::unique_ptr<GlutFont>> m_fonts;
    std::map<std::pair<std::uint32_t, std::uint32_t>,
             std::unique_ptr<StaticMesh>>
            m_spheres;
    RenderContext m_context;
    std::string m_capture_path;
    GpuBuffer m_capture_buffer;
    bool m_capture_pending = false;
};

// glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT): the current
// swapchain size.
std::int32_t windowWidth();
std::int32_t windowHeight();

// The full window again, with the menus' perspective (60 degrees, near 1,
// far 1000000) and an identity view: how the game left things after each
// sub-viewport it drew (the minimap, help, inventory, tank preview).
void resetToFullWindow(RenderContext& context);

// A gray overlay panel's own viewport (the help manual and inventory):
// viewport cleared to (0.75, 0.75, 0.75, 1), seen through a 60-degree
// perspective of aspect width / (1.5 * height), from window height / 4 *
// tan(60 degrees) away.
void beginOverlayPanel(RenderContext& context,
                       GlRect const& viewport,
                       std::int32_t width,
                       std::int32_t height);

}  // namespace vulkan_earth::render

#endif  // VULKAN_EARTH_RENDER_RENDERER_H
