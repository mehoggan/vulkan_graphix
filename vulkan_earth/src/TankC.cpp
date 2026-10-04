#include "vulkan_earth/TankC.h"
#include <cstdint>
#include "vulkan_earth/Tank.h"
#include "vulkan_earth/VBOShaderLibrary.h"
#include "vulkan_earth/MacroCrtdbg.h"

const char* tank_c_name = "HeavyD";
const std::int32_t tank_c_hp = 1000;
const std::int32_t tank_c_power = 10;
const std::int32_t tank_c_armor = 7;
const std::int32_t tank_c_speed = 5;

TankC::TankC() = default;
TankC::TankC(float /*x*/, float /*y*/, float /*z*/) {
    initBody();
    initHead();
    initTurret();
    initWheel();
    initDuration();

    m_turret_offset[0] = 0;
    m_turret_offset[1] = 70;
    m_turret_offset[2] = 100.0001;
    m_head_offset[0] = 0;
    m_head_offset[1] = 0;
    m_head_offset[2] = 0;
    m_body_offset[0] = 0;
    m_body_offset[1] = 90;
    m_body_offset[2] = 0;

    for (std::int32_t i = 0; i < 3; i++) {
        m_body_scale[i] = 80;
        m_head_scale[i] = 80;
        m_turret_scale[i] = 80;
        m_wheel_scale[i] = 80;
    }

    m_power = tank_c_power;
    m_armor = tank_c_armor;
    m_speed = tank_c_speed;
    m_current_power = 10;
    m_previous_power = 1000;
    m_previous_angle = 1;
    m_hp = m_armor * 100;

    m_vbo_shader_head = new VBOShaderLibrary();
    m_vbo_shader_body = new VBOShaderLibrary();
    m_vbo_shader_turret = new VBOShaderLibrary();

    m_vbo_shader_turret->loadClientData("./HeavyD/HeavyD_Turret.ogl");
    m_vbo_shader_body->loadClientData("./HeavyD/HeavyD_Body.ogl");
    m_vbo_shader_head->loadClientData("./HeavyD/HeavyD_Head.ogl");

    m_vbo_shader_turret->loadTexture("TestImage.raw", 1024, 1024);
    m_vbo_shader_body->loadTexture("TestImage.raw", 1024, 1024);
    m_vbo_shader_head->loadTexture("TestImage.raw", 1024, 1024);

    m_projectile_land_pos[0] = 9999999;
    m_projectile_land_pos[1] = 9999999;
}
TankC::~TankC() {
    delete m_vbo_shader_head;
    delete m_vbo_shader_body;
    delete m_vbo_shader_turret;
}

// GETTERS
std::int32_t TankC::getBaseHP() { return tank_c_hp; }
std::int32_t TankC::getBasePower() { return tank_c_power; }
std::int32_t TankC::getBaseArmor() { return tank_c_armor; }
std::int32_t TankC::getBaseSpeed() { return tank_c_speed; }
std::string TankC::getName() { return tank_c_name; }

void TankC::updateHitBox() {
    m_tank_pos.x = m_head_matrix[12];
    m_tank_pos.y = m_head_matrix[13];
    m_tank_pos.z = m_head_matrix[14];
    m_right.x = -m_head_matrix[0];
    m_right.y = -m_head_matrix[1];
    m_right.z = -m_head_matrix[2];
    m_left.x = m_head_matrix[0];
    m_left.y = m_head_matrix[1];
    m_left.z = m_head_matrix[2];
    m_up.x = m_head_matrix[4];
    m_up.y = m_head_matrix[5];
    m_up.z = m_head_matrix[6];
    m_down.x = -m_head_matrix[4];
    m_down.y = -m_head_matrix[5];
    m_down.z = -m_head_matrix[6];
    m_at.x = -m_head_matrix[8];
    m_at.y = -m_head_matrix[9];
    m_at.z = -m_head_matrix[10];
    m_back.x = m_head_matrix[8];
    m_back.y = m_head_matrix[9];
    m_back.z = m_head_matrix[10];
}