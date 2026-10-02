#include "vulkan_earth/render/UiBuilders.h"

#include <vector>

#include "vulkan_graphix/UiGeometry.h"

namespace vulkan_earth::render {

void appendBevel(UiMesh& mesh,
                 float x,
                 float y,
                 float z,
                 float width,
                 float height,
                 Vec4 const& color,
                 bool pressed,
                 float bevel_size) {
    // buildButtonBevel() works y-down; negating y on the way in and out
    // mirrors it into the game's y-up space exactly.
    std::vector<vulkan_graphix::UiGeometry::ColoredQuad> const quads =
            vulkan_graphix::UiGeometry::buildButtonBevel(Vec2(x, -y),
                                                         Vec2(width, height),
                                                         color,
                                                         pressed,
                                                         bevel_size);
    for (auto const& quad : quads) {
        std::array<Vec3, 4> corners;
        for (std::size_t i = 0; i < corners.size(); ++i) {
            corners[i] = Vec3(quad.corners[i].x, -quad.corners[i].y, z);
        }
        mesh.addQuad(corners, quad.color);
    }
}

void appendFrame(UiMesh& mesh,
                 float x,
                 float y,
                 float z,
                 float width,
                 float height,
                 Vec4 const& top_left,
                 Vec4 const& face,
                 Vec4 const& bottom_right,
                 float border) {
    float const b = border;
    mesh.addQuad({Vec3(x, y, z),
                  Vec3(x - b, y + b, z),
                  Vec3(x + width + b, y + b, z),
                  Vec3(x + width, y, z)},
                 top_left);
    mesh.addQuad({Vec3(x - b, y + b, z),
                  Vec3(x - b, y - height - b, z),
                  Vec3(x, y - height, z),
                  Vec3(x, y, z)},
                 top_left);
    mesh.addQuad({Vec3(x, y, z),
                  Vec3(x, y - height, z),
                  Vec3(x + width, y - height, z),
                  Vec3(x + width, y, z)},
                 face);
    mesh.addQuad({Vec3(x - b, y - height - b, z),
                  Vec3(x + width + b, y - height - b, z),
                  Vec3(x + width, y - height, z),
                  Vec3(x, y - height, z)},
                 bottom_right);
    mesh.addQuad({Vec3(x + width, y, z),
                  Vec3(x + width + b, y + b, z),
                  Vec3(x + width + b, y - height - b, z),
                  Vec3(x + width, y + -height, z)},
                 bottom_right);
}

void appendMenuPanel(UiMesh& mesh,
                     float width,
                     float height,
                     float percent_border) {
    float const b = percent_border * (height);
    float const left = -1 * (width / 2.0);
    float const right = (width / 2.0);
    float const top = (height / 2.0);
    float const bottom = -1 * (height / 2.0);
    mesh.addQuad({Vec3(left, top, 0),
                  Vec3(left, bottom, 0),
                  Vec3(left + b, bottom + b, 0),
                  Vec3(left + b, top - b, 0)},
                 Vec4(0.85f, 0.85f, 0.85f, 1.0f));
    mesh.addQuad({Vec3(left, top, 0),
                  Vec3(left + b, top - b, 0),
                  Vec3(right - b, top - b, 0),
                  Vec3(right, top, 0)},
                 Vec4(0.80f, 0.80f, 0.80f, 1.0f));
    mesh.addQuad({Vec3(left + b, top - b, 0),
                  Vec3(left + b, bottom + b, 0),
                  Vec3(right - b, bottom + b, 0),
                  Vec3(right - b, top - b, 0)},
                 Vec4(0.75f, 0.75f, 0.75f, 1.0f));
    mesh.addQuad({Vec3(left + b, bottom + b, 0),
                  Vec3(left, bottom, 0),
                  Vec3(right, bottom, 0),
                  Vec3(right - b, bottom + b, 0)},
                 Vec4(0.45f, 0.45f, 0.45f, 1.0f));
    mesh.addQuad({Vec3(right - b, top - b, 0),
                  Vec3(right - b, bottom + b, 0),
                  Vec3(right, bottom, 0),
                  Vec3(right, top, 0)},
                 Vec4(0.40f, 0.40f, 0.40f, 1.0f));
}

void appendQuad(UiMesh& mesh,
                Vec3 const& v0,
                Vec3 const& v1,
                Vec3 const& v2,
                Vec3 const& v3,
                Vec4 const& color) {
    mesh.addQuad({v0, v1, v2, v3}, color);
}

}  // namespace vulkan_earth::render
