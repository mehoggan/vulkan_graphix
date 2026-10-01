#ifndef WORLDCAM_H
#define WORLDCAM_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>

class WorldCam {
public:
    WorldCam();
    WorldCam(float x, float y, float z);
    ~WorldCam();
    void view();
    void moveCam(float x, float y, float z);
    float* getMatrix();
    void setShakeCam(std::int32_t magnitude);
    void updateShakeCam();

private:
    float matrix[16];
    std::int32_t shake_cam_pos[3];
};

#endif