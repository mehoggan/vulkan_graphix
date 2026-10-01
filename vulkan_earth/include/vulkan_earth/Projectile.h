#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <cstdint>
#include <string>
#include "vulkan_graphix/Ballistics.h"

const std::int32_t default_damage = 100;
const std::int32_t default_radius = 5;

class ChaseCam;
class Weapon;
class VBOShaderLibrary;
class GameState;

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
    void draw();
    // void update(float gravity);
    void update(float x, float y, float z);
    void chaseView();
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
    ChaseCam* chase_cam;
    float pos[3];
    float v_vec[3];
    float speed;
    float wind;  // implement later
    Weapon* weapon;
    Weapon* default_weapon;
    VBOShaderLibrary* projectile_default;
    VBOShaderLibrary** projectile_models;
    float rotate;
    float y_not;
    std::int32_t timer;
    bool printed;
    GameState* parent;
    vulkan_graphix::Ballistics::Launch launch;
};

#endif /*	PROJECTILE_H_	*/