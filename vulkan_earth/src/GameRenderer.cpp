#include "vulkan_earth/GameRenderer.h"

#include <cmath>
#include <cstdio>
#include <memory>
#include <optional>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "vulkan_graphix/Tools.h"
#include "vulkan_graphix/UiGeometry.h"

namespace vulkan_earth {

namespace {
using Vec2 = math::Vec2<float>;
using Vec3 = math::Vec3<float>;
using Vec4 = math::Vec4<float>;
using Mat4 = math::Mat4<float>;

constexpr const char* c_serif_font_path =
        "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf";
constexpr const char* c_mono_font_path =
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf";
// Sized like GLUT's TIMES_ROMAN_24 and 9_BY_15 bitmap fonts were (the
// mono face advances 9 pixels per glyph, as 9_BY_15 did).
constexpr float c_serif_pixel_height = 24.0f;
constexpr float c_mono_pixel_height = 15.0f;

Pipelines g_pipelines{};
std::unique_ptr<render::Font> g_serif_font;
std::unique_ptr<render::Font> g_mono_font;

std::optional<render::PipelineHandle> createPipeline(
        render::Renderer& renderer,
        const char* shader,
        const render::VertexLayout& layout,
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        bool depth = true,
        VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL) {
    render::PipelineDescription description;
    description.m_vertex_shader =
            std::string("shaders/") + shader + ".vert.spv";
    description.m_fragment_shader =
            std::string("shaders/") + shader + ".frag.spv";
    description.m_vertex_layout = layout;
    description.m_topology = topology;
    description.m_depth_test = depth;
    description.m_depth_write = depth;
    description.m_polygon_mode = polygon_mode;
    return renderer.createPipeline(description);
}

// buildBevelFrame()'s y-down quads, mirrored into the game's y-up plane at
// depth z.
void appendQuads(
        render::UiMesh& mesh,
        const std::vector<vulkan_graphix::UiGeometry::ColoredQuad>& quads,
        float z) {
    for (const auto& quad : quads) {
        std::array<Vec3, 4> corners;
        for (std::size_t i = 0; i < corners.size(); ++i) {
            corners[i] = Vec3(quad.m_corners[i].x, -quad.m_corners[i].y, z);
        }
        mesh.addQuad(corners, quad.m_color);
    }
}
}  // namespace

bool initializeGameRendering(render::Renderer& renderer) {
    const render::VertexLayout ui_layout =
            render::vertexLayout<render::UiVertex>();
    const render::VertexLayout mesh_layout =
            render::vertexLayout<render::MeshVertex>();
    auto ui_triangles = createPipeline(renderer, "ui", ui_layout);
    auto ui_lines = createPipeline(
            renderer, "ui", ui_layout, VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
    auto text = createPipeline(renderer, "text", ui_layout);
    auto mesh_pipeline = createPipeline(renderer, "mesh", mesh_layout);
    auto mesh_background = createPipeline(renderer,
                                          "mesh",
                                          mesh_layout,
                                          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                          false);
    auto terrain = createPipeline(renderer, "terrain", mesh_layout);
    auto water = createPipeline(renderer, "water", mesh_layout);
    auto flat_color = createPipeline(renderer, "flat", mesh_layout);
    auto flat_color_wireframe =
            createPipeline(renderer,
                           "flat",
                           mesh_layout,
                           VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                           true,
                           VK_POLYGON_MODE_LINE);
    if (!ui_triangles || !ui_lines || !text || !mesh_pipeline ||
        !mesh_background || !terrain || !water || !flat_color ||
        !flat_color_wireframe) {
        return false;
    }
    g_pipelines = {*ui_triangles,
                   *ui_lines,
                   *text,
                   *mesh_pipeline,
                   *mesh_background,
                   *terrain,
                   *water,
                   *flat_color,
                   *flat_color_wireframe};
    renderer.setUiPipelines(g_pipelines.m_ui_triangles,
                            g_pipelines.m_ui_lines);
    renderer.setTextPipeline(g_pipelines.m_text);

    g_serif_font = renderer.loadFont(c_serif_font_path, c_serif_pixel_height);
    g_mono_font = renderer.loadFont(c_mono_font_path, c_mono_pixel_height);
    if (!g_serif_font || !g_mono_font) {
        std::fprintf(stderr,
                     "vulkan_earth: is the fonts-dejavu-core package "
                     "installed?\n");
        return false;
    }
    return true;
}

void releaseGameRendering() {
    g_serif_font.reset();
    g_mono_font.reset();
}

const Pipelines& pipelines() { return g_pipelines; }

const render::Font& font(FontId id) {
    return id == FontId::Fixed9By15 ? *g_mono_font : *g_serif_font;
}

std::int32_t textAdvance(FontId id, char character) {
    return static_cast<std::int32_t>(
            std::lround(font(id).textWidth(std::string(1, character))));
}

std::int32_t windowWidth() {
    return static_cast<std::int32_t>(
            render::Renderer::instance().extent().width);
}

std::int32_t windowHeight() {
    return static_cast<std::int32_t>(
            render::Renderer::instance().extent().height);
}

render::Rect glRect(std::int32_t x,
                    std::int32_t y,
                    std::int32_t width,
                    std::int32_t height) {
    return render::Rect::fromBottomLeft(x, y, width, height, windowHeight());
}

void resetToFullWindow(render::RenderContext& context) {
    std::int32_t win_width = windowWidth();
    std::int32_t win_height = windowHeight();
    context.setViewport({0, 0, win_width, win_height});
    context.setCamera(vulkan_graphix::Tools::getPerspectiveProjectionMatrix(
                              static_cast<float>(win_width) /
                                      static_cast<float>(win_height),
                              60.0,
                              1.0,
                              1000000.0),
                      Mat4(1.0f));
}

void beginOverlayPanel(render::RenderContext& context,
                       const render::Rect& viewport,
                       std::int32_t width,
                       std::int32_t height) {
    context.setViewport(viewport);
    context.clearColorAndDepth(Vec4(0.75, 0.75, 0.75, 1));
    std::int32_t distance =
            static_cast<std::int32_t>(windowHeight() / 4 * tan(1.04719755));
    context.setCamera(vulkan_graphix::Tools::getPerspectiveProjectionMatrix(
                              (static_cast<float>(width) /
                               (1.5 * static_cast<float>(height))),
                              60.0,
                              1,
                              2.0e8f),
                      glm::lookAt(Vec3(0, 0, distance),
                                  Vec3(0, 0, 0),
                                  Vec3(0.0f, 1.0f, 0.0f)));
}

void appendBevel(render::UiMesh& mesh,
                 float x,
                 float y,
                 float z,
                 float width,
                 float height,
                 const Vec4& color,
                 bool pressed,
                 float bevel_size) {
    appendQuads(
            mesh,
            vulkan_graphix::UiGeometry::buildButtonBevel(Vec2(x, -y),
                                                         Vec2(width, height),
                                                         color,
                                                         pressed,
                                                         bevel_size),
            z);
}

void appendFrame(render::UiMesh& mesh,
                 float x,
                 float y,
                 float z,
                 float width,
                 float height,
                 const Vec4& top_left,
                 const Vec4& face,
                 const Vec4& bottom_right,
                 float border) {
    appendQuads(mesh,
                vulkan_graphix::UiGeometry::buildBevelFrame(
                        Vec2(x, -y),
                        Vec2(width, height),
                        {face, top_left, top_left, bottom_right, bottom_right},
                        border),
                z);
}

void appendMenuPanel(render::UiMesh& mesh,
                     float width,
                     float height,
                     float percent_border) {
    const float b = percent_border * (height);
    const float left = -1 * (width / 2.0);
    const float top_edge = (height / 2.0);
    appendQuads(mesh,
                vulkan_graphix::UiGeometry::buildBevelFrame(
                        Vec2(left + b, -(top_edge - b)),
                        Vec2(width - 2 * b, height - 2 * b),
                        {Vec4(0.75f, 0.75f, 0.75f, 1.0f),
                         Vec4(0.80f, 0.80f, 0.80f, 1.0f),
                         Vec4(0.85f, 0.85f, 0.85f, 1.0f),
                         Vec4(0.45f, 0.45f, 0.45f, 1.0f),
                         Vec4(0.40f, 0.40f, 0.40f, 1.0f)},
                        b),
                0.0f);
}

}  // namespace vulkan_earth
