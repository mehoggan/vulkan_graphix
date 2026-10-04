#ifndef VULKAN_GRAPHIX_CURVESAMPLE3D_HPP
#define VULKAN_GRAPHIX_CURVESAMPLE3D_HPP

#include <limits>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Math {

// A single sample along a CubicCurve: the position, tangent, and curve
// parameter (t) it was evaluated at.
template <typename T>
struct CurveSample3D {
    CurveSample3D()
            : position(T(0), T(0), T(0))
            , tangent(T(0), T(0), T(0))
            , parameter(T(0)) {}

    CurveSample3D(const Vec3<T>& position_in,
                  const Vec3<T>& tangent_in,
                  T parameter_in)
            : position(position_in)
            , tangent(tangent_in)
            , parameter(parameter_in) {}

    void normalizeTangent() {
        if (glm::length(tangent) > std::numeric_limits<T>::epsilon()) {
            tangent = glm::normalize(tangent);
        }
    }

    Vec3<T> position;
    Vec3<T> tangent;
    T parameter;

    friend bool operator<(const CurveSample3D& lhs, const CurveSample3D& rhs) {
        return lhs.parameter < rhs.parameter;
    }
};

}  // namespace vulkan_graphix::Math

#endif
