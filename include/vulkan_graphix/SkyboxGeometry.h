#ifndef VULKAN_GRAPHIX_SKYBOXGEOMETRY_H
#define VULKAN_GRAPHIX_SKYBOXGEOMETRY_H

// vulkan_earth's skybox (SkyboxFactory): six textured faces of a box,
// every face mapped to the whole texture. Pure geometry, no Vulkan
// coupling - the game adds each face as a textured quad, and Tutorial11/
// Tutorial21 index the 24 corners with c_quad_triangle_indices per face.

#include <array>
#include <cstdint>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::SkyboxGeometry {

struct Face {
  // In GL_QUADS drawing order.
  std::array<Math::Vec3<float>, 4> m_corners;
  std::array<Math::Vec2<float>, 4> m_texcoords;
};

// Face order front (z = min), right (x = max), back (z = max), left
// (x = min), top (y = max), bottom (y = min), each face's corners in the
// game's original order with texcoords (0,1), (0,0), (1,0), (1,1).
// vulkan_earth's own box is asymmetric - its top sits at half the height
// of its bottom - which callers express through max_corner.y.
std::array<Face, 6> buildFaces(
    const Math::Vec3<float>& min_corner, const Math::Vec3<float>& max_corner);

// One quad's two triangles (0-1-2, 0-2-3): GL_QUADS' fan triangulation.
inline constexpr std::array<std::uint32_t, 6> c_quad_triangle_indices = {
    0, 1, 2, 0, 2, 3};

}  // namespace vulkan_graphix::SkyboxGeometry

#endif  // VULKAN_GRAPHIX_SKYBOXGEOMETRY_H
