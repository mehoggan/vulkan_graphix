#ifndef VULKAN_GRAPHIX_CHASECAMERA_H
#define VULKAN_GRAPHIX_CHASECAMERA_H

#include "vulkan_graphix/Camera.h"
#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix {

// ************************************************************ //
// ChaseCamera                                                  //
//                                                              //
// vulkan_earth's projectile-following camera (its ChaseCam):   //
// sits behind and above a moving object, looking ahead along   //
// its direction of travel.                                     //
// ************************************************************ //
class ChaseCamera : public Camera {
public:
  // Both point at three floats owned by the followed object, read every
  // time eye()/target() is called, so they must outlive this camera.
  ChaseCamera(const float* followed_position, const float* followed_direction);

  Math::Vec3<float> eye() const override;
  Math::Vec3<float> target() const override;

  // Rise high overhead, no longer trailing along x (vulkan_earth does
  // this while a shell's explosion plays).
  void riseOverhead();
  // Back to following from behind, 100 units up.
  void resetHeight();

private:
  float directionLength() const;

  const float* m_followed_position;
  const float* m_followed_direction;
  float m_back_factor;
  float m_up_factor;
};

}  // namespace vulkan_graphix

#endif
