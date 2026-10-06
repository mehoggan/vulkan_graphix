#include "vulkan_earth/ChaseCam.h"
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "math.h"
#include "vulkan_earth/MacroCrtdbg.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

ChaseCam::ChaseCam() = default;
ChaseCam::ChaseCam(float* new_target_pos, float* new_target_at) {
    m_target_pos = new_target_pos;
    m_target_at = new_target_at;
    m_shake_cam_pos[0] = 0;
    m_shake_cam_pos[1] = 0;
    m_shake_cam_pos[2] = 0;
    m_back_factor = 1;
    m_up_factor = 1;
}

math::Mat4<float> ChaseCam::view() {
    float mag = sqrt(m_target_at[0] * m_target_at[0] +
      m_target_at[1] * m_target_at[1] + m_target_at[2] * m_target_at[2]);
    // (The target's y using target_at[0] rather than [1] is the
    // original's, kept as-is.)
    const math::Mat4<float> view_matrix = glm::lookAt(
      math::Vec3<float>(m_target_pos[0] -
          500 * m_target_at[0] / mag * (m_back_factor) + m_shake_cam_pos[0],
        m_target_pos[1] + m_up_factor + m_shake_cam_pos[1],
        m_target_pos[2] - 500 * m_target_at[2] / mag + m_shake_cam_pos[2]),
      math::Vec3<float>(
        m_target_pos[0] + 200 * m_target_at[0] / mag + m_shake_cam_pos[0],
        m_target_pos[1] + 200 * m_target_at[0] / mag + m_shake_cam_pos[1],
        m_target_pos[2] + 200 * m_target_at[2] / mag + m_shake_cam_pos[2]),
      math::Vec3<float>(0, 1, 0));
    updateShakeCam();
    return view_matrix;
}

void ChaseCam::updateFactor() {
    m_back_factor = 0;
    m_up_factor = 4000;
}

void ChaseCam::resetFactor() {
    m_back_factor = 1;
    m_up_factor = 100;
}

void ChaseCam::setShakeCam(std::int32_t magnitude) {
    m_shake_cam_pos[0] = magnitude;
    m_shake_cam_pos[1] = magnitude;
    m_shake_cam_pos[2] = magnitude;
}

void ChaseCam::updateShakeCam() {
    if (m_shake_cam_pos[0] != 0) {
        m_shake_cam_pos[0] = m_shake_cam_pos[0] / 1.015281239159713;
        if (m_shake_cam_pos[0] % 3 == 0) {
            m_shake_cam_pos[0] *= -1;
        }
    }
    if (m_shake_cam_pos[1] != 0) {
        m_shake_cam_pos[1] = m_shake_cam_pos[1] / 1.015281239159713;
        if (m_shake_cam_pos[1] % 3 == 1) {
            m_shake_cam_pos[1] *= -1;
        }
    }
    if (m_shake_cam_pos[2] != 0) {
        m_shake_cam_pos[2] = m_shake_cam_pos[2] / 1.015281239159713;
        if (m_shake_cam_pos[2] % 3 == 2) {
            m_shake_cam_pos[2] *= -1;
        }
    }
}
