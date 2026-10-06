/*
 * skybox.h
 *
 *  Created on: Sep 10, 2010
 *      Author: Matthew Hoggan
 */
#ifndef VULKAN_EARTH_EXPLOSION_H
#define VULKAN_EARTH_EXPLOSION_H

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
  Explosion(
      float new_x, float new_y, float new_z, std::int32_t new_weapon_radius);
  ~Explosion() override;
  void draw(vulkan_graphix::Render::RenderContext& context) override;
  void setColors1(float* new_colors1) override;
  void setColors2(float* new_colors2) override;
  void setColors3(float* new_colors3) override;
  void setColors4(float* new_colors4) override;
  void setDefaultColors() override;

private:
  float m_x;
  float m_y;
  float m_z;
  float m_time;
  float m_trans_matrix[16];
  // The color the original left in effect where draw() sets none
  // (75 <= timer < 100): its own last one.
  vulkan_graphix::Math::Vec4<float> m_current_color =
      vulkan_graphix::Math::Vec4<float>(1.0f);
  vulkan_graphix::EffectSimulation::Explosion m_simulation;
  float m_colors1[3];
  float m_colors2[3];
  float m_colors3[3];
  float m_colors4[3];
  std::int32_t m_weapon_radius;
};

#endif  // VULKAN_EARTH_EXPLOSION_H