#ifndef VULKAN_EARTH_CHASECAM_H
#define VULKAN_EARTH_CHASECAM_H

#include <cstdint>

#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Render {
class RenderContext;
}

class ChaseCam {
public:
  ChaseCam();
  ChaseCam(float* new_target_pos, float* new_target_at);
  ~ChaseCam() = default;
  vulkan_graphix::Math::Mat4<float> view();
  void setShakeCam(std::int32_t magnitude);
  void updateShakeCam();
  void updateFactor();
  void resetFactor();

private:
  float* m_target_pos;
  float* m_target_at;
  std::int32_t m_shake_cam_pos[3];
  float m_back_factor;
  float m_up_factor;
};

#endif
