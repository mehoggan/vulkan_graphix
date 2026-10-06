#ifndef VULKAN_GRAPHIX_MATH_AXISALIGNED2D_HPP
#define VULKAN_GRAPHIX_MATH_AXISALIGNED2D_HPP

#include <limits>
#include <utility>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Math {

// A 2D axis-aligned bounding box.
template <typename T>
class AxisAligned2D {
public:
  AxisAligned2D() :
      m_lower_left(
          std::numeric_limits<T>::max(), std::numeric_limits<T>::max()),
      m_upper_right(
          -std::numeric_limits<T>::max(), -std::numeric_limits<T>::max()) {}

  AxisAligned2D(const Vec2<T>& point1, const Vec2<T>& point2) :
      AxisAligned2D() {
    growToContain(point1);
    growToContain(point2);
  }

  AxisAligned2D(const Vec2<T>& lower_left, const T& width, const T& height) :
      AxisAligned2D() {
    growToContain(lower_left);
    growToContain(Vec2<T>(lower_left.x + width, lower_left.y + height));
  }

  bool isValid() const {
    return (m_lower_left.x < m_upper_right.x) &&
        (m_lower_left.y < m_upper_right.y);
  }

  void growToContain(const Vec2<T>& point) {
    m_lower_left = glm::min(m_lower_left, point);
    m_upper_right = glm::max(m_upper_right, point);
  }

  bool contains(const Vec2<T>& point) const {
    return (point.x >= m_lower_left.x && point.y >= m_lower_left.y) &&
        (point.x <= m_upper_right.x && point.y <= m_upper_right.y);
  }

  Vec2<T>& lowerLeft() { return m_lower_left; }
  Vec2<T>& upperRight() { return m_upper_right; }

  T width() const { return m_upper_right.x - m_lower_left.x; }
  T height() const { return m_upper_right.y - m_lower_left.y; }

  Vec2<T> center() const { return (m_lower_left + m_upper_right) / T(2); }

private:
  Vec2<T> m_lower_left;
  Vec2<T> m_upper_right;

  friend bool operator==(const AxisAligned2D& lhs, const AxisAligned2D& rhs) {
    return (lhs.m_lower_left == rhs.m_lower_left) &&
        (lhs.m_upper_right == rhs.m_upper_right);
  }

  friend void swap(AxisAligned2D& lhs, AxisAligned2D& rhs) {
    std::swap(lhs.m_lower_left, rhs.m_lower_left);
    std::swap(lhs.m_upper_right, rhs.m_upper_right);
  }
};

}  // namespace vulkan_graphix::Math

#endif
