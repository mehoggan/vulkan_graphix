#ifndef WORLDCAM_H
#define WORLDCAM_H

#include <cstdint>
#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Render {
class RenderContext;
}

class WorldCam {
public:
    WorldCam();
    WorldCam(float x, float y, float z);
    ~WorldCam() = default;
    // The camera's view matrix (what gluLookAt() applied).
    vulkan_graphix::Math::Mat4<float> view();
    void moveCam(float x, float y, float z);
    float* getMatrix();
    void setShakeCam(std::int32_t magnitude);
    void updateShakeCam();

private:
    float matrix[16];
    std::int32_t shake_cam_pos[3];
};

#endif