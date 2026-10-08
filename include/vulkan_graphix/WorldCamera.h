#ifndef VULKAN_GRAPHIX_WORLDCAMERA_H
#define VULKAN_GRAPHIX_WORLDCAMERA_H

#include "vulkan_graphix/Camera.h"
#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix {

// ************************************************************ //
// WorldCamera                                                  //
//                                                              //
// vulkan_earth's overview camera (its WorldCam): looks down    //
// on the terrain, tilted 10 degrees toward +X, from a          //
// position the player pans with move().                        //
// ************************************************************ //
class WorldCamera : public Camera {
public:
  WorldCamera(float x, float y, float z);

  Math::Vec3<float> eye() const override;
  Math::Vec3<float> target() const override;
  Math::Vec3<float> up() const override;

  void move(float dx, float dy, float dz);

private:
  // Column 1 is up, column 2 the view direction, column 3 the position -
  // the layout of the GL matrix the game originally kept.
  Math::Mat4<float> m_frame;
};

}  // namespace vulkan_graphix

#endif
