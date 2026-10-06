#ifndef VULKAN_GRAPHIX_ORBITCAMERA_H
#define VULKAN_GRAPHIX_ORBITCAMERA_H

#include <cstdint>
#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix {

// ************************************************************ //
// OrbitCamera                                                  //
//                                                              //
// Mouse-driven orbit camera: left-drag rotates around a fixed  //
// target, the scroll wheel zooms.                              //
// ************************************************************ //
class OrbitCamera {
public:
    OrbitCamera();

    // Lets a tutorial start from a specific vantage point (e.g. a 3/4 view)
    // instead of the default front-on one, without changing the default
    // constructor's behavior for tutorials that don't care.
    OrbitCamera(float initial_yaw_radians,
      float initial_pitch_radians,
      float initial_distance);

    void onMouseButton(std::int32_t button,
      bool pressed,
      std::int32_t pos_x,
      std::int32_t pos_y);
    void onMouseMove(std::int32_t pos_x, std::int32_t pos_y);

    Math::Vec3<float> eye() const;
    const Math::Vec3<float>& target() const;

private:
    float m_yaw;
    float m_pitch;
    float m_distance;
    bool m_dragging;
    std::int32_t m_last_x;
    std::int32_t m_last_y;
    Math::Vec3<float> m_target;
};

}  // namespace vulkan_graphix

#endif
