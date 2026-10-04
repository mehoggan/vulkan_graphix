#ifndef VULKAN_EARTH_SPECIALEFFECT_H
#define VULKAN_EARTH_SPECIALEFFECT_H

#include "vulkan_graphix/Render/Renderer.h"

namespace vulkan_graphix::Render {
class RenderContext;
}

class SpecialEffect {
public:
    SpecialEffect();
    virtual ~SpecialEffect();
    virtual void draw(vulkan_graphix::Render::RenderContext& context) = 0;
    virtual void setColors1(float* colors1) = 0;
    virtual void setColors2(float* colors2) = 0;
    virtual void setColors3(float* colors3) = 0;
    virtual void setColors4(float* colors4) = 0;
    virtual void setDefaultColors() = 0;
};

#endif  // VULKAN_EARTH_SPECIALEFFECT_H