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
    hit_box_height = 200;
    hit_box_length = 400;
    hit_box_width = 300;
    tank_alive = true;
    smoke_gen = nullptr;
    acid_gen = nullptr;
    float_gen = nullptr;
}
Tank::~Tank() {
    if (smoke_gen) {
        smoke_gen->killGenerator();
        delete smoke_gen;
    }
    if (acid_gen) {
        acid_gen->killGenerator();
        delete acid_gen;
    }
    if (float_gen) {
        float_gen->killGenerator();
        delete float_gen;
    }
}

void Tank::printTurretMatrix() {
    cout << " Turret Matrix " << endl;
    cout << "|" << turret_matrix[0] << " " << turret_matrix[4] << " "
         << turret_matrix[8] << " " << turret_matrix[12] << "|" << endl;
    cout << "|" << turret_matrix[1] << " " << turret_matrix[5] << " "
         << turret_matrix[9] << " " << turret_matrix[13] << "|" << endl;
    cout << "|" << turret_matrix[2] << " " << turret_matrix[6] << " "
         << turret_matrix[10] << " " << turret_matrix[14] << "|" << endl;
    cout << "|" << turret_matrix[3] << " " << turret_matrix[7] << " "
         << turret_matrix[11] << " " << turret_matrix[15] << "|" << endl;
}

void Tank::printHeadMatrix() {
    cout << " Head Matrix " << endl;
    cout << "|" << head_matrix[0] << " " << head_matrix[4] << " "
         << head_matrix[8] << " " << head_matrix[12] << "|" << endl;
    cout << "|" << head_matrix[1] << " " << head_matrix[5] << " "
         << head_matrix[9] << " " << head_matrix[13] << "|" << endl;
    cout << "|" << head_matrix[2] << " " << head_matrix[6] << " "
         << head_matrix[10] << " " << head_matrix[14] << "|" << endl;
    cout << "|" << head_matrix[3] << " " << head_matrix[7] << " "
         << head_matrix[11] << " " << head_matrix[15] << "|" << endl;
}

void Tank::printBodyMatrix() {
    cout << " Body Matrix " << endl;
    cout << "|" << body_matrix[0] << " " << body_matrix[4] << " "
         << body_matrix[8] << " " << body_matrix[12] << "|" << endl;
    cout << "|" << body_matrix[1] << " " << body_matrix[5] << " "
         << body_matrix[9] << " " << body_matrix[13] << "|" << endl;
    cout << "|" << body_matrix[2] << " " << body_matrix[6] << " "
         << body_matrix[10] << " " << body_matrix[14] << "|" << endl;
    cout << "|" << body_matrix[3] << " " << body_matrix[7] << " "
         << body_matrix[11] << " " << body_matrix[15] << "|" << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
         << endl;
}

void Tank::setTankPos(float x, float y, float z) {
    // Keep Track of Tanks Position for Falling Damage
    previous_height = current_height;
    current_height = y;

    // Hierarchical placement (body -> head -> turret, each child's offset
    // rotated through its parent's basis) - libvulkan_graphix's
    // TankPlacement, shared with the tutorials that draw a tank.
    namespace vg = vulkan_graphix;
    vg::TankPlacement::PartTranslations const parts =
            vg::TankPlacement::composePartTranslations(
                    vg::Math::Vec3<float>(x, y, z),
                    glm::make_mat4(body_matrix),
                    glm::make_mat4(head_matrix),
                    {vg::Math::Vec3<float>(
                             body_offset[0], body_offset[1], body_offset[2]),
                     vg::Math::Vec3<float>(
                             head_offset[0], head_offset[1], head_offset[2]),
                     vg::Math::Vec3<float>(turret_offset[0],
                                           turret_offset[1],
                                           turret_offset[2])});
    for (std::int32_t i = 0; i < 3; ++i) {
        body_matrix[12 + i] = parts.body[i];
        head_matrix[12 + i] = parts.head[i];
        turret_matrix[12 + i] = parts.turret[i];
    }
    updateHitBox();
    if (smoke_gen)
        smoke_gen->update(body_matrix[12] + body_offset[0] + head_offset[0],
                          body_matrix[13] + body_offset[1] + head_offset[1],
                          body_matrix[14] + body_offset[2] + head_offset[2]);
}

void Tank::resetTurret() {
    turret_matrix[12] = head_matrix[12] + turret_offset[0] * head_matrix[0] +
                        turret_offset[1] * head_matrix[4] +
                        turret_offset[2] * head_matrix[8];
    turret_matrix[13] = head_matrix[13] + turret_offset[0] * head_matrix[1] +
                        turret_offset[1] * head_matrix[5] +
                        turret_offset[2] * head_matrix[9];
    turret_matrix[14] = head_matrix[14] + turret_offset[0] * head_matrix[2] +
                        turret_offset[1] * head_matrix[6] +
                        turret_offset[2] * head_matrix[10];
}

void Tank::orientTank(vulkan_graphix::Math::Vec3<float>* n) {
    rotate_degrees = 0;
    alignment_vector.x = n->x;
    alignment_vector.y = n->y;
    alignment_vector.z = n->z;

    std::optional<vulkan_graphix::TankOrientation::Alignment> const alignment =
            vulkan_graphix::TankOrientation::alignToGround(
                    glm::make_mat4(body_matrix),
                    vulkan_graphix::Math::Vec3<float>(n->x, n->y, n->z));
    if (alignment) {
        rotate_about.x = alignment->axis.x;
        rotate_about.y = alignment->axis.y;
        rotate_about.z = alignment->axis.z;
        // Body/head/turret/wheel all take the same aligned matrix, as the
        // original's four glGetFloatv(GL_MODELVIEW_MATRIX, ...) calls did.
        float const* aligned = glm::value_ptr(alignment->matrix);
        std::copy(aligned, aligned + 16, body_matrix);
        std::copy(aligned, aligned + 16, head_matrix);
        std::copy(aligned, aligned + 16, turret_matrix);
        std::copy(aligned, aligned + 16, wheel_matrix);
        turret_degrees = 0;
    } else {
        // Already upright along the normal: no rotation axis.
        rotate_about.x = 0;
        rotate_about.y = 0;
        rotate_about.z = 0;
    }

    updateHitBox();
}

void Tank::rotateHead(float degrees) {
    rotate_degrees += degrees;
    if (rotate_degrees > 360) {
        rotate_degrees -= 360;
    } else if (rotate_degrees < 0) {
        rotate_degrees += 360;
    }

    float angle = vulkan_graphix::TankOrientation::angleBetweenDegrees(
            vulkan_graphix::Math::Vec3<float>(
                    head_matrix[4], head_matrix[5], head_matrix[6]),
            vulkan_graphix::Math::Vec3<float>(
                    turret_matrix[4], turret_matrix[5], turret_matrix[6]));

    std::copy_n(glm::value_ptr(
                        glm::rotate(glm::make_mat4(head_matrix),
                                    glm::radians(static_cast<float>(degrees)),
                                    math::Vec3<float>(0, 1, 0))),
                16,
                head_matrix);

    std::copy_n(glm::value_ptr(
                        glm::rotate(glm::make_mat4(turret_matrix),
                                    glm::radians(static_cast<float>(-angle)),
                                    math::Vec3<float>(1, 0, 0))),
                16,
                turret_matrix);
    std::copy_n(glm::value_ptr(
                        glm::translate(glm::make_mat4(turret_matrix),
                                       math::Vec3<float>(-turret_offset[0],
                                                         -turret_offset[1],
                                                         -turret_offset[2]))),
                16,
                turret_matrix);
    std::copy_n(glm::value_ptr(
                        glm::rotate(glm::make_mat4(turret_matrix),
                                    glm::radians(static_cast<float>(degrees)),
                                    math::Vec3<float>(0, 1, 0))),
                16,
                turret_matrix);
    std::copy_n(glm::value_ptr(
                        glm::translate(glm::make_mat4(turret_matrix),
                                       math::Vec3<float>(turret_offset[0],
                                                         turret_offset[1],
                                                         turret_offset[2]))),
                16,
                turret_matrix);
    std::copy_n(
            glm::value_ptr(glm::rotate(glm::make_mat4(turret_matrix),
                                       glm::radians(static_cast<float>(angle)),
                                       math::Vec3<float>(1, 0, 0))),
            16,
            turret_matrix);
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
    if ((turret_degrees + degrees <= 90) && (turret_degrees + degrees >= 0)) {
        turret_degrees += degrees;
        std::copy_n(glm::value_ptr(glm::rotate(
                            glm::make_mat4(turret_matrix),
                            glm::radians(static_cast<float>(degrees)),
                            math::Vec3<float>(1, 0, 0))),
                    16,
                    turret_matrix);
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
    if (sqrt(pow((x - tank_pos.x), 2) + pow((y - tank_pos.y), 2) +
             pow((z - tank_pos.z), 2)) < 50000000) {
        // top
        float plane_x = tank_pos.x + hit_box_length / 2.0 * at.x +
                        hit_box_width / 2.0 * right.x +
                        hit_box_height / 2.0 * up.x;
        float plane_y = tank_pos.y + hit_box_length / 2.0 * at.y +
                        hit_box_width / 2.0 * right.y +
                        hit_box_height / 2.0 * up.y;
        float plane_z = tank_pos.z + hit_box_length / 2.0 * at.z +
                        hit_box_width / 2.0 * right.z +
                        hit_box_height / 2.0 * up.z;
        float d = up.x * plane_x + up.y * plane_y + up.z * plane_z;
        if (up.x * x + up.y * y + up.z * z - d <= 0) {
            // right
            plane_x = tank_pos.x + hit_box_length / 2.0 * at.x +
                      hit_box_width / 2.0 * right.x +
                      hit_box_height / 2.0 * up.x;
            plane_y = tank_pos.y + hit_box_length / 2.0 * at.y +
                      hit_box_width / 2.0 * right.y +
                      hit_box_height / 2.0 * up.y;
            plane_z = tank_pos.z + hit_box_length / 2.0 * at.z +
                      hit_box_width / 2.0 * right.z +
                      hit_box_height / 2.0 * up.z;
            d = right.x * plane_x + right.y * plane_y + right.z * plane_z;
            if (right.x * x + right.y * y + right.z * z - d <= 0) {
                // left
                plane_x = tank_pos.x + hit_box_length / 2.0 * at.x +
                          hit_box_width / 2.0 * left.x +
                          hit_box_height / 2.0 * up.x;
                plane_y = tank_pos.y + hit_box_length / 2.0 * at.y +
                          hit_box_width / 2.0 * left.y +
                          hit_box_height / 2.0 * up.y;
                plane_z = tank_pos.z + hit_box_length / 2.0 * at.z +
                          hit_box_width / 2.0 * left.z +
                          hit_box_height / 2.0 * up.z;
                d = left.x * plane_x + left.y * plane_y + left.z * plane_z;
                if (left.x * x + left.y * y + left.z * z - d <= 0) {
                    // front
                    plane_x = tank_pos.x + hit_box_length / 2.0 * at.x +
                              hit_box_width / 2.0 * left.x +
                              hit_box_height / 2.0 * up.x;
                    plane_y = tank_pos.y + hit_box_length / 2.0 * at.y +
                              hit_box_width / 2.0 * left.y +
                              hit_box_height / 2.0 * up.y;
                    plane_z = tank_pos.z + hit_box_length / 2.0 * at.z +
                              hit_box_width / 2.0 * left.z +
                              hit_box_height / 2.0 * up.z;
                    d = at.x * plane_x + at.y * plane_y + at.z * plane_z;
                    if (at.x * x + at.y * y + at.z * z - d <= 0) {
                        // back
                        plane_x = tank_pos.x + hit_box_length / 2.0 * back.x +
                                  hit_box_width / 2.0 * left.x +
                                  hit_box_height / 2.0 * up.x;
                        plane_y = tank_pos.y + hit_box_length / 2.0 * back.y +
                                  hit_box_width / 2.0 * left.y +
                                  hit_box_height / 2.0 * up.y;
                        plane_z = tank_pos.z + hit_box_length / 2.0 * back.z +
                                  hit_box_width / 2.0 * left.z +
                                  hit_box_height / 2.0 * up.z;
                        d = back.x * plane_x + back.y * plane_y +
                            back.z * plane_z;
                        if (back.x * x + back.y * y + back.z * z - d <= 0) {
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
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        tank_alive = false;
        initDuration();
        smoke_gen = new ParticleGenerator(10, 5, 1, 100, 0);
    }
}

void Tank::checkFallingDamage() {
    if (previous_height != current_height) {
        dealDamage((previous_height - current_height) / 10);
    }
}

bool Tank::isAlive() { return tank_alive; }

void Tank::tankRevive() {
    if (smoke_gen) {
        smoke_gen = nullptr;
    }
    tank_alive = true;
}

void Tank::adjustPower(float amount) {
    if (((current_power + amount) <= 10.0) &&
        ((current_power + amount) >= 0.0)) {
        current_power += amount;
    }
}

void Tank::initDuration() {
    duration_acid = 0;
    duration_shield = 0;
    duration_emp = 0;
    duration_float = 0;
    duration_double_action = 0;
    duration_padlock = 0;
    duration_cloak = 0;
    duration_paralyze = 0;
    delete acid_gen;
    delete float_gen;
}

void Tank::setBodyColor(float r, float g, float b, float a) {
    body_color[0] = r;
    body_color[1] = g;
    body_color[2] = b;
    body_color[3] = a;
}
void Tank::setBodyScale(float x, float y, float z) {
    body_scale[0] = x;
    body_scale[1] = y;
    body_scale[2] = z;
}

void Tank::setHeadColor(float r, float g, float b, float a) {
    head_color[0] = r;
    head_color[1] = g;
    head_color[2] = b;
    head_color[3] = a;
}
void Tank::setHeadScale(float x, float y, float z) {
    head_scale[0] = x;
    head_scale[1] = y;
    head_scale[2] = z;
}

void Tank::setTurretColor(float r, float g, float b, float a) {
    turret_color[0] = r;
    turret_color[1] = g;
    turret_color[2] = b;
    turret_color[3] = a;
}
void Tank::setTurretScale(float x, float y, float z) {
    turret_scale[0] = x;
    turret_scale[1] = y;
    turret_scale[2] = z;
}

void Tank::setWheelColor(float r, float g, float b, float a) {
    wheel_color[0] = r;
    wheel_color[1] = g;
    wheel_color[2] = b;
    wheel_color[3] = a;
}
void Tank::setWheelScale(float x, float y, float z) {
    wheel_scale[0] = x;
    wheel_scale[1] = y;
    wheel_scale[2] = z;
}
void Tank::setHP(std::int32_t new_hp) { hp = new_hp; }
void Tank::setPower(std::int32_t p) { power = p; }
void Tank::setArmor(std::int32_t a) { armor = a; }
void Tank::setSpeed(std::int32_t d) { speed = d; }

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
    tank_pos.x = body_matrix[12];
    tank_pos.y = body_matrix[13];
    tank_pos.z = body_matrix[14];
    right.x = -body_matrix[0];
    right.y = -body_matrix[1];
    right.z = -body_matrix[2];
    left.x = body_matrix[0];
    left.y = body_matrix[1];
    left.z = body_matrix[2];
    up.x = body_matrix[4];
    up.y = body_matrix[5];
    up.z = body_matrix[6];
    down.x = -body_matrix[4];
    down.y = -body_matrix[5];
    down.z = -body_matrix[6];
    at.x = -body_matrix[8];
    at.y = -body_matrix[9];
    at.z = -body_matrix[10];
    back.x = body_matrix[8];
    back.y = body_matrix[9];
    back.z = body_matrix[10];
}

void Tank::keyHandler() {}

// Virtuals
std::int32_t Tank::getBaseHP() { return 0; }
std::int32_t Tank::getBasePower() { return 0; }
std::int32_t Tank::getBaseArmor() { return 0; }
std::int32_t Tank::getBaseSpeed() { return 0; }
std::string Tank::getName() { return "Huh?"; }
vulkan_graphix::Math::Vec3<float> Tank::getAlignmentVector() {
    return alignment_vector;
}
vulkan_graphix::Math::Vec3<float> Tank::getRotateAbout() {
    return rotate_about;
}
float Tank::getTurretDegrees() { return turret_degrees; }

void Tank::draw(render::RenderContext& context) {
    // Each part: its own matrix (glMultMatrixf) then its scale (glScalef).
    vbo_shader_turret->draw(context,
                            glm::scale(glm::make_mat4(turret_matrix),
                                       math::Vec3<float>(turret_scale[0],
                                                         turret_scale[1],
                                                         turret_scale[2])));
    vbo_shader_head->draw(
            context,
            glm::scale(glm::make_mat4(head_matrix),
                       math::Vec3<float>(
                               head_scale[0], head_scale[1], head_scale[2])));
    math::Mat4<float> const body = glm::make_mat4(body_matrix);
    // float effect must be same orientation as the tank
    if (float_gen) {
        float_gen->update(0, -body_offset[1], 0);
        float_gen->draw(context, body);
    }
    // Draw tank body
    vbo_shader_body->draw(
            context,
            glm::scale(body,
                       math::Vec3<float>(
                               body_scale[0], body_scale[1], body_scale[2])));
    // shield: glutSolidSphere(300, 20, 20) in translucent blue
    if (duration_shield > 0) {
        context.drawMesh(render::Renderer::instance().sphere(20, 20),
                         vulkan_earth::pipelines().flat_color,
                         nullptr,
                         glm::scale(body, math::Vec3<float>(300, 300, 300)),
                         math::Vec4<float>(0, 0, 1, 0.2));
    }

    // update and draw particles
    if (smoke_gen) {
        smoke_gen->update(body_matrix[12] + body_offset[0] + head_offset[0],
                          body_matrix[13] + body_offset[1] + head_offset[1],
                          body_matrix[14] + body_offset[2] + head_offset[2]);
        smoke_gen->draw(context);
    }
    if (acid_gen) {
        acid_gen->update(body_matrix[12], body_matrix[13], body_matrix[14]);
        acid_gen->draw(context);
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
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * at.x +
                                      hit_box_width / 2.0 * right.x +
                                      hit_box_height / 2.0 * up.x,
                              tank_pos.y + hit_box_length / 2.0 * at.y +
                                      hit_box_width / 2.0 * right.y +
                                      hit_box_height / 2.0 * up.y,
                              tank_pos.z + hit_box_length / 2.0 * at.z +
                                      hit_box_width / 2.0 * right.z +
                                      hit_box_height / 2.0 * up.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 1.0, 0.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * at.x +
                                      hit_box_width / 2.0 * left.x +
                                      hit_box_height / 2.0 * up.x,
                              tank_pos.y + hit_box_length / 2.0 * at.y +
                                      hit_box_width / 2.0 * left.y +
                                      hit_box_height / 2.0 * up.y,
                              tank_pos.z + hit_box_length / 2.0 * at.z +
                                      hit_box_width / 2.0 * left.z +
                                      hit_box_height / 2.0 * up.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 1.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * back.x +
                                      hit_box_width / 2.0 * left.x +
                                      hit_box_height / 2.0 * up.x,
                              tank_pos.y + hit_box_length / 2.0 * back.y +
                                      hit_box_width / 2.0 * left.y +
                                      hit_box_height / 2.0 * up.y,
                              tank_pos.z + hit_box_length / 2.0 * back.z +
                                      hit_box_width / 2.0 * left.z +
                                      hit_box_height / 2.0 * up.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(1.0, 1.0, 1.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * back.x +
                                      hit_box_width / 2.0 * right.x +
                                      hit_box_height / 2.0 * up.x,
                              tank_pos.y + hit_box_length / 2.0 * back.y +
                                      hit_box_width / 2.0 * right.y +
                                      hit_box_height / 2.0 * up.y,
                              tank_pos.z + hit_box_length / 2.0 * back.z +
                                      hit_box_width / 2.0 * right.z +
                                      hit_box_height / 2.0 * up.z),
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
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * at.x +
                                      hit_box_width / 2.0 * right.x +
                                      hit_box_height / 2.0 * up.x,
                              tank_pos.y + hit_box_length / 2.0 * at.y +
                                      hit_box_width / 2.0 * right.y +
                                      hit_box_height / 2.0 * up.y,
                              tank_pos.z + hit_box_length / 2.0 * at.z +
                                      hit_box_width / 2.0 * right.z +
                                      hit_box_height / 2.0 * up.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 1.0, 0.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * at.x +
                                      hit_box_width / 2.0 * right.x +
                                      hit_box_height / 2.0 * down.x,
                              tank_pos.y + hit_box_length / 2.0 * at.y +
                                      hit_box_width / 2.0 * right.y +
                                      hit_box_height / 2.0 * down.y,
                              tank_pos.z + hit_box_length / 2.0 * at.z +
                                      hit_box_width / 2.0 * right.z +
                                      hit_box_height / 2.0 * down.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 1.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * back.x +
                                      hit_box_width / 2.0 * right.x +
                                      hit_box_height / 2.0 * down.x,
                              tank_pos.y + hit_box_length / 2.0 * back.y +
                                      hit_box_width / 2.0 * right.y +
                                      hit_box_height / 2.0 * down.y,
                              tank_pos.z + hit_box_length / 2.0 * back.z +
                                      hit_box_width / 2.0 * right.z +
                                      hit_box_height / 2.0 * down.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(1.0, 1.0, 1.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * back.x +
                                      hit_box_width / 2.0 * right.x +
                                      hit_box_height / 2.0 * up.x,
                              tank_pos.y + hit_box_length / 2.0 * back.y +
                                      hit_box_width / 2.0 * right.y +
                                      hit_box_height / 2.0 * up.y,
                              tank_pos.z + hit_box_length / 2.0 * back.z +
                                      hit_box_width / 2.0 * right.z +
                                      hit_box_height / 2.0 * up.z),
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
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * at.x +
                                      hit_box_width / 2.0 * left.x +
                                      hit_box_height / 2.0 * up.x,
                              tank_pos.y + hit_box_length / 2.0 * at.y +
                                      hit_box_width / 2.0 * left.y +
                                      hit_box_height / 2.0 * up.y,
                              tank_pos.z + hit_box_length / 2.0 * at.z +
                                      hit_box_width / 2.0 * left.z +
                                      hit_box_height / 2.0 * up.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 1.0, 0.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * at.x +
                                      hit_box_width / 2.0 * left.x +
                                      hit_box_height / 2.0 * down.x,
                              tank_pos.y + hit_box_length / 2.0 * at.y +
                                      hit_box_width / 2.0 * left.y +
                                      hit_box_height / 2.0 * down.y,
                              tank_pos.z + hit_box_length / 2.0 * at.z +
                                      hit_box_width / 2.0 * left.z +
                                      hit_box_height / 2.0 * down.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 1.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * back.x +
                                      hit_box_width / 2.0 * left.x +
                                      hit_box_height / 2.0 * down.x,
                              tank_pos.y + hit_box_length / 2.0 * back.y +
                                      hit_box_width / 2.0 * left.y +
                                      hit_box_height / 2.0 * down.y,
                              tank_pos.z + hit_box_length / 2.0 * back.z +
                                      hit_box_width / 2.0 * left.z +
                                      hit_box_height / 2.0 * down.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(1.0, 1.0, 1.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * back.x +
                                      hit_box_width / 2.0 * left.x +
                                      hit_box_height / 2.0 * up.x,
                              tank_pos.y + hit_box_length / 2.0 * back.y +
                                      hit_box_width / 2.0 * left.y +
                                      hit_box_height / 2.0 * up.y,
                              tank_pos.z + hit_box_length / 2.0 * back.z +
                                      hit_box_width / 2.0 * left.z +
                                      hit_box_height / 2.0 * up.z),
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
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * at.x +
                                      hit_box_width / 2.0 * right.x +
                                      hit_box_height / 2.0 * up.x,
                              tank_pos.y + hit_box_length / 2.0 * at.y +
                                      hit_box_width / 2.0 * right.y +
                                      hit_box_height / 2.0 * up.y,
                              tank_pos.z + hit_box_length / 2.0 * at.z +
                                      hit_box_width / 2.0 * right.z +
                                      hit_box_height / 2.0 * up.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 1.0, 0.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * at.x +
                                      hit_box_width / 2.0 * left.x +
                                      hit_box_height / 2.0 * up.x,
                              tank_pos.y + hit_box_length / 2.0 * at.y +
                                      hit_box_width / 2.0 * left.y +
                                      hit_box_height / 2.0 * up.y,
                              tank_pos.z + hit_box_length / 2.0 * at.z +
                                      hit_box_width / 2.0 * left.z +
                                      hit_box_height / 2.0 * up.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 1.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * at.x +
                                      hit_box_width / 2.0 * left.x +
                                      hit_box_height / 2.0 * down.x,
                              tank_pos.y + hit_box_length / 2.0 * at.y +
                                      hit_box_width / 2.0 * left.y +
                                      hit_box_height / 2.0 * down.y,
                              tank_pos.z + hit_box_length / 2.0 * at.z +
                                      hit_box_width / 2.0 * left.z +
                                      hit_box_height / 2.0 * down.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 0.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * at.x +
                                      hit_box_width / 2.0 * right.x +
                                      hit_box_height / 2.0 * down.x,
                              tank_pos.y + hit_box_length / 2.0 * at.y +
                                      hit_box_width / 2.0 * right.y +
                                      hit_box_height / 2.0 * down.y,
                              tank_pos.z + hit_box_length / 2.0 * at.z +
                                      hit_box_width / 2.0 * right.z +
                                      hit_box_height / 2.0 * down.z),
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
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * back.x +
                                      hit_box_width / 2.0 * right.x +
                                      hit_box_height / 2.0 * up.x,
                              tank_pos.y + hit_box_length / 2.0 * back.y +
                                      hit_box_width / 2.0 * right.y +
                                      hit_box_height / 2.0 * up.y,
                              tank_pos.z + hit_box_length / 2.0 * back.z +
                                      hit_box_width / 2.0 * right.z +
                                      hit_box_height / 2.0 * up.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 1.0, 0.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * back.x +
                                      hit_box_width / 2.0 * left.x +
                                      hit_box_height / 2.0 * up.x,
                              tank_pos.y + hit_box_length / 2.0 * back.y +
                                      hit_box_width / 2.0 * left.y +
                                      hit_box_height / 2.0 * up.y,
                              tank_pos.z + hit_box_length / 2.0 * back.z +
                                      hit_box_width / 2.0 * left.z +
                                      hit_box_height / 2.0 * up.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 1.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * back.x +
                                      hit_box_width / 2.0 * left.x +
                                      hit_box_height / 2.0 * down.x,
                              tank_pos.y + hit_box_length / 2.0 * back.y +
                                      hit_box_width / 2.0 * left.y +
                                      hit_box_height / 2.0 * down.y,
                              tank_pos.z + hit_box_length / 2.0 * back.z +
                                      hit_box_width / 2.0 * left.z +
                                      hit_box_height / 2.0 * down.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    color = math::Vec4<float>(0.0, 0.0, 0.0, .75);
    quad.push_back(render::UiVertex{
            math::Vec3<float>(tank_pos.x + hit_box_length / 2.0 * back.x +
                                      hit_box_width / 2.0 * right.x +
                                      hit_box_height / 2.0 * down.x,
                              tank_pos.y + hit_box_length / 2.0 * back.y +
                                      hit_box_width / 2.0 * right.y +
                                      hit_box_height / 2.0 * down.y,
                              tank_pos.z + hit_box_length / 2.0 * back.z +
                                      hit_box_width / 2.0 * right.z +
                                      hit_box_height / 2.0 * down.z),
            color,
            math::Vec2<float>(0.0f)});
    if (quad.size() == 4) {
        for (std::size_t corner : {0U, 1U, 2U, 0U, 2U, 3U})
            out_tris.push_back(quad[corner]);
        quad.clear();
    }
    context.drawTransient(
            out_tris, vulkan_earth::pipelines().ui_triangles, nullptr);
}

float Tank::getCurrentPower() { return current_power; }
std::int32_t Tank::getPreviousPower() { return previous_power; }
std::int32_t Tank::getPreviousAngle() { return previous_angle; }
float* Tank::getProjectileLandPos() { return projectile_land_pos; }
void Tank::setPreviousPower(std::int32_t new_previous_power) {
    previous_power = new_previous_power;
}
void Tank::setPreviousAngle(std::int32_t new_previous_angle) {
    previous_angle = new_previous_angle;
}
void Tank::setProjectileLandPos(float x, float y) {
    projectile_land_pos[0] = x;
    projectile_land_pos[1] = y;
}

// remove later probably
void Tank::rotateWheel(float degrees) {
    if (!(wheel_degrees + degrees > 90) || (wheel_degrees - degrees < 0)) {
        wheel_degrees += degrees;
        std::copy_n(glm::value_ptr(glm::rotate(
                            glm::make_mat4(wheel_matrix),
                            glm::radians(static_cast<float>(degrees)),
                            math::Vec3<float>(wheel_matrix[4],
                                              wheel_matrix[5],
                                              wheel_matrix[6]))),
                    16,
                    wheel_matrix);
    }
}

void Tank::changeHeadTexture(std::int32_t current_player_index) {
    if (current_player_index == 0)
        vbo_shader_head->swapTexture("player1Head.raw", 1024, 1024);
    else if (current_player_index == 1)
        vbo_shader_head->swapTexture("player2Head.raw", 1024, 1024);
    else if (current_player_index == 2)
        vbo_shader_head->swapTexture("player3Head.raw", 1024, 1024);
    else if (current_player_index == 3)
        vbo_shader_head->swapTexture("player4Head.raw", 1024, 1024);
    else if (current_player_index == 4)
        vbo_shader_head->swapTexture("player5Head.raw", 1024, 1024);
    else if (current_player_index == 5)
        vbo_shader_head->swapTexture("player6Head.raw", 1024, 1024);
    else if (current_player_index == 6)
        vbo_shader_head->swapTexture("player7Head.raw", 1024, 1024);
    else if (current_player_index == 7)
        vbo_shader_head->swapTexture("player8Head.raw", 1024, 1024);
    else if (current_player_index == 8)
        vbo_shader_head->swapTexture("player9Head.raw", 1024, 1024);
    else if (current_player_index == 9)
        vbo_shader_head->swapTexture("player10Head.raw", 1024, 1024);
    else {
        printf("\nERROR <Tank::changeHeadTexture(int)>: Wrong parameter "
               "passing.\n");
    }
}

std::int32_t Tank::getCurrentHeight() { return current_height; }

void Tank::setCurrentHeight(std::int32_t curr_height) {
    current_height = curr_height;
}

std::int32_t Tank::getPreviousHeight() { return previous_height; }

void Tank::setPreviousHeight(std::int32_t prev_height) {
    previous_height = prev_height;
}

// Getters for durations
std::int32_t Tank::getDurationAcid() { return duration_acid; }
std::int32_t Tank::getDurationShield() { return duration_shield; }
std::int32_t Tank::getDurationEMP() { return duration_emp; }
std::int32_t Tank::getDurationFloat() { return duration_float; }
std::int32_t Tank::getDurationDoubleAction() { return duration_double_action; }
std::int32_t Tank::getDurationPadlock() { return duration_padlock; }
std::int32_t Tank::getDurationCloak() { return duration_cloak; }
std::int32_t Tank::getDurationParalyze() { return duration_paralyze; }
// Setters for durations
void Tank::setDurationAcid(std::int32_t value) {
    if (value != 0) {
        delete acid_gen;
        acid_gen = new ParticleGenerator(10, 5, 2, 100, 1);
    } else {
        if (acid_gen) {
            acid_gen->killGenerator();
            acid_gen = nullptr;
        }
    }
    duration_acid = value;
}
void Tank::setDurationShield(std::int32_t value) { duration_shield = value; }
void Tank::setDurationEMP(std::int32_t value) { duration_emp = value; }
void Tank::setDurationFloat(std::int32_t value) {
    if (value != 0) {
        delete float_gen;
        float_gen = new ParticleGenerator(10, 5, 2, 100, 2);
    } else {
        if (float_gen) {
            float_gen->killGenerator();
            float_gen = nullptr;
        }
    }
    duration_float = value;
}
void Tank::setDurationDoubleAction(std::int32_t value) {
    duration_double_action = value;
}
void Tank::setDurationPadlock(std::int32_t value) { duration_padlock = value; }
void Tank::setDurationCloak(std::int32_t value) { duration_cloak = value; }
void Tank::setDurationParalyze(std::int32_t value) {
    duration_paralyze = value;
}
void Tank::setDurationAllPassTurn() {
    // acid
    if (duration_acid > 0) {
        duration_acid--;
    }
    if ((acid_gen) && (duration_acid == 0)) {
        acid_gen->killGenerator();
        acid_gen = nullptr;
    }

    // float
    if (duration_float > 0) {
        duration_float--;
    }
    if ((float_gen) && (duration_float == 0)) {
        float_gen->killGenerator();
        float_gen = nullptr;
    }
    if (duration_emp > 0) duration_emp--;
    if (duration_double_action > 0) duration_double_action--;
    if (duration_padlock > 0) duration_padlock--;
    if (duration_cloak > 0) duration_cloak--;
    if (duration_paralyze > 0) duration_paralyze--;
}
void Tank::initBody() {
    // The upright right/up/at basis every tank part starts from
    // (libvulkan_graphix's TankPlacement, shared with the tutorials).
    std::copy_n(
            glm::value_ptr(vulkan_graphix::TankPlacement::uprightPartBasis()),
            16,
            body_matrix);
    for (std::int32_t i = 0; i < 3; i++) {
        body_right[i] = body_matrix[i];
        body_up[i] = body_matrix[4 + i];
        body_at[i] = body_matrix[8 + i];
    }

    body_color[0] = 0.50f;
    body_color[1] = 0.50f;
    body_color[2] = 0.50f;
    body_color[3] = 1.0f;
    body_scale[0] = 70;
    body_scale[1] = 70;
    body_scale[2] = 70;
    previous_height = current_height = body_matrix[13];
}
void Tank::initHead() {
    // The upright right/up/at basis every tank part starts from
    // (libvulkan_graphix's TankPlacement, shared with the tutorials).
    std::copy_n(
            glm::value_ptr(vulkan_graphix::TankPlacement::uprightPartBasis()),
            16,
            head_matrix);
    for (std::int32_t i = 0; i < 3; i++) {
        head_right[i] = head_matrix[i];
        head_up[i] = head_matrix[4 + i];
        head_at[i] = head_matrix[8 + i];
    }
    head_color[0] = 0.50f;
    head_color[1] = 0.50f;
    head_color[2] = 0.50f;
    head_color[3] = 1.0f;
    head_scale[0] = 70;
    head_scale[1] = 70;
    head_scale[2] = 70;
}
void Tank::initTurret() {
    // The upright right/up/at basis every tank part starts from
    // (libvulkan_graphix's TankPlacement, shared with the tutorials).
    std::copy_n(
            glm::value_ptr(vulkan_graphix::TankPlacement::uprightPartBasis()),
            16,
            turret_matrix);
    for (std::int32_t i = 0; i < 3; i++) {
        turret_right[i] = turret_matrix[i];
        turret_up[i] = turret_matrix[4 + i];
        turret_at[i] = turret_matrix[8 + i];
    }
    turret_color[0] = 0.50f;
    turret_color[1] = 0.50f;
    turret_color[2] = 0.50f;
    turret_color[3] = 1.0f;
    turret_scale[0] = 70;
    turret_scale[1] = 70;
    turret_scale[2] = 70;

    turret_offset[0] = 0;
    turret_offset[1] = 0;
    turret_offset[2] = 0;

    turret_degrees = 0;
    wheel_degrees = 0;
}
void Tank::initWheel() {
    // The upright right/up/at basis every tank part starts from
    // (libvulkan_graphix's TankPlacement, shared with the tutorials).
    std::copy_n(
            glm::value_ptr(vulkan_graphix::TankPlacement::uprightPartBasis()),
            16,
            wheel_matrix);
    for (std::int32_t i = 0; i < 3; i++) {
        wheel_right[i] = wheel_matrix[i];
        wheel_up[i] = wheel_matrix[4 + i];
        wheel_at[i] = wheel_matrix[8 + i];
    }
    wheel_color[0] = 0.50f;
    wheel_color[1] = 0.50f;
    wheel_color[2] = 0.50f;
    wheel_color[3] = 1.0f;
    wheel_scale[0] = 70;
    wheel_scale[1] = 70;
    wheel_scale[2] = 70;
}

// GETTERS
const float* Tank::getBodyMatrix() { return &body_matrix[0]; }
const float* Tank::getBodyColor() { return &body_color[0]; }
float* Tank::getBodyScale() { return &body_scale[0]; }

const float* Tank::getHeadMatrix() { return &head_matrix[0]; }
const float* Tank::getHeadColor() { return &head_color[0]; }
float* Tank::getHeadScale() { return &head_scale[0]; }

float* Tank::getTurretMatrix() { return &turret_matrix[0]; }
const float* Tank::getTurretColor() { return &turret_color[0]; }
float* Tank::getTurretScale() { return &turret_scale[0]; }

const float* Tank::getWheelMatrix() { return &wheel_matrix[0]; }
const float* Tank::getWheelColor() { return &wheel_color[0]; }
float* Tank::getWheelScale() { return &wheel_scale[0]; }

std::int32_t Tank::getHP() { return hp; }
std::int32_t Tank::getPower() { return power; }
std::int32_t Tank::getArmor() { return armor; }
std::int32_t Tank::getSpeed() { return speed; }
Tank* Tank::getTankPointer() { return this; }