#ifndef VULKAN_GRAPHIX_ORBITCAMERA_H
#define VULKAN_GRAPHIX_ORBITCAMERA_H

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

    void onMouseButton(int button, bool pressed, int pos_x, int pos_y);
    void onMouseMove(int pos_x, int pos_y);

    Math::Vec3<float> eye() const;
    Math::Vec3<float> const& target() const;

private:
    float m_yaw;
    float m_pitch;
    float m_distance;
    bool m_dragging;
    int m_last_x;
    int m_last_y;
    Math::Vec3<float> m_target;
};

}  // namespace vulkan_graphix

#endif
