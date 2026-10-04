#ifndef VULKAN_EARTH_GAME_RENDERER_H
#define VULKAN_EARTH_GAME_RENDERER_H

// What the game sets up on top of libvulkan_graphix's Render module: its
// own pipelines (its shaders in resources/vulkan_earth/Shaders/), its two
// fonts, and the window/viewport conventions its drawing code was written
// against (OpenGL's: bottom-left-origin viewports, a y-up menu plane).

#include <cstdint>

#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Renderer.h"

namespace vulkan_earth {

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

// The game's two text faces: its proportional serif text and the smaller
// fixed-width text it switches to on narrow windows.
enum class FontId : std::uint8_t {
    TimesRoman24,
    Fixed9By15,
};

struct Pipelines {
    render::PipelineHandle ui_triangles;
    render::PipelineHandle ui_lines;
    render::PipelineHandle text;
    render::PipelineHandle mesh;
    // mesh, without depth testing or writing (the skybox).
    render::PipelineHandle mesh_background;
    render::PipelineHandle terrain;
    render::PipelineHandle water;
    render::PipelineHandle flat_color;
    // flat_color rasterized as lines (the terrain's wireframe view).
    render::PipelineHandle flat_color_wireframe;
};

// Creates the game's pipelines and fonts on renderer (after it is
// initialized); releaseGameRendering() frees them before it shuts down.
bool initializeGameRendering(render::Renderer& renderer);
void releaseGameRendering();

Pipelines const& pipelines();
render::Font const& font(FontId id);
// The pen advance of character in font id, in whole pixels.
std::int32_t textAdvance(FontId id, char character);

// The window (swapchain) size.
std::int32_t windowWidth();
std::int32_t windowHeight();
// A viewport rectangle given with its origin at the window's bottom-left.
render::Rect glRect(std::int32_t x,
                    std::int32_t y,
                    std::int32_t width,
                    std::int32_t height);

// The full window again, with the menus' perspective (60 degrees, near 1,
// far 1000000) and an identity view: how the game leaves things after each
// sub-viewport it draws (the minimap, help, inventory, tank preview).
void resetToFullWindow(render::RenderContext& context);
// A gray overlay panel's own viewport (the help manual and inventory):
// viewport cleared to (0.75, 0.75, 0.75, 1), seen through a 60-degree
// perspective of aspect width / (1.5 * height), from window height / 4 *
// tan(60 degrees) away.
void beginOverlayPanel(render::RenderContext& context,
                       render::Rect const& viewport,
                       std::int32_t width,
                       std::int32_t height);

// The game's beveled shapes, in its y-up menu plane at depth z, built by
// UiGeometry::buildBevelFrame()/buildButtonBevel():
// - a raised or pressed button bevel: top_left is (x, y), the face
//   extending width right and height down;
void appendBevel(render::UiMesh& mesh,
                 float x,
                 float y,
                 float z,
                 float width,
                 float height,
                 math::Vec4<float> const& color,
                 bool pressed,
                 float bevel_size = 3.0f);
// - the same frame with explicit colors (top and left wedges in top_left,
//   bottom and right in bottom_right);
void appendFrame(render::UiMesh& mesh,
                 float x,
                 float y,
                 float z,
                 float width,
                 float height,
                 math::Vec4<float> const& top_left,
                 math::Vec4<float> const& face,
                 math::Vec4<float> const& bottom_right,
                 float border = 3.0f);
// - the whole-window background panel MainMenu, ReadyMenu, and ShopMenu
//   open with: centered on the origin at z = 0, a percent_border * height
//   border in five shades of gray (left 0.85, top 0.80, face 0.75, bottom
//   0.45, right 0.40).
void appendMenuPanel(render::UiMesh& mesh,
                     float width,
                     float height,
                     float percent_border);

}  // namespace vulkan_earth

#endif  // VULKAN_EARTH_GAME_RENDERER_H
