/*
 * skybox.h
 *
 *  Created on: Sep 10, 2010
 *      Author: Matthew Hoggan
 */
#ifndef EXPLOSION_H_
#define EXPLOSION_H_

#include <cstdint>
#include "vulkan_earth/SpecialEffect.h"
#include "vulkan_graphix/EffectSimulation.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Renderer.h"

using namespace std;

namespace vulkan_graphix::Render {
class RenderContext;
}

class Explosion : public SpecialEffect {
public:
    /*
     * Constructors and De-constructor
     */
    Explosion();
    Explosion(float new_x,
              float new_y,
              float new_z,
              std::int32_t new_weapon_radius);
    ~Explosion() override;
    void draw(vulkan_graphix::Render::RenderContext& context) override;
    void setColors1(float* new_colors1) override;
    void setColors2(float* new_colors2) override;
    void setColors3(float* new_colors3) override;
    void setColors4(float* new_colors4) override;
    void setDefaultColors() override;

private:
    float x;
    float y;
    float z;
    float time;
    float trans_matrix[16];
    // The color the original left in effect where draw() sets none
    // (75 <= timer < 100): its own last one.
    vulkan_graphix::Math::Vec4<float> current_color =
            vulkan_graphix::Math::Vec4<float>(1.0f);
    vulkan_graphix::EffectSimulation::Explosion simulation;
    float colors1[3];
    float colors2[3];
    float colors3[3];
    float colors4[3];
    std::int32_t weapon_radius;
};

#endif /* EXPLOSION_H_ */