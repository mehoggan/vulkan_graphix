#ifndef VULKAN_GRAPHIX_UIGEOMETRY_H
#define VULKAN_GRAPHIX_UIGEOMETRY_H

// Reusable 2D UI-quad geometry, ported from vulkan_earth's
// ControlItemButton::draw() (see vulkan_earth/src/ControlItemButton.cpp)
// - a flat center quad plus four shaded border wedges forming a classic
// raised/pressed 3D button bevel. Pure geometry, no Vulkan/rendering
// coupling: a caller renders each ColoredQuad however it renders 2D
// quads (Tutorial15 samples BitmapFont::solidTexelUv() through its own
// glyph-atlas pipeline, tinted by the quad's color).
//
// The append*() templates below turn that geometry (plus BitmapFont
// glyph quads and textured image quads) into two-triangle vertex lists,
// shared by every 2D UI tutorial (15/17/18/19/22) and any future
// vulkan_earth UI port. VertexData is any aggregate laid out as
// {Math::Vec4<float> position; Math::Vec2<float> texcoord;
// Math::Vec4<float> color;} - each tutorial keeps its own named vertex
// struct for its own AttributeTraits, all with that same shape.

#include <array>
#include <cstddef>
#include <string>
#include <vector>

#include "vulkan_graphix/BitmapFont.h"
#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::UiGeometry {

// Four corners in the same winding vulkan_earth's own GL_QUADS calls
// used (top-left, then counter-clockwise-in-screen-space) - not
// necessarily an axis-aligned rectangle, since the bevel's four border
// wedges are trapezoids, not rectangles.
struct ColoredQuad {
    std::array<Math::Vec2<float>, 4> m_corners;
    Math::Vec4<float> m_color;
};

// The five quads of a beveled frame, each in its own color.
struct BevelColors {
    Math::Vec4<float> m_face;
    Math::Vec4<float> m_top;
    Math::Vec4<float> m_left;
    Math::Vec4<float> m_bottom;
    Math::Vec4<float> m_right;
};

// top_left/size describe the flat face in a top-left-origin, y-down
// convention; the four border wedges extend bevel_size outward from it.
// Always returns 5 quads: [0] = flat center, [1..4] = top/left/bottom/right
// border wedges.
std::vector<ColoredQuad> buildBevelFrame(Math::Vec2<float> top_left,
  Math::Vec2<float> size,
  const BevelColors& colors,
  float bevel_size);

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

// Appends one glyph as two triangles, tinted by color.
template <typename VertexData>
void appendGlyphQuad(std::vector<VertexData>& vertex_data,
  const BitmapFontGlyphQuad& glyph,
  Math::Vec4<float> color) {
    const Math::Vec2<float>& top_left = glyph.m_top_left;
    const Math::Vec2<float>& bottom_right = glyph.m_bottom_right;
    const Math::Vec2<float>& uv_top_left = glyph.m_uv_top_left;
    const Math::Vec2<float>& uv_bottom_right = glyph.m_uv_bottom_right;

    const VertexData top_left_vertex{
      Math::Vec4<float>(top_left.x, top_left.y, 0.0f, 1.0f),
      uv_top_left,
      color};
    const VertexData bottom_left_vertex{
      Math::Vec4<float>(top_left.x, bottom_right.y, 0.0f, 1.0f),
      Math::Vec2<float>(uv_top_left.x, uv_bottom_right.y),
      color};
    const VertexData bottom_right_vertex{
      Math::Vec4<float>(bottom_right.x, bottom_right.y, 0.0f, 1.0f),
      uv_bottom_right,
      color};
    const VertexData top_right_vertex{
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

// Appends a flat-color quad (e.g. one of buildButtonBevel()'s
// ColoredQuads) as two triangles, every vertex sampling solid_uv - pass
// BitmapFont::solidTexelUv() to draw it through a glyph-atlas pipeline.
template <typename VertexData>
void appendColoredQuad(std::vector<VertexData>& vertex_data,
  const std::array<Math::Vec2<float>, 4>& corners,
  Math::Vec4<float> color,
  Math::Vec2<float> solid_uv) {
    std::array<VertexData, 4> quad_vertices;
    for (std::size_t i = 0; i < corners.size(); ++i) {
        quad_vertices[i] =
          VertexData{Math::Vec4<float>(corners[i].x, corners[i].y, 0.0f, 1.0f),
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

// Lays out text via font.layoutText() and appends every glyph quad.
template <typename VertexData>
void appendText(std::vector<VertexData>& vertex_data,
  const BitmapFont& font,
  const std::string& text,
  Math::Vec2<float> origin,
  Math::Vec4<float> color) {
    const std::vector<BitmapFontGlyphQuad> glyphs =
      font.layoutText(text, origin);
    for (const BitmapFontGlyphQuad& glyph : glyphs) {
        appendGlyphQuad(vertex_data, glyph, color);
    }
}

// Appends an untinted (white) textured quad spanning uv_min..uv_max -
// e.g. one icon out of an atlas.
template <typename VertexData>
void appendImageQuad(std::vector<VertexData>& vertex_data,
  Math::Vec2<float> top_left,
  Math::Vec2<float> size,
  Math::Vec2<float> uv_min,
  Math::Vec2<float> uv_max) {
    const Math::Vec4<float> white(1.0f, 1.0f, 1.0f, 1.0f);
    const Math::Vec2<float> bottom_right(
      top_left.x + size.x, top_left.y + size.y);

    const VertexData top_left_vertex{
      Math::Vec4<float>(top_left.x, top_left.y, 0.0f, 1.0f), uv_min, white};
    const VertexData bottom_left_vertex{
      Math::Vec4<float>(top_left.x, bottom_right.y, 0.0f, 1.0f),
      Math::Vec2<float>(uv_min.x, uv_max.y),
      white};
    const VertexData bottom_right_vertex{
      Math::Vec4<float>(bottom_right.x, bottom_right.y, 0.0f, 1.0f),
      uv_max,
      white};
    const VertexData top_right_vertex{
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

}  // namespace vulkan_graphix::UiGeometry

#endif  // VULKAN_GRAPHIX_UIGEOMETRY_H
