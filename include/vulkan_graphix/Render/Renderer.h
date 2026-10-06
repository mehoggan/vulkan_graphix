#ifndef VULKAN_GRAPHIX_RENDER_RENDERER_H
#define VULKAN_GRAPHIX_RENDER_RENDERER_H

// A general-purpose renderer on top of TutorialBase's device/swapchain
// bring-up, for any program here that would otherwise hand-roll its own
// render pass, frame loop, buffers, and textures (vulkan_earth uses it).
//
// Design:
// - Renderer owns one render pass (color + D32 depth), its framebuffers,
//   two frames in flight, a descriptor pool for textures, and the
//   pipelines its client describes (PipelineDescription) - each created
//   twice, with and without depth testing, so a draw can switch depth
//   testing off (RenderContext::setDepthTest()) without the client
//   describing both.
// - Every pipeline shares one layout: set 0 binding 0 is a combined image
//   sampler (the draw's texture, or a 1x1 white one), and the push
//   constant block is DrawConstants - the full model-view-projection,
//   computed on the CPU, plus four draw-specific parameters.
// - Resources are owned by whoever creates them: Textures and Meshes are
//   uploaded once; a RetainedMesh is rebuilt only when it changes. The one
//   per-frame stream is a transient ring buffer per frame in flight, for
//   geometry that genuinely changes every frame.
// - Destroying a Texture/Mesh/RetainedMesh hands its Vulkan objects to
//   deferRelease(), which frees them once every in-flight frame that might
//   still read them has completed.
// - beginFrame() hands out a RenderContext, the per-frame recording API:
//   viewports, cameras, clears, depth testing on/off, and draws.
// - requestCapture() copies a finished frame back to a PPM file, for
//   comparing renders across runs.

#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Font.h"
#include "vulkan_graphix/Render/Mesh.h"
#include "vulkan_graphix/Render/Texture.h"
#include "vulkan_graphix/Render/Vertex.h"

namespace vulkan_graphix {
class TutorialBase;
}

namespace vulkan_graphix::Render {

class Renderer;

// Every Render pipeline's push-constant block.
struct DrawConstants {
  Math::Mat4<float> m_mvp;
  Math::Vec4<float> m_params;
};

// Index of a pipeline created by Renderer::createPipeline().
using PipelineHandle = std::uint32_t;

struct PipelineDescription {
  // SPIR-V files, relative to the executable's directory.
  std::string m_vertex_shader;
  std::string m_fragment_shader;
  VertexLayout m_vertex_layout;
  VkPrimitiveTopology m_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  bool m_depth_test = true;
  bool m_depth_write = true;
  // SRC_ALPHA / ONE_MINUS_SRC_ALPHA.
  bool m_blend = true;
  // VK_POLYGON_MODE_LINE falls back to FILL where the device lacks
  // fillModeNonSolid.
  VkPolygonMode m_polygon_mode = VK_POLYGON_MODE_FILL;
};

// A framebuffer rectangle, origin at the top-left.
struct Rect {
  std::int32_t m_x;
  std::int32_t m_y;
  std::int32_t m_width;
  std::int32_t m_height;

  // The same rectangle given with its origin at the bottom-left (OpenGL's
  // window convention), in a framebuffer framebuffer_height tall.
  static Rect fromBottomLeft(std::int32_t x,
      std::int32_t y,
      std::int32_t width,
      std::int32_t height,
      std::int32_t framebuffer_height);
};

class RenderContext {
public:
  explicit RenderContext(Renderer& renderer);

  VkCommandBuffer commandBuffer() const;
  VkExtent2D extent() const;

  // Viewport and the matching scissor.
  void setViewport(const Rect& rect);
  // The scissor alone (the viewport is unchanged).
  void setScissor(const Rect& rect);
  const Rect& viewport() const;

  // Every subsequent draw's mvp is projection * view * model.
  void setCamera(
      const Math::Mat4<float>& projection, const Math::Mat4<float>& view);
  const Math::Mat4<float>& projection() const;
  const Math::Mat4<float>& view() const;

  // When off, later draws neither test nor write depth (overlays).
  void setDepthTest(bool enabled);
  bool depthTest() const;

  // Clears depth, or color and depth, within the current scissor.
  void clearDepth();
  void clearColorAndDepth(const Math::Vec4<float>& color);

  // A UiMesh, through the Renderer's UI pipelines (setUiPipelines()).
  void draw(
      UiMesh& mesh, const Math::Mat4<float>& model = Math::Mat4<float>(1.0f));
  // Any RetainedMesh, its triangles and lines through the pipelines given.
  template <typename V>
  void draw(RetainedMesh<V>& mesh,
      PipelineHandle triangle_pipeline,
      PipelineHandle line_pipeline,
      const Math::Mat4<float>& model = Math::Mat4<float>(1.0f)) {
    drawRetained(mesh.upload(),
        mesh,
        static_cast<std::uint32_t>(mesh.triangles().size()),
        static_cast<std::uint32_t>(mesh.lines().size()),
        triangle_pipeline,
        line_pipeline,
        model);
  }
  // One frame's geometry, through the transient ring.
  template <typename V>
  void drawTransient(const std::vector<V>& vertices,
      PipelineHandle pipeline,
      const Texture* texture,
      const Math::Mat4<float>& model = Math::Mat4<float>(1.0f),
      const Math::Vec4<float>& params = Math::Vec4<float>(0.0f),
      float line_width = 1.0f) {
    drawTransientBytes(vertices.data(),
        vertices.size() * sizeof(V),
        static_cast<std::uint32_t>(vertices.size()),
        pipeline,
        texture,
        model,
        params,
        line_width);
  }
  void drawMesh(const Mesh& mesh,
      PipelineHandle pipeline,
      const Texture* texture,
      const Math::Mat4<float>& model,
      const Math::Vec4<float>& params);
  // Text at a raster position, the way OpenGL's glRasterPos() +
  // glutBitmapCharacter() placed it: raster_position (under model) is
  // projected to the window; the text's baseline starts at that pixel,
  // advancing in window pixels, drawn at its depth - and nothing is drawn
  // if the position falls outside the view volume. Uses the Renderer's
  // text pipeline (setTextPipeline()).
  void drawText(const Font& font,
      const Math::Vec3<float>& raster_position,
      std::string_view text,
      const Math::Vec4<float>& color,
      const Math::Mat4<float>& model = Math::Mat4<float>(1.0f));

private:
  friend class Renderer;
  void begin(VkCommandBuffer command_buffer, VkExtent2D extent);
  void bindPipeline(PipelineHandle pipeline);
  void bindTexture(const Texture* texture);
  void pushConstants(
      const Math::Mat4<float>& model, const Math::Vec4<float>& params);
  void applyViewport(const Rect& rect);
  VkRect2D toVkRect(const Rect& rect) const;
  void drawRetained(const HostBuffer& buffer,
      const RetainedMeshBase& mesh,
      std::uint32_t triangle_count,
      std::uint32_t line_count,
      PipelineHandle triangle_pipeline,
      PipelineHandle line_pipeline,
      const Math::Mat4<float>& model);
  void drawTransientBytes(const void* data,
      std::size_t byte_count,
      std::uint32_t vertex_count,
      PipelineHandle pipeline,
      const Texture* texture,
      const Math::Mat4<float>& model,
      const Math::Vec4<float>& params,
      float line_width);

  Renderer& m_renderer;
  VkCommandBuffer m_command_buffer = VK_NULL_HANDLE;
  VkExtent2D m_extent = {0, 0};
  Rect m_viewport = {0, 0, 0, 0};
  Rect m_scissor = {0, 0, 0, 0};
  Math::Mat4<float> m_projection = Math::Mat4<float>(1.0f);
  Math::Mat4<float> m_view = Math::Mat4<float>(1.0f);
  VkPipeline m_bound_pipeline = VK_NULL_HANDLE;
  bool m_depth_test = true;
  VkDescriptorSet m_bound_descriptor_set = VK_NULL_HANDLE;
};

class Renderer {
public:
  static constexpr std::uint32_t c_frames_in_flight = 2;

  // The process-wide renderer, set while one is initialized.
  static Renderer& instance();
  static bool hasInstance();

  Renderer();
  ~Renderer();

  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  // Renders into base's device and swapchain (base must have finished
  // prepareVulkan()).
  bool initialize(const TutorialBase& base);
  // Frees everything tied to the old swapchain's images (framebuffers,
  // depth buffer) - call before the swapchain is rebuilt (TutorialBase::
  // childClear()).
  void releaseSwapchainResources();
  // After base rebuilt its swapchain (TutorialBase::
  // childOnWindowSizeChanged()).
  bool onSwapchainRecreated(const TutorialBase& base);
  void shutdown();

  // Both depth variants of description; nullopt if either fails.
  std::optional<PipelineHandle> createPipeline(
      const PipelineDescription& description);
  // The pipelines draw(UiMesh&) uses.
  void setUiPipelines(PipelineHandle triangles, PipelineHandle lines);
  // The pipeline drawText() uses: UiVertex, sampling the glyph atlas's
  // alpha.
  void setTextPipeline(PipelineHandle text);

  // Waits for this frame slot, acquires a swapchain image, and begins
  // the render pass with color cleared to clear_color and depth to 1.
  // Returns nullptr if the swapchain must be recreated first.
  RenderContext* beginFrame(const Math::Vec4<float>& clear_color);
  // Ends and submits the frame and presents it. Returns false if the
  // swapchain must be recreated.
  bool endFrame();

  // The next completed frame is written to path as a binary PPM.
  void requestCapture(std::string path);

  VkExtent2D extent() const;
  std::uint64_t frameNumber() const;

  // RGBA8 pixels, width * height * 4 bytes. CLAMP_TO_BORDER samples an
  // opaque black border (OpenGL's GL_CLAMP on an RGB texture).
  std::shared_ptr<Texture> createTexture(const std::vector<char>& pixels,
      std::uint32_t width,
      std::uint32_t height,
      VkSamplerAddressMode address_mode);
  // Headerless RGB .raw images (Tools::getRawImageData()), loaded once
  // per (filename, address mode) and shared.
  std::shared_ptr<Texture> loadRawTexture(const std::string& filename,
      std::uint32_t width,
      std::uint32_t height,
      VkSamplerAddressMode address_mode);
  // .jpg/.png images (Tools::getImageData()), shared the same way.
  std::shared_ptr<Texture> loadImageTexture(
      const std::string& filename, VkSamplerAddressMode address_mode);
  const Texture& whiteTexture() const;
  // A TrueType font baked at pixel_height, its atlas uploaded.
  std::unique_ptr<Font> loadFont(
      const std::string& font_path, float pixel_height);

  template <typename V>
  std::unique_ptr<Mesh> createMesh(const std::vector<V>& vertices) {
    return createMeshFromBytes(vertices.data(),
        vertices.size() * sizeof(V),
        static_cast<std::uint32_t>(vertices.size()));
  }
  template <typename... Ts>
  std::unique_ptr<Mesh> createMesh(
      const VertexTypes::InterleavedData<Ts...>& data) {
    const std::vector<std::byte> bytes = packInterleaved(data);
    return createMeshFromBytes(bytes.data(),
        bytes.size(),
        static_cast<std::uint32_t>(data.getAttributeCount()));
  }
  // A unit sphere of MeshVertex in GLUT's glutSolidSphere(1, slices,
  // stacks) tessellation and triangle order (top fan, stacks top to
  // bottom, bottom fan), created once per (slices, stacks). Draw it
  // scaled by the radius.
  const Mesh& sphere(std::uint32_t slices, std::uint32_t stacks);

  // Host-visible, coherent, persistently mapped.
  HostBuffer createHostBuffer(VkDeviceSize size, VkBufferUsageFlags usage);
  // Frees buffer / image (+ its descriptor set) once no in-flight frame
  // can still be using it.
  void deferRelease(BufferParameters buffer);
  void deferRelease(ImageParameters image, VkDescriptorSet descriptor_set);

private:
  friend class RenderContext;

  struct FrameSlot {
    VkCommandBuffer m_command_buffer = VK_NULL_HANDLE;
    VkSemaphore m_image_available = VK_NULL_HANDLE;
    VkFence m_in_flight = VK_NULL_HANDLE;
    HostBuffer m_transient;
    VkDeviceSize m_transient_offset = 0;
    std::vector<std::function<void()>> m_releases;
  };

  struct DeviceInfo {
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    VkQueue m_graphics_queue = VK_NULL_HANDLE;
    std::uint32_t m_graphics_family = 0;
    VkQueue m_present_queue = VK_NULL_HANDLE;
  };

  struct SwapchainInfo {
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkFormat m_format = VK_FORMAT_UNDEFINED;
    VkExtent2D m_extent = {0, 0};
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_views;
  };

  static SwapchainInfo swapchainInfo(const TutorialBase& base);

  bool createRenderPass();
  bool createSwapchainResources();
  void destroySwapchainResources();
  bool createPipelineVariant(const PipelineDescription& description,
      bool depth_test,
      VkPipeline* out);
  bool createDescriptorResources();
  bool createFrameSlots();
  void runReleases(FrameSlot& slot);
  VkPipeline pipeline(PipelineHandle handle, bool depth_test) const;
  VkPipelineLayout pipelineLayout() const;
  float clampLineWidth(float width) const;
  // Copies size bytes into the current frame's transient ring; false when
  // the ring is full.
  bool allocateTransient(const void* data,
      VkDeviceSize size,
      VkBuffer* buffer,
      VkDeviceSize* offset);
  bool writeCapture();
  VkDescriptorSet allocateTextureDescriptor(
      VkImageView view, VkSampler sampler);
  std::unique_ptr<Mesh> createMeshFromBytes(
      const void* data, std::size_t byte_count, std::uint32_t vertex_count);

  static Renderer* s_instance;

  DeviceInfo m_device;
  SwapchainInfo m_swapchain;
  VkRenderPass m_render_pass = VK_NULL_HANDLE;
  ImageParameters m_depth_image;
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
  // [handle] = {depth-tested variant, depth-off variant}.
  std::vector<std::array<VkPipeline, 2>> m_pipelines;
  std::optional<PipelineHandle> m_ui_triangle_pipeline;
  std::optional<PipelineHandle> m_ui_line_pipeline;
  std::optional<PipelineHandle> m_text_pipeline;
  bool m_wireframe_supported = false;
  std::array<float, 2> m_line_width_range = {1.0f, 1.0f};
  std::shared_ptr<Texture> m_white_texture;
  std::map<std::string, std::weak_ptr<Texture>> m_texture_cache;
  std::map<std::pair<std::uint32_t, std::uint32_t>, std::unique_ptr<Mesh>>
      m_spheres;
  RenderContext m_context;
  std::string m_capture_path;
  HostBuffer m_capture_buffer;
};

}  // namespace vulkan_graphix::Render

#endif  // VULKAN_GRAPHIX_RENDER_RENDERER_H
