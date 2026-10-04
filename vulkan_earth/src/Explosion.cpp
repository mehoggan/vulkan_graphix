/*
 * Explosion.cpp
 *
 *  Created on: Sep 16, 2010
 *      Author: Matthew Hoggan
 */

#include "vulkan_earth/Explosion.h"
#include <algorithm>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/OpenGLColors.h"
#include "vulkan_graphix/Math/MathTypes.hpp"

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

/*
 * Constructors and De-constructors
 */
Explosion::Explosion() = default;
Explosion::Explosion(float new_x,
                     float new_y,
                     float new_z,
                     std::int32_t new_weapon_radius) {
    std::copy_n(glm::value_ptr(math::Mat4<float>(1.0f)), 16, m_trans_matrix);
    m_x = new_x;
    m_y = new_y;
    m_z = new_z;
    m_trans_matrix[12] = m_x;
    m_trans_matrix[13] = m_y;
    m_trans_matrix[14] = m_z;
    m_weapon_radius = new_weapon_radius;

    float temp_colors1[3] = {White};
    float temp_colors2[3] = {Yellow};
    float temp_colors3[3] = {Orange};
    float temp_colors4[3] = {Red};
    for (std::int32_t i = 0; i < 3; i++) {
        m_colors1[i] = temp_colors1[i];
        m_colors2[i] = temp_colors2[i];
        m_colors3[i] = temp_colors3[i];
        m_colors4[i] = temp_colors4[i];
    }
}

Explosion::~Explosion() = default;

void Explosion::draw(render::RenderContext& context) {
    // The growth and color timeline are libvulkan_graphix's
    // EffectSimulation (shared with Tutorial20).
    const vulkan_graphix::EffectSimulation::ExplosionFrame frame =
            vulkan_graphix::EffectSimulation::advanceExplosion(m_simulation);
    const float* const colors[4] = {
            m_colors1, m_colors2, m_colors3, m_colors4};
    if (frame.m_color_index >= 0) {
        const float* color = colors[frame.m_color_index];
        m_current_color =
                math::Vec4<float>(color[0], color[1], color[2], frame.m_alpha);
    }

    const float sphere_radius =
            vulkan_graphix::EffectSimulation::explosionSphereRadius(
                    m_simulation, m_weapon_radius);
    context.drawMesh(
            render::Renderer::instance().sphere(90, 180),
            vulkan_earth::pipelines().m_flat_color,
            nullptr,
            glm::scale(glm::translate(math::Mat4<float>(1.0f),
                                      math::Vec3<float>(m_x, m_y, m_z)),
                       math::Vec3<float>(
                               sphere_radius, sphere_radius, sphere_radius)),
            m_current_color);
}

void Explosion::setColors1(float* new_colors1) {
    m_colors1[0] = new_colors1[0];
    m_colors1[1] = new_colors1[1];
    m_colors1[2] = new_colors1[2];
}
void Explosion::setColors2(float* new_colors2) {
    m_colors2[0] = new_colors2[0];
    m_colors2[1] = new_colors2[1];
    m_colors2[2] = new_colors2[2];
}
void Explosion::setColors3(float* new_colors3) {
    m_colors3[0] = new_colors3[0];
    m_colors3[1] = new_colors3[1];
    m_colors3[2] = new_colors3[2];
}
void Explosion::setColors4(float* new_colors4) {
    m_colors4[0] = new_colors4[0];
    m_colors4[1] = new_colors4[1];
    m_colors4[2] = new_colors4[2];
}
void Explosion::setDefaultColors() {
    float temp_colors1[3] = {White};
    float temp_colors2[3] = {Yellow};
    float temp_colors3[3] = {Orange};
    float temp_colors4[3] = {Red};
    for (std::int32_t i = 0; i < 3; i++) {
        m_colors1[i] = temp_colors1[i];
        m_colors2[i] = temp_colors2[i];
        m_colors3[i] = temp_colors3[i];
        m_colors4[i] = temp_colors4[i];
    }
}