#ifndef VULKAN_GRAPHIX_MATH_CURVESAMPLE3D_HPP
#define VULKAN_GRAPHIX_MATH_CURVESAMPLE3D_HPP

#include <limits>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Math {

// A single sample along a CubicCurve: the position, tangent, and curve
// parameter (t) it was evaluated at.
template <typename T>
struct CurveSample3D {
  CurveSample3D() :
      m_position(T(0), T(0), T(0)),
      m_tangent(T(0), T(0), T(0)),
      m_parameter(T(0)) {}

  CurveSample3D(
      const Vec3<T>& position_in, const Vec3<T>& tangent_in, T parameter_in) :
      m_position(position_in),
      m_tangent(tangent_in),
      m_parameter(parameter_in) {}

  void normalizeTangent() {
    if (glm::length(m_tangent) > std::numeric_limits<T>::epsilon()) {
      m_tangent = glm::normalize(m_tangent);
    }
  }

  Vec3<T> m_position;
  Vec3<T> m_tangent;
  T m_parameter;

  friend bool operator<(const CurveSample3D& lhs, const CurveSample3D& rhs) {
    return lhs.m_parameter < rhs.m_parameter;
  }
};

}  // namespace vulkan_graphix::Math

#endif
