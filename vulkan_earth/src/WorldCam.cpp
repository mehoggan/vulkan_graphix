#include "vulkan_earth/WorldCam.h"
#include <algorithm>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vulkan_earth/MacroCrtdbg.h"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

WorldCam::WorldCam() = default;
WorldCam::WorldCam(float x, float y, float z) {
    matrix[0] = 0;
    matrix[1] = 0;
    matrix[2] = 1;
    matrix[3] = 0;
    matrix[4] = 1;
    matrix[5] = 0;
    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = 0;
    matrix[9] = -1;
    matrix[10] = 0;
    matrix[11] = 0;
    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
    matrix[15] = 0;
    std::copy_n(glm::value_ptr(glm::rotate(
                        glm::make_mat4(matrix),
                        glm::radians(static_cast<float>(-10)),
                        math::Vec3<float>(matrix[4], matrix[5], matrix[6]))),
                16,
                matrix);
    shake_cam_pos[0] = 0;
    shake_cam_pos[1] = 0;
    shake_cam_pos[2] = 0;
}

math::Mat4<float> WorldCam::view() {
    const math::Mat4<float> view_matrix = glm::lookAt(
            math::Vec3<float>(matrix[12] + shake_cam_pos[0],
                              matrix[13] + shake_cam_pos[1],
                              matrix[14] + shake_cam_pos[2]),
            math::Vec3<float>(matrix[12] + matrix[8] + shake_cam_pos[0],
                              matrix[13] + matrix[9] + shake_cam_pos[1],
                              matrix[14] + matrix[10] + shake_cam_pos[2]),
            math::Vec3<float>(matrix[4], matrix[5], matrix[6]));
    updateShakeCam();
    return view_matrix;
}

void WorldCam::moveCam(float x, float y, float z) {
    matrix[12] += x;
    matrix[13] += y;
    matrix[14] += z;
}

float* WorldCam::getMatrix() { return matrix; }

void WorldCam::setShakeCam(std::int32_t magnitude) {
    shake_cam_pos[0] = magnitude;
    shake_cam_pos[1] = magnitude;
    shake_cam_pos[2] = magnitude;
}

void WorldCam::updateShakeCam() {
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