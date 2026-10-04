#include "vulkan_graphix/UiGeometry.h"

namespace vulkan_graphix::UiGeometry {

std::vector<ColoredQuad> buildBevelFrame(Math::Vec2<float> top_left,
                                         Math::Vec2<float> size,
                                         BevelColors const& colors,
                                         float bevel_size) {
    float const x0 = top_left.x;
    float const y0 = top_left.y;
    float const x1 = top_left.x + size.x;
    float const y1 = top_left.y + size.y;
    float const b = bevel_size;

    std::vector<ColoredQuad> quads;
    quads.reserve(5);

    // Flat center face.
    quads.push_back({{{Math::Vec2<float>(x0, y0),
                       Math::Vec2<float>(x0, y1),
                       Math::Vec2<float>(x1, y1),
                       Math::Vec2<float>(x1, y0)}},
                     colors.face});

    // Top border wedge.
    quads.push_back({{{Math::Vec2<float>(x0, y0),
                       Math::Vec2<float>(x0 - b, y0 - b),
                       Math::Vec2<float>(x1 + b, y0 - b),
                       Math::Vec2<float>(x1, y0)}},
                     colors.top});

    // Left border wedge.
    quads.push_back({{{Math::Vec2<float>(x0 - b, y0 - b),
                       Math::Vec2<float>(x0 - b, y1 + b),
                       Math::Vec2<float>(x0, y1),
                       Math::Vec2<float>(x0, y0)}},
                     colors.left});

    // Bottom border wedge.
    quads.push_back({{{Math::Vec2<float>(x0 - b, y1 + b),
                       Math::Vec2<float>(x1 + b, y1 + b),
                       Math::Vec2<float>(x1, y1),
                       Math::Vec2<float>(x0, y1)}},
                     colors.bottom});

    // Right border wedge.
    quads.push_back({{{Math::Vec2<float>(x1, y0),
                       Math::Vec2<float>(x1 + b, y0 - b),
                       Math::Vec2<float>(x1 + b, y1 + b),
                       Math::Vec2<float>(x1, y1)}},
                     colors.right});

    return quads;
}

std::vector<ColoredQuad> buildButtonBevel(Math::Vec2<float> top_left,
                                          Math::Vec2<float> size,
                                          Math::Vec4<float> base_color,
                                          bool pressed,
                                          float bevel_size) {
    Math::Vec4<float> const light(base_color.r + 0.2f,
                                  base_color.g + 0.2f,
                                  base_color.b + 0.2f,
                                  base_color.a);
    Math::Vec4<float> const dark(base_color.r - 0.4f,
                                 base_color.g - 0.4f,
                                 base_color.b - 0.4f,
                                 base_color.a);
    // Raised: light catches the top/left edges, shadow falls on
    // bottom/right. Pressed swaps the two, simulating the face sinking.
    Math::Vec4<float> const top_left_color = pressed ? dark : light;
    Math::Vec4<float> const bottom_right_color = pressed ? light : dark;
    return buildBevelFrame(top_left,
                           size,
                           {base_color,
                            top_left_color,
                            top_left_color,
                            bottom_right_color,
                            bottom_right_color},
                           bevel_size);
}

}  // namespace vulkan_graphix::UiGeometry
