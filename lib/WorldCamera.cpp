#include "vulkan_graphix/WorldCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace vulkan_graphix {

namespace {

// The game's original GL matrix: looking straight down (-Y) with +X as
// screen-up, then tilted 10 degrees about its own side axis so it looks
// slightly ahead along +X.
Math::Mat4<float> initialFrame(float x, float y, float z) {
  const Math::Mat4<float> frame(
      0.0f,
      0.0f,
      1.0f,
      0.0f,
      1.0f,
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      -1.0f,
      0.0f,
      0.0f,
      x,
      y,
      z,
      0.0f);
  return glm::rotate(
      frame,
      glm::radians(-10.0f),
      Math::Vec3<float>(frame[1][0], frame[1][1], frame[1][2]));
}

}  // namespace

WorldCamera::WorldCamera(float x, float y, float z) :
    m_frame(initialFrame(x, y, z)) {}

Math::Vec3<float> WorldCamera::eye() const {
  return Math::Vec3<float>(m_frame[3]);
}

Math::Vec3<float> WorldCamera::target() const {
  return eye() + Math::Vec3<float>(m_frame[2]);
}

Math::Vec3<float> WorldCamera::up() const {
  return Math::Vec3<float>(m_frame[1]);
}

void WorldCamera::move(float dx, float dy, float dz) {
  m_frame[3] += Math::Vec4<float>(dx, dy, dz, 0.0f);
}

}  // namespace vulkan_graphix
