#include "ChaseCam.h"
#include "macro_crtdbg.h"
#include "math.h"

ChaseCam::ChaseCam() = default;
ChaseCam::ChaseCam(GLfloat* target_pos, GLfloat* target_at) {
    this->targetPos = target_pos;
    this->targetAt = target_at;
    shakeCamPos[0] = 0;
    shakeCamPos[1] = 0;
    shakeCamPos[2] = 0;
    backFactor = 1;
    upFactor = 1;
}
ChaseCam::~ChaseCam() = default;

void ChaseCam::view() {
    GLfloat mag = sqrt(this->targetAt[0] * this->targetAt[0] +
                       this->targetAt[1] * this->targetAt[1] +
                       this->targetAt[2] * this->targetAt[2]);
    gluLookAt(targetPos[0] - 500 * this->targetAt[0] / mag * (backFactor) +
                      shakeCamPos[0],
              targetPos[1] + upFactor + shakeCamPos[1],
              targetPos[2] - 500 * this->targetAt[2] / mag + shakeCamPos[2],
              targetPos[0] + 200 * this->targetAt[0] / mag + shakeCamPos[0],
              targetPos[1] + 200 * this->targetAt[0] / mag + shakeCamPos[1],
              targetPos[2] + 200 * this->targetAt[2] / mag + shakeCamPos[2],
              0,
              1,
              0);
    updateShakeCam();
}

void ChaseCam::updateFactor() {
    backFactor = 0;
    upFactor = 4000;
}

void ChaseCam::resetFactor() {
    backFactor = 1;
    upFactor = 100;
}

void ChaseCam::setShakeCam(int magnitude) {
    shakeCamPos[0] = magnitude;
    shakeCamPos[1] = magnitude;
    shakeCamPos[2] = magnitude;
}

void ChaseCam::updateShakeCam() {
    if (shakeCamPos[0] != 0) {
        shakeCamPos[0] = shakeCamPos[0] / 1.015281239159713;
        if (shakeCamPos[0] % 3 == 0) {
            shakeCamPos[0] *= -1;
        }
    }
    if (shakeCamPos[1] != 0) {
        shakeCamPos[1] = shakeCamPos[1] / 1.015281239159713;
        if (shakeCamPos[1] % 3 == 1) {
            shakeCamPos[1] *= -1;
        }
    }
    if (shakeCamPos[2] != 0) {
        shakeCamPos[2] = shakeCamPos[2] / 1.015281239159713;
        if (shakeCamPos[2] % 3 == 2) {
            shakeCamPos[2] *= -1;
        }
    }
}
