#include "vulkan_graphix/TankOrientation.h"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

namespace vulkan_graphix::TankOrientation {

namespace {
// Tank::normalizeVector()/GameState::normalizeVector(): leaves a zero-
// length vector as-is rather than dividing by zero.
Math::Vec3<float> normalizedOrZero(const Math::Vec3<float>& vec) {
    const float mag = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    if (mag != 0) {
        return vec / mag;
    }
    return vec;
}
}  // namespace

float angleBetweenDegrees(
  const Math::Vec3<float>& one, const Math::Vec3<float>& two) {
    const Math::Vec3<float> u = normalizedOrZero(one);
    const Math::Vec3<float> v = normalizedOrZero(two);
    const float pi_value = 3.141592653f;
    const float dot_product = u.x * v.x + u.y * v.y + u.z * v.z;
    // calcAngleBetweenVectors() detected acos()'s domain error via errno;
    // test the domain directly instead (and a NaN input explicitly).
    if (std::isnan(dot_product) || dot_product < -1.0f || dot_product > 1.0f) {
        return 0.01f;
    }
    return static_cast<float>(std::acos(dot_product) * (180.0 / pi_value));
}

std::optional<Alignment> alignToGround(const Math::Mat4<float>& body_matrix,
  const Math::Vec3<float>& ground_normal) {
    const Math::Vec3<float> tanks_up(body_matrix[1]);
    const float angle = angleBetweenDegrees(ground_normal, tanks_up);

    const Math::Vec3<float>& u = ground_normal;
    const Math::Vec3<float>& v = tanks_up;
    Math::Vec3<float> axis(
      u.y * v.z - v.y * u.z, u.z * v.x - u.x * v.z, u.x * v.y - v.x * u.y);

    const float mag =
      static_cast<float>(std::sqrt(std::pow(static_cast<double>(axis.x), 2.0) +
        std::pow(static_cast<double>(axis.y), 2.0) +
        std::pow(static_cast<double>(axis.z), 2.0)));
    if (mag == 0) {
        return std::nullopt;
    }
    axis /= mag;

    const Math::Mat4<float> rotated = glm::rotate(body_matrix,
      glm::radians(angle),
      Math::Vec3<float>(axis.z, axis.y, axis.x));
    return Alignment{rotated, axis, angle};
}

}  // namespace vulkan_graphix::TankOrientation
