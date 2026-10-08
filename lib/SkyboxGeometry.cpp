#include "vulkan_graphix/SkyboxGeometry.h"

namespace vulkan_graphix::SkyboxGeometry {

std::array<Face, 6> buildFaces(
    const Math::Vec3<float>& min_corner, const Math::Vec3<float>& max_corner) {
  using Vec3 = Math::Vec3<float>;
  const float x0 = min_corner.x;
  const float y0 = min_corner.y;
  const float z0 = min_corner.z;
  const float x1 = max_corner.x;
  const float y1 = max_corner.y;
  const float z1 = max_corner.z;
  const std::array<Math::Vec2<float>, 4> texcoords = {
      Math::Vec2<float>(0.0f, 1.0f),
      Math::Vec2<float>(0.0f, 0.0f),
      Math::Vec2<float>(1.0f, 0.0f),
      Math::Vec2<float>(1.0f, 1.0f)};
  return {{
      // front
      {{Vec3(x0, y0, z0),
           Vec3(x1, y0, z0),
           Vec3(x1, y1, z0),
           Vec3(x0, y1, z0)},
          texcoords},
      // right
      {{Vec3(x1, y0, z0),
           Vec3(x1, y0, z1),
           Vec3(x1, y1, z1),
           Vec3(x1, y1, z0)},
          texcoords},
      // back
      {{Vec3(x1, y0, z1),
           Vec3(x0, y0, z1),
           Vec3(x0, y1, z1),
           Vec3(x1, y1, z1)},
          texcoords},
      // left
      {{Vec3(x0, y0, z1),
           Vec3(x0, y0, z0),
           Vec3(x0, y1, z0),
           Vec3(x0, y1, z1)},
          texcoords},
      // top
      {{Vec3(x0, y1, z0),
           Vec3(x1, y1, z0),
           Vec3(x1, y1, z1),
           Vec3(x0, y1, z1)},
          texcoords},
      // bottom
      {{Vec3(x0, y0, z0),
           Vec3(x0, y0, z1),
           Vec3(x1, y0, z1),
           Vec3(x1, y0, z0)},
          texcoords},
  }};
}

}  // namespace vulkan_graphix::SkyboxGeometry
