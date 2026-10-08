#include "vulkan_graphix/ChaseCamera.h"

#include <cmath>

namespace vulkan_graphix {

ChaseCamera::ChaseCamera(
    const float* followed_position, const float* followed_direction) :
    m_followed_position(followed_position),
    m_followed_direction(followed_direction),
    m_back_factor(1.0f),
    m_up_factor(1.0f) {}

float ChaseCamera::directionLength() const {
  return std::sqrt(m_followed_direction[0] * m_followed_direction[0] +
      m_followed_direction[1] * m_followed_direction[1] +
      m_followed_direction[2] * m_followed_direction[2]);
}

// The eye's z ignoring m_back_factor is the original's, kept as-is.
Math::Vec3<float> ChaseCamera::eye() const {
  const float mag = directionLength();
  return Math::Vec3<float>(m_followed_position[0] -
          500 * m_followed_direction[0] / mag * m_back_factor,
      m_followed_position[1] + m_up_factor,
      m_followed_position[2] - 500 * m_followed_direction[2] / mag);
}

// The target's y using the direction's x rather than its y is the
// original's, kept as-is.
Math::Vec3<float> ChaseCamera::target() const {
  const float mag = directionLength();
  return Math::Vec3<float>(
      m_followed_position[0] + 200 * m_followed_direction[0] / mag,
      m_followed_position[1] + 200 * m_followed_direction[0] / mag,
      m_followed_position[2] + 200 * m_followed_direction[2] / mag);
}

void ChaseCamera::riseOverhead() {
  m_back_factor = 0;
  m_up_factor = 4000;
}

void ChaseCamera::resetHeight() {
  m_back_factor = 1;
  m_up_factor = 100;
}

}  // namespace vulkan_graphix
