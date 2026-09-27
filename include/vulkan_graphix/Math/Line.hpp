#ifndef VULKAN_GRAPHIX_LINE_HPP
#define VULKAN_GRAPHIX_LINE_HPP

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Math {

// A line segment between two 3D points.
template <typename T>
class Line {
public:
    Line(Vec3<T> const& point0, Vec3<T> const& point1)
            : m_point0(point0), m_point1(point1) {}

    Vec3<T> const& p0() const { return m_point0; }
    Vec3<T> const& p1() const { return m_point1; }

private:
    Vec3<T> m_point0;
    Vec3<T> m_point1;
};

}  // namespace vulkan_graphix::Math

#endif
