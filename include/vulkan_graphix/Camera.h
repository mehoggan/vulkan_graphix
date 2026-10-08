#ifndef VULKAN_GRAPHIX_CAMERA_H
#define VULKAN_GRAPHIX_CAMERA_H

#include <array>
#include <cstdint>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix {

// ************************************************************ //
// Camera                                                       //
//                                                              //
// What every camera shares: a subclass says where it is (eye), //
// what it looks at (target), and which way is up; the base     //
// turns that into a view matrix and adds a decaying shake.     //
// ************************************************************ //
class Camera {
public:
  virtual ~Camera() = default;

  virtual Math::Vec3<float> eye() const = 0;
  virtual Math::Vec3<float> target() const = 0;
  // World +Y unless a subclass carries its own orientation.
  virtual Math::Vec3<float> up() const;

  // lookAt(eye, target, up), with eye and target both offset by the
  // current shake (what vulkan_earth's gluLookAt() calls did).
  Math::Mat4<float> view() const;

  // Starts a shake of the given magnitude on all three axes (0 stops it).
  void setShake(std::int32_t magnitude);
  // Advances the shake one frame: each axis decays and flips sign on its
  // own cadence until it reaches zero. vulkan_earth calls this once per
  // frame, right after taking that frame's view().
  void updateShake();
  Math::Vec3<float> shakeOffset() const;

protected:
  Camera() = default;
  Camera(const Camera&) = default;
  Camera& operator=(const Camera&) = default;

private:
  std::array<std::int32_t, 3> m_shake{0, 0, 0};
};

}  // namespace vulkan_graphix

#endif
