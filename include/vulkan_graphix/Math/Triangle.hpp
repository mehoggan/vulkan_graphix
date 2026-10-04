#ifndef VULKAN_GRAPHIX_MATH_TRIANGLE_HPP
#define VULKAN_GRAPHIX_MATH_TRIANGLE_HPP

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Math {

// A triangle defined by three 3D points.
template <typename T>
class Triangle {
public:
    Triangle(const Vec3<T>& point0,
             const Vec3<T>& point1,
             const Vec3<T>& point2) :
            m_point0(point0),
            m_point1(point1),
            m_point2(point2) {}

    const Vec3<T>& p0() const { return m_point0; }
    const Vec3<T>& p1() const { return m_point1; }
    const Vec3<T>& p2() const { return m_point2; }

    Vec3<T> centroid() const {
        return (m_point0 + m_point1 + m_point2) / T(3);
    }

private:
    Vec3<T> m_point0;
    Vec3<T> m_point1;
    Vec3<T> m_point2;
};

}  // namespace vulkan_graphix::Math

#endif
