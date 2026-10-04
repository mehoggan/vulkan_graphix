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
    default_weapon = new WeaponDefault(10);
    parent = new_parent;
    launch = vulkan_graphix::Ballistics::launchFromBarrel(
            glm::make_mat4(turret_matrix), new_speed, c_muzzle_distance);
    pos[0] = launch.origin.x;
    pos[1] = launch.origin.y;
    pos[2] = launch.origin.z;
    v_vec[0] = launch.velocity.x;
    v_vec[1] = launch.velocity.y;
    v_vec[2] = launch.velocity.z;

    chase_cam = new ChaseCam(pos, v_vec);
    weapon = nullptr;
    projectile_models = new_projectile_models;

    projectile_default = new VBOShaderLibrary();
    projectile_default->loadClientData("Projectiles/projectileDefault.ogl");
    projectile_default->loadTexture(
            "Projectiles/projectileDefault.raw", 512, 512);

    rotate = 4;
    timer = 0;
    printed = false;
}

Projectile::~Projectile() {
    delete chase_cam;
    delete projectile_default;
}

// void Projectile::update(float gravity) {
//	pos[0]+=vVec[0];
//	pos[1]+=vVec[1];
//	pos[2]+=vVec[2];
//	vVec[1]+=gravity;
// }

void Projectile::update(float x, float y, float z) {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
}

void Projectile::draw(render::RenderContext& context) {
    math::Mat4<float> model =
            glm::translate(math::Mat4<float>(1.0f),
                           math::Vec3<float>(pos[0], pos[1], pos[2]));
    model = glm::rotate(model,
                        glm::radians(static_cast<float>(rotate)),
                        math::Vec3<float>(-1, .3, -.4));
    if (weapon == nullptr) {
        projectile_default->draw(
                context, glm::scale(model, math::Vec3<float>(60, 60, 60)));
    } else {
        projectile_models[weapon->getUNIQUEIDENTIFIER()]->draw(
                context,
                glm::scale(model,
                           math::Vec3<float>(weapon->getScale(),
                                             weapon->getScale(),
                                             weapon->getScale())));
    }
    rotate += 4;
}

float* Projectile::getPos() { return pos; }

math::Mat4<float> Projectile::chaseView() { return chase_cam->view(); }

Weapon* Projectile::getWeapon() { return weapon; }
void Projectile::setWeapon(Weapon* wpn) { weapon = wpn; }
Weapon* Projectile::getDefaultWeapon() { return default_weapon; }
std::int32_t Projectile::getDefaultDamage() { return default_damage; }
std::int32_t Projectile::getDefaultRadius() { return default_radius; }
ChaseCam* Projectile::getChaseCam() { return chase_cam; }
std::int32_t Projectile::getRadius() { return default_radius; }
std::int32_t Projectile::getDamage() { return default_damage; }
const vulkan_graphix::Ballistics::Launch& Projectile::getLaunch() {
    return launch;
}
