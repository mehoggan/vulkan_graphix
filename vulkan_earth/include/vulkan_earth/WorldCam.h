#ifndef WORLDCAM_H
#define WORLDCAM_H

#include <cstdint>
#include "vulkan_earth/render/RenderTypes.h"

namespace vulkan_earth::render {
class RenderContext;
}

class WorldCam {
public:
    WorldCam();
    WorldCam(float x, float y, float z);
    ~WorldCam();
    // The camera's view matrix (what gluLookAt() applied).
    vulkan_earth::render::Mat4 view();
    void moveCam(float x, float y, float z);
    float* getMatrix();
    void setShakeCam(std::int32_t magnitude);
    void updateShakeCam();

private:
    float matrix[16];
    std::int32_t shake_cam_pos[3];
};

#endif