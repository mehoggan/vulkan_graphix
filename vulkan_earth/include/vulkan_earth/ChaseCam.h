#ifndef CHASECAM_H
#define CHASECAM_H

#include <cstdint>
#include "vulkan_earth/render/RenderTypes.h"

namespace vulkan_earth::render {
class RenderContext;
}

class ChaseCam {
public:
    ChaseCam();
    ChaseCam(float* new_target_pos, float* new_target_at);
    ~ChaseCam();
    // The camera's view matrix (what gluLookAt() applied).
    vulkan_earth::render::Mat4 view();
    void setShakeCam(std::int32_t magnitude);
    void updateShakeCam();
    void updateFactor();
    void resetFactor();

private:
    float* target_pos;
    float* target_at;
    std::int32_t shake_cam_pos[3];
    float back_factor;
    float up_factor;
};

#endif