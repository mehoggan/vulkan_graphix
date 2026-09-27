#ifndef VULKAN_GRAPHIX_GEOMETRY_HPP
#define VULKAN_GRAPHIX_GEOMETRY_HPP

#include <cmath>
#include <limits>

#include "vulkan_graphix/Math/Line.hpp"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Math/Triangle.hpp"

namespace vulkan_graphix::Math {

// Trig.h itself is superseded by glm::radians/glm::degrees, but
// SphericalCoordinates still needs to know which unit its angles are in.
enum class AngleMode { Degrees, Radians };

template <typename T>
bool pointsOfTriangleAreCollinear(
        Triangle<T> const& tri,
        float epsilon = std::numeric_limits<float>::epsilon()) {
    Vec3<T> edge1 = tri.p1() - tri.p0();
    Vec3<T> edge2 = tri.p2() - tri.p0();
    T area = glm::length(glm::cross(edge1, edge2));
    return !(area > epsilon);
}

template <typename T>
Vec3<T> centroidOfTriangle(Triangle<T> const& tri) {
    return (tri.p0() + tri.p1() + tri.p2()) / T(3);
}

template <typename T>
Vec3<T> midpointOfLine(Line<T> const& line) {
    return (line.p0() + line.p1()) / T(2);
}

template <typename T, AngleMode AM>
class SphericalCoordinates {
public:
    SphericalCoordinates(T theta, T phi, T radius)
            : m_theta(theta), m_phi(phi), m_radius(radius) {}

    T theta() const { return m_theta; }
    T phi() const { return m_phi; }
    T radius() const { return m_radius; }
    AngleMode angleMode() const { return AM; }

private:
    T m_theta;
    T m_phi;
    T m_radius;
};

template <typename T, AngleMode AM>
Vec3<T> sphericalToCartesian(SphericalCoordinates<T, AM> const& coords) {
    T theta = coords.theta();
    T azimuth = coords.phi();
    if constexpr (AM == AngleMode::Degrees) {
        theta = glm::radians(theta);
        azimuth = glm::radians(azimuth);
    }
    return Vec3<T>(coords.radius() * std::cos(azimuth) * std::cos(theta),
                   coords.radius() * std::sin(azimuth),
                   coords.radius() * std::cos(azimuth) * std::sin(theta));
}

template <typename T, AngleMode AM>
SphericalCoordinates<T, AM> cartesianToSpherical(Vec3<T> const& point) {
    T radius = glm::length(point);
    T azimuth = std::asin(point.y / radius);
    T theta = std::acos(point.x / (radius * std::cos(azimuth)));

    if constexpr (AM == AngleMode::Degrees) {
        theta = glm::degrees(theta);
        azimuth = glm::degrees(azimuth);
    }

    return SphericalCoordinates<T, AM>(theta, azimuth, radius);
}

}  // namespace vulkan_graphix::Math

#endif
