#include "vulkan_earth/Projectile.h"
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vulkan_earth/ChaseCam.h"
#include "vulkan_earth/GameState.h"
#include "vulkan_earth/VBOShaderLibrary.h"
#include "vulkan_earth/Weapon.h"
#include "vulkan_earth/WeaponDefault.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_earth/MacroCrtdbg.h"

using namespace std;

namespace render = vulkan_graphix::Render;
namespace math = vulkan_graphix::Math;

Projectile::Projectile() = default;

Projectile::Projectile(GameState* new_parent,
                       float* turret_matrix,
                       float new_speed,
                       VBOShaderLibrary** new_projectile_models) {
    m_default_weapon = new WeaponDefault(10);
    m_parent = new_parent;
    m_launch = vulkan_graphix::Ballistics::launchFromBarrel(
            glm::make_mat4(turret_matrix), new_speed, c_muzzle_distance);
    m_pos[0] = m_launch.m_origin.x;
    m_pos[1] = m_launch.m_origin.y;
    m_pos[2] = m_launch.m_origin.z;
    m_v_vec[0] = m_launch.m_velocity.x;
    m_v_vec[1] = m_launch.m_velocity.y;
    m_v_vec[2] = m_launch.m_velocity.z;

    m_chase_cam = new ChaseCam(m_pos, m_v_vec);
    m_weapon = nullptr;
    m_projectile_models = new_projectile_models;

    m_projectile_default = new VBOShaderLibrary();
    m_projectile_default->loadClientData("Projectiles/projectileDefault.ogl");
    m_projectile_default->loadTexture(
            "Projectiles/projectileDefault.raw", 512, 512);

    m_rotate = 4;
    m_timer = 0;
    m_printed = false;
}

Projectile::~Projectile() {
    delete m_chase_cam;
    delete m_projectile_default;
}

// void Projectile::update(float gravity) {
//	pos[0]+=vVec[0];
//	pos[1]+=vVec[1];
//	pos[2]+=vVec[2];
//	vVec[1]+=gravity;
// }

void Projectile::update(float x, float y, float z) {
    m_pos[0] = x;
    m_pos[1] = y;
    m_pos[2] = z;
}

void Projectile::draw(render::RenderContext& context) {
    math::Mat4<float> model =
            glm::translate(math::Mat4<float>(1.0f),
                           math::Vec3<float>(m_pos[0], m_pos[1], m_pos[2]));
    model = glm::rotate(model,
                        glm::radians(static_cast<float>(m_rotate)),
                        math::Vec3<float>(-1, .3, -.4));
    if (m_weapon == nullptr) {
        m_projectile_default->draw(
                context, glm::scale(model, math::Vec3<float>(60, 60, 60)));
    } else {
        m_projectile_models[m_weapon->getUNIQUEIDENTIFIER()]->draw(
                context,
                glm::scale(model,
                           math::Vec3<float>(m_weapon->getScale(),
                                             m_weapon->getScale(),
                                             m_weapon->getScale())));
    }
    m_rotate += 4;
}

float* Projectile::getPos() { return m_pos; }

math::Mat4<float> Projectile::chaseView() { return m_chase_cam->view(); }

Weapon* Projectile::getWeapon() { return m_weapon; }
void Projectile::setWeapon(Weapon* wpn) { m_weapon = wpn; }
Weapon* Projectile::getDefaultWeapon() { return m_default_weapon; }
std::int32_t Projectile::getDefaultDamage() { return default_damage; }
std::int32_t Projectile::getDefaultRadius() { return default_radius; }
ChaseCam* Projectile::getChaseCam() { return m_chase_cam; }
std::int32_t Projectile::getRadius() { return default_radius; }
std::int32_t Projectile::getDamage() { return default_damage; }
const vulkan_graphix::Ballistics::Launch& Projectile::getLaunch() {
    return m_launch;
}
