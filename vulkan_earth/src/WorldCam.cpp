#include "vulkan_earth/WorldCam.h"
#include <algorithm>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

WorldCam::WorldCam() = default;
WorldCam::WorldCam(float x, float y, float z) {
  m_matrix[0] = 0;
  m_matrix[1] = 0;
  m_matrix[2] = 1;
  m_matrix[3] = 0;
  m_matrix[4] = 1;
  m_matrix[5] = 0;
  m_matrix[6] = 0;
  m_matrix[7] = 0;
  m_matrix[8] = 0;
  m_matrix[9] = -1;
  m_matrix[10] = 0;
  m_matrix[11] = 0;
  m_matrix[12] = x;
  m_matrix[13] = y;
  m_matrix[14] = z;
  m_matrix[15] = 0;
  std::copy_n(glm::value_ptr(glm::rotate(glm::make_mat4(m_matrix),
                  glm::radians(static_cast<float>(-10)),
                  math::Vec3<float>(m_matrix[4], m_matrix[5], m_matrix[6]))),
      16,
      m_matrix);
  m_shake_cam_pos[0] = 0;
  m_shake_cam_pos[1] = 0;
  m_shake_cam_pos[2] = 0;
}

math::Mat4<float> WorldCam::view() {
  const math::Mat4<float> view_matrix =
      glm::lookAt(math::Vec3<float>(m_matrix[12] + m_shake_cam_pos[0],
                      m_matrix[13] + m_shake_cam_pos[1],
                      m_matrix[14] + m_shake_cam_pos[2]),
          math::Vec3<float>(m_matrix[12] + m_matrix[8] + m_shake_cam_pos[0],
              m_matrix[13] + m_matrix[9] + m_shake_cam_pos[1],
              m_matrix[14] + m_matrix[10] + m_shake_cam_pos[2]),
          math::Vec3<float>(m_matrix[4], m_matrix[5], m_matrix[6]));
  updateShakeCam();
  return view_matrix;
}

void WorldCam::moveCam(float x, float y, float z) {
  m_matrix[12] += x;
  m_matrix[13] += y;
  m_matrix[14] += z;
}

float* WorldCam::getMatrix() { return m_matrix; }

void WorldCam::setShakeCam(std::int32_t magnitude) {
  m_shake_cam_pos[0] = magnitude;
  m_shake_cam_pos[1] = magnitude;
  m_shake_cam_pos[2] = magnitude;
}

void WorldCam::updateShakeCam() {
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