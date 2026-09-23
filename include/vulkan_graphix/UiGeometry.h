#ifndef VULKAN_GRAPHIX_UI_GEOMETRY_H
#define VULKAN_GRAPHIX_UI_GEOMETRY_H

// Reusable 2D UI-quad geometry, ported from vulkan_earth's
// ControlItemButton::draw() (see vulkan_earth/src/ControlItemButton.cpp)
// - a flat center quad plus four shaded border wedges forming a classic
// raised/pressed 3D button bevel. Pure geometry, no Vulkan/rendering
// coupling: a caller renders each ColoredQuad however it renders 2D
// quads (Tutorial15 samples BitmapFont::solidTexelUv() through its own
// glyph-atlas pipeline, tinted by the quad's color).

#include <array>
#include <vector>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::UiGeometry {

// Four corners in the same winding vulkan_earth's own GL_QUADS calls
// used (top-left, then counter-clockwise-in-screen-space) - not
// necessarily an axis-aligned rectangle, since the bevel's four border
// wedges are trapezoids, not rectangles.
struct ColoredQuad {
    std::array<Math::Vec2<float>, 4> corners;
    Math::Vec4<float> color;
};

// top_left/size describe the button's flat face in a top-left-origin,
// y-down screen convention (y grows downward). Colors are offset from
// base_color by the same -0.4/+0.2 vulkan_earth's ControlItemButton::
// draw() uses; which edges get which offset flips between pressed and
// raised, simulating the face sinking in on press. Always returns 5
// quads: [0] = flat center, [1..4] = top/left/bottom/right border
// wedges.
std::vector<ColoredQuad> buildButtonBevel(Math::Vec2<float> top_left,
                                          Math::Vec2<float> size,
                                          Math::Vec4<float> base_color,
                                          bool pressed,
                                          float bevel_size = 3.0f);

}  // namespace vulkan_graphix::UiGeometry

#endif  // VULKAN_GRAPHIX_UI_GEOMETRY_H
