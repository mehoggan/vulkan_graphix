#include "vulkan_earth/TankB.h"
#include <cstdint>
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/VBOShaderLibrary.h"
#include "vulkan_graphix/HellfireTank.h"
#include "vulkan_earth/MacroCrtdbg.h"

const char* tank_b_name = "Hellfire";
const std::int32_t tank_b_hp = 1000;
const std::int32_t tank_b_power = 7;
const std::int32_t tank_b_armor = 5;
const std::int32_t tank_b_speed = 50;

TankB::TankB() = default;
TankB::TankB(float x, float y, float z) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    // The Hellfire's part offsets and scale live in libvulkan_graphix's
    // HellfireTank, shared with the tutorials that draw it.
    vulkan_graphix::TankPlacement::PartOffsets const& offsets =
            vulkan_graphix::HellfireTank::getPartOffsets();
    for (std::int32_t i = 0; i < 3; i++) {
        turret_offset[i] = offsets.turret[i];
        head_offset[i] = offsets.head[i];
        body_offset[i] = offsets.body[i];
    }

    for (std::int32_t i = 0; i < 3; i++) {
        body_scale[i] = vulkan_graphix::HellfireTank::c_part_scale;
        head_scale[i] = vulkan_graphix::HellfireTank::c_part_scale;
        turret_scale[i] = vulkan_graphix::HellfireTank::c_part_scale;
        wheel_scale[i] = vulkan_graphix::HellfireTank::c_part_scale;
    }

    power = tank_b_power;
    armor = tank_b_armor;
    speed = tank_b_speed;
    current_power = 10;
    previous_power = 1000;
    previous_angle = 1;
    hp = armor * 100;

    vbo_shader_head = new VBOShaderLibrary();
    vbo_shader_body = new VBOShaderLibrary();
    vbo_shader_turret = new VBOShaderLibrary();

    vbo_shader_turret->loadClientData("./Hellfire/Hellfire_Turret.ogl");
    vbo_shader_body->loadClientData("./Hellfire/Hellfire_Body.ogl");
    vbo_shader_head->loadClientData("./Hellfire/Hellfire_Head.ogl");

    vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);

    projectile_land_pos[0] = 9999999;
    projectile_land_pos[1] = 9999999;
}
TankB::~TankB() {
    delete vbo_shader_head;
    delete vbo_shader_body;
    delete vbo_shader_turret;
}

// GETTERS
std::int32_t TankB::getBaseHP() { return tank_b_hp; }
std::int32_t TankB::getBasePower() { return tank_b_power; }
std::int32_t TankB::getBaseArmor() { return tank_b_armor; }
std::int32_t TankB::getBaseSpeed() { return tank_b_speed; }
std::string TankB::getName() { return tank_b_name; }
