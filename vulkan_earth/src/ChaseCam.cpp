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
    target_pos = new_target_pos;
    target_at = new_target_at;
    shake_cam_pos[0] = 0;
    shake_cam_pos[1] = 0;
    shake_cam_pos[2] = 0;
    back_factor = 1;
    up_factor = 1;
}

math::Mat4<float> ChaseCam::view() {
    float mag =
            sqrt(target_at[0] * target_at[0] + target_at[1] * target_at[1] +
                 target_at[2] * target_at[2]);
    // (The target's y using target_at[0] rather than [1] is the
    // original's, kept as-is.)
    math::Mat4<float> const view_matrix = glm::lookAt(
            math::Vec3<float>(
                    target_pos[0] - 500 * target_at[0] / mag * (back_factor) +
                            shake_cam_pos[0],
                    target_pos[1] + up_factor + shake_cam_pos[1],
                    target_pos[2] - 500 * target_at[2] / mag +
                            shake_cam_pos[2]),
            math::Vec3<float>(target_pos[0] + 200 * target_at[0] / mag +
                                      shake_cam_pos[0],
                              target_pos[1] + 200 * target_at[0] / mag +
                                      shake_cam_pos[1],
                              target_pos[2] + 200 * target_at[2] / mag +
                                      shake_cam_pos[2]),
            math::Vec3<float>(0, 1, 0));
    updateShakeCam();
    return view_matrix;
}

void ChaseCam::updateFactor() {
    back_factor = 0;
    up_factor = 4000;
}

void ChaseCam::resetFactor() {
    back_factor = 1;
    up_factor = 100;
}

void ChaseCam::setShakeCam(std::int32_t magnitude) {
    shake_cam_pos[0] = magnitude;
    shake_cam_pos[1] = magnitude;
    shake_cam_pos[2] = magnitude;
}

void ChaseCam::updateShakeCam() {
    if (shake_cam_pos[0] != 0) {
        shake_cam_pos[0] = shake_cam_pos[0] / 1.015281239159713;
        if (shake_cam_pos[0] % 3 == 0) {
            shake_cam_pos[0] *= -1;
        }
    }
    if (shake_cam_pos[1] != 0) {
        shake_cam_pos[1] = shake_cam_pos[1] / 1.015281239159713;
        if (shake_cam_pos[1] % 3 == 1) {
            shake_cam_pos[1] *= -1;
        }
    }
    if (shake_cam_pos[2] != 0) {
        shake_cam_pos[2] = shake_cam_pos[2] / 1.015281239159713;
        if (shake_cam_pos[2] % 3 == 2) {
            shake_cam_pos[2] *= -1;
        }
    }
}
