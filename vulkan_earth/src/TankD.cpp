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
TankD::TankD(float /*x*/, float /*y*/, float /*z*/) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    m_turret_offset[0] = 0;
    m_turret_offset[1] = 10;
    m_turret_offset[2] = 0.0001;
    m_head_offset[0] = 0;
    m_head_offset[1] = 70;
    m_head_offset[2] = 40;
    m_body_offset[0] = 0;
    m_body_offset[1] = 45;
    m_body_offset[2] = 0;

    for (std::int32_t i = 0; i < 3; i++) {
        m_body_scale[i] = 50;
        m_head_scale[i] = 50;
        m_turret_scale[i] = 50;
        m_wheel_scale[i] = 50;
    }

    m_power = tank_d_power;
    m_armor = tank_d_armor;
    m_speed = tank_d_speed;
    m_current_power = 10;
    m_previous_power = 1000;
    m_previous_angle = 1;
    m_hp = m_armor * 100;

    m_vbo_shader_head = new VBOShaderLibrary();
    m_vbo_shader_body = new VBOShaderLibrary();
    m_vbo_shader_turret = new VBOShaderLibrary();

    m_vbo_shader_turret->loadClientData("./Panzer/Panzer_Turret.ogl");
    m_vbo_shader_body->loadClientData("./Panzer/Panzer_Body.ogl");
    m_vbo_shader_head->loadClientData("./Panzer/Panzer_Head.ogl");

    m_vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
    m_vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
    m_vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);

    m_projectile_land_pos[0] = 9999999;
    m_projectile_land_pos[1] = 9999999;
}
TankD::~TankD() {
    delete m_vbo_shader_head;
    delete m_vbo_shader_body;
    delete m_vbo_shader_turret;
}

// GETTERS
std::int32_t TankD::getBaseHP() { return tank_d_hp; }
std::int32_t TankD::getBasePower() { return tank_d_power; }
std::int32_t TankD::getBaseArmor() { return tank_d_armor; }
std::int32_t TankD::getBaseSpeed() { return tank_d_speed; }
std::string TankD::getName() { return tank_d_name; }
