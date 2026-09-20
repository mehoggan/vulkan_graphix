#include "WorldCam.h"
#include "macro_crtdbg.h"

WorldCam::WorldCam() = default;
WorldCam::WorldCam(GLfloat x, GLfloat y, GLfloat z) {
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
    glPushMatrix();
    glLoadMatrixf(matrix);
    glRotatef(-10, matrix[4], matrix[5], matrix[6]);
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
    glPopMatrix();
    shakeCamPos[0] = 0;
    shakeCamPos[1] = 0;
    shakeCamPos[2] = 0;
}
WorldCam::~WorldCam() = default;

void WorldCam::view() {
    gluLookAt(matrix[12] + shakeCamPos[0],
              matrix[13] + shakeCamPos[1],
              matrix[14] + shakeCamPos[2],
              matrix[12] + matrix[8] + shakeCamPos[0],
              matrix[13] + matrix[9] + shakeCamPos[1],
              matrix[14] + matrix[10] + shakeCamPos[2],
              matrix[4],
              matrix[5],
              matrix[6]);
    updateShakeCam();
}

void WorldCam::moveCam(float x, float y, float z) {
    matrix[12] += x;
    matrix[13] += y;
    matrix[14] += z;
}

GLfloat* WorldCam::getMatrix() { return matrix; }

void WorldCam::setShakeCam(int magnitude) {
    shakeCamPos[0] = magnitude;
    shakeCamPos[1] = magnitude;
    shakeCamPos[2] = magnitude;
}

void WorldCam::updateShakeCam() {
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