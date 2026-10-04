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
    std::copy_n(glm::value_ptr(math::Mat4<float>(1.0f)), 16, trans_matrix);
    x = new_x;
    y = new_y;
    z = new_z;
    trans_matrix[12] = x;
    trans_matrix[13] = y;
    trans_matrix[14] = z;
    weapon_radius = new_weapon_radius;

    float temp_colors1[3] = {White};
    float temp_colors2[3] = {Yellow};
    float temp_colors3[3] = {Orange};
    float temp_colors4[3] = {Red};
    for (std::int32_t i = 0; i < 3; i++) {
        colors1[i] = temp_colors1[i];
        colors2[i] = temp_colors2[i];
        colors3[i] = temp_colors3[i];
        colors4[i] = temp_colors4[i];
    }
}

Explosion::~Explosion() = default;

void Explosion::draw(render::RenderContext& context) {
    // The growth and color timeline are libvulkan_graphix's
    // EffectSimulation (shared with Tutorial20).
    vulkan_graphix::EffectSimulation::ExplosionFrame const frame =
            vulkan_graphix::EffectSimulation::advanceExplosion(simulation);
    float const* const colors[4] = {colors1, colors2, colors3, colors4};
    if (frame.color_index >= 0) {
        float const* color = colors[frame.color_index];
        current_color =
                math::Vec4<float>(color[0], color[1], color[2], frame.alpha);
    }

    float const sphere_radius =
            vulkan_graphix::EffectSimulation::explosionSphereRadius(
                    simulation, weapon_radius);
    context.drawMesh(
            render::Renderer::instance().sphere(90, 180),
            vulkan_earth::pipelines().flat_color,
            nullptr,
            glm::scale(glm::translate(math::Mat4<float>(1.0f),
                                      math::Vec3<float>(x, y, z)),
                       math::Vec3<float>(
                               sphere_radius, sphere_radius, sphere_radius)),
            current_color);
}

void Explosion::setColors1(float* new_colors1) {
    colors1[0] = new_colors1[0];
    colors1[1] = new_colors1[1];
    colors1[2] = new_colors1[2];
}
void Explosion::setColors2(float* new_colors2) {
    colors2[0] = new_colors2[0];
    colors2[1] = new_colors2[1];
    colors2[2] = new_colors2[2];
}
void Explosion::setColors3(float* new_colors3) {
    colors3[0] = new_colors3[0];
    colors3[1] = new_colors3[1];
    colors3[2] = new_colors3[2];
}
void Explosion::setColors4(float* new_colors4) {
    colors4[0] = new_colors4[0];
    colors4[1] = new_colors4[1];
    colors4[2] = new_colors4[2];
}
void Explosion::setDefaultColors() {
    float temp_colors1[3] = {White};
    float temp_colors2[3] = {Yellow};
    float temp_colors3[3] = {Orange};
    float temp_colors4[3] = {Red};
    for (std::int32_t i = 0; i < 3; i++) {
        colors1[i] = temp_colors1[i];
        colors2[i] = temp_colors2[i];
        colors3[i] = temp_colors3[i];
        colors4[i] = temp_colors4[i];
    }
}