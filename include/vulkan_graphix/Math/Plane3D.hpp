#ifndef VULKAN_GRAPHIX_MATH_PLANE3D_HPP
#define VULKAN_GRAPHIX_MATH_PLANE3D_HPP

#include <utility>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Math {

// A 3D plane stored as the coefficients (a, b, c, d) of
// a*x + b*y + c*z + d = 0.
template <typename T>
class Plane3D {
public:
    // Default-constructs an invalid plane (all coefficients zero).
    Plane3D() :
            m_coefficients(T(0), T(0), T(0), T(0)) {}

    Plane3D(const T& coeff_a,
            const T& coeff_b,
            const T& coeff_c,
            const T& coeff_d) :
            m_coefficients(coeff_a, coeff_b, coeff_c, coeff_d) {}

    // Not explicit so a coefficient vector can convert implicitly.
    Plane3D(const Vec4<T>& coefficients) :
            m_coefficients(coefficients) {}

    T a() const { return m_coefficients.x; }
    T b() const { return m_coefficients.y; }
    T c() const { return m_coefficients.z; }
    T d() const { return m_coefficients.w; }

    void a(const T& value) { m_coefficients.x = value; }
    void b(const T& value) { m_coefficients.y = value; }
    void c(const T& value) { m_coefficients.z = value; }
    void d(const T& value) { m_coefficients.w = value; }

    Vec3<T> normal(bool normalize = true) const {
        Vec3<T> result(a(), b(), c());
        return normalize ? glm::normalize(result) : result;
    }

    // See: http://mathworld.wolfram.com/Point-PlaneDistance.html
    T distanceFromPoint(const Vec3<T>& point) const {
        return glm::dot(normal(true), point) + d();
    }

    bool isValid() const {
        return !((a() == T(0)) && (b() == T(0)) && (c() == T(0)));
    }

private:
    Vec4<T> m_coefficients;

    friend bool operator==(const Plane3D& lhs, const Plane3D& rhs) {
        return lhs.m_coefficients == rhs.m_coefficients;
    }

    friend bool operator!=(const Plane3D& lhs, const Plane3D& rhs) {
        return !(lhs == rhs);
    }

    friend void swap(Plane3D& lhs, Plane3D& rhs) {
        std::swap(lhs.m_coefficients, rhs.m_coefficients);
    }
};

}  // namespace vulkan_graphix::Math

#endif
