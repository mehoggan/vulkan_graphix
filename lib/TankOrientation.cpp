#include "vulkan_graphix/TankOrientation.h"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

namespace vulkan_graphix::TankOrientation {

namespace {
// Tank::normalizeVector()/GameState::normalizeVector(): leaves a zero-
// length vector as-is rather than dividing by zero.
Math::Vec3<float> normalizedOrZero(Math::Vec3<float> const& vec) {
    float const mag = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    if (mag != 0) {
        return vec / mag;
    }
    return vec;
}
}  // namespace

float angleBetweenDegrees(Math::Vec3<float> const& one,
                          Math::Vec3<float> const& two) {
    Math::Vec3<float> const u = normalizedOrZero(one);
    Math::Vec3<float> const v = normalizedOrZero(two);
    float const pi_value = 3.141592653f;
    float const dot_product = u.x * v.x + u.y * v.y + u.z * v.z;
    // calcAngleBetweenVectors() detected acos()'s domain error via errno;
    // test the domain directly instead (and a NaN input explicitly).
    if (std::isnan(dot_product) || dot_product < -1.0f || dot_product > 1.0f) {
        return 0.01f;
    }
    return static_cast<float>(std::acos(dot_product) * (180.0 / pi_value));
}

std::optional<Alignment> alignToGround(
        Math::Mat4<float> const& body_matrix,
        Math::Vec3<float> const& ground_normal) {
    Math::Vec3<float> const tanks_up(body_matrix[1]);
    float const angle = angleBetweenDegrees(ground_normal, tanks_up);

    Math::Vec3<float> const& u = ground_normal;
    Math::Vec3<float> const& v = tanks_up;
    Math::Vec3<float> axis(u.y * v.z - v.y * u.z,
                           u.z * v.x - u.x * v.z,
                           u.x * v.y - v.x * u.y);

    float const mag = static_cast<float>(
            std::sqrt(std::pow(static_cast<double>(axis.x), 2.0) +
                      std::pow(static_cast<double>(axis.y), 2.0) +
                      std::pow(static_cast<double>(axis.z), 2.0)));
    if (mag == 0) {
        return std::nullopt;
    }
    axis /= mag;

    Math::Mat4<float> const rotated =
            glm::rotate(body_matrix,
                        glm::radians(angle),
                        Math::Vec3<float>(axis.z, axis.y, axis.x));
    return Alignment{rotated, axis, angle};
}

}  // namespace vulkan_graphix::TankOrientation
