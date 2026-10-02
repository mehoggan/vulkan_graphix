#include "vulkan_earth/TankD.h"
#include <cstdint>
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/VBOShaderLibrary.h"
#include "vulkan_earth/MacroCrtdbg.h"

const char* tank_d_name = "Panzer";
const std::int32_t tank_d_hp = 1000;
const std::int32_t tank_d_power = 9;
const std::int32_t tank_d_armor = 5;
const std::int32_t tank_d_speed = 30;

TankD::TankD() = default;
TankD::TankD(float x, float y, float z) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    turret_offset[0] = 0;
    turret_offset[1] = 10;
    turret_offset[2] = 0.0001;
    head_offset[0] = 0;
    head_offset[1] = 70;
    head_offset[2] = 40;
    body_offset[0] = 0;
    body_offset[1] = 45;
    body_offset[2] = 0;

    for (std::int32_t i = 0; i < 3; i++) {
        body_scale[i] = 50;
        head_scale[i] = 50;
        turret_scale[i] = 50;
        wheel_scale[i] = 50;
    }

    power = tank_d_power;
    armor = tank_d_armor;
    speed = tank_d_speed;
    current_power = 10;
    previous_power = 1000;
    previous_angle = 1;
    hp = armor * 100;

    vbo_shader_head = new VBOShaderLibrary();
    vbo_shader_body = new VBOShaderLibrary();
    vbo_shader_turret = new VBOShaderLibrary();

    vbo_shader_turret->loadClientData("./Panzer/Panzer_Turret.ogl");
    vbo_shader_body->loadClientData("./Panzer/Panzer_Body.ogl");
    vbo_shader_head->loadClientData("./Panzer/Panzer_Head.ogl");

    vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);

    projectile_land_pos[0] = 9999999;
    projectile_land_pos[1] = 9999999;
}
TankD::~TankD() {
    delete vbo_shader_head;
    delete vbo_shader_body;
    delete vbo_shader_turret;
}

// GETTERS
std::int32_t TankD::getBaseHP() { return tank_d_hp; }
std::int32_t TankD::getBasePower() { return tank_d_power; }
std::int32_t TankD::getBaseArmor() { return tank_d_armor; }
std::int32_t TankD::getBaseSpeed() { return tank_d_speed; }
std::string TankD::getName() { return tank_d_name; }
