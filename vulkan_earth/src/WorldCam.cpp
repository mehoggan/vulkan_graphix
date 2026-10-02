#include "vulkan_earth/WorldCam.h"
#include <cstdint>
#include "vulkan_earth/render/Camera.h"
#include "vulkan_earth/render/GlMatrix.h"
#include "vulkan_earth/MacroCrtdbg.h"

namespace render = vulkan_earth::render;

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
    render::glmatrix::rotate(matrix, -10, matrix[4], matrix[5], matrix[6]);
    shake_cam_pos[0] = 0;
    shake_cam_pos[1] = 0;
    shake_cam_pos[2] = 0;
}
WorldCam::~WorldCam() = default;

render::Mat4 WorldCam::view() {
    render::Mat4 const view_matrix = render::camera::lookAt(
            render::Vec3(matrix[12] + shake_cam_pos[0],
                         matrix[13] + shake_cam_pos[1],
                         matrix[14] + shake_cam_pos[2]),
            render::Vec3(matrix[12] + matrix[8] + shake_cam_pos[0],
                         matrix[13] + matrix[9] + shake_cam_pos[1],
                         matrix[14] + matrix[10] + shake_cam_pos[2]),
            render::Vec3(matrix[4], matrix[5], matrix[6]));
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