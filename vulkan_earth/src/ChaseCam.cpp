#include "ChaseCam.h"
#include "macro_crtdbg.h"
#include "math.h"

ChaseCam::ChaseCam() = default;
ChaseCam::ChaseCam(GLfloat* target_pos, GLfloat* target_at) {
    this->target_pos = target_pos;
    this->target_at = target_at;
    shake_cam_pos[0] = 0;
    shake_cam_pos[1] = 0;
    shake_cam_pos[2] = 0;
    back_factor = 1;
    up_factor = 1;
}
ChaseCam::~ChaseCam() = default;

void ChaseCam::view() {
    GLfloat mag = sqrt(this->target_at[0] * this->target_at[0] +
                       this->target_at[1] * this->target_at[1] +
                       this->target_at[2] * this->target_at[2]);
    gluLookAt(
            target_pos[0] - 500 * this->target_at[0] / mag * (back_factor) +
                    shake_cam_pos[0],
            target_pos[1] + up_factor + shake_cam_pos[1],
            target_pos[2] - 500 * this->target_at[2] / mag + shake_cam_pos[2],
            target_pos[0] + 200 * this->target_at[0] / mag + shake_cam_pos[0],
            target_pos[1] + 200 * this->target_at[0] / mag + shake_cam_pos[1],
            target_pos[2] + 200 * this->target_at[2] / mag + shake_cam_pos[2],
            0,
            1,
            0);
    updateShakeCam();
}

void ChaseCam::updateFactor() {
    back_factor = 0;
    up_factor = 4000;
}

void ChaseCam::resetFactor() {
    back_factor = 1;
    up_factor = 100;
}

void ChaseCam::setShakeCam(int magnitude) {
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
