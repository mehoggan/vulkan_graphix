#ifndef VULKAN_GRAPHIX_MATH_LINE_HPP
#define VULKAN_GRAPHIX_MATH_LINE_HPP

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Math {

// A line segment between two 3D points.
template <typename T>
class Line {
public:
    Line(const Vec3<T>& point0, const Vec3<T>& point1) :
            m_point0(point0),
            m_point1(point1) {}

    const Vec3<T>& p0() const { return m_point0; }
    const Vec3<T>& p1() const { return m_point1; }

private:
    Vec3<T> m_point0;
    Vec3<T> m_point1;
};

}  // namespace vulkan_graphix::Math

#endif
