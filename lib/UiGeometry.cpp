#include "vulkan_graphix/UiGeometry.h"

namespace vulkan_graphix::UiGeometry {

std::vector<ColoredQuad> buildButtonBevel(Math::Vec2<float> top_left,
                                          Math::Vec2<float> size,
                                          Math::Vec4<float> base_color,
                                          bool pressed,
                                          float bevel_size) {
    float const x0 = top_left.x;
    float const y0 = top_left.y;
    float const x1 = top_left.x + size.x;
    float const y1 = top_left.y + size.y;
    float const b = bevel_size;

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

    std::vector<ColoredQuad> quads;
    quads.reserve(5);

    // Flat center face.
    quads.push_back({{{Math::Vec2<float>(x0, y0),
                       Math::Vec2<float>(x0, y1),
                       Math::Vec2<float>(x1, y1),
                       Math::Vec2<float>(x1, y0)}},
                     base_color});

    // Top border wedge.
    quads.push_back({{{Math::Vec2<float>(x0, y0),
                       Math::Vec2<float>(x0 - b, y0 - b),
                       Math::Vec2<float>(x1 + b, y0 - b),
                       Math::Vec2<float>(x1, y0)}},
                     top_left_color});

    // Left border wedge.
    quads.push_back({{{Math::Vec2<float>(x0 - b, y0 - b),
                       Math::Vec2<float>(x0 - b, y1 + b),
                       Math::Vec2<float>(x0, y1),
                       Math::Vec2<float>(x0, y0)}},
                     top_left_color});

    // Bottom border wedge.
    quads.push_back({{{Math::Vec2<float>(x0 - b, y1 + b),
                       Math::Vec2<float>(x1 + b, y1 + b),
                       Math::Vec2<float>(x1, y1),
                       Math::Vec2<float>(x0, y1)}},
                     bottom_right_color});

    // Right border wedge.
    quads.push_back({{{Math::Vec2<float>(x1, y0),
                       Math::Vec2<float>(x1 + b, y0 - b),
                       Math::Vec2<float>(x1 + b, y1 + b),
                       Math::Vec2<float>(x1, y1)}},
                     bottom_right_color});

    return quads;
}

}  // namespace vulkan_graphix::UiGeometry
