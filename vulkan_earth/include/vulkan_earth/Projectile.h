#ifndef VULKAN_EARTH_PROJECTILE_H
#define VULKAN_EARTH_PROJECTILE_H

#include <cstdint>
#include "vulkan_graphix/Ballistics.h"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Render/Renderer.h"

const std::int32_t default_damage = 100;
const std::int32_t default_radius = 5;

class ChaseCam;
class Weapon;
class VBOShaderLibrary;
class GameState;

namespace vulkan_graphix::Render {
class RenderContext;
}

class Projectile {
public:
    // How far out along the turret's barrel a shell starts. Player's CPU
    // shot simulation uses this same constant, so its predicted arc starts
    // where a real shot does.
    static constexpr float c_muzzle_distance = 500.0f;

    Projectile();
    Projectile(GameState* new_parent,
               float* turret_matrix,
               float new_speed,
               VBOShaderLibrary** new_projectile_models);
    ~Projectile();
    void draw(vulkan_graphix::Render::RenderContext& context);
    // void update(float gravity);
    void update(float x, float y, float z);
    // The chase camera's view matrix (its gluLookAt()).
    vulkan_graphix::Math::Mat4<float> chaseView();
    float* getPos();
    Weapon* getWeapon();
    void setWeapon(Weapon* wpn);
    Weapon* getDefaultWeapon();
    std::int32_t getDefaultDamage();
    std::int32_t getDefaultRadius();
    ChaseCam* getChaseCam();
    std::int32_t getRadius();
    std::int32_t getDamage();
    // Where and how fast this shell left the barrel - feed to
    // vulkan_graphix::Ballistics::positionAt() for its flight.
    const vulkan_graphix::Ballistics::Launch& getLaunch();

private:
    ChaseCam* m_chase_cam;
    float m_pos[3];
    float m_v_vec[3];
    float m_speed;
    float m_wind;  // implement later
    Weapon* m_weapon;
    Weapon* m_default_weapon;
    VBOShaderLibrary* m_projectile_default;
    VBOShaderLibrary** m_projectile_models;
    float m_rotate;
    float m_y_not;
    std::int32_t m_timer;
    bool m_printed;
    GameState* m_parent;
    vulkan_graphix::Ballistics::Launch m_launch;
};

#endif  // VULKAN_EARTH_PROJECTILE_H