#include "vulkan_earth/Tank.h"
#include <algorithm>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <optional>
#include "vulkan_earth/GameRenderer.h"
#include "vulkan_earth/ParticleGenerator.h"
#include "vulkan_earth/VBOShaderLibrary.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/TankOrientation.h"
#include "vulkan_graphix/TankPlacement.h"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

Tank::Tank() {
    m_hit_box_height = 200;
    m_hit_box_length = 400;
    m_hit_box_width = 300;
    m_tank_alive = true;
    m_smoke_gen = nullptr;
    m_acid_gen = nullptr;
    m_float_gen = nullptr;
}
Tank::~Tank() {
    if (m_smoke_gen) {
        m_smoke_gen->killGenerator();
        delete m_smoke_gen;
    }
    if (m_acid_gen) {
        m_acid_gen->killGenerator();
        delete m_acid_gen;
    }
    if (m_float_gen) {
        m_float_gen->killGenerator();
        delete m_float_gen;
    }
}

void Tank::printTurretMatrix() {
    cout << " Turret Matrix " << endl;
    cout << "|" << m_turret_matrix[0] << " " << m_turret_matrix[4] << " "
         << m_turret_matrix[8] << " " << m_turret_matrix[12] << "|" << endl;
    cout << "|" << m_turret_matrix[1] << " " << m_turret_matrix[5] << " "
         << m_turret_matrix[9] << " " << m_turret_matrix[13] << "|" << endl;
    cout << "|" << m_turret_matrix[2] << " " << m_turret_matrix[6] << " "
         << m_turret_matrix[10] << " " << m_turret_matrix[14] << "|" << endl;
    cout << "|" << m_turret_matrix[3] << " " << m_turret_matrix[7] << " "
         << m_turret_matrix[11] << " " << m_turret_matrix[15] << "|" << endl;
}

void Tank::printHeadMatrix() {
    cout << " Head Matrix " << endl;
    cout << "|" << m_head_matrix[0] << " " << m_head_matrix[4] << " "
         << m_head_matrix[8] << " " << m_head_matrix[12] << "|" << endl;
    cout << "|" << m_head_matrix[1] << " " << m_head_matrix[5] << " "
         << m_head_matrix[9] << " " << m_head_matrix[13] << "|" << endl;
    cout << "|" << m_head_matrix[2] << " " << m_head_matrix[6] << " "
         << m_head_matrix[10] << " " << m_head_matrix[14] << "|" << endl;
    cout << "|" << m_head_matrix[3] << " " << m_head_matrix[7] << " "
         << m_head_matrix[11] << " " << m_head_matrix[15] << "|" << endl;
}

void Tank::printBodyMatrix() {
    cout << " Body Matrix " << endl;
    cout << "|" << m_body_matrix[0] << " " << m_body_matrix[4] << " "
         << m_body_matrix[8] << " " << m_body_matrix[12] << "|" << endl;
    cout << "|" << m_body_matrix[1] << " " << m_body_matrix[5] << " "
         << m_body_matrix[9] << " " << m_body_matrix[13] << "|" << endl;
    cout << "|" << m_body_matrix[2] << " " << m_body_matrix[6] << " "
         << m_body_matrix[10] << " " << m_body_matrix[14] << "|" << endl;
    cout << "|" << m_body_matrix[3] << " " << m_body_matrix[7] << " "
         << m_body_matrix[11] << " " << m_body_matrix[15] << "|" << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
         << endl;
}

void Tank::setTankPos(float x, float y, float z) {
    // Keep Track of Tanks Position for Falling Damage
    m_previous_height = m_current_height;
    m_current_height = y;

    // Hierarchical placement (body -> head -> turret, each child's offset
    // rotated through its parent's basis) - libvulkan_graphix's
    // TankPlacement, shared with the tutorials that draw a tank.
    namespace vg = vulkan_graphix;
    const vg::TankPlacement::PartTranslations parts =
      vg::TankPlacement::composePartTranslations(
        vg::Math::Vec3<float>(x, y, z),
        glm::make_mat4(m_body_matrix),
        glm::make_mat4(m_head_matrix),
        {vg::Math::Vec3<float>(
           m_body_offset[0], m_body_offset[1], m_body_offset[2]),
          vg::Math::Vec3<float>(
            m_head_offset[0], m_head_offset[1], m_head_offset[2]),
          vg::Math::Vec3<float>(
            m_turret_offset[0], m_turret_offset[1], m_turret_offset[2])});
    for (std::int32_t i = 0; i < 3; ++i) {
        m_body_matrix[12 + i] = parts.m_body[i];
        m_head_matrix[12 + i] = parts.m_head[i];
        m_turret_matrix[12 + i] = parts.m_turret[i];
    }
    updateHitBox();
    if (m_smoke_gen)
        m_smoke_gen->update(
          m_body_matrix[12] + m_body_offset[0] + m_head_offset[0],
          m_body_matrix[13] + m_body_offset[1] + m_head_offset[1],
          m_body_matrix[14] + m_body_offset[2] + m_head_offset[2]);
}

void Tank::resetTurret() {
    m_turret_matrix[12] = m_head_matrix[12] +
      m_turret_offset[0] * m_head_matrix[0] +
      m_turret_offset[1] * m_head_matrix[4] +
      m_turret_offset[2] * m_head_matrix[8];
    m_turret_matrix[13] = m_head_matrix[13] +
      m_turret_offset[0] * m_head_matrix[1] +
      m_turret_offset[1] * m_head_matrix[5] +
      m_turret_offset[2] * m_head_matrix[9];
    m_turret_matrix[14] = m_head_matrix[14] +
      m_turret_offset[0] * m_head_matrix[2] +
      m_turret_offset[1] * m_head_matrix[6] +
      m_turret_offset[2] * m_head_matrix[10];
}

void Tank::orientTank(vulkan_graphix::Math::Vec3<float>* n) {
    m_rotate_degrees = 0;
    m_alignment_vector.x = n->x;
    m_alignment_vector.y = n->y;
    m_alignment_vector.z = n->z;

    const std::optional<vulkan_graphix::TankOrientation::Alignment> alignment =
      vulkan_graphix::TankOrientation::alignToGround(
        glm::make_mat4(m_body_matrix),
        vulkan_graphix::Math::Vec3<float>(n->x, n->y, n->z));
    if (alignment) {
        m_rotate_about.x = alignment->m_axis.x;
        m_rotate_about.y = alignment->m_axis.y;
        m_rotate_about.z = alignment->m_axis.z;
        // Body/head/turret/wheel all take the same aligned matrix, as the
        // original's four glGetFloatv(GL_MODELVIEW_MATRIX, ...) calls did.
        const float* aligned = glm::value_ptr(alignment->m_matrix);
        std::copy(aligned, aligned + 16, m_body_matrix);
        std::copy(aligned, aligned + 16, m_head_matrix);
        std::copy(aligned, aligned + 16, m_turret_matrix);
        std::copy(aligned, aligned + 16, m_wheel_matrix);
        m_turret_degrees = 0;
    } else {
        // Already upright along the normal: no rotation axis.
        m_rotate_about.x = 0;
        m_rotate_about.y = 0;
        m_rotate_about.z = 0;
    }

    updateHitBox();
}

void Tank::rotateHead(float degrees) {
    m_rotate_degrees += degrees;
    if (m_rotate_degrees > 360) {
        m_rotate_degrees -= 360;
    } else if (m_rotate_degrees < 0) {
        m_rotate_degrees += 360;
    }

    float angle = vulkan_graphix::TankOrientation::angleBetweenDegrees(
      vulkan_graphix::Math::Vec3<float>(
        m_head_matrix[4], m_head_matrix[5], m_head_matrix[6]),
      vulkan_graphix::Math::Vec3<float>(
        m_turret_matrix[4], m_turret_matrix[5], m_turret_matrix[6]));

    std::copy_n(glm::value_ptr(glm::rotate(glm::make_mat4(m_head_matrix),
                  glm::radians(static_cast<float>(degrees)),
                  math::Vec3<float>(0, 1, 0))),
      16,
      m_head_matrix);

    std::copy_n(glm::value_ptr(glm::rotate(glm::make_mat4(m_turret_matrix),
                  glm::radians(static_cast<float>(-angle)),
                  math::Vec3<float>(1, 0, 0))),
      16,
      m_turret_matrix);
    std::copy_n(
      glm::value_ptr(glm::translate(glm::make_mat4(m_turret_matrix),
        math::Vec3<float>(
          -m_turret_offset[0], -m_turret_offset[1], -m_turret_offset[2]))),
      16,
      m_turret_matrix);
    std::copy_n(glm::value_ptr(glm::rotate(glm::make_mat4(m_turret_matrix),
                  glm::radians(static_cast<float>(degrees)),
                  math::Vec3<float>(0, 1, 0))),
      16,
      m_turret_matrix);
    std::copy_n(
      glm::value_ptr(glm::translate(glm::make_mat4(m_turret_matrix),
        math::Vec3<float>(
          m_turret_offset[0], m_turret_offset[1], m_turret_offset[2]))),
      16,
      m_turret_matrix);
    std::copy_n(glm::value_ptr(glm::rotate(glm::make_mat4(m_turret_matrix),
                  glm::radians(static_cast<float>(angle)),
                  math::Vec3<float>(1, 0, 0))),
      16,
      m_turret_matrix);
    /*cout << "63. ";
    printTurretMatrix();
    printHeadMatrix();
    printBodyMatrix();*/
    /*cout << "64. ";
    printTurretMatrix();
    printHeadMatrix();
    printBodyMatrix();*/
    updateHitBox();
}

void Tank::rotateTurret(float degrees) {
    if ((m_turret_degrees + degrees <= 90) &&
      (m_turret_degrees + degrees >= 0)) {
        m_turret_degrees += degrees;
        std::copy_n(glm::value_ptr(glm::rotate(glm::make_mat4(m_turret_matrix),
                      glm::radians(static_cast<float>(degrees)),
                      math::Vec3<float>(1, 0, 0))),
          16,
          m_turret_matrix);
        // cout << "65. ";
        // printTurretMatrix();
        // printHeadMatrix();
        // printBodyMatrix();
        // cout << "66. ";
        // printTurretMatrix();
        // printHeadMatrix();
        // printBodyMatrix();
    }
}

bool Tank::checkCollision(float x, float y, float z) {
    // First check distance from tank, then check each face of hit box
    if (sqrt(pow((x - m_tank_pos.x), 2) + pow((y - m_tank_pos.y), 2) +
          pow((z - m_tank_pos.z), 2)) < 50000000) {
        // top
        float plane_x = m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
          m_hit_box_width / 2.0 * m_right.x + m_hit_box_height / 2.0 * m_up.x;
        float plane_y = m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
          m_hit_box_width / 2.0 * m_right.y + m_hit_box_height / 2.0 * m_up.y;
        float plane_z = m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
          m_hit_box_width / 2.0 * m_right.z + m_hit_box_height / 2.0 * m_up.z;
        float d = m_up.x * plane_x + m_up.y * plane_y + m_up.z * plane_z;
        if (m_up.x * x + m_up.y * y + m_up.z * z - d <= 0) {
            // right
            plane_x = m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
              m_hit_box_width / 2.0 * m_right.x +
              m_hit_box_height / 2.0 * m_up.x;
            plane_y = m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
              m_hit_box_width / 2.0 * m_right.y +
              m_hit_box_height / 2.0 * m_up.y;
            plane_z = m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
              m_hit_box_width / 2.0 * m_right.z +
              m_hit_box_height / 2.0 * m_up.z;
            d =
              m_right.x * plane_x + m_right.y * plane_y + m_right.z * plane_z;
            if (m_right.x * x + m_right.y * y + m_right.z * z - d <= 0) {
                // left
                plane_x = m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
                  m_hit_box_width / 2.0 * m_left.x +
                  m_hit_box_height / 2.0 * m_up.x;
                plane_y = m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
                  m_hit_box_width / 2.0 * m_left.y +
                  m_hit_box_height / 2.0 * m_up.y;
                plane_z = m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
                  m_hit_box_width / 2.0 * m_left.z +
                  m_hit_box_height / 2.0 * m_up.z;
                d =
                  m_left.x * plane_x + m_left.y * plane_y + m_left.z * plane_z;
                if (m_left.x * x + m_left.y * y + m_left.z * z - d <= 0) {
                    // front
                    plane_x = m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
                      m_hit_box_width / 2.0 * m_left.x +
                      m_hit_box_height / 2.0 * m_up.x;
                    plane_y = m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
                      m_hit_box_width / 2.0 * m_left.y +
                      m_hit_box_height / 2.0 * m_up.y;
                    plane_z = m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
                      m_hit_box_width / 2.0 * m_left.z +
                      m_hit_box_height / 2.0 * m_up.z;
                    d = m_at.x * plane_x + m_at.y * plane_y + m_at.z * plane_z;
                    if (m_at.x * x + m_at.y * y + m_at.z * z - d <= 0) {
                        // back
                        plane_x = m_tank_pos.x +
                          m_hit_box_length / 2.0 * m_back.x +
                          m_hit_box_width / 2.0 * m_left.x +
                          m_hit_box_height / 2.0 * m_up.x;
                        plane_y = m_tank_pos.y +
                          m_hit_box_length / 2.0 * m_back.y +
                          m_hit_box_width / 2.0 * m_left.y +
                          m_hit_box_height / 2.0 * m_up.y;
                        plane_z = m_tank_pos.z +
                          m_hit_box_length / 2.0 * m_back.z +
                          m_hit_box_width / 2.0 * m_left.z +
                          m_hit_box_height / 2.0 * m_up.z;
                        d = m_back.x * plane_x + m_back.y * plane_y +
                          m_back.z * plane_z;
                        if (m_back.x * x + m_back.y * y + m_back.z * z - d <=
                          0) {
                            cout << "COLLISION WITH TANK DETECTED" << endl;
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void Tank::dealDamage(std::int32_t damage) {
    if (damage < 0) cout << "ERROR: Negative damage";
    m_hp -= damage;
    if (m_hp <= 0) {
        m_hp = 0;
        m_tank_alive = false;
        initDuration();
        m_smoke_gen = new ParticleGenerator(10, 5, 1, 100, 0);
    }
}

void Tank::checkFallingDamage() {
    if (m_previous_height != m_current_height) {
        dealDamage((m_previous_height - m_current_height) / 10);
    }
}

bool Tank::isAlive() { return m_tank_alive; }

void Tank::tankRevive() {
    if (m_smoke_gen) {
        m_smoke_gen = nullptr;
    }
    m_tank_alive = true;
}

void Tank::adjustPower(float amount) {
    if (((m_current_power + amount) <= 10.0) &&
      ((m_current_power + amount) >= 0.0)) {
        m_current_power += amount;
    }
}

void Tank::initDuration() {
    m_duration_acid = 0;
    m_duration_shield = 0;
    m_duration_emp = 0;
    m_duration_float = 0;
    m_duration_double_action = 0;
    m_duration_padlock = 0;
    m_duration_cloak = 0;
    m_duration_paralyze = 0;
    delete m_acid_gen;
    delete m_float_gen;
}

void Tank::setBodyColor(float r, float g, float b, float a) {
    m_body_color[0] = r;
    m_body_color[1] = g;
    m_body_color[2] = b;
    m_body_color[3] = a;
}
void Tank::setBodyScale(float x, float y, float z) {
    m_body_scale[0] = x;
    m_body_scale[1] = y;
    m_body_scale[2] = z;
}

void Tank::setHeadColor(float r, float g, float b, float a) {
    m_head_color[0] = r;
    m_head_color[1] = g;
    m_head_color[2] = b;
    m_head_color[3] = a;
}
void Tank::setHeadScale(float x, float y, float z) {
    m_head_scale[0] = x;
    m_head_scale[1] = y;
    m_head_scale[2] = z;
}

void Tank::setTurretColor(float r, float g, float b, float a) {
    m_turret_color[0] = r;
    m_turret_color[1] = g;
    m_turret_color[2] = b;
    m_turret_color[3] = a;
}
void Tank::setTurretScale(float x, float y, float z) {
    m_turret_scale[0] = x;
    m_turret_scale[1] = y;
    m_turret_scale[2] = z;
}

void Tank::setWheelColor(float r, float g, float b, float a) {
    m_wheel_color[0] = r;
    m_wheel_color[1] = g;
    m_wheel_color[2] = b;
    m_wheel_color[3] = a;
}
void Tank::setWheelScale(float x, float y, float z) {
    m_wheel_scale[0] = x;
    m_wheel_scale[1] = y;
    m_wheel_scale[2] = z;
}
void Tank::setHP(std::int32_t new_hp) { m_hp = new_hp; }
void Tank::setPower(std::int32_t p) { m_power = p; }
void Tank::setArmor(std::int32_t a) { m_armor = a; }
void Tank::setSpeed(std::int32_t d) { m_speed = d; }

// Other functions
void Tank::fire() {}

void Tank::updateHitBox() {
    /*vulkan_graphix::Math::Vec3<float>
    tankPos(bodyMatrix[12],bodyMatrix[13],bodyMatrix[14]);
    vulkan_graphix::Math::Vec3<float>
    right(-bodyMatrix[0],-bodyMatrix[1],-bodyMatrix[2]);
    vulkan_graphix::Math::Vec3<float>
    up(bodyMatrix[4],bodyMatrix[5],bodyMatrix[6]);
    vulkan_graphix::Math::Vec3<float>
    at(-bodyMatrix[8],-bodyMatrix[9],-bodyMatrix[10]);

    vulkan_graphix::Math::Vec3<float>
    left(bodyMatrix[0],bodyMatrix[1],bodyMatrix[2]);
    vulkan_graphix::Math::Vec3<float>
    down(-bodyMatrix[4],-bodyMatrix[5],-bodyMatrix[6]);
    vulkan_graphix::Math::Vec3<float>
    back(bodyMatrix[8],bodyMatrix[9],bodyMatrix[10]);*/
    m_tank_pos.x = m_body_matrix[12];
    m_tank_pos.y = m_body_matrix[13];
    m_tank_pos.z = m_body_matrix[14];
    m_right.x = -m_body_matrix[0];
    m_right.y = -m_body_matrix[1];
    m_right.z = -m_body_matrix[2];
    m_left.x = m_body_matrix[0];
    m_left.y = m_body_matrix[1];
    m_left.z = m_body_matrix[2];
    m_up.x = m_body_matrix[4];
    m_up.y = m_body_matrix[5];
    m_up.z = m_body_matrix[6];
    m_down.x = -m_body_matrix[4];
    m_down.y = -m_body_matrix[5];
    m_down.z = -m_body_matrix[6];
    m_at.x = -m_body_matrix[8];
    m_at.y = -m_body_matrix[9];
    m_at.z = -m_body_matrix[10];
    m_back.x = m_body_matrix[8];
    m_back.y = m_body_matrix[9];
    m_back.z = m_body_matrix[10];
}

void Tank::keyHandler() {}

// Virtuals
std::int32_t Tank::getBaseHP() { return 0; }
std::int32_t Tank::getBasePower() { return 0; }
std::int32_t Tank::getBaseArmor() { return 0; }
std::int32_t Tank::getBaseSpeed() { return 0; }
std::string Tank::getName() { return "Huh?"; }
vulkan_graphix::Math::Vec3<float> Tank::getAlignmentVector() {
    return m_alignment_vector;
}
vulkan_graphix::Math::Vec3<float> Tank::getRotateAbout() {
    return m_rotate_about;
}
float Tank::getTurretDegrees() { return m_turret_degrees; }

void Tank::draw(render::RenderContext& context) {
    // Each part: its own matrix (glMultMatrixf) then its scale (glScalef).
    m_vbo_shader_turret->draw(context,
      glm::scale(glm::make_mat4(m_turret_matrix),
        math::Vec3<float>(
          m_turret_scale[0], m_turret_scale[1], m_turret_scale[2])));
    m_vbo_shader_head->draw(context,
      glm::scale(glm::make_mat4(m_head_matrix),
        math::Vec3<float>(m_head_scale[0], m_head_scale[1], m_head_scale[2])));
    const math::Mat4<float> body = glm::make_mat4(m_body_matrix);
    // float effect must be same orientation as the tank
    if (m_float_gen) {
        m_float_gen->update(0, -m_body_offset[1], 0);
        m_float_gen->draw(context, body);
    }
    // Draw tank body
    m_vbo_shader_body->draw(context,
      glm::scale(body,
        math::Vec3<float>(m_body_scale[0], m_body_scale[1], m_body_scale[2])));
    // shield: glutSolidSphere(300, 20, 20) in translucent blue
    if (m_duration_shield > 0) {
        context.drawMesh(render::Renderer::instance().sphere(20, 20),
          vulkan_earth::pipelines().m_flat_color,
          nullptr,
          glm::scale(body, math::Vec3<float>(300, 300, 300)),
          math::Vec4<float>(0, 0, 1, 0.2));
    }

    // update and draw particles
    if (m_smoke_gen) {
        m_smoke_gen->update(
          m_body_matrix[12] + m_body_offset[0] + m_head_offset[0],
          m_body_matrix[13] + m_body_offset[1] + m_head_offset[1],
          m_body_matrix[14] + m_body_offset[2] + m_head_offset[2]);
        m_smoke_gen->draw(context);
    }
    if (m_acid_gen) {
        m_acid_gen->update(
          m_body_matrix[12], m_body_matrix[13], m_body_matrix[14]);
        m_acid_gen->draw(context);
    }
}

void Tank::drawTankHitBox(render::RenderContext& context) {
    // Six translucent quads with per-vertex colors (debug view).
    std::vector<render::UiVertex> out_tris;
    std::vector<render::UiVertex> out_lines;
    std::vector<render::UiVertex> quad;
    math::Vec4<float> color(1.0f);
    // top
    color = math::Vec4<float>(1.0, 0.0, 0.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
          m_hit_box_width / 2.0 * m_right.x + m_hit_box_height / 2.0 * m_up.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
          m_hit_box_width / 2.0 * m_right.y + m_hit_box_height / 2.0 * m_up.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
          m_hit_box_width / 2.0 * m_right.z + m_hit_box_height / 2.0 * m_up.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 1.0, 0.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
          m_hit_box_width / 2.0 * m_left.x + m_hit_box_height / 2.0 * m_up.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
          m_hit_box_width / 2.0 * m_left.y + m_hit_box_height / 2.0 * m_up.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
          m_hit_box_width / 2.0 * m_left.z + m_hit_box_height / 2.0 * m_up.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 1.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_back.x +
          m_hit_box_width / 2.0 * m_left.x + m_hit_box_height / 2.0 * m_up.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_back.y +
          m_hit_box_width / 2.0 * m_left.y + m_hit_box_height / 2.0 * m_up.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_back.z +
          m_hit_box_width / 2.0 * m_left.z + m_hit_box_height / 2.0 * m_up.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(1.0, 1.0, 1.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_back.x +
          m_hit_box_width / 2.0 * m_right.x + m_hit_box_height / 2.0 * m_up.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_back.y +
          m_hit_box_width / 2.0 * m_right.y + m_hit_box_height / 2.0 * m_up.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_back.z +
          m_hit_box_width / 2.0 * m_right.z + m_hit_box_height / 2.0 * m_up.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    // right
    color = math::Vec4<float>(1.0, 0.0, 0.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
          m_hit_box_width / 2.0 * m_right.x + m_hit_box_height / 2.0 * m_up.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
          m_hit_box_width / 2.0 * m_right.y + m_hit_box_height / 2.0 * m_up.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
          m_hit_box_width / 2.0 * m_right.z + m_hit_box_height / 2.0 * m_up.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 1.0, 0.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
          m_hit_box_width / 2.0 * m_right.x +
          m_hit_box_height / 2.0 * m_down.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
          m_hit_box_width / 2.0 * m_right.y +
          m_hit_box_height / 2.0 * m_down.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
          m_hit_box_width / 2.0 * m_right.z +
          m_hit_box_height / 2.0 * m_down.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 1.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_back.x +
          m_hit_box_width / 2.0 * m_right.x +
          m_hit_box_height / 2.0 * m_down.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_back.y +
          m_hit_box_width / 2.0 * m_right.y +
          m_hit_box_height / 2.0 * m_down.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_back.z +
          m_hit_box_width / 2.0 * m_right.z +
          m_hit_box_height / 2.0 * m_down.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(1.0, 1.0, 1.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_back.x +
          m_hit_box_width / 2.0 * m_right.x + m_hit_box_height / 2.0 * m_up.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_back.y +
          m_hit_box_width / 2.0 * m_right.y + m_hit_box_height / 2.0 * m_up.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_back.z +
          m_hit_box_width / 2.0 * m_right.z + m_hit_box_height / 2.0 * m_up.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    // left
    color = math::Vec4<float>(1.0, 0.0, 0.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
          m_hit_box_width / 2.0 * m_left.x + m_hit_box_height / 2.0 * m_up.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
          m_hit_box_width / 2.0 * m_left.y + m_hit_box_height / 2.0 * m_up.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
          m_hit_box_width / 2.0 * m_left.z + m_hit_box_height / 2.0 * m_up.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 1.0, 0.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
          m_hit_box_width / 2.0 * m_left.x + m_hit_box_height / 2.0 * m_down.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
          m_hit_box_width / 2.0 * m_left.y + m_hit_box_height / 2.0 * m_down.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
          m_hit_box_width / 2.0 * m_left.z +
          m_hit_box_height / 2.0 * m_down.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 1.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_back.x +
          m_hit_box_width / 2.0 * m_left.x + m_hit_box_height / 2.0 * m_down.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_back.y +
          m_hit_box_width / 2.0 * m_left.y + m_hit_box_height / 2.0 * m_down.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_back.z +
          m_hit_box_width / 2.0 * m_left.z +
          m_hit_box_height / 2.0 * m_down.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(1.0, 1.0, 1.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_back.x +
          m_hit_box_width / 2.0 * m_left.x + m_hit_box_height / 2.0 * m_up.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_back.y +
          m_hit_box_width / 2.0 * m_left.y + m_hit_box_height / 2.0 * m_up.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_back.z +
          m_hit_box_width / 2.0 * m_left.z + m_hit_box_height / 2.0 * m_up.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    // front
    color = math::Vec4<float>(1.0, 0.0, 0.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
          m_hit_box_width / 2.0 * m_right.x + m_hit_box_height / 2.0 * m_up.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
          m_hit_box_width / 2.0 * m_right.y + m_hit_box_height / 2.0 * m_up.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
          m_hit_box_width / 2.0 * m_right.z + m_hit_box_height / 2.0 * m_up.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 1.0, 0.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
          m_hit_box_width / 2.0 * m_left.x + m_hit_box_height / 2.0 * m_up.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
          m_hit_box_width / 2.0 * m_left.y + m_hit_box_height / 2.0 * m_up.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
          m_hit_box_width / 2.0 * m_left.z + m_hit_box_height / 2.0 * m_up.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 1.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
          m_hit_box_width / 2.0 * m_left.x + m_hit_box_height / 2.0 * m_down.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
          m_hit_box_width / 2.0 * m_left.y + m_hit_box_height / 2.0 * m_down.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
          m_hit_box_width / 2.0 * m_left.z +
          m_hit_box_height / 2.0 * m_down.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 0.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_at.x +
          m_hit_box_width / 2.0 * m_right.x +
          m_hit_box_height / 2.0 * m_down.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_at.y +
          m_hit_box_width / 2.0 * m_right.y +
          m_hit_box_height / 2.0 * m_down.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_at.z +
          m_hit_box_width / 2.0 * m_right.z +
          m_hit_box_height / 2.0 * m_down.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    // back
    color = math::Vec4<float>(1.0, 0.0, 0.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_back.x +
          m_hit_box_width / 2.0 * m_right.x + m_hit_box_height / 2.0 * m_up.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_back.y +
          m_hit_box_width / 2.0 * m_right.y + m_hit_box_height / 2.0 * m_up.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_back.z +
          m_hit_box_width / 2.0 * m_right.z + m_hit_box_height / 2.0 * m_up.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 1.0, 0.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_back.x +
          m_hit_box_width / 2.0 * m_left.x + m_hit_box_height / 2.0 * m_up.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_back.y +
          m_hit_box_width / 2.0 * m_left.y + m_hit_box_height / 2.0 * m_up.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_back.z +
          m_hit_box_width / 2.0 * m_left.z + m_hit_box_height / 2.0 * m_up.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 1.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_back.x +
          m_hit_box_width / 2.0 * m_left.x + m_hit_box_height / 2.0 * m_down.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_back.y +
          m_hit_box_width / 2.0 * m_left.y + m_hit_box_height / 2.0 * m_down.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_back.z +
          m_hit_box_width / 2.0 * m_left.z +
          m_hit_box_height / 2.0 * m_down.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 0.0, .75);
    quad.push_back(render::UiVertex{
      math::Vec3<float>(m_tank_pos.x + m_hit_box_length / 2.0 * m_back.x +
          m_hit_box_width / 2.0 * m_right.x +
          m_hit_box_height / 2.0 * m_down.x,
        m_tank_pos.y + m_hit_box_length / 2.0 * m_back.y +
          m_hit_box_width / 2.0 * m_right.y +
          m_hit_box_height / 2.0 * m_down.y,
        m_tank_pos.z + m_hit_box_length / 2.0 * m_back.z +
          m_hit_box_width / 2.0 * m_right.z +
          m_hit_box_height / 2.0 * m_down.z),
      color,
      math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    context.drawTransient(
      out_tris, vulkan_earth::pipelines().m_ui_triangles, nullptr);
}

float Tank::getCurrentPower() { return m_current_power; }
std::int32_t Tank::getPreviousPower() { return m_previous_power; }
std::int32_t Tank::getPreviousAngle() { return m_previous_angle; }
float* Tank::getProjectileLandPos() { return m_projectile_land_pos; }
void Tank::setPreviousPower(std::int32_t new_previous_power) {
    m_previous_power = new_previous_power;
}
void Tank::setPreviousAngle(std::int32_t new_previous_angle) {
    m_previous_angle = new_previous_angle;
}
void Tank::setProjectileLandPos(float x, float y) {
    m_projectile_land_pos[0] = x;
    m_projectile_land_pos[1] = y;
}

// remove later probably
void Tank::rotateWheel(float degrees) {
    if (!(m_wheel_degrees + degrees > 90) || (m_wheel_degrees - degrees < 0)) {
        m_wheel_degrees += degrees;
        std::copy_n(
          glm::value_ptr(glm::rotate(glm::make_mat4(m_wheel_matrix),
            glm::radians(static_cast<float>(degrees)),
            math::Vec3<float>(
              m_wheel_matrix[4], m_wheel_matrix[5], m_wheel_matrix[6]))),
          16,
          m_wheel_matrix);
    }
}

void Tank::changeHeadTexture(std::int32_t current_player_index) {
    if (current_player_index == 0)
        m_vbo_shader_head->swapTexture("player1Head.raw", 1024, 1024);
    else if (current_player_index == 1)
        m_vbo_shader_head->swapTexture("player2Head.raw", 1024, 1024);
    else if (current_player_index == 2)
        m_vbo_shader_head->swapTexture("player3Head.raw", 1024, 1024);
    else if (current_player_index == 3)
        m_vbo_shader_head->swapTexture("player4Head.raw", 1024, 1024);
    else if (current_player_index == 4)
        m_vbo_shader_head->swapTexture("player5Head.raw", 1024, 1024);
    else if (current_player_index == 5)
        m_vbo_shader_head->swapTexture("player6Head.raw", 1024, 1024);
    else if (current_player_index == 6)
        m_vbo_shader_head->swapTexture("player7Head.raw", 1024, 1024);
    else if (current_player_index == 7)
        m_vbo_shader_head->swapTexture("player8Head.raw", 1024, 1024);
    else if (current_player_index == 8)
        m_vbo_shader_head->swapTexture("player9Head.raw", 1024, 1024);
    else if (current_player_index == 9)
        m_vbo_shader_head->swapTexture("player10Head.raw", 1024, 1024);
    else {
        printf(
          "\nERROR <Tank::changeHeadTexture(int)>: Wrong parameter "
          "passing.\n");
    }
}

std::int32_t Tank::getCurrentHeight() { return m_current_height; }

void Tank::setCurrentHeight(std::int32_t curr_height) {
    m_current_height = curr_height;
}

std::int32_t Tank::getPreviousHeight() { return m_previous_height; }

void Tank::setPreviousHeight(std::int32_t prev_height) {
    m_previous_height = prev_height;
}

// Getters for durations
std::int32_t Tank::getDurationAcid() { return m_duration_acid; }
std::int32_t Tank::getDurationShield() { return m_duration_shield; }
std::int32_t Tank::getDurationEMP() { return m_duration_emp; }
std::int32_t Tank::getDurationFloat() { return m_duration_float; }
std::int32_t Tank::getDurationDoubleAction() {
    return m_duration_double_action;
}
std::int32_t Tank::getDurationPadlock() { return m_duration_padlock; }
std::int32_t Tank::getDurationCloak() { return m_duration_cloak; }
std::int32_t Tank::getDurationParalyze() { return m_duration_paralyze; }
// Setters for durations
void Tank::setDurationAcid(std::int32_t value) {
    if (value != 0) {
        delete m_acid_gen;
        m_acid_gen = new ParticleGenerator(10, 5, 2, 100, 1);
    } else {
        if (m_acid_gen) {
            m_acid_gen->killGenerator();
            m_acid_gen = nullptr;
        }
    }
    m_duration_acid = value;
}
void Tank::setDurationShield(std::int32_t value) { m_duration_shield = value; }
void Tank::setDurationEMP(std::int32_t value) { m_duration_emp = value; }
void Tank::setDurationFloat(std::int32_t value) {
    if (value != 0) {
        delete m_float_gen;
        m_float_gen = new ParticleGenerator(10, 5, 2, 100, 2);
    } else {
        if (m_float_gen) {
            m_float_gen->killGenerator();
            m_float_gen = nullptr;
        }
    }
    m_duration_float = value;
}
void Tank::setDurationDoubleAction(std::int32_t value) {
    m_duration_double_action = value;
}
void Tank::setDurationPadlock(std::int32_t value) {
    m_duration_padlock = value;
}
void Tank::setDurationCloak(std::int32_t value) { m_duration_cloak = value; }
void Tank::setDurationParalyze(std::int32_t value) {
    m_duration_paralyze = value;
}
void Tank::setDurationAllPassTurn() {
    // acid
    if (m_duration_acid > 0) {
        m_duration_acid--;
    }
    if ((m_acid_gen) && (m_duration_acid == 0)) {
        m_acid_gen->killGenerator();
        m_acid_gen = nullptr;
    }

    // float
    if (m_duration_float > 0) {
        m_duration_float--;
    }
    if ((m_float_gen) && (m_duration_float == 0)) {
        m_float_gen->killGenerator();
        m_float_gen = nullptr;
    }
    if (m_duration_emp > 0) m_duration_emp--;
    if (m_duration_double_action > 0) m_duration_double_action--;
    if (m_duration_padlock > 0) m_duration_padlock--;
    if (m_duration_cloak > 0) m_duration_cloak--;
    if (m_duration_paralyze > 0) m_duration_paralyze--;
}
void Tank::initBody() {
    // The upright right/up/at basis every tank part starts from
    // (libvulkan_graphix's TankPlacement, shared with the tutorials).
    std::copy_n(
      glm::value_ptr(vulkan_graphix::TankPlacement::uprightPartBasis()),
      16,
      m_body_matrix);
    for (std::int32_t i = 0; i < 3; i++) {
        m_body_right[i] = m_body_matrix[i];
        m_body_up[i] = m_body_matrix[4 + i];
        m_body_at[i] = m_body_matrix[8 + i];
    }

    m_body_color[0] = 0.50f;
    m_body_color[1] = 0.50f;
    m_body_color[2] = 0.50f;
    m_body_color[3] = 1.0f;
    m_body_scale[0] = 70;
    m_body_scale[1] = 70;
    m_body_scale[2] = 70;
    m_previous_height = m_current_height = m_body_matrix[13];
}
void Tank::initHead() {
    // The upright right/up/at basis every tank part starts from
    // (libvulkan_graphix's TankPlacement, shared with the tutorials).
    std::copy_n(
      glm::value_ptr(vulkan_graphix::TankPlacement::uprightPartBasis()),
      16,
      m_head_matrix);
    for (std::int32_t i = 0; i < 3; i++) {
        m_head_right[i] = m_head_matrix[i];
        m_head_up[i] = m_head_matrix[4 + i];
        m_head_at[i] = m_head_matrix[8 + i];
    }
    m_head_color[0] = 0.50f;
    m_head_color[1] = 0.50f;
    m_head_color[2] = 0.50f;
    m_head_color[3] = 1.0f;
    m_head_scale[0] = 70;
    m_head_scale[1] = 70;
    m_head_scale[2] = 70;
}
void Tank::initTurret() {
    // The upright right/up/at basis every tank part starts from
    // (libvulkan_graphix's TankPlacement, shared with the tutorials).
    std::copy_n(
      glm::value_ptr(vulkan_graphix::TankPlacement::uprightPartBasis()),
      16,
      m_turret_matrix);
    for (std::int32_t i = 0; i < 3; i++) {
        m_turret_right[i] = m_turret_matrix[i];
        m_turret_up[i] = m_turret_matrix[4 + i];
        m_turret_at[i] = m_turret_matrix[8 + i];
    }
    m_turret_color[0] = 0.50f;
    m_turret_color[1] = 0.50f;
    m_turret_color[2] = 0.50f;
    m_turret_color[3] = 1.0f;
    m_turret_scale[0] = 70;
    m_turret_scale[1] = 70;
    m_turret_scale[2] = 70;

    m_turret_offset[0] = 0;
    m_turret_offset[1] = 0;
    m_turret_offset[2] = 0;

    m_turret_degrees = 0;
    m_wheel_degrees = 0;
}
void Tank::initWheel() {
    // The upright right/up/at basis every tank part starts from
    // (libvulkan_graphix's TankPlacement, shared with the tutorials).
    std::copy_n(
      glm::value_ptr(vulkan_graphix::TankPlacement::uprightPartBasis()),
      16,
      m_wheel_matrix);
    for (std::int32_t i = 0; i < 3; i++) {
        m_wheel_right[i] = m_wheel_matrix[i];
        m_wheel_up[i] = m_wheel_matrix[4 + i];
        m_wheel_at[i] = m_wheel_matrix[8 + i];
    }
    m_wheel_color[0] = 0.50f;
    m_wheel_color[1] = 0.50f;
    m_wheel_color[2] = 0.50f;
    m_wheel_color[3] = 1.0f;
    m_wheel_scale[0] = 70;
    m_wheel_scale[1] = 70;
    m_wheel_scale[2] = 70;
}

// GETTERS
const float* Tank::getBodyMatrix() { return &m_body_matrix[0]; }
const float* Tank::getBodyColor() { return &m_body_color[0]; }
float* Tank::getBodyScale() { return &m_body_scale[0]; }

const float* Tank::getHeadMatrix() { return &m_head_matrix[0]; }
const float* Tank::getHeadColor() { return &m_head_color[0]; }
float* Tank::getHeadScale() { return &m_head_scale[0]; }

float* Tank::getTurretMatrix() { return &m_turret_matrix[0]; }
const float* Tank::getTurretColor() { return &m_turret_color[0]; }
float* Tank::getTurretScale() { return &m_turret_scale[0]; }

const float* Tank::getWheelMatrix() { return &m_wheel_matrix[0]; }
const float* Tank::getWheelColor() { return &m_wheel_color[0]; }
float* Tank::getWheelScale() { return &m_wheel_scale[0]; }

std::int32_t Tank::getHP() { return m_hp; }
std::int32_t Tank::getPower() { return m_power; }
std::int32_t Tank::getArmor() { return m_armor; }
std::int32_t Tank::getSpeed() { return m_speed; }
Tank* Tank::getTankPointer() { return this; }