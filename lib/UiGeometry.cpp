#include "vulkan_graphix/UiGeometry.h"

namespace vulkan_graphix::UiGeometry {

std::vector<ColoredQuad> buildBevelFrame(
    Math::Vec2<float> top_left,
    Math::Vec2<float> size,
    const BevelColors& colors,
    float bevel_size) {
  const float x0 = top_left.x;
  const float y0 = top_left.y;
  const float x1 = top_left.x + size.x;
  const float y1 = top_left.y + size.y;
  const float b = bevel_size;

  std::vector<ColoredQuad> quads;
  quads.reserve(5);

  // Flat center face.
  quads.push_back(
      {{{Math::Vec2<float>(x0, y0),
         Math::Vec2<float>(x0, y1),
         Math::Vec2<float>(x1, y1),
         Math::Vec2<float>(x1, y0)}},
       colors.m_face});

  // Top border wedge.
  quads.push_back(
      {{{Math::Vec2<float>(x0, y0),
         Math::Vec2<float>(x0 - b, y0 - b),
         Math::Vec2<float>(x1 + b, y0 - b),
         Math::Vec2<float>(x1, y0)}},
       colors.m_top});

  // Left border wedge.
  quads.push_back(
      {{{Math::Vec2<float>(x0 - b, y0 - b),
         Math::Vec2<float>(x0 - b, y1 + b),
         Math::Vec2<float>(x0, y1),
         Math::Vec2<float>(x0, y0)}},
       colors.m_left});

  // Bottom border wedge.
  quads.push_back(
      {{{Math::Vec2<float>(x0 - b, y1 + b),
         Math::Vec2<float>(x1 + b, y1 + b),
         Math::Vec2<float>(x1, y1),
         Math::Vec2<float>(x0, y1)}},
       colors.m_bottom});

  // Right border wedge.
  quads.push_back(
      {{{Math::Vec2<float>(x1, y0),
         Math::Vec2<float>(x1 + b, y0 - b),
         Math::Vec2<float>(x1 + b, y1 + b),
         Math::Vec2<float>(x1, y1)}},
       colors.m_right});

  return quads;
}

std::vector<ColoredQuad> buildButtonBevel(
    Math::Vec2<float> top_left,
    Math::Vec2<float> size,
    Math::Vec4<float> base_color,
    bool pressed,
    float bevel_size) {
  const Math::Vec4<float> light(
      base_color.r + 0.2f,
      base_color.g + 0.2f,
      base_color.b + 0.2f,
      base_color.a);
  const Math::Vec4<float> dark(
      base_color.r - 0.4f,
      base_color.g - 0.4f,
      base_color.b - 0.4f,
      base_color.a);
  // Raised: light catches the top/left edges, shadow falls on
  // bottom/right. Pressed swaps the two, simulating the face sinking.
  const Math::Vec4<float> top_left_color = pressed ? dark : light;
  const Math::Vec4<float> bottom_right_color = pressed ? light : dark;
  return buildBevelFrame(
      top_left,
      size,
      {base_color,
       top_left_color,
       top_left_color,
       bottom_right_color,
       bottom_right_color},
      bevel_size);
}

Math::Vec2<float> GridLayout::cellSize() const {
  const float cell_width =
      (m_size.x - static_cast<float>(m_columns - 1) * m_gap) /
      static_cast<float>(m_columns);
  const float cell_height =
      (m_size.y - static_cast<float>(m_rows - 1) * m_gap) /
      static_cast<float>(m_rows);
  return Math::Vec2<float>(cell_width, cell_height);
}

Math::Vec2<float> GridLayout::cellTopLeft(std::size_t index) const {
  const Math::Vec2<float> cell_size = cellSize();
  const std::uint32_t column = static_cast<std::uint32_t>(index) % m_columns;
  const std::uint32_t grid_row = static_cast<std::uint32_t>(index) / m_columns;
  return Math::Vec2<float>(
      m_top_left.x + static_cast<float>(column) * (cell_size.x + m_gap),
      m_top_left.y + static_cast<float>(grid_row) * (cell_size.y + m_gap));
}

Math::Vec4<float> healthBarColor(float ratio) {
  return Math::Vec4<float>(1.0f - ratio, ratio, 0.0f, 1.0f);
}

Math::Vec4<float> powerBarColor(float ratio) {
  return Math::Vec4<float>(ratio, 1.0f - ratio, 0.0f, 1.0f);
}

}  // namespace vulkan_graphix::UiGeometry
