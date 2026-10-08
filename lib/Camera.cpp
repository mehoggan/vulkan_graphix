#include "vulkan_graphix/Camera.h"
#include <cstddef>
#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>

namespace vulkan_graphix {

Math::Vec3<float> Camera::up() const {
  return Math::Vec3<float>(0.0f, 1.0f, 0.0f);
}

Math::Mat4<float> Camera::view() const {
  const Math::Vec3<float> shake = shakeOffset();
  return glm::lookAt(eye() + shake, target() + shake, up());
}

void Camera::setShake(std::int32_t magnitude) { m_shake.fill(magnitude); }

void Camera::updateShake() {
  // Ported verbatim from vulkan_earth's WorldCam/ChaseCam: integer
  // division by ~1.0153 truncates toward zero, and axis N flips sign
  // whenever its value is N mod 3.
  constexpr double c_decay = 1.015281239159713;
  for (std::size_t axis = 0; axis < m_shake.size(); ++axis) {
    std::int32_t& value = m_shake[axis];
    if (value != 0) {
      value = static_cast<std::int32_t>(value / c_decay);
      if (value % 3 == static_cast<std::int32_t>(axis)) {
        value *= -1;
      }
    }
  }
}

Math::Vec3<float> Camera::shakeOffset() const {
  return Math::Vec3<float>(static_cast<float>(m_shake[0]),
      static_cast<float>(m_shake[1]),
      static_cast<float>(m_shake[2]));
}

}  // namespace vulkan_graphix
