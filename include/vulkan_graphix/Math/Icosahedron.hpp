#ifndef VULKAN_GRAPHIX_MATH_ICOSAHEDRON_HPP
#define VULKAN_GRAPHIX_MATH_ICOSAHEDRON_HPP

#include <cstdint>
#include <vector>

#include <glm/geometric.hpp>

#include "vulkan_graphix/Math/Geometry.hpp"
#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Math {

// A regular icosahedron: 12 vertices, 20 triangular faces. Used on its own,
// or as the base mesh Sphere<T, I> subdivides into an icosphere.
//
// Vertices are placed with spherical coordinates in the Y-up, elevation
// convention SphericalCoordinates<T, AngleMode> already uses (elevation
// +90 at the top vertex, -90 at the bottom vertex), matching this project's
// camera/view conventions.
template <typename T = float, typename I = std::uint32_t>
class Icosahedron {
public:
  explicit Icosahedron(T radius) :
      m_radius(radius) {
    generateVertices();
    generateIndices();
  }

  const std::vector<Vec3<T>>& points() const { return m_points; }
  const std::vector<Vec3<T>>& normals() const { return m_normals; }
  const std::vector<I>& indices() const { return m_indices; }

  T radius() const { return m_radius; }

private:
  T m_radius;
  std::vector<Vec3<T>> m_points;
  std::vector<Vec3<T>> m_normals;
  std::vector<I> m_indices;

  Vec3<T> pointAt(T theta_degrees, T elevation_degrees) const {
    SphericalCoordinates<T, AngleMode::Degrees> coords(
        theta_degrees, elevation_degrees, m_radius);
    return sphericalToCartesian(coords);
  }

  void generateVertices() {
    // Top vertex.
    m_points.push_back(pointAt(T(0), T(90)));

    // Upper ring: five points 72° apart, 30° above the equator.
    constexpr T theta_step_degrees = T(72);
    T theta_degrees = T(360) - T(36);
    for (std::uint8_t point = 0u; point < 5u; ++point) {
      m_points.push_back(pointAt(theta_degrees, T(30)));
      theta_degrees += theta_step_degrees;
    }

    // Lower ring: five points 72° apart, 30° below the equator.
    theta_degrees = T(0);
    for (std::uint8_t point = 0u; point < 5u; ++point) {
      m_points.push_back(pointAt(theta_degrees, T(-30)));
      theta_degrees += theta_step_degrees;
    }

    // Bottom vertex.
    m_points.push_back(pointAt(T(0), T(-90)));

    generateNormals();
  }

  void generateNormals() {
    m_normals.reserve(m_points.size());
    for (const Vec3<T>& point : m_points) {
      m_normals.push_back(glm::normalize(point));
    }
  }

  void generateIndices() {
    // Five triangles fanning out from the top vertex.
    for (I index = 1; index <= 5; ++index) {
      m_indices.push_back(0);
      m_indices.push_back(index);
      m_indices.push_back(index != 5 ? index + 1 : 1);
    }

    // Middle band: ten triangles alternating up/down between the rings.
    constexpr I upper_ring_size = 5;
    for (I index = 1; index <= 5; ++index) {
      const I next = index != 5 ? index + 1 : 1;

      m_indices.push_back(index);
      m_indices.push_back(index + upper_ring_size);
      m_indices.push_back(next);

      m_indices.push_back(next);
      m_indices.push_back(index + upper_ring_size);
      m_indices.push_back(next + upper_ring_size);
    }

    // Five triangles fanning in to the bottom vertex.
    for (I index = 6; index <= 10; ++index) {
      m_indices.push_back(11);
      m_indices.push_back(index != 10 ? index + 1 : 6);
      m_indices.push_back(index);
    }
  }
};

}  // namespace vulkan_graphix::Math

#endif
